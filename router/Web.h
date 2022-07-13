//
// Created by zyq on 7/7/22.
//

#ifndef MYHTTPSERVER_WEB_H
#define MYHTTPSERVER_WEB_H

#include<string>
#include "Router.h"
#include "Context.h"

class Web {
public:
    typedef void(*handler)(Context*);
public:
    Web();
    ~Web();
    void GET(std::string, handler);
    void POST(std::string, handler);
    void run(Context* context);

private:
    void addRoute(std::string method, std::string path, handler handleFunction);

private:
    Router * router_;
};


#endif //MYHTTPSERVER_WEB_H
