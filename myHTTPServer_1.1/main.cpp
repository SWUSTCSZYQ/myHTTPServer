#include <iostream>
#include "Server.h"

int main() {
    Server *server = Server::getInstance(8888);
    server->run();
    return 0;
}
