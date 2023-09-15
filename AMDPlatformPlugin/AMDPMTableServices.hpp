//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#pragma once
#include <IOKit/IODeviceMemory.h>
#include <libkern/c++/OSObject.h>

class AMDPlatformPluginSMUServices;
class AMDPlatformPlugin;

struct PMTable {
    uint32_t pmTableVersion;
    uint32_t dramMapSize;
    uint32_t dramMapSize2;
    IOMemoryMap *dramMap;
    IOMemoryMap *dramMap2;
    uint32_t *dramMapPtr;
    uint32_t *dramMapPtr2;
};

class AMDPMTableServices : public OSObject {
    OSDeclareDefaultStructors(AMDPMTableServices);

    public:
    bool init() APPLE_KEXT_OVERRIDE;
    void dumpPmTableState();
    uint32_t getPmTableVersion();
    uint32_t sendPmTableToDram();
    uint32_t getPmTableSize();
    uint32_t setupPmTableServices();
    static AMDPMTableServices *createPMTableServices();

    uint64_t dramBaseAddr;
    uint64_t dramBaseAddr2;
    PMTable pmTbl;
	AMDPlatformPlugin *amdpp;
	AMDPlatformPluginSMUServices *smuServices;
};
