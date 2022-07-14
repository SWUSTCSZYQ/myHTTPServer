//
// Created by zyq on 7/14/22.
//

#ifndef MYHTTPSERVER_GETTER_H
#define MYHTTPSERVER_GETTER_H

#include <string>

//如果缓存不存在，应从数据源（文件，数据库等）获取数据并添加到缓存中。
//设计了一个回调函数(callback)，在缓存不存在时，调用这个函数，得到源数据。
class Getter {
public:
    std::string get();
};


#endif //MYHTTPSERVER_GETTER_H
