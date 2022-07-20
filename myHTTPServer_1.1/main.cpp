#include <iostream>
#include "Server.h"

int main() {
    //std::cout << "Hello, World!" << std::endl;
    Server *server = Server::getInstance(8888);
    server->run();
    return 0;
}
