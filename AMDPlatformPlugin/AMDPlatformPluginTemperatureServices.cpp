//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#include "AMDPlatformPluginTemperatureServices.hpp"

AMDPlatformPluginTemperatureServices *AMDPlatformPluginTemperatureServices::createTemperatureServices() {
	auto *temp = new AMDPlatformPluginTemperatureServices{};
    if (!temp) { return nullptr; }
    if (temp->init()) { return temp; }
	temp->release();
	return nullptr;
}
