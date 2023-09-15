//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#include <IOKit/IOService.h>

struct AMDPPTSStruct {
	uint32_t timeBeforeRecheck;
};

class AMDPlatformPluginTemperatureServices : public OSObject {
    OSDeclareDefaultStructors(AMDPlatformPluginTemperatureServices);

    virtual bool init() APPLE_KEXT_OVERRIDE;
    void dumpServicesState();

    public:
	static AMDPlatformPluginTemperatureServices *createTemperatureServices();
};

