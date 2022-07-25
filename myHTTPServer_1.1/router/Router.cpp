//
// Created by zyq on 7/23/22.
//

#include "Router.h"

Router* Router::instance = nullptr;

Router::Router() : routerNode_(new RouterNode()){

}

Router::~Router() {
    delete routerNode_;
    delete instance;
}

void Router::GET(std::string path, Router::handler handlerFunction) {
    addRoute("GET", path, handlerFunction);
}

void Router::POST(std::string path, Router::handler handlerFunction) {
    addRoute("POST", path, handlerFunction);
}

void Router::run(Context *context) {
    routerNode_->handle(context);
}

void Router::addRoute(std::string method, std::string path, Router::handler handleFunction) {
    routerNode_->addRouterNode(method, path, handleFunction);
}

Router *Router::getInstance() {
    if(instance == nullptr)
    {
        instance = new Router();
    }
    return instance;
}
