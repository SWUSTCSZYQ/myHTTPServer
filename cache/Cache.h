//
// Created by zyq on 7/13/22.
//

#ifndef MYHTTPSERVER_CACHE_H
#define MYHTTPSERVER_CACHE_H

#include "LeastRecentlyUsed.h"
#include "string"
#include "MutexLock.h"

class Cache {
public:
    Cache() = default;
    ~Cache();
private:
    LeastRecentlyUsed<std::string, std::string>lru;
    MutexLock mutex_;
};


#endif //MYHTTPSERVER_CACHE_H
