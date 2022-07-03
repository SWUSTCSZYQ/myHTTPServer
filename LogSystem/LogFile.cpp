//
// Created by zyq on 7/2/22.
//

#include "LogFile.h"

LogFile::LogFile(const std::string &baseName, int flushEveryN) :
        baseName_(baseName),
        flushEveryN_(flushEveryN),
        count_(0),
        mutex_(new MutexLock)
{
    file_.reset(new AppendFile(baseName));
}

LogFile::~LogFile() {}

void LogFile::append(const char *logline, int len) {
    MutexLockGuard lock(*mutex_);
    append_unlocked(logline, len);

}

void LogFile::flush() {
    MutexLockGuard lock(*mutex_);
    file_->flush();
}

void LogFile::append_unlocked(const char *logline, int len) {
    file_->append(logline, len);
    ++count_;
    if(count_ >= flushEveryN_)
    {
        count_ = 0;
        file_->flush();
    }
}
