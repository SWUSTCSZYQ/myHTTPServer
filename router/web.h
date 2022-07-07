//
// Created by zyq on 7/7/22.
//

#ifndef MYHTTPSERVER_WEB_H
#define MYHTTPSERVER_WEB_H

#include<string>

class web {
public:
    typedef void(*handler)(void);
public:
    void init();
    void GET(std::string, handler);
    void POST(std::string, handler);

private:
    void addRoute(std::string type, std::string handler, handler);
};


#endif //MYHTTPSERVER_WEB_H
