#pragma once
#include <iostream>  
#include <memory>  

// ��������  
class Request {
public:
    virtual ~Request() {}
    virtual void handle() = 0;
};

//// ��������  
//class Handler {
//public:
//    virtual ~Handler() {}
//    virtual void setNext(std::shared_ptr<Handler> next) = 0;
//    virtual void handleRequest(std::shared_ptr<Request> request) = 0;
//};

//// ���崦����A  
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
//        // ����ʵ������ж��Ƿ��ܴ�������  
//        return true; // ����A�ܴ�����������  
//    }
//};

//// ���崦����B  
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
//        // ����ʵ������ж��Ƿ��ܴ�������  
//        return false; // ����B���ܴ����κ�����  
//    }
//};

//// ��������  
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
//    handlerA->handleRequest(request); // �����handlerA��ʼ����  
//
//    return 0;
//}