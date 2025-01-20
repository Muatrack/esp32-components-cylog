#pragma once

#include <iostream>
#include "errno.hpp"
#include "common.hpp"
#include "cylog_store.hpp"


class StoreLinux : public StoreAbs {

public:
    // StoreLinux(uint8_t fMaxCount, uint8_t fMaxLen, std::shared_ptr<std::string> &fDir, std::shared_ptr<std::string> &fPrefix):
    //                                                                             StoreAbs(fMaxCount, fMaxLen, fDir, fPrefix) {}

    void configSet(uint8_t fMaxCount, uint32_t fMaxLen, const std::string &fDir, const std::string &fPrefix) override;

    ~StoreLinux() {
        std::cout << "~StoreLinux()" << std::endl;
    }

    CL_TYPE_t init() override;
};
