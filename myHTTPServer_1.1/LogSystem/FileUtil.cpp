//
// Created by zyq on 7/2/22.
//

#include "FileUtil.h"

AppendFile::AppendFile(std::string fileName) :fp(fopen(fileName.c_str(), "ae"))
{
    /// 本地缓冲区
    setbuffer(fp, buffer_, sizeof buffer_);

}

AppendFile::~AppendFile()
{
    fclose(fp);
}

void AppendFile::append(const char *logline, const size_t len)
{
    size_t n = this->write(logline, len);
    size_t remain = len - n;
    while(remain > 0)
    {
        size_t x = this->write(logline + n, remain);
        if(x == 0)
        {
            int err = ferror(fp);
            if(err)fprintf(stderr, "AppendFile::append() failed !\n");
            break;
        }
        n += x;
        remain = len - x;
    }
}

void AppendFile::flush()
{
    fflush(fp);
}

size_t AppendFile::write(const char *logline, size_t len)
{
    //无锁版
    return fwrite_unlocked(logline, 1, len, fp);
}
