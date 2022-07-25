//
// Created by zyq on 7/23/22.
//

#ifndef MYHTTPSERVER_1_1_CONTEXT_H
#define MYHTTPSERVER_1_1_CONTEXT_H


#include <string>
#include <unordered_map>
#include <utility>

class Context {
public:
    Context(std::string& method_, std::string& path_, int statesCode_);
    ~Context() = default;
    std::string& getPath(){return path;}
    std::string& getMethod(){return method;}
    void setParams(std::unordered_map<std::string, std::string>params_){params = std::move(params_);}
    std::unordered_map<std::string, std::string>& getParams(){return params;}
    void String(int code);
    const std::string& getOutBuffer(){return outBuffer;}

private:
    std::string method;
    std::string path;
    int statesCode;
    std::unordered_map<std::string, std::string>params;
    std::string outBuffer;
};


#endif //MYHTTPSERVER_1_1_CONTEXT_H
