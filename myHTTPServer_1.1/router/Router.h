//
// Created by zyq on 7/23/22.
//

#ifndef MYHTTPSERVER_1_1_ROUTER_H
#define MYHTTPSERVER_1_1_ROUTER_H

#include "Context.h"
#include "RouterNode.h"
#include <string>

class Router {
public:
    typedef void(*handler)(Context*);

public:
    static Router* getInstance();
    void GET(std::string, handler);
    void POST(std::string, handler);
    void run(Context* context);

private:
    Router();
    ~Router();
    void addRoute(std::string method, std::string path, handler handleFunction);

private:
    static Router* instance;
    RouterNode* routerNode_;
};


#endif //MYHTTPSERVER_1_1_ROUTER_H
