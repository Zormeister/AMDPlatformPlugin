//! Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5.
//! See LICENSE for details.

#pragma once
#include "AMDPlatformPlugin.hpp"

class AMDPlatformPluginEventRecorder : public OSObject {
	OSDeclareDefaultStructors(AMDPlatformPluginEventRecorder);
public:
	static AMDPlatformPluginEventRecorder *eventRecorder();
	
	void addStringToRecord(const char *string);
	char *getRecord();
	size_t getRecordSize();
	
	void setupTimers();
private:
	size_t size;
	char record[];
};
