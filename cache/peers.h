//
// Created by zyq on 7/17/22.
//

#ifndef MYHTTPSERVER_PEERS_H
#define MYHTTPSERVER_PEERS_H

#include <string>

class PeerGetter
{
public:
    ///用于从对应 group 查找缓存值
    virtual std::string get(std::string group, std::string key) = 0;
};

///PeerPicker是定位必须实现的接口
class PeerPicker{
public:
    ///用于根据传入的 key 选择相应节点 PeerGetter
    virtual PeerGetter* pickPeer(std::string key) = 0;
};

#endif //MYHTTPSERVER_PEERS_H
