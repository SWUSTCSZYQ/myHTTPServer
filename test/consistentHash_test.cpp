//
// Created by zyq on 7/16/22.
//

#include "ConsistentHash.h"
#include "iostream"
using namespace std;

int main()
{
    ConsistentHash *servers = new ConsistentHash();
    servers->addNode("1.4.4.255:80", 10);
    servers->addNode("1.5.4.255:80", 16);
    servers->addNode("1.7.4.255:80", 2);
    servers->addNode("1.8.4.255:80", 5);
    servers->addNode("1.114.4.255:80", 15);
    servers->addNode("1.4.123.255:80", 10);
    servers->addNode("1.41.4.255:80", 20);
    servers->addNode("1.4.45.255:80", 50);
    cout << servers->showTime() << endl;
    cout << servers->statDist() << endl;
}