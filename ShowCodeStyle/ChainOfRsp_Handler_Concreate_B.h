#pragma once
#include "ChainOfRsp_Handler.h"

// ���崦����B  
class ConcreteHandlerB : public Handler {
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
        // ����ʵ������ж��Ƿ��ܴ�������  
        return false; // ����B���ܴ����κ�����  
    }
};