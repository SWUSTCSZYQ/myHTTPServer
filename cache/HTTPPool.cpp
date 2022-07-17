//
// Created by zyq on 7/17/22.
//

#include "HTTPPool.h"

///选择一个peer 根据key
PeerGetter *HTTPPool::pickPeer(std::string key) {
    mutex_.lock();
    std::string peer = peers->getServeName(key);
    if(peer == self)return nullptr;
    return httpGetters[peer];
}

HTTPPool::HTTPPool(std::string& address, std::vector<std::string>&peers_)
    :self(address),
    basePath("/cache/"),
    mutex_(),
    peers(),
    httpGetters()
{
    for(auto peer : peers_)
    {
        ///每个真实节点有50个虚拟节点
        peers->addNode(peer, 50);
        std::string url = peer + basePath;
        httpGetters[peer] = new HttpGetter(url);
    }
}

HTTPPool::~HTTPPool() {
    delete peers;
    for(auto iter = httpGetters.begin(); iter != httpGetters.end();++iter)
    {
        delete iter->second;
    }
}
