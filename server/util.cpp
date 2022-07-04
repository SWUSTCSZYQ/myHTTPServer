//
// Created by zyq on 7/3/22.
//

#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <csignal>
#include <cerrno>
#include <cstring>

const int MAX_BUFF = 4096;
ssize_t readn(int fd, void*buff, size_t n)
{
    size_t nLeft = n;
    ssize_t nRead = 0, readSum = 0;
    char *ptr = (char*)buff;
    while(nLeft > 0)
    {
        if((nRead = read(fd, ptr, nLeft)) < 0)
        {
            if(errno == EINTR)
                nRead = 0;
            else if(errno == EAGAIN)
                return readSum;
            else return -1;
        }
        else if(nRead == 0)break;
        readSum += nRead;
        nLeft -= nRead;
        ptr += nRead;
    }
    return readSum;
}

ssize_t readn(int fd, std::string &inBuffer)
{
    ssize_t nRead = 0, readSum = 0;
    while(true)
    {
        char buff[MAX_BUFF];
        if((nRead = read(fd, buff, MAX_BUFF)) < 0)
        {
            if(errno == EINTR)continue;
            else if(errno == EAGAIN)return readSum;
            else
            {
                perror("read error");
                return -1;
            }
        }
        else if(nRead == 0)break;
        readSum += nRead;
        inBuffer += std::string(buff, buff + nRead);
    }
    return readSum;
}

ssize_t writen(int fd, void *buff, size_t n)
{
    size_t nLeft = n;
    ssize_t nWritten = 0, writeSum = 0;
    char *ptr = (char*)buff;
    while(nLeft > 0)
    {
        if((nWritten = write(fd, ptr, nLeft)) <= 0)
        {
            if(errno == EINTR)
            {
                nWritten = 0;
                continue;
            }
            else if(errno == EAGAIN)
                return writeSum;
            else return -1;
        }
        writeSum += nWritten;
        nLeft -= nWritten;
        ptr += nWritten;
    }
    return writeSum;
}

ssize_t writen(int fd, std::string &sBuff)
{
    size_t nLeft = sBuff.length();
    const char *ptr = sBuff.c_str();
    ssize_t writeSum = write(fd, ptr, nLeft);
    if(writeSum == sBuff.length())sBuff.clear();
    else
        sBuff = sBuff.substr(writeSum);
    return writeSum;
}

void handle_for_sigpipe()
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE,&sa, nullptr))return;
}

int setSocketNonBlocking(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    if(flag == -1)return -1;
    flag |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flag) == -1)return -1;
    return 0;
}