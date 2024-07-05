#pragma once
#include "ChainOfRsp_Request.h"

// ��������  
class Handler {
public:
    virtual ~Handler() {}
    virtual void setNext(std::shared_ptr<Handler> next) = 0;
    virtual void handleRequest(std::shared_ptr<Request> request) = 0;
};