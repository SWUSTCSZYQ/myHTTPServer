//
// Created by zyq on 7/19/22.
//

#ifndef MYHTTPSERVER_1_1_UTILS_H
#define MYHTTPSERVER_1_1_UTILS_H

#include <cstdlib>
#include <string>

int setSocketNonBlocking(int fd);
void handle_for_sigpipe();

ssize_t readn(int fd, void *buff, size_t n);
ssize_t readn(int fd, std::string &inBuffer);
ssize_t writen(int fd, void *buff, size_t n);
ssize_t writen(int fd, std::string &sbuff);

#endif //MYHTTPSERVER_1_1_UTILS_H
