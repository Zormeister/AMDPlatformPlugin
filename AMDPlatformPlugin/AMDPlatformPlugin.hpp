//  Copyright © 2022-2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#pragma once
#include <IOKit/IOService.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>
#include "../IOPlatformPluginFamily/IOPlatformPluginFamilyPriv.hpp" // will change on final release

enum AMDEPPMode : uint32_t {
	Performance = 0,
	BalancePerformance,
	BalancePowerSavings,
	PowerSavings,
};

class AMDPlatformPlugin : public IOPlatformPluginFamilyPriv {
	virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
	void log(uint32_t logLevel, const char *fmt, ...);
};
