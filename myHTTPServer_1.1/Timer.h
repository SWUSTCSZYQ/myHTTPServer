//
// Created by zyq on 7/18/22.
//

#ifndef MYHTTPSERVER_1_1_TIMER_H
#define MYHTTPSERVER_1_1_TIMER_H

#include "nonCopyable.h"
#include "MutexLock.h"
#include <unistd.h>
#include <memory>
#include <queue>
#include <deque>
#include <sys/time.h>
#include "Epoll.h"


template<typename RequestData_>
class TimerNode
{
public:
    typedef std::shared_ptr<RequestData_>SP_ReqData;

public:
    TimerNode(SP_ReqData requestData_, int timeout);
    ~TimerNode();
    void update(int timeout);
    bool isValid();
    void clearReq();
    void setDeleted();
    bool isDeleted() const;
    size_t getExpTime() const;
private:
    bool deleted;
    size_t expired_time;
    SP_ReqData requestData;
    Epoll<RequestData_>* epoll;
};

template<typename RequestData_>
TimerNode<RequestData_>::TimerNode(TimerNode::SP_ReqData requestData_, int timeout)
    :deleted(false),
    requestData(requestData_),
    epoll(Epoll<RequestData_>::getInstance())
{
        struct timeval now;
    gettimeofday(&now, nullptr);
    /// 以毫秒计
    expired_time = ((now.tv_sec * 1000) + (now.tv_usec / 1000)) + timeout;
}

template<typename RequestData_>
TimerNode<RequestData_>::~TimerNode() {
    if(requestData)
    {
        epoll->epoll_del(requestData->getFd());
    }
}

template<typename RequestData_>
void TimerNode<RequestData_>::update(int timeout) {
    struct timeval now;
    gettimeofday(&now, nullptr);
    expired_time = ((now.tv_sec * 1000) + (now.tv_usec / 1000)) + timeout;
}

template<typename RequestData_>
bool TimerNode<RequestData_>::isValid() {
    struct timeval now;
    gettimeofday(&now, nullptr);
    size_t temp = ((now.tv_sec * 1000) + (now.tv_usec / 1000));
    if(temp < expired_time)
        return true;
    else
    {
        this->setDeleted();
        return false;
    }
}

template<typename RequestData_>
void TimerNode<RequestData_>::clearReq() {
    requestData.reset();
    this->setDeleted();
}

template<typename RequestData_>
void TimerNode<RequestData_>::setDeleted() {
    deleted = true;
}

template<typename RequestData_>
bool TimerNode<RequestData_>::isDeleted() const {
    return deleted;
}

template<typename RequestData_>
size_t TimerNode<RequestData_>::getExpTime() const {
    return expired_time;
}

template<typename RequestData_>
struct timerCmp
{
    bool operator()(std::shared_ptr<TimerNode<RequestData_>> &a, std::shared_ptr<TimerNode<RequestData_>> &b) const
    {
        return a->getExpTime() > b->getExpTime();
    }
};


template<typename RequestData_>
class TimerManager
{
public:
    typedef std::shared_ptr<RequestData_>SP_ReqData;
    typedef std::shared_ptr<TimerNode<RequestData_>>SP_TimerNode;
public:
    static TimerManager<RequestData_>* getInstance()
    {
        if(instance == nullptr)
        {
            instance = new TimerManager();
        }
        return instance;
    }

    void addTimer(SP_ReqData requestData, int timeout);
    //void addTimer(SP_TimerNode timerNode);
    void handleExpiredEvent();

private:
    TimerManager();
    ~TimerManager();
private:
    std::priority_queue<SP_TimerNode, std::deque<SP_TimerNode>, timerCmp<RequestData_>>timerNodeQueue;
    MutexLock mutex_;
    static TimerManager<RequestData_>* instance;
};
template<typename RequestData_>
TimerManager<RequestData_>* TimerManager<RequestData_>::instance = nullptr;

template<typename RequestData_>
TimerManager<RequestData_>::TimerManager() {

}

template<typename RequestData_>
TimerManager<RequestData_>::~TimerManager() {
    delete instance;
}

template<typename RequestData_>
void TimerManager<RequestData_>::addTimer(TimerManager::SP_ReqData requestData, int timeout) {
    SP_TimerNode newNode(new TimerNode<RequestData_>(requestData, timeout));
    {
        MutexLockGuard locker(mutex_);
        timerNodeQueue.push(newNode);
    }
    requestData->linkTimer(newNode);
}



///因为(1) 优先队列不支持随机访问
///(2) 即使支持，随机删除某节点后破坏了堆的结构，需要重新更新堆结构。
///所以对于被置为deleted的时间节点，会延迟到它(1)超时 或 (2)它前面的节点都被删除时，它才会被删除。
///一个点被置为deleted,它最迟会在TIMER_TIME_OUT时间后被删除。
///这样做有两个好处：
///(1) 第一个好处是不需要遍历优先队列，省时。
///(2) 第二个好处是给超时时间一个容忍的时间，就是设定的超时时间是删除的下限(并不是一到超时时间就立即删除)，如果监听的请求在超时后的下一次请求中又一次出现了，
///就不用再重新申请RequestData节点了，这样可以继续重复利用前面的RequestData，减少了一次delete和一次new的时间。
template<typename RequestData_>
void TimerManager<RequestData_>::handleExpiredEvent() {
    MutexLockGuard locker(mutex_);
    while(!timerNodeQueue.empty())
    {
        SP_TimerNode pTimer = timerNodeQueue.top();
        if(pTimer->isDeleted() or !pTimer->isValid())
        {
            timerNodeQueue.pop();
        }
        else break;
    }
}


#endif //MYHTTPSERVER_1_1_TIMER_H
