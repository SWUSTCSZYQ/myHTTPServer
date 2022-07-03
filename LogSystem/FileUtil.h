//
// Created by zyq on 7/2/22.
//

#ifndef MYHTTPSERVER_FILEUTIL_H
#define MYHTTPSERVER_FILEUTIL_H

#include <string>
#include "nonCopyable.h"

class AppendFile : nonCopyable{
public:
    explicit AppendFile(std::string fileName);
    ~AppendFile();

    void append(const char* logline, const size_t len);
    void flush();
private:
    size_t write(const char* logline, size_t len);
    FILE *fp;
    char buffer_[64 * 1024];
};


#endif //MYHTTPSERVER_FILEUTIL_H
