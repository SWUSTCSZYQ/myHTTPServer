//
// Created by zyq on 7/16/22.
//

#include "ConsistentHash.h"

ConsistentHash::ConsistentHash(HashFunc *func) {
    if(func)
    {
        hashFunc = func;
    }
    else hashFunc = new MD5HashFunc();
}

ConsistentHash::~ConsistentHash() {
    for(auto it = realNode.begin(); it != realNode.end();)
    {
        delete *it;
        realNode.erase(it++);
    }
}

bool ConsistentHash::addNode(std::string nodeName, int vNum) {
    Node *node = new Node(nodeName, vNum);
    if(!addVirNode(node))return false;
    if(!addRealNode(node))return false;
    return true;
}

bool ConsistentHash::delNode(std::string nodeName) {
    Node *node = findRealNode(nodeName);
    if(!delRealNode(node))return false;
    if(!delVirNode(node))return false;
    delete node;
    return true;
}

std::string ConsistentHash::getServeName(std::string cliName) {
    ///根据客户端的ip找到可以提供服务的节点
    std::string serName;
    int cliKey = hashFunc->GetKey(cliName);
    auto iter = nodes.begin();
    ///找到第一个比cliKey大的节点
    while(iter != nodes.end())
    {
        if(cliKey < iter->first)
        {
            serName = iter->second;
            break;
        }
        ++iter;
    }
    if(iter == nodes.end())
        serName = nodes.begin()->second;
    return serName;
}

int ConsistentHash::getRealNum() {
    return realNode.size();
}

int ConsistentHash::getVirNum(std::string serName) {
    Node *node = nullptr;
    for(auto iter = realNode.begin(); iter != realNode.end(); ++iter)
    {
        if((*iter)->GetNodeName() == serName)
            node = *iter;
    }
    return node ? node->GetVNodeNum() : -1;
}

int ConsistentHash::getAllNodeNum() {
    return realNode.size() + nodes.size();
}

std::string ConsistentHash::showTime() {
    std::ostringstream oss;
    for (auto it = nodes.begin(); it != nodes.end(); ++it){
        oss << "key : " << it->first << "\tval : " << it->second << "\n";
    }
    return oss.str();
}

std::string ConsistentHash::statDist() {
    //max = 2^32 = 4294967296
    //所以分8个区 500000000 1000000000 1500000000 ---
    int i = 1;
    unsigned int sum = 0;
    std::ostringstream oss;
    for (auto it = nodes.begin(); it != nodes.end(); ++it){
        if(it->first / 500000000  < i){
            ++sum;
        }
        else{
            oss<< "hash( " << 5 * (i - 1) <<", "<< 5 * i <<" )million\tnodeNum = "<< sum <<"\n";
            ++i;
            sum = 0;
        }
    }
    return oss.str();
}

bool ConsistentHash::addRealNode(Node *node) {
    if(!node)return false;
    std::string ip = node->GetNodeName();
    unsigned int key = hashFunc->GetKey(ip);
    realNode.push_back(node);
    nodes.insert(std::make_pair(key, ip));
    return true;
}

bool ConsistentHash::addVirNode(Node *node) {
    if(!node)
    {
        //todo: LOG << addVirNode  node为空
        return false;
    }
    int vNum = node->GetVNodeNum();
    if(vNum < 1)
    {
        //todo:LOG << addVirNode  虚拟节点个数<1
        return false;
    }
    unsigned int key;
    std::string virName;                            ///虚拟节点名称
    std::string nodeName = node->GetNodeName();     ///真实节点名
    std::stringstream ss;
    ///vNode添加后缀
    for(int i = 0; i < vNum; ++i)
    {
        ss << nodeName << "##" << i + 1;
        ss >> virName;
        ss.clear();
        key = hashFunc->GetKey(virName);
        nodes.insert(std::make_pair(key, nodeName));//存的都是真实的名称
    }
    return true;
}

bool ConsistentHash::delRealNode(Node *node) {
    if(!node)
    {
        //todo: LOG << delRealNode node为空;
        return false;
    }
    std::string nodeName = node->GetNodeName();
    ///在multimap中删除真实节点
    unsigned int key = hashFunc->GetKey(nodeName);
    int count = nodes.count(key);
    auto delIter = nodes.find(key);
    while(delIter != nodes.end() and count--)
    {
        if(delIter->second == nodeName)
        {
            nodes.erase(delIter);
            break;
        }
        ++delIter;
    }
    return true;
}

bool ConsistentHash::delVirNode(Node *node) {
    if(!node)
    {
        //todo: LOG << delVirNode node为空;
        return false;
    }
    int vNum = node->GetVNodeNum(), count;
    unsigned int key;
    std::string virName, nodeName = node->GetNodeName();
    std::stringstream ss;
    for(int i = 0; i < vNum; ++i)
    {
        ss << nodeName << "##" << i + 1;
        ss >> virName;
        ss.clear();
        key = hashFunc->GetKey(virName);
        count = nodes.count(key);
        auto delIter = nodes.find(key);
        while(delIter != nodes.end() and count--)
        {
            if(delIter->second == nodeName)
            {
                nodes.erase(delIter++);
            }
            else ++delIter;
        }
    }
    return true;
}

Node *ConsistentHash::findRealNode(std::string nodeName) {
    Node * node = nullptr;
    for(auto iter = realNode.begin(); iter != realNode.end(); ++iter)
    {
        if((*iter)->GetNodeName() == nodeName)
        {
            node = *iter;
            realNode.erase(iter);
            break;
        }
    }
    if(!node)
    {
        //todo:LOG << list没找到真实节点;
    }
    return node;
}
