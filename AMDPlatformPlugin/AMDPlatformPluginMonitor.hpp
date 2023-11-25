//! Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5.
//! See LICENSE for details.

#pragma once
#include "AMDPlatformPlugin.hpp"
#include <IOKit/pwr_mgt/IOPMPowerSource.h>

struct AMDMonitorStatistics {
	bool isAcConnected;
	UInt32 temp;
	UInt32 voltage;
};

class AMDPlatformPluginMonitor : public OSObject {
	OSDeclareDefaultStructors(AMDPlatformPluginMonitor);
	
	virtual bool init() APPLE_KEXT_OVERRIDE;
	
	static AMDPlatformPluginMonitor *withPlatformPlugin(AMDPlatformPlugin *pp);
	
	AMDMonitorStatistics *getStatistics();
	
	void setupTimers();
	
	AMDMonitorStatistics stats;
};
