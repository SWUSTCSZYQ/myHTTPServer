//
// Created by zyq on 7/7/22.
//

#ifndef MYHTTPSERVER_ROUTER_H
#define MYHTTPSERVER_ROUTER_H

#include <unordered_map>
#include <vector>
#include <string>
#include "Trie.h"
#include "Context.h"

class Router {
public:
    typedef void(*handler)(Context*);
    using umpss = std::unordered_map<std::string, std::string>;
public:
    Router() = default;
    void addRoute(std::string& method, std::string& pattern, handler HandlerFunc);
    std::pair<Trie*, umpss> getRoute(std::string& method, std::string& path);
    void handle(Context* c);
private:
    std::vector<std::string> parsePattern(std::string& pattern);
    std::string join(std::vector<std::string>arr, std::string str);
    std::vector<std::string>split(std::string s, char ch);

private:
    std::unordered_map<std::string, Trie*>roots;
    std::unordered_map<std::string, handler>handlers;
};


#endif //MYHTTPSERVER_ROUTER_H
