//
// Created by zyq on 7/16/22.
//

#ifndef MYHTTPSERVER_NODE_H
#define MYHTTPSERVER_NODE_H

#include <string>

/// 实体节点
class Node {
public:
    Node(std::string nodeName, int vNodeNum)
            :nodeName(nodeName), vNodeNum(vNodeNum){}
    ~Node() = default;
    int GetVNodeNum(){ return vNodeNum; }
    std::string GetNodeName(){ return nodeName; }
private:
    std::string nodeName;//节点名，也是服务器名
    int vNodeNum;//虚拟节点个数
};


#endif //MYHTTPSERVER_NODE_H
