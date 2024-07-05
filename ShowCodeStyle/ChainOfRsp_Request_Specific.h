#pragma once
#include "ChainOfRsp_Request.h"

// æﬂÃÂ«Î«Û  
class SpecificRequest : public Request {
public:
    void handle() override {
        std::cout << "SpecificRequest handled." << std::endl;
    }
};