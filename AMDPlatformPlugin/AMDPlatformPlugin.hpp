//  Copyright © 2022-2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#pragma once
#include <IOKit/IOService.h>
#include "../IOPlatformPluginFamily/IOPlatformPluginFamilyPriv.hpp" // will change on final release

class AMDPlatformPlugin : public IOPlatformPluginFamilyPriv {
	virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
};
