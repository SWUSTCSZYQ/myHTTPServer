//
// Created by zyq on 7/17/22.
//

#ifndef MYHTTPSERVER_HTTPGETTER_H
#define MYHTTPSERVER_HTTPGETTER_H

#include "peers.h"

class HttpGetter: public PeerGetter{
public:
    explicit HttpGetter(std::string& url);
    ~HttpGetter() = default;
    std::string get(std::string group, std::string key) override;

private:
    std::string baseURL; ///表示将要访问的远程节点的地址
};


#endif //MYHTTPSERVER_HTTPGETTER_H
