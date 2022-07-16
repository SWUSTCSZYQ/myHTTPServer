//
// Created by zyq on 7/15/22.
//

#ifndef MYHTTPSERVER_CACHEHTTPSERVER_H
#define MYHTTPSERVER_CACHEHTTPSERVER_H

#include <string>


class CacheHttpServer {
public:
    CacheHttpServer(std::string ip, int port, std::string baseName);
    ~CacheHttpServer() = default;
private:
    void init();

private:
    std::string ip_;
    int port_;
    int listenFd;
    std::string baseName_;  ///作为节点间通讯地址的前缀
};


#endif //MYHTTPSERVER_CACHEHTTPSERVER_H
