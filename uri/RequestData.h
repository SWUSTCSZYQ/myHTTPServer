//
// Created by zyq on 7/4/22.
//

#ifndef MYHTTPSERVER_REQUESTDATA_H
#define MYHTTPSERVER_REQUESTDATA_H

#include "time.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <sys/epoll.h>

const int STATE_PARSE_URI = 1;
const int STATE_PARSE_HEADERS = 2;
const int STATE_RECV_BODY = 3;
const int STATE_ANALYSIS = 4;
const int STATE_FINISH = 5;

const int MAX_BUFF = 4000;
/// 有请求出现但是读不到数据,可能是Request Aborted,
/// 或者来自网络的数据没有达到等原因,
/// 对这样的请求尝试超过一定的次数就抛弃
const int AGAIN_MAX_TIMES = 200;

const int PARSE_URI_AGAIN = -1;
const int PARSE_URI_ERROR = -2;
const int PARSE_URI_SUCCESS = 0;

const int PARSE_HEADER_AGAIN = -1;
const int PARSE_HEADER_ERROR = -2;
const int PARSE_HEADER_SUCCESS = 0;

const int ANALYSIS_ERROR = -2;
const int ANALYSIS_SUCCESS = 0;

const int METHOD_POST = 1;
const int METHOD_GET = 2;
const int HTTP_10 = 1;
const int HTTP_11 = 2;

const int EPOLL_WAIT_TIME = 500;

enum HeadersState
{
    h_start = 0,
    h_key,
    h_colon,
    h_spaces_after_colon,
    h_value,
    h_CR,
    h_LF,
    h_end_CR,
    h_end_LF
};

class TimerNode;

class RequestData: public std::enable_shared_from_this<RequestData>{
public:
    RequestData();
    RequestData(int _epollfd, int _fd, std::string _path);
    ~RequestData();
    void linkTimer(std::shared_ptr<TimerNode>mTimer);
    void reset();
    void seperateTimer();
    int getFd();
    void setFd(int _fd);
    void handleRead();
    void handleWrite();
    void handleError(int fd, int err_num, std::string short_msg);
    void handleConn();

    void disableReadAndWrite();
    void enableRead();
    void enableWrite();
    bool canRead();
    bool canWrite();


private:
    int parse_URI();
    int parse_Header();
    int analysisRequest();

private:
    std::string path;
    int fd;
    int epollfd;

    std::string inBuffer;
    std::string outBuffer;
    __uint32_t events;
    bool error;

    int method;
    int HTTPVersion;
    std::string file_name;
    int now_read_pos;
    int state;
    int h_state;
    bool isFinish;
    bool keep_alive;
    std::unordered_map<std::string, std::string>headers;
    std::weak_ptr<TimerNode>timer;
    bool isAbleRead;
    bool isAbleWrite;
};


#endif //MYHTTPSERVER_REQUESTDATA_H
