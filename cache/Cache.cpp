//
// Created by zyq on 7/13/22.
//

#include "Cache.h"


Cache::~Cache() {

}

Cache::Cache(int cap)
    :cacheBytes(cap),
    lru(cacheBytes),
    mutex_()
{}

void Cache::add(std::string key, std::string value) {
    mutex_.lock();
    lru.put(key, value);
    mutex_.unlock();
}

std::string Cache::get(std::string key) {

    mutex_.lock();
    std::string value = lru.get(key);
    mutex_.unlock();
    return value;
}
