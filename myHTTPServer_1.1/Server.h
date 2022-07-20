//
// Created by zyq on 7/17/22.
//

#ifndef MYHTTPSERVER_1_1_SERVER_H
#define MYHTTPSERVER_1_1_SERVER_H

#include "Epoll.h"
#include "Timer.h"
#include <vector>
#include <memory>

class RequestData;
class Server {
public:
    typedef std::shared_ptr<RequestData>SP_ReqData;

public:
    static Server* getInstance(int port);
    void run();
private:
    int socket_bind_listen();
    void acceptConnection();
    std::vector<std::shared_ptr<RequestData>> getEventsRequest(int eventsNum);

private:
    explicit Server(int port);
    ~Server();
    Server(const Server&);
    Server& operator=(const Server&);
private:
    int port_;
    int listenFd;
    Epoll<RequestData>* epoll;
    TimerManager<RequestData>*timerManager;
    static Server* instance;
    int timeout_;
};



#endif //MYHTTPSERVER_1_1_SERVER_H
