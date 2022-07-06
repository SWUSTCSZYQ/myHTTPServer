//
// Created by zyq on 7/4/22.
//

#ifndef MYHTTPSERVER_EPOLL_H
#define MYHTTPSERVER_EPOLL_H

#include "RequestData.h"
#include "Timer.h"
#include <vector>
#include <unordered_map>
#include <sys/epoll.h>
#include <memory>
using namespace std;

class Epoll {
public:
    typedef std::shared_ptr<RequestData>SP_ReqData;
public:
    static int epoll_init(int maxEvent, int listenNum);
    static int epoll_add(int fd, SP_ReqData request, __uint32_t events);
    static int epoll_mod(int fd, SP_ReqData request, __uint32_t events);
    static int epoll_del(int fd, __uint32_t events = (EPOLLIN | EPOLLET | EPOLLONESHOT));
    static void my_epoll_wait(int listen_fd, int max_events, int timeout);
    static void acceptConnection(int listenfd, int epoll_fd, const std::string path);
    static vector<SP_ReqData> getEventsRequest(int listen_fd, int events_num, const std::string path);

    static void add_timer(SP_ReqData requestData, int timeout);

private:
    static const int MAXFDS = 1000;
    static epoll_event *events;
    static SP_ReqData fd2req[MAXFDS];
    static int epoll_fd;
    static const string Path;

    static TimerManager timer_manager;

};


#endif //MYHTTPSERVER_EPOLL_H
