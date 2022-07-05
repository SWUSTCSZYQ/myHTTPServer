//
// Created by zyq on 7/4/22.
//

#ifndef MYHTTPSERVER_TIMER_H
#define MYHTTPSERVER_TIMER_H

#include "RequestData.h"
#include "nonCopyable.h"
#include "MutexLock.h"
#include <unistd.h>
#include <memory>
#include <queue>
#include <deque>

class RequestData;

class TimerNode
{
    typedef std::shared_ptr<RequestData>SP_ReqData;
public:
    TimerNode(SP_ReqData _request_data, int timeout);
    ~TimerNode();
    void update(int timeout);
    bool isValid();
    void clearReq();
    void setDeleted();
    bool isDeleted() const;
    size_t getExpTime()const;
private:
    bool deleted;
    size_t expired_time;
    SP_ReqData request_data;
};

struct timerCmp
{
    bool operator()(std::shared_ptr<TimerNode>&a, std::shared_ptr<TimerNode>& b)const
    {
        return a->getExpTime() > b->getExpTime();
    }
};

class TimerManager
{
    typedef std::shared_ptr<RequestData>SP_ReqData;
    typedef std::shared_ptr<TimerNode>SP_TimerNode;
private:
    std::priority_queue<SP_TimerNode, std::deque<SP_TimerNode>, timerCmp>TimerNodeQueue;
    MutexLock lock;

public:
    TimerManager();
    ~TimerManager();
    void addTimer(SP_ReqData request_data, int timeout);
    //void addTimer(SP_TimerNode timer_node);
    void handle_expired_event();
};


#endif //MYHTTPSERVER_TIMER_H
