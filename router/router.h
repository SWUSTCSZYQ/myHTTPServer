//
// Created by zyq on 7/7/22.
//

#ifndef MYHTTPSERVER_ROUTER_H
#define MYHTTPSERVER_ROUTER_H

#include <unordered_map>
#include <vector>
#include <string>
#include "Trie.h"
#include "context.h"

class router {
public:
    typedef void(*handler)(context*);
    using umpss = std::unordered_map<std::string, std::string>;
public:
    router() = default;
    void addRoute(std::string& method, std::string& pattern, handler HandlerFunc);
    std::pair<Trie*, umpss> getRoute(std::string& method, std::string& path);
    void handle(context* c);
private:
    std::vector<std::string> parsePattern(std::string& pattern);
    std::string join(std::vector<std::string>arr, std::string str);
    std::vector<std::string>split(std::string s, char ch);

private:
    std::unordered_map<std::string, Trie*>roots;
    std::unordered_map<std::string, handler>handlers;
};


#endif //MYHTTPSERVER_ROUTER_H
