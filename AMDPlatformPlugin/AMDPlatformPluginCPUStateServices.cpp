//
//  AMDPlatformPluginCPUStateServices.cpp
//  AMDPlatformPlugin
//
//  Created by Zormeister on 15/9/23.
//  Copyright © 2023 VisualDevelopment. All rights reserved.
//

#include "AMDPlatformPluginCPUStateServices.hpp"

bool AMDPlatformPluginCPUStateServices::grabCPPCTable() {
	cpu->getACPITableData("_CPC");
	return false;
}
