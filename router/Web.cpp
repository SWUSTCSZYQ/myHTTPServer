//
// Created by zyq on 7/7/22.
//

#include "Web.h"

Web::Web() :router_(new Router())
{}

Web::~Web()
{
    delete router_;
}

void Web::GET(std::string path, Web::handler handlerFunction)
{
    addRoute("GET", path, handlerFunction);
}

void Web::POST(std::string path, Web::handler handlerFunction)
{
    addRoute("POST", path, handlerFunction);
}

void Web::addRoute(std::string method, std::string path, Web::handler handleFunction)
{
    router_->addRoute(method, path, handleFunction);
}

void Web::run(Context* context) {
    router_->handle(context);
}



