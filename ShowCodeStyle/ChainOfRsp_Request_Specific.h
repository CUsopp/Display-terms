#pragma once
#include "ChainOfRsp_Request.h"

// ��������  
class SpecificRequest : public Request {
public:
    void handle() override {
        std::cout << "SpecificRequest handled." << std::endl;
    }
};