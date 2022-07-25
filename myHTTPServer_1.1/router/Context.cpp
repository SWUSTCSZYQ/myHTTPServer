//
// Created by zyq on 7/23/22.
//

#include "Context.h"
#include "../utils.h"

void Context::String(int code) {
    switch (code) {
        case 404:
        {
            std::string res = "404 NOT FOUND: " + path + "\n";
            outBuffer += res;
        }
    }
}

Context::Context(std::string &method_, std::string &path_, int statesCode_)
        :method(method_),
        path(method_),
        statesCode(statesCode_),
        outBuffer()
{}