//
// Created by zyq on 7/14/22.
//

#ifndef MYHTTPSERVER_CACHECONTROL_H
#define MYHTTPSERVER_CACHECONTROL_H

#include <string>
#include <unordered_map>
#include <vector>
#include "Cache.h"
#include "MutexLock.h"
#include "HTTPPool.h"

typedef std::string(*Callback)(std::string);
///一个 Group 可以认为是一个缓存的命名空间

///                            是
/// 接收 key --> 检查是否被缓存 -----> 返回缓存值 ⑴
///                  |否                          是
///                  |-----> 是否应当从远程节点获取 -----> 与远程节点交互 --> 返回缓存值 ⑵
///                              |  否
///                              |-----> 调用`回调函数`，获取值并添加到缓存 --> 返回缓存值 ⑶

struct Group
{
    std::string name_;                               ///每个 Group 拥有一个唯一的名称 name
    Cache cache_;
    Callback callback_;                              ///在缓存不存在时，调用这个函数，得到源数据
    HTTPPool* peers;
    Group(std::string name, int cacheBytes, Callback func);
    std::string get(std::string key);
    void registerPeers(std::string& address, std::vector<std::string>&peers_);
    std::string getFromPeer(PeerGetter* peer, std::string& key);

private:
    std::string load(std::string key);
    std::string getLocally(std::string key);
    void populateCache(std::string key, std::string value);
};

class CacheControl {
public:
    CacheControl();
    ~CacheControl();
    void addGroup(std::string name, int cacheBytes, Callback func);
    Group* getGroup(std::string name);
private:
    std::unordered_map<std::string, Group*>groups;
    MutexLock mutex_;
};


#endif //MYHTTPSERVER_CACHECONTROL_H
