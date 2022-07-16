//
// Created by zyq on 7/16/22.
//

#ifndef MYHTTPSERVER_HASHFUNC_H
#define MYHTTPSERVER_HASHFUNC_H

#include <string>

class HashFunc
{
public:
    virtual unsigned int GetKey(std::string str) = 0;
};


#endif //MYHTTPSERVER_HASHFUNC_H
