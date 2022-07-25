//
// Created by zyq on 7/19/22.
//

#include "RequestData.h"
#include <memory>
#include <iostream>
#include <string>
#include <cstring>
#include "../utils.h"


bool RequestData::canWrite() {
    return isAbleWrite;
}

bool RequestData::canRead() {
    return isAbleRead;
}

void RequestData::enableWrite() {
    isAbleWrite = true;
}

void RequestData::enableRead() {
    isAbleRead = true;
}

void RequestData::disableReadAndWrite() {
    isAbleRead = false;
    isAbleWrite = false;
}

void RequestData::setFd(int fd) {
    fd_ = fd;
}

int RequestData::getFd() {
    return fd_;
}

RequestData::RequestData()
    :now_read_pos(0),
    state(STATE_PARSE_URI),
    h_state(h_start),
    keep_alive(false),
    isAbleRead(true),
    isAbleWrite(false),
    events(0),
    error(false),
    fd_(-1),
    epollInstance(Epoll<RequestData>::getInstance()),
    timerManagerInstance(TimerManager<RequestData>::getInstance())
{

}

RequestData::RequestData(int epollFd, int fd)
    :now_read_pos(0),
     state(STATE_PARSE_URI),
     h_state(h_start),
     keep_alive(false),
     fd_(fd),
     epollFd_(epollFd),
     isAbleRead(true),
     isAbleWrite(false),
     events(0),
     error(false),
     epollInstance(Epoll<RequestData>::getInstance()),
     timerManagerInstance(TimerManager<RequestData>::getInstance())
{

}

RequestData::~RequestData() {
    close(fd_);
}

void RequestData::linkTimer(std::shared_ptr<TimerNode<RequestData>> mTimer) {
    timer = mTimer;
}

void RequestData::reset() {
    inBuffer.clear();
    now_read_pos = 0;
    state = STATE_PARSE_URI;
    h_state = h_state;
    headers.clear();
    if(timer.lock())
    {
        std::shared_ptr<TimerNode<RequestData>> my_timer(timer.lock());
        my_timer->clearReq();
        timer.reset();
    }
}

void RequestData::seperateTimer() {
    if(timer.lock())
    {
        std::shared_ptr<TimerNode<RequestData>> my_timer(timer.lock());
        my_timer->clearReq();
        timer.reset();
    }
}

void RequestData::handleRead() {
    do{
        int readNum = readn(fd_, inBuffer);
        if(readNum < 0)
        {
            perror("1");
            error = true;
            handleError(fd_, 400, "Bad Request");
            break;
        }
        else if(readNum == 0)
        {
            /// 有请求出现但是读不到数据，可能是Request Aborted，或者来自网络的数据没有达到等原因
            /// 最可能是对端已经关闭了，统一按照对端已经关闭处理
            error = true;
            break;
        }

        if(state == STATE_PARSE_URI)
        {
            int flag = parse_URI();
            if(flag == PARSE_URI_AGAIN)break;
            else if(flag == PARSE_URI_ERROR)
            {
                perror("2");
                error = true;
                handleError(fd_, 400, "Bad Request");
                break;
            }
            else state = STATE_PARSE_HEADERS;
        }

        if(state == STATE_PARSE_HEADERS)
        {
            int flag = parse_Headers();
            if(flag == PARSE_HEADER_AGAIN)break;
            else if(flag == PARSE_HEADER_ERROR)
            {
                perror("3");
                error = true;
                handleError(fd_, 400, "Bad Request");
                break;
            }
            if(method == METHOD_POST)
            {
                state = STATE_RECV_BODY;
            }
            else state = STATE_ANALYSIS;
        }

        if (state == STATE_RECV_BODY)
        {
            int content_length = -1;
            if (headers.count("Content-length"))
            {
                content_length = stoi(headers["Content-length"]);
            }
            else
            {
                error = true;
                handleError(fd_, 400, "Bad Request: Lack of argument (Content-length)");
                break;
            }
            if (inBuffer.size() < content_length)
                break;
            state = STATE_ANALYSIS;
        }
        if (state == STATE_ANALYSIS)
        {
            int flag = this->analysisRequest();
            if (flag == ANALYSIS_SUCCESS)
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

    if (!error)
    {
        if (outBuffer.size() > 0)
            events |= EPOLLOUT;
        if (state == STATE_FINISH)
        {
            std::cout << "keep_alive=" << keep_alive << std::endl;
            if (keep_alive)
            {
                this->reset();
                events |= EPOLLIN;
            }
            else
                return;
        }
        else
            events |= EPOLLIN;
    }
}

void RequestData::handleWrite() {
    if (!error)
    {
        if (writen(fd_, outBuffer) < 0)
        {
            perror("writen");
            events = 0;
            error = true;
        }
        else if (outBuffer.size() > 0)
            events |= EPOLLOUT;
    }
}

void RequestData::handleError(int fd, int errNum, std::string shortMsg) {
    shortMsg = " " + shortMsg;
    char send_buff[MAX_BUFF];
    std::string body_buff, header_buff;
    body_buff += "<html><title>哎~出错了</title>";
    body_buff += "<body bgcolor=\"ffffff\">";
    body_buff += std::to_string(errNum) + shortMsg;
    body_buff += "<hr><em> My Web Server</em>\n</body></html>";

    header_buff += "HTTP/1.1 " + std::to_string(errNum) + shortMsg + "\r\n";
    header_buff += "Content-type: text/html\r\n";
    header_buff += "Connection: close\r\n";
    header_buff += "Content-length: " + std::to_string(body_buff.size()) + "\r\n";
    header_buff += "\r\n";
    // 错误处理不考虑writen不完的情况
    sprintf(send_buff, "%s", header_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
    sprintf(send_buff, "%s", body_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
}

void RequestData::handleConn() {
    if (!error)
    {
        if (events != 0)
        {
            // 一定要先加时间信息，否则可能会出现刚加进去，下个in触发来了，然后分离失败后，又加入队列，最后超时被删，然后正在线程中进行的任务出错，double free错误。
            // 新增时间信息
            int timeout = 2000;
            if (keep_alive)
                timeout = 5 * 60 * 1000;
            isAbleRead = false;
            isAbleWrite = false;
            timerManagerInstance->addTimer(shared_from_this(), timeout);
            if ((events & EPOLLIN) && (events & EPOLLOUT))
            {
                events = __uint32_t(0);
                events |= EPOLLOUT;
            }
            events |= (EPOLLET | EPOLLONESHOT);
            __uint32_t _events = events;
            events = 0;
            if (epollInstance->epoll_mod(fd_, shared_from_this(), _events) < 0)
            {
                printf("Epoll::epoll_mod error\n");
            }
        }
        else if (keep_alive)
        {
            events |= (EPOLLIN | EPOLLET | EPOLLONESHOT);
            int timeout = 5 * 60 * 1000;
            isAbleRead = false;
            isAbleWrite = false;
            timerManagerInstance->addTimer(shared_from_this(), timeout);
            __uint32_t _events = events;
            events = 0;
            if (epollInstance->epoll_mod(fd_, shared_from_this(), _events) < 0)
            {
                printf("Epoll::epoll_mod error\n");
            }
        }
    }
}

int RequestData::parse_URI() {
    std::string &str = inBuffer;
    // 读到完整的请求行再开始解析请求
    int pos = str.find('\r', now_read_pos);
    if (pos < 0)
    {
        return PARSE_URI_AGAIN;
    }
    // 去掉请求行所占的空间，节省空间
    std::string request_line = str.substr(0, pos);
    if (str.size() > pos + 1)
        str = str.substr(pos + 1);
    else
        str.clear();
    // Method
    pos = request_line.find("GET");
    if (pos < 0)
    {
        pos = request_line.find("POST");
        if (pos < 0)
            return PARSE_URI_ERROR;
        else
            method = METHOD_POST;
    }
    else
        method = METHOD_GET;
    //printf("method = %d\n", method);
    // filename
    pos = request_line.find("/", pos);
    if (pos < 0)
        return PARSE_URI_ERROR;
    else
    {
        int _pos = request_line.find(' ', pos);
        if (_pos < 0)
            return PARSE_URI_ERROR;
        else
        {
            if (_pos - pos > 1)
            {
                file_name = request_line.substr(pos + 1, _pos - pos - 1);
                int __pos = file_name.find('?');
                if (__pos >= 0)
                {
                    file_name = file_name.substr(0, __pos);
                }
            }

            else
                file_name = "index.html";
        }
        pos = _pos;
    }
    // HTTP 版本号
    pos = request_line.find("/", pos);
    if (pos < 0)
        return PARSE_URI_ERROR;
    else
    {
        if (request_line.size() - pos <= 3)
            return PARSE_URI_ERROR;
        else
        {
            std::string ver = request_line.substr(pos + 1, 3);
            if (ver == "1.0")
                HTTPVersion = HTTP_10;
            else if (ver == "1.1")
                HTTPVersion = HTTP_11;
            else
                return PARSE_URI_ERROR;
        }
    }
    return PARSE_URI_SUCCESS;
}

int RequestData::parse_Headers() {
    std::string &str = inBuffer;
    int key_start = -1, key_end = -1, value_start = -1, value_end = -1;
    int now_read_line_begin = 0;
    bool notFinish = true;
    for (int i = 0; i < str.size() && notFinish; ++i)
    {
        switch(h_state)
        {
            case h_start:
            {
                if (str[i] == '\n' || str[i] == '\r')
                    break;
                h_state = h_key;
                key_start = i;
                now_read_line_begin = i;
                break;
            }
            case h_key:
            {
                if (str[i] == ':')
                {
                    key_end = i;
                    if (key_end - key_start <= 0)
                        return PARSE_HEADER_ERROR;
                    h_state = h_colon;
                }
                else if (str[i] == '\n' || str[i] == '\r')
                    return PARSE_HEADER_ERROR;
                break;
            }
            case h_colon:
            {
                if (str[i] == ' ')
                {
                    h_state = h_spaces_after_colon;
                }
                else
                    return PARSE_HEADER_ERROR;
                break;
            }
            case h_spaces_after_colon:
            {
                h_state = h_value;
                value_start = i;
                break;
            }
            case h_value:
            {
                if (str[i] == '\r')
                {
                    h_state = h_CR;
                    value_end = i;
                    if (value_end - value_start <= 0)
                        return PARSE_HEADER_ERROR;
                }
                else if (i - value_start > 255)
                    return PARSE_HEADER_ERROR;
                break;
            }
            case h_CR:
            {
                if (str[i] == '\n')
                {
                    h_state = h_LF;
                    std::string key(str.begin() + key_start, str.begin() + key_end);
                    std::string value(str.begin() + value_start, str.begin() + value_end);
                    headers[key] = value;
                    now_read_line_begin = i;
                }
                else
                    return PARSE_HEADER_ERROR;
                break;
            }
            case h_LF:
            {
                if (str[i] == '\r')
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
                if (str[i] == '\n')
                {
                    h_state = h_end_LF;
                }
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
    if (h_state == h_end_LF)
    {
        str = str.substr(now_read_line_begin);
        return PARSE_HEADER_SUCCESS;
    }
    str = str.substr(now_read_line_begin);
    return PARSE_HEADER_AGAIN;
}

int RequestData::analysisRequest() {
    std::string header("HTTP/1.1 200 OK\r\n");
    if(headers.count("Connection") && headers["Connection"] == "keep-alive")
    {
        keep_alive = true;
        header += std::string("Connection: keep-alive\r\n")
                  + "keep-alive:timeout=" + std::to_string(5 * 60 * 1000) + "\r\n";
    }
    if(method == METHOD_POST)
    {


    }
    else if(method == METHOD_GET)
    {
        //std::string header("HTTP/1.1 200 OK\r\n");
    }
    header += std::string("Content-length: ") + std::to_string(5) + "\r\n\r\n";
    header += "12345";
    outBuffer += header;
    //handleError(fd_, 400, "123");
    return ANALYSIS_SUCCESS;
}


