//
// Created by zyq on 7/3/22.
//

#ifndef MYHTTPSERVER_UTIL_H
#define MYHTTPSERVER_UTIL_H

#include <cstdlib>
#include <string>

ssize_t readn(int fd, void *buff, size_t n);
ssize_t readn(int fd, std::string &inBuffer);
ssize_t writen(int fd, void *buff, size_t n);
ssize_t writen(int fd, std::string &sBuff);
void handle_for_sigpipe();
int setSocketNonBlocking(int fd);


#endif //MYHTTPSERVER_UTIL_H
