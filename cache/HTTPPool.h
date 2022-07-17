//
// Created by zyq on 7/17/22.
//

#ifndef MYHTTPSERVER_HTTPPOOL_H
#define MYHTTPSERVER_HTTPPOOL_H

#include <string>
#include <unordered_map>
#include <vector>
#include "peers.h"
#include "HttpGetter.h"
#include "MutexLock.h"
#include "ConsistentHash.h"



class HTTPPool: public PeerPicker{
public:
    PeerGetter* pickPeer(std::string key) override;
    HTTPPool(std::string& address, std::vector<std::string>&peers_);
    ~HTTPPool();
private:
    std::string self;           ///用来记录自己的地址，包括主机名/IP 和端口。
    std::string basePath;       ///作为节点间通讯地址的前缀
    MutexLock mutex_;
    ConsistentHash *peers;
    std::unordered_map<std::string, HttpGetter*>httpGetters;

};


#endif //MYHTTPSERVER_HTTPPOOL_H
