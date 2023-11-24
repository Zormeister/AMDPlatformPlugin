//! Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5.
//! See LICENSE for details.

#include "AMDPlatformPluginMonitor.hpp"
#include <IOKit/IOTimerEventSource.h>

bool AMDPlatformPluginMonitor::init() {
	//! Need to setup a routine IOPM prober for some keys, as a check for AC/BAT pwr
	//! That and Thermal control.
	return true;
}
