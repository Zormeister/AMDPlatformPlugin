//! Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5.
//! See LICENSE for details.

#pragma once
#include "AMDPlatformPlugin.hpp"
#include <IOKit/pwr_mgt/IOPMPowerSource.h>
#include <IOKit/IOTimerEventSource.h>

constexpr const char *configCheckinTime = "APMCheckinTime=";

struct AMDMonitorStatistics {
	bool isAcConnected;
	UInt32 temp;
	UInt32 voltage;
};

struct FrequencyData {
	UInt32 highestFreq;
};

enum struct MonitorRequest {
    Tempreature,
    Voltage,
	FrequencyControlData,
};

struct MonitorRequestData {
    MonitorRequest req;
	const char *dataName;
    UInt64 coreId;
	void *additionalData;
};

class AMDPlatformPluginMonitor : public OSObject {
	OSDeclareDefaultStructors(AMDPlatformPluginMonitor);
	
	virtual bool init() APPLE_KEXT_OVERRIDE;
	
	static AMDPlatformPluginMonitor *withPlatformPlugin(AMDPlatformPlugin *pp);

	IOReturn registerInterface(class AMDPlatformPluginInterface *interface);

	IOReturn setDataFromInterface(MonitorRequestData *data);

	static IOReturn checkInOnInterfaces(OSObject *target, void *arg0, void *arg1, void *arg2, void *arg3);

	AMDMonitorStatistics *getStatistics();

	void handoffDiagnostics();
	
	void setupTimers();
	
	AMDMonitorStatistics stats;
	OSArray *interfaceArray;
	IOTimerEventSource *checkInTimer;
};
