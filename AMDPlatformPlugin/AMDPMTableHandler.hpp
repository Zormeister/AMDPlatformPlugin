//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#pragma once
#include <libkern/c++/OSObject.h>

struct PMTable {
	uint32_t pmTableVersion;
};

class AMDPMTableHandler : public OSObject {
	OSDeclareDefaultStructors(AMDPMTableHandler);
	public:
	bool init() APPLE_KEXT_OVERRIDE;
	uint32_t getPmTableVersion();
	bool setupPmTableServices();
	static AMDPMTableHandler *createPMTableHandler();
	
	static AMDPMTableHandler *callback;
};
