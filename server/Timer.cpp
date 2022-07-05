//
// Created by zyq on 7/4/22.
//

#include "Timer.h"
#include "Epoll.h"
#include <unordered_map>
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <deque>
#include <queue>
#include <iostream>
using namespace std;

TimerNode::TimerNode(TimerNode::SP_ReqData _request_data, int timeout)
    : deleted(false),
    request_data(_request_data)
{
    struct timeval now;
    gettimeofday(&now, nullptr);
    // 以毫秒计
    expired_time = now.tv_sec * 1000 + now.tv_usec / 1000 + timeout;
}

TimerNode::~TimerNode() {
    if(request_data)
    {
        Epoll::epoll_del(request_data->getFd());
    }
}

void TimerNode::update(int timeout) {
    struct timeval now;
    gettimeofday(&now, nullptr);
    expired_time = now.tv_sec * 1000 + now.tv_usec / 1000 + timeout;
}

bool TimerNode::isValid() {
    struct timeval now;
    gettimeofday(&now, nullptr);
    size_t cur = now.tv_sec * 1000 + now.tv_usec / 1000;
    if(cur < expired_time)
    {
        return true;
    }
    else
    {
        this->setDeleted();
        return false;
    }
}

void TimerNode::clearReq() {
    request_data.reset();
    this->setDeleted();
}

void TimerNode::setDeleted() {
    deleted = true;
}

bool TimerNode::isDeleted() const {
    return deleted;
}

size_t TimerNode::getExpTime() const {
    return expired_time;
}

TimerManager::TimerManager() {
}

TimerManager::~TimerManager() {
}

void TimerManager::addTimer(TimerManager::SP_ReqData request_data, int timeout) {
    SP_TimerNode new_node(new TimerNode(request_data, timeout));
    {
        MutexLockGuard locker(this->lock);
        TimerNodeQueue.push(new_node);
    }
    request_data->linkTimer(new_node);
}

void TimerManager::handle_expired_event() {
    MutexLockGuard locker(lock);
    while(!TimerNodeQueue.empty())
    {
        SP_TimerNode  cur = TimerNodeQueue.top();
        if(cur->isDeleted() or !cur->isValid())
        {
            TimerNodeQueue.pop();
        }
        else break;
    }

}
