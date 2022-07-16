//
// Created by zyq on 7/16/22.
//

#ifndef MYHTTPSERVER_CONSISTENTHASH_H
#define MYHTTPSERVER_CONSISTENTHASH_H

#include <map>
#include <list>
#include <string>
#include <vector>
#include <sstream>
#include <HashFunc.h>
#include "MD5HashFunc.h"
#include "Node.h"

class ConsistentHash {
public:
    explicit ConsistentHash(HashFunc *func = nullptr);
    ~ConsistentHash();
    bool addNode(std::string nodeName, int vNum);
    bool delNode(std::string nodeName);
    ///根据client找对应的server
    std::string getServeName(std::string cliName);
    ///获取真实节点个数
    int getRealNum();
    ///获取某个真实节点的虚拟节点个数
    int getVirNum(std::string serName);
    ///获取所有节点数量
    int getAllNodeNum();
    ///打印节点（供测试使用）
    std::string showTime();
    ///统计环上所有节点分布情况（供测试使用）
    std::string statDist();
private:
    bool addRealNode(Node *node);
    bool addVirNode(Node *node);
    bool delRealNode(Node *node);
    bool delVirNode(Node *node);
    ///删除realNode中的节点 并 返回这个节点
    Node *findRealNode(std::string nodeName);


private:
    std::multimap<unsigned int, std::string>nodes; ///用multimap来处理hash冲突
    std::list<Node *>realNode;
    HashFunc *hashFunc;
};


#endif //MYHTTPSERVER_CONSISTENTHASH_H
