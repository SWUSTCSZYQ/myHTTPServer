//
// Created by zyq on 7/19/22.
//

#include "RequestData.h"
#include <memory>



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
    error(false)
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
     error(false)
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
    outBuffer.clear();
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

}

void RequestData::handleWrite() {

}

void RequestData::handleError(int fd, int errNum, std::string shortMsg) {

}

void RequestData::handleConn() {

}


