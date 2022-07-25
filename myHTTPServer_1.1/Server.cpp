//
// Created by zyq on 7/17/22.
//

#include "Server.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <memory>
#include <iostream>
#include "RequestData.h"
#include "utils.h"
#include "ThreadPool.h"


Server* Server::instance = nullptr;

Server *Server::getInstance(int port) {
    if(instance == nullptr)
    {
        instance = new Server(port);
    }
    return instance;
}

int Server::socket_bind_listen() {
    /// 检查port值，取正确区间范围
    if(port_ < 1024 or port_ > 65535)
        return -1;

    /// 创建socket(IPv4 + TCP)，返回监听描述符
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenFd == -1)return -1;

    /// 消除bind时"Address already in use"错误
    int optVal = 1;
    if(setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal)) == -1)
        return -1;

    /// 设置服务器IP和Port，和监听描述副绑定
    struct sockaddr_in serverAddress;
    bzero(&serverAddress, sizeof serverAddress);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port_);
    if(bind(listenFd, (struct sockaddr*)&serverAddress, sizeof serverAddress) == -1)
        return -1;


    /// 开始监听，最大等待队列长为LISTEN
    if(listen(listenFd, 1024) == -1)
        return -1;

    if(listenFd == -1)
    {
        close(listenFd);
        return -1;
    }
    return 0;
}

Server::Server(int port)
    :port_(port),
    listenFd(0),
    epoll(nullptr),
    timerManager(TimerManager<RequestData>::getInstance()),
    timeout_(500)
{
    handle_for_sigpipe();
    epoll = Epoll<RequestData>::getInstance();
    int ret = socket_bind_listen();
    if(ret < 0)
    {
        std::cout << "socket_bind_listen() error" << std:: endl;
        exit(-1);
    }
    ret = setSocketNonBlocking(listenFd);
    if(ret < 0)
    {
        std::cout << "setSocketNonBlocking() error" << std:: endl;
        exit(-1);
    }
    std::shared_ptr<RequestData>requestData(new RequestData());
    ret = epoll->epoll_add(listenFd, requestData, EPOLLIN | EPOLLET);
    if(ret < 0)
    {
        std::cout << "epoll_add() error" << std:: endl;
        exit(-1);
    }
}

Server::~Server() {
    delete instance;
}

void Server::run() {
    if(ThreadPool::threadpool_create(4, 2048) < 0)
    {
        std::cout << "ThreadPool create failed" << std::endl;
        exit(-1);
    }
    while(true)
    {
        int eventCount = epoll->epoll_wait(listenFd, -1);
        std::vector<SP_ReqData>reqData = getEventsRequest(eventCount);;
        if(reqData.size() > 0)
        {
            for(auto& req : reqData)
            {
                ///加入到线程池中
                if(ThreadPool::threadpool_add(req) < 0)
                {
                    /// 线程池满了或者关闭了等原因，抛弃本次监听到的请求。
                    break;
                }
            }
        }
        timerManager->handleExpiredEvent();
    }
}

void Server::acceptConnection() {
    struct sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof clientAddress);
    socklen_t clientAddressLen = sizeof(clientAddress);
    int acceptFd = 0;
    while((acceptFd = accept(listenFd, (struct sockaddr*)&clientAddress, &clientAddressLen)) > 0)
    {
        std::cout << inet_ntoa(clientAddress.sin_addr) << std::endl;
        std::cout << ntohs(clientAddress.sin_port) << std::endl;

        /// 限制服务器的最大并发连接数
        if(acceptFd >= epoll->getMaxFd())
        {
            close(acceptFd);
            continue;
        }
        int ret = setSocketNonBlocking(acceptFd);
        if(ret < 0)
        {
            perror("Set non block failed!");
            return;
        }

        SP_ReqData reqInfo(new RequestData(epoll->getEpollFd(), acceptFd));
        //SP_ReqData reqInfo(new RequestData());
        __uint32_t _epo_event = EPOLLIN | EPOLLET | EPOLLONESHOT;
        epoll->epoll_add(acceptFd, reqInfo, _epo_event);
        timerManager->addTimer(reqInfo, timeout_);
    }
}

std::vector<std::shared_ptr<RequestData>> Server::getEventsRequest(int eventsNum) {
    std::vector<SP_ReqData>reqData;
    for(int i = 0; i < eventsNum; ++i)
    {
        auto events = epoll->getEvents();
        int fd = events[i].data.fd;
        auto fd2req = epoll->getFd2req();
        if(fd == listenFd)
        {
            acceptConnection();
        }
        else if(fd < 3)
        {
            printf("fd < 3\n");
            break;
        }
        else
        {
            if(events[i].events & EPOLLERR or events[i].events & EPOLLHUP)
            {
                std::cout << "error events" << std::endl;
                if(fd2req[fd])
                    fd2req[fd]->seperateTimer();
                fd2req[fd].reset();
                continue;
            }

            /// 将请求任务加入到线程池中
            /// 加入线程池之前将Timer和request分离
            SP_ReqData curReq = fd2req[fd];
            if(curReq)
            {
                if ((events[i].events & EPOLLIN) || (events[i].events & EPOLLPRI))
                    curReq->enableRead();
                if (events[i].events & EPOLLOUT)
                    curReq->enableWrite();

                curReq->seperateTimer();
                reqData.push_back(curReq);
                fd2req[fd].reset();
            }
            else
            {
                std::cout << "SP cur_req is invalid" << std::endl;
            }
        }
    }
    return reqData;
}

