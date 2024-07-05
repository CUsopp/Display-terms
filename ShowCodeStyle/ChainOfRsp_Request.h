#pragma once
#include <iostream>  
#include <memory>  

// 抽象请求  
class Request {
public:
    virtual ~Request() {}
    virtual void handle() = 0;
};

//// 抽象处理者  
//class Handler {
//public:
//    virtual ~Handler() {}
//    virtual void setNext(std::shared_ptr<Handler> next) = 0;
//    virtual void handleRequest(std::shared_ptr<Request> request) = 0;
//};

//// 具体处理者A  
//class ConcreteHandlerA : public Handler {
//private:
//    std::shared_ptr<Handler> nextHandler;
//public:
//    void setNext(std::shared_ptr<Handler> next) override {
//        nextHandler = next;
//    }
//    void handleRequest(std::shared_ptr<Request> request) override {
//        if (canHandle(request)) {
//            request->handle();
//        }
//        else if (nextHandler) {
//            nextHandler->handleRequest(request);
//        }
//    }
//    bool canHandle(std::shared_ptr<Request> request) {
//        // 根据实际情况判断是否能处理请求  
//        return true; // 假设A能处理所有请求  
//    }
//};

//// 具体处理者B  
//class ConcreteHandlerB : public Handler {
//private:
//    std::shared_ptr<Handler> nextHandler;
//public:
//    void setNext(std::shared_ptr<Handler> next) override {
//        nextHandler = next;
//    }
//    void handleRequest(std::shared_ptr<Request> request) override {
//        if (canHandle(request)) {
//            request->handle();
//        }
//        else if (nextHandler) {
//            nextHandler->handleRequest(request);
//        }
//    }
//    bool canHandle(std::shared_ptr<Request> request) {
//        // 根据实际情况判断是否能处理请求  
//        return false; // 假设B不能处理任何请求  
//    }
//};

//// 具体请求  
//class SpecificRequest : public Request {
//public:
//    void handle() override {
//        std::cout << "SpecificRequest handled." << std::endl;
//    }
//};

//int main() {
//    std::shared_ptr<Handler> handlerA = std::make_shared<ConcreteHandlerA>();
//    std::shared_ptr<Handler> handlerB = std::make_shared<ConcreteHandlerB>();
//    handlerA->setNext(handlerB);
//
//    std::shared_ptr<Request> request = std::make_shared<SpecificRequest>();
//    handlerA->handleRequest(request); // 请求从handlerA开始传递  
//
//    return 0;
//}