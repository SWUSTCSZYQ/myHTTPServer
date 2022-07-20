//
// Created by zyq on 7/2/22.
//

#ifndef MYHTTPSERVER_LOGFILE_H
#define MYHTTPSERVER_LOGFILE_H

#include <memory>
#include <string>
#include "FileUtil.h"
#include "MutexLock.h"
#include "nonCopyable.h"

class LogFile : nonCopyable{
public:
    // 每被append flushEveryN次，flush一下，会往文件写，只不过，文件也是带缓冲区的
    LogFile(const std::string& baseName, int flushEveryN = 1024);
    ~LogFile();

    void append(const char* logline, int len);
    void flush();
    //bool rollFile();

private:
    void append_unlocked(const char* logline, int len);
    const std::string baseName_;
    const int flushEveryN_;

    int count_;
    std::unique_ptr<MutexLock> mutex_;
    std::unique_ptr<AppendFile> file_;
};


#endif //MYHTTPSERVER_LOGFILE_H
