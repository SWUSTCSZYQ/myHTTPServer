//
// Created by zyq on 7/8/22.
//

#include "Context.h"
#include "util.h"

void Context::String(int code) {
    switch (code) {
        case 404:
        {
            std::string res = "404 NOT FOUND: " + path + "\n";
            writen(connFd, res);
        }
    }
}

Context::Context(int connFd_, std::string &method_, std::string &path_, int statesCode_)
    :connFd(connFd_), method(method_), path(method_), statesCode(statesCode_)
{}
