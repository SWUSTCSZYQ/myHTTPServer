//
// Created by zyq on 7/16/22.
//

#ifndef MYHTTPSERVER_MD5HASHFUNC_H
#define MYHTTPSERVER_MD5HASHFUNC_H

#include "HashFunc.h"

class MD5HashFunc: public HashFunc{
public:
    virtual unsigned int GetKey(std::string str);
};


#endif //MYHTTPSERVER_MD5HASHFUNC_H
