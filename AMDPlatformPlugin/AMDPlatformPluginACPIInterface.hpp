//! Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5.
//! See LICENSE for details.

#pragma once
#include <IOKit/IOService.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>

class AMDPlatformPluginACPIInterface : public IOService {
	OSDeclareDefaultStructors(AMDPlatformPluginACPIInterface);
	
	virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
	
private:
	IOACPIPlatformDevice *cpu;
};
