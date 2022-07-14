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
    explicit Cache(int cap);
    ~Cache();

    void add(std::string key, std::string value);
    std::string get(std::string);
private:
    int cacheBytes;
    LeastRecentlyUsed<std::string, std::string>lru;
    MutexLock mutex_;
};


#endif //MYHTTPSERVER_CACHE_H
