//
// Created by zyq on 7/14/22.
//

#include "CacheControl.h"
#include "iostream"

Group::Group(std::string name, int cacheBytes, Callback func)
    :name_(name),
     cache_(cacheBytes),
     callback_(func)
{}

std::string Group::get(std::string key) {
    if(key == "")
    {
        std::cout << "key is required" << std::endl;
        return "key is required";
    }
    std::string value = cache_.get(key);
    if(value != "")
    {
        //缓存命中
        std::cout << "缓存命中 : " << value << std::endl;
        return value;
    }

    return load(key);
}

std::string Group::load(std::string key) {
    return getLocally(key);
}

std::string Group::getLocally(std::string key) {
    std::string bytes = callback_(key);
    if(bytes == "")
    {
        std::cout << "查找失败" << std::endl;
        return "";
    }
    populateCache(key, bytes);
    return bytes;
}

void Group::populateCache(std::string key, std::string value) {
    cache_.add(key, value);
}

void CacheControl::addGroup(std::string name, int cacheBytes, Callback func) {
    mutex_.lock();
    groups[name] = new Group(name, cacheBytes, func);
    mutex_.unlock();
}

Group *CacheControl::getGroup(std::string name) {
    Group * group = nullptr;
    mutex_.lock();
    if(groups.count(name))group = groups[name];
    mutex_.unlock();
    return group;

}

CacheControl::CacheControl() :
    groups(),
    mutex_()
{

}

CacheControl::~CacheControl() {
    mutex_.lock();
    for(auto iter = groups.begin(); iter != groups.end(); ++iter)
    {
        delete iter->second;
    }
    mutex_.unlock();
}
