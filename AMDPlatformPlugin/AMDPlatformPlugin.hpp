//! Copyright © 2023-2024 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5.
//! See LICENSE for details.

#pragma once
#include <IOPlatformPluginFamily/IOPlatformPluginFamilyPriv.hpp>

class AMDPlatformPluginEventRecorder;
class AMDPlatformPluginMonitor;

enum struct AMDRequest {
	EventRecord = 0,
	ActiveEPPIndex,
};

enum struct AMDMonitorMessage {
	SwitchedToACPower = 0,
	SwitchedToBatteryPower,
	CPUIsTooHot,
};

enum struct AMDEPPIndex {
	Default = 0,
	Performance,
	BalancedPerformance,
	BalancedPowerSaving,
	PowerSaving,
};

class AMDPlatformPlugin : public IOPlatformPluginFamilyPriv {
	OSDeclareDefaultStructors(AMDPlatformPlugin);
	
	virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
	
	IOReturn handleMessage(AMDMonitorMessage msg);
	
	AMDPlatformPluginMonitor *getMonitor();

	friend class AMDPlatformPluginInterface;
	friend class AMDPlatformPluginACPICPCInterface;
	friend class AMDPlatformPluginACPIThermalZoneInterface;
	
private:
	bool hasSMBIOSBias;
	
	AMDPlatformPluginEventRecorder *recorder;
	AMDPlatformPluginMonitor *monitor;
	
	AMDEPPIndex activeIdx = AMDEPPIndex::Default;
	UInt32 cpuFamily;
	UInt32 cpuModel;
};
