//! Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5.
//! See LICENSE for details.

#pragma once
#include <IOKit/IOService.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>
#include "AMDPlatformPluginInterface"

class AMDPlatformPluginACPICPCInterface : public AMDPlatformPluginInterface {
	OSDeclareDefaultStructors(AMDPlatformPluginACPICPCInterface);
	
	virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;

	virtual IOReturn handleMonitorRequest(MonitorRequestData *req) APPLE_KEXT_OVERRIDE;
	virtual const char *getInterfaceReporterName() APPLE_KEXT_OVERRIDE;
	
private:
	IOACPIPlatformDevice *cpuHandle;
	char name[128];
};
