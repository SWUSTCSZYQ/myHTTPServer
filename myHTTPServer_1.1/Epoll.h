//
// Created by zyq on 7/17/22.
//

#ifndef MYHTTPSERVER_1_1_EPOLL_H
#define MYHTTPSERVER_1_1_EPOLL_H

#include <sys/epoll.h>
#include <memory>

template <typename RequestData>
class Epoll {
public:
    typedef std::shared_ptr<RequestData>SP_ReqData;

public:
    static Epoll* getInstance(int maxEvents = 5000, int listenNum = 1024);
    int epoll_add(int fd, SP_ReqData request, __uint32_t events);
    int epoll_mod(int fd, SP_ReqData request, __uint32_t events);
    int epoll_del(int fd, __uint32_t events = (EPOLLIN | EPOLLET | EPOLLONESHOT));
    int epoll_wait(int listenFd, int timeout);
    int getEpollFd(){return epollFd;}
    int getMaxFd(){return maxFD;}
    epoll_event* getEvents(){return event;}
    SP_ReqData* getFd2req(){return fd2req;}
private:
    Epoll(int maxEvents, int listenNum);
    ~Epoll();


private:
    static const int maxFD = 1000;
    static Epoll* instance;
    epoll_event *event;
    SP_ReqData fd2req[maxFD];
    int epollFd;
    const std::string PATH;
    int maxEvents_;
};
template<typename RequestData>
Epoll<RequestData>* Epoll<RequestData>::instance = nullptr;

template<typename RequestData>
Epoll<RequestData> *Epoll<RequestData>::getInstance(int maxEvents, int listenNum) {
    if(instance == nullptr)
    {
        instance = new Epoll<RequestData>(maxEvents, listenNum);
    }
    return instance;
}

template<typename RequestData>
Epoll<RequestData>::~Epoll() {
    delete instance;
}

template<typename RequestData>
Epoll<RequestData>::Epoll(int maxEvents, int listenNum)
    :PATH("/"),
    epollFd(0),
    maxEvents_(maxEvents)
{
        epollFd = epoll_create(listenNum + 1);
        if(epollFd == -1)
            exit(-1);
        event = new epoll_event[maxEvents];
}

template<typename RequestData>
int Epoll<RequestData>::epoll_add(int fd, Epoll::SP_ReqData request, __uint32_t events) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = events;
    fd2req[fd] = request;
    if(epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        perror("epoll_add error");
        return -1;
    }
    return 0;
}

template<typename RequestData>
int Epoll<RequestData>::epoll_mod(int fd, Epoll::SP_ReqData request, __uint32_t events) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = events;
    fd2req[fd] = request;
    if(epoll_ctl(epollFd, fd, EPOLL_CTL_MOD, &event) < 0)
    {
        perror("epoll_mod error");
        fd2req[fd].reset();
        return -1;
    }
    return 0;
}

template<typename RequestData>
int Epoll<RequestData>::epoll_del(int fd, __uint32_t events) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = events;
    if(epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, &event) < 0)
    {
        perror("epoll_del error");
        return -1;
    }
    fd2req[fd].reset();
    return 0;
}

template<typename RequestData>
int Epoll<RequestData>::epoll_wait(int listenFd, int timeout) {
    int eventCount = ::epoll_wait(epollFd, event, maxEvents_, timeout);
    if(eventCount < 0)
        perror("epoll wait error");
    return eventCount;
}


#endif //MYHTTPSERVER_1_1_EPOLL_H
