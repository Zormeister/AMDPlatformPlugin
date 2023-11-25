//! Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5.
//! See LICENSE for details.

#include "AMDPlatformPluginEventRecorder.hpp"
#include <IOKit/IOLib.h>

OSDefineMetaClassAndStructors(AMDPlatformPluginEventRecorder, OSObject);

AMDPlatformPluginEventRecorder *AMDPlatformPluginEventRecorder::eventRecorder() {
	auto *ev = new AMDPlatformPluginEventRecorder;
	if (!ev) {
		IOLog("ERROR: %s: failed to create EventRecorder!", __BASE_FILE__);
		return nullptr;
	}
	if (!ev->init()) {
		IOLog("ERROR: %s: failed to create EventRecorder!", __BASE_FILE__);
		ev->free();
		return nullptr;
	}
	return ev;
}

void AMDPlatformPluginEventRecorder::addStringToRecord(const char *string) {
	this->size = (this->size + strlen(string));
	snprintf(this->record, this->size, "%s \n", string);
}

char *AMDPlatformPluginEventRecorder::getRecord() { return this->record; }
size_t AMDPlatformPluginEventRecorder::getRecordSize() { return this->size; }
