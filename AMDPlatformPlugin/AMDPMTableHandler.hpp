//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#pragma once
#include "AMDPlatformPluginSMUServices.hpp"

class AMDPMTableHandler : public OSObject {
	public:
	bool init();
	SMUReturn interpretPMTable();
};
