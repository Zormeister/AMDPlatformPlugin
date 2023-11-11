//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#include "AMDPlatformPluginCPUStateServices.hpp"

OSObject *AMDPlatformPluginCPUStateServices::grabCPPCTable() {
    OSObject *cpc;
    if (!cpu->validateObject("_CPC")) {
        if (!cpu->evaluateObject("_CPC", &cpc, nullptr, 0, 0)) { return cpc; }
    }
    return nullptr;
}
