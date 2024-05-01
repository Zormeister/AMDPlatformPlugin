//! Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5.
//! See LICENSE for details.

#pragma once
#include <IOKit/IOService.h>
#include "AMDPlatformPluginMonitor.hpp"

class AMDPlatformPluginInterface : public IOService {
    OSDeclareAbstractStructors(AMDPlatformPluginInterface);

    virtual IOReturn handleMonitorRequest(MonitorRequestData *data) = 0;
    virtual const char *getInterfaceReporterName() = 0;
};