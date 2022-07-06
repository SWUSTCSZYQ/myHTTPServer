//
// Created by zyq on 7/4/22.
//

#include "RequestData.h"
#include "util.h"
#include "Epoll.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <queue>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <string>

using namespace std;

RequestData::RequestData():
    now_read_pos(0),
    state(STATE_PARSE_URI),
    h_state(h_start),
    keep_alive(false),
    isAbleRead(true),
    isAbleWrite(false),
    events(0),
    error(false)
{
    cout << "RequestData()" << endl;
}

RequestData::RequestData(int _epollfd, int _fd, std::string _path) :
    now_read_pos(0),
    state(STATE_PARSE_URI),
    h_state(h_start),
    keep_alive(false),
    path(_path),
    fd(_fd),
    epollfd(_epollfd),
    isAbleRead(true),
    isAbleWrite(false),
    events(0),
    error(false)
{
    cout << "a new RequestData object" << endl;
}

RequestData::~RequestData()
{
    cout << "a RequestData object destroyed" << endl;
    close(fd);
}

void RequestData::linkTimer(std::shared_ptr<TimerNode> mTimer)
{
    timer = mTimer;
}

void RequestData::reset() {
    inBuffer.clear();
    file_name.clear();
    path.clear();
    now_read_pos = 0;
    state = STATE_PARSE_URI;
    h_state = h_start;
    headers.clear();

    if(timer.lock())
    {
        shared_ptr<TimerNode>myTimer(timer.lock());
        myTimer->clearReq();
        timer.reset();
    }
}

void RequestData::seperateTimer()
{
    if (timer.lock())
    {
        shared_ptr<TimerNode> my_timer(timer.lock());
        my_timer->clearReq();
        timer.reset();
    }
}

int RequestData::getFd()
{
    return fd;
}

void RequestData::setFd(int _fd) {
    fd = _fd;
}

void RequestData::handleRead()
{
    do {
        int read_num = readn(fd, inBuffer);
        if(read_num < 0)
        {
            perror("1");
            error = true;
            handleError(fd, 400, "Bad Request");
        }
        else if(read_num == 0)
        {
            // 有请求出现但是读不到数据，可能是Request Aborted，或者来自网络的数据没有达到等原因
            // 最可能是对端已经关闭了，统一按照对端已经关闭处理
            error = true;
            break;
        }

        if(state == STATE_PARSE_URI)
        {
            int flag = this->parse_URI();
            if(flag == PARSE_URI_AGAIN)break;
            else if(flag == PARSE_URI_ERROR)
            {
                cout << inBuffer << endl;
                perror("x");
                error = true;
                handleError(fd, 400, "Bad Request");
                break;
            }
            else state = STATE_PARSE_HEADERS;
        }
        if(state == STATE_PARSE_HEADERS)
        {
            int flag = this->parse_Header();
            if(flag == PARSE_HEADER_AGAIN)break;
            else if(flag == PARSE_HEADER_ERROR)
            {
                perror("3");
                error = true;
                handleError(fd, 400, "Bad Request");
            }
            if(method == METHOD_POST)
            {
                state = STATE_RECV_BODY;
            }
            else
            {
                state = STATE_ANALYSIS;
            }
        }
        if(state == STATE_RECV_BODY)
        {
            int content_length = -1;
            if(headers.find("Content-length") != headers.end())
            {
                content_length = stoi(headers["Content-length"]);
            }
            else
            {
                error = true;
                handleError(fd, 400, "Bad Request: lack of argument (Content-length)");
                break;
            }
            if(inBuffer.size() < content_length)break;
            state = STATE_ANALYSIS;
        }
        if(state == STATE_ANALYSIS)
        {
            int flag = this->analysisRequest();
            if(flag == ANALYSIS_SUCCESS)
            {
                state = STATE_FINISH;
                break;
            }
            else
            {
                error = true;
                break;
            }
        }
    }while(false);

    if(!error)
    {
        if(outBuffer.size() > 0)events |= EPOLLOUT;
        if(state == STATE_FINISH)
        {
            cout << "keep_alive= " << keep_alive << endl;
            if(keep_alive)
            {
                this->reset();
                events |= EPOLLIN;
            }
            else return;
        }
        else events |= EPOLLIN;
    }

}

void RequestData::handleWrite() {
    if(!error)
    {
        if(writen(fd, outBuffer) < 0)
        {
            perror("writen");
            events = 0;
            error = true;
        }
        else if(outBuffer.size() > 0)
        {
            events |= EPOLLOUT;
        }
    }
}

void RequestData::handleError(int fd, int err_num, std::string short_msg) {
    short_msg = " " + short_msg;
    char send_buff[MAX_BUFF];
    string body_buff, header_buff;
    body_buff += "<html><title>哎~出错了</title>";
    body_buff += "<body bgcolor=\"ffffff\">";
    body_buff += to_string(err_num) + short_msg;
    body_buff += "<hr><em> ZYQ's Web Server</em>\n</body></html>";

    header_buff += "HTTP/1.1 " + to_string(err_num) + short_msg + "\r\n";
    header_buff += "Content-type: text/html\r\n";
    header_buff += "Connection: close\r\n";
    header_buff += "Content-length: " + to_string(body_buff.size()) + "\r\n";
    header_buff += "\r\n";
    // 错误处理不考虑writen不完的情况
    sprintf(send_buff, "%s", header_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
    sprintf(send_buff, "%s", body_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
}

void RequestData::handleConn() {
    if(!error)
    {
        if(events != 0)
        {
            ///要先加时间信息，否则可能会出现刚加进去，下个in触发来了，然后分离失败后，又加入队列，最后超时被删,
            /// 然后正在线程中进行的任务出错，double free错误。
            ///新增时间信息
            int timeout = 2000;
            if(keep_alive)timeout = 5 * 60 * 1000;
            isAbleRead = false;
            isAbleWrite = false;
            Epoll::add_timer(shared_from_this(), timeout);
            if(events & EPOLLIN and events & EPOLLOUT)
            {
                events = __uint32_t(0);
                events |= EPOLLOUT;
            }
            events |= (EPOLLET | EPOLLONESHOT);
            __uint32_t _events = events;
            events = 0;
            if(Epoll::epoll_mod(fd, shared_from_this(), _events) < 0)
            {
                cout << "Epoll::epoll_mod error" << endl;
            }
        }
        else if(keep_alive)
        {
            events |= (EPOLLIN | EPOLLET | EPOLLONESHOT);
            int timeout = 5 * 60 * 1000;
            isAbleRead = false;
            isAbleWrite = false;
            Epoll::add_timer(shared_from_this(), timeout);
            __uint32_t _events = events;
            events = 0;
            if(Epoll::epoll_mod(fd, shared_from_this(), _events) < 0)
            {
                cout << "Epoll::epoll_mod error" << endl;
            }
        }
    }
}

void RequestData::disableReadAndWrite() {
    isAbleRead = false;
    isAbleWrite = false;
}

void RequestData::enableRead() {
    isAbleRead = true;
}

void RequestData::enableWrite() {
    isAbleWrite = true;
}

bool RequestData::canRead() {
    return isAbleRead;
}

bool RequestData::canWrite() {
    return isAbleWrite;
}

int RequestData::parse_URI() {
    string &str = inBuffer;
    // 读到完整的请求行再开始解析请求
    int pos = str.find('\r', now_read_pos);
    if(pos < 0)return PARSE_URI_AGAIN;
    string request_line = str.substr(0, pos);
    if(str.size() > pos + 1)
        str = str.substr(pos + 1);
    else
        str.clear();

    pos = request_line.find("GET");
    if(pos < 0)
    {
        pos = request_line.find("POST");
        if(pos < 0)
            return PARSE_URI_ERROR;
        else method = METHOD_POST;
    }
    else method = METHOD_GET;

    pos = request_line.find("/", pos);
    if(pos < 0)
        return PARSE_URI_ERROR;
    else
    {
        int _pos = request_line.find(' ', pos);
        if(_pos < 0)
            return PARSE_URI_ERROR;
        else
        {
            if(_pos - pos > 1)
            {
                file_name = request_line.substr(pos + 1, _pos - pos - 1);
                int __pos = file_name.find('?');
                if(__pos >= 0)
                    file_name = file_name.substr(0, __pos);
            }
            else file_name = "index.html";
        }
        pos = _pos;
    }
    /// HTTP 版本号
    pos = request_line.find("/", pos);
    if(pos < 0)return PARSE_URI_ERROR;
    else
    {
        if(request_line.size() - pos <= 3)
            return PARSE_URI_ERROR;
        else
        {
            string ver = request_line.substr(pos + 1, 3);
            if(ver == "1.0")HTTPVersion = HTTP_10;
            else if(ver == "1.1")HTTPVersion = HTTP_11;
            else return PARSE_URI_ERROR;
        }
    }
    return PARSE_URI_SUCCESS;
}

int RequestData::parse_Header() {
    string &str = inBuffer;
    int key_start = -1, key_end = -1, value_star = -1, value_end = -1;
    int now_read_line_begin = 0;
    bool notFinish = true;
    for(int i = 0; i < str.size() and notFinish; ++i)
    {
        switch (h_state) {
            case h_start:
            {
                if(str[i] == '\n' or str[i] == '\r')break;
                h_state = h_key;
                key_start = i;
                now_read_line_begin = 1;
                break;
            }
            case h_key:
            {
                if(str[i] == ':')
                {
                    key_end = i;
                    if(key_end - key_start <= 0)
                        return PARSE_HEADER_ERROR;
                    h_state = h_colon;
                }
                else if(str[i] == '\n' or str[i] == '\r')
                    return PARSE_HEADER_ERROR;
                break;
            }
            case h_colon:
            {
                if(str[i] == ' ')h_state = h_spaces_after_colon;
                else return PARSE_HEADER_ERROR;
                break;
            }
            case h_spaces_after_colon:
            {
                h_state = h_value;
                value_star = i;
                break;
            }
            case h_value:
            {
                if(str[i] == '\r')
                {
                    h_state = h_CR;
                    value_end = i;
                    if(value_end - value_star <= 0)
                        return PARSE_HEADER_ERROR;
                }
                else if(i -value_star > 255)
                    return PARSE_URI_ERROR;
                break;
            }
            case h_CR:
            {
                if(str[i] == '\n')
                {
                    h_state = h_LF;
                    string key(str.begin() + key_start, str.begin() + key_end);
                    string val(str.begin() + value_star, str.begin() + value_end);
                    headers[key] = val;
                    now_read_line_begin = i;
                }
                else return PARSE_HEADER_ERROR;
                break;
            }
            case h_LF:
            {
                if(str[i] == '\r')
                {
                    h_state = h_end_CR;
                }
                else
                {
                    key_start = i;
                    h_state = h_key;
                }
                break;
            }
            case h_end_CR:
            {
                if(str[i] == '\n')
                    h_state = h_end_LF;
                else
                    return PARSE_HEADER_ERROR;
                break;
            }
            case h_end_LF:
            {
                notFinish = false;
                key_start = i;
                now_read_line_begin = i;
                break;
            }
        }
    }
    str = str.substr(now_read_line_begin);
    if(h_state == h_end_LF)return PARSE_URI_SUCCESS;
    return PARSE_HEADER_AGAIN;
}

int RequestData::analysisRequest() {
    if(method == METHOD_POST){
        std::string header;
        header += std::string("HTTP/1.1 200 OK\r\n");
        std::string ans = "我已经收到了你的post请求";
        if(headers.find("Connection") != headers.end() && headers["Connection"] == "keep-alive") {
            keep_alive = true;
            header +=
                    std::string("Connection: keep-alive\r\n") + "Keep-Alive: timeout=" + std::to_string(5 * 60 * 1000) +
                    "\r\n";
        }
        int length = stoi(headers["Content-length"]);
        std::vector<char> data(inBuffer.begin(), inBuffer.begin() + length);
        std::cout << " data.size()=" << data.size() << std::endl;

        header += std::string("Content-length: ") + std::to_string(ans.size()) + "\r\n\r\n";
        outBuffer += header;
        outBuffer += "<html><title>post Request</title>";
        outBuffer += "<body bgcolor=\"ffffff\">";
        outBuffer += ans;
        outBuffer += "<hr><em> My Web Server</em>\n</body></html>";
        inBuffer = inBuffer.substr(length);
        return ANALYSIS_SUCCESS;
    }
    else if(method == METHOD_GET){
        //LOG << "收到一个get请求";
        std::string header;
        std::string ans = "我已经收到了你的get请求";
        header += "HTTP/1.1 200 OK\r\n";
        if(headers.find("Connection") != headers.end() && headers["Connection"] == "keep-alive")
        {
            keep_alive = true;
            header += std::string("Connection: keep-alive\r\n") + "Keep-Alive: timeout=" + std::to_string(5 * 60 * 1000) + "\r\n";
        }
        header += "Content-type: text/html\r\n";
        header += "Content-length: " + std::to_string(ans.size()) + "\r\n";
        // 头部结束
        header += "\r\n";
        outBuffer += header;
        outBuffer += "<html><title>get Request</title>";
        outBuffer += "<body bgcolor=\"ffffff\">";
        outBuffer += ans;
        outBuffer += "<hr><em> My Web Server</em>\n</body></html>";

        return ANALYSIS_SUCCESS;
    }
    else
        return ANALYSIS_ERROR;
}
