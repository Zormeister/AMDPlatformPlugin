//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#pragma once
#include <IOKit/IODeviceMemory.h>
#include <libkern/c++/OSObject.h>

struct PMTable {
    uint32_t pmTableVersion;
    uint32_t dramMapSize;
    uint32_t dramMapSize2;
    IOMemoryMap *dramMap;
    IOMemoryMap *dramMap2;
    uint32_t *dramMapPtr;
    uint32_t *dramMapPtr2;
};

class AMDPMTableHandler : public OSObject {
    OSDeclareDefaultStructors(AMDPMTableHandler);

    public:
    bool init() APPLE_KEXT_OVERRIDE;
    void dumpPmTableState();
    uint32_t getPmTableVersion();
    uint32_t sendPmTableToDram();
    uint32_t getPmTableSize();
    uint32_t setupPmTableServices();
    static AMDPMTableHandler *createPMTableHandler();

    static AMDPMTableHandler *callback;
    uint64_t dramBaseAddr;
    uint64_t dramBaseAddr2;
    PMTable pmTbl;
};
