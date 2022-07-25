//
// Created by zyq on 7/24/22.
//

#ifndef MYHTTPSERVER_1_1_ROUTERNODE_H
#define MYHTTPSERVER_1_1_ROUTERNODE_H

#include <unordered_map>
#include <vector>
#include <string>
#include "Trie.h"
#include "Context.h"


class RouterNode {
public:
    typedef void(*handler)(Context*);
    using umpss = std::unordered_map<std::string, std::string>;

public:
    RouterNode() = default;
    void addRouterNode(std::string& method, std::string& pattern, handler HandlerFunc);
    std::pair<Trie*, umpss> getRouterNode(std::string& method, std::string& path);
    void handle(Context* c);

private:
    static std::vector<std::string> parsePattern(std::string& pattern);
    static std::string join(std::vector<std::string>arr, std::string str);
    static std::vector<std::string>split(std::string s, char ch);

private:
    std::unordered_map<std::string, Trie*>roots;
    std::unordered_map<std::string, handler>handlers;
};


#endif //MYHTTPSERVER_1_1_ROUTERNODE_H
