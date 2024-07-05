#pragma once
#include "ChainOfRsp_Handler.h"

// 具体处理者A  
class ConcreteHandlerA : public Handler {
private:
    std::shared_ptr<Handler> nextHandler;
public:
    void setNext(std::shared_ptr<Handler> next) override {
        nextHandler = next;
    }

    void handleRequest(std::shared_ptr<Request> request) override {
        if (canHandle(request)) {
            request->handle();
        }
        else if (nextHandler) {
            nextHandler->handleRequest(request);
        }
    }
    bool canHandle(std::shared_ptr<Request> request) {
        // 根据实际情况判断是否能处理请求  
        return true; // 假设A能处理所有请求  
    }
};