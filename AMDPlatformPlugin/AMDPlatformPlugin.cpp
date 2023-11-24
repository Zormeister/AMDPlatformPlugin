//! Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5.
//! See LICENSE for details.

#include "AMDPlatformPlugin.hpp"
#include "AMDPlatformPluginEventRecorder.hpp"
#include "AMDPlatformPluginMonitor.hpp"

#define super IOPlatformPluginFamilyPriv

bool AMDPlatformPlugin::start(IOService *provider) {
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
