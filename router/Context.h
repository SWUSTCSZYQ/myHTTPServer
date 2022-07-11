//
// Created by zyq on 7/8/22.
//

#ifndef MYHTTPSERVER_CONTEXT_H
#define MYHTTPSERVER_CONTEXT_H

#include <string>
#include <unordered_map>
#include <utility>



class Context {
public:
    Context(int connFd_, std::string& method_, std::string& path_, int statesCode_);
    ~Context() = default;
    void setFd(int fd_){connFd = fd_;}
    int getFd() const{return connFd;}
    std::string& getPath(){return path;}
    std::string& getMethod(){return method;}
    void setParams(std::unordered_map<std::string, std::string>params_){params = std::move(params_);}
    std::unordered_map<std::string, std::string>& getParams(){return params;}
    void String(int code);

private:
    std::string method;
    std::string path;
    int statesCode;
    int connFd;
    std::unordered_map<std::string, std::string>params;
};


#endif //MYHTTPSERVER_CONTEXT_H
