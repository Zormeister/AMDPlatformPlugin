//! Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5.
//! See LICENSE for details.

#include <i386/cpuid.h>
#include <i386/pmCPU.h>
#include "AMDPlatformPlugin.hpp"
#include "AMDPlatformPluginEventRecorder.hpp"
#include "AMDPlatformPluginMonitor.hpp"

#define super IOPlatformPluginFamilyPriv

bool AMDPlatformPlugin::start(IOService *provider) {
	if (!super::start(provider)) {
		IOLog("%s@%s: failed to start superclass!!!", this->getMetaClass()->getClassName(), __FUNCTION__);
		return false;
	}

	this->monitor = AMDPlatformPluginMonitor::withPlatformPlugin(this);
	if (this->monitor == nullptr) {
		IOLog("%s@%s: failed to allocate monitor!!!", this->getMetaClass()->getClassName(), __FUNCTION__);
		super::stop(provider);
		return false
	}

	OSDictionary *dict = IOService::serviceMatching("AMDPlatformPluginInterface");
	UInt32 count = dict->getCount();
	for (int i = 0; i < count; i++) {
		IOReturn ret = this->monitor->registerInterface(dict->getObject(i));
		if (ret) {
			IOLog("%s@%s: failed to register interface @ idx %d", this->getMetaClass()->getClassName(), __FUNCTION__, i);
			super::stop(provider);
			return false;
		}
	}
	return true;
};

IOReturn AMDPlatformPlugin::handleMessage(AMDMonitorMessage msg) {
	switch (msg) {
		case AMDMonitorMessage::SwitchedToACPower:
			this->recorder->addStringToRecord("Recieved switched to AC Power message, allowing performance & balanced performance");
		case AMDMonitorMessage::SwitchedToBatteryPower:
			this->recorder->addStringToRecord("Recieved switched to Battery Power message, disallowing performance & balanced performance");
	}
	return kIOReturnSuccess;
}
