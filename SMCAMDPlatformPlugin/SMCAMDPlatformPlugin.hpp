//! Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5.
//! See LICENSE for details.

#pragma once
#include <AMDPlatformPlugin/AMDPlatformPlugin.hpp>
#include <VirtualSMCSDK/kern_vsmcapi.hpp>
#include <VirtualSMCSDK/AppleSmc.h>

//! Uses AMDPlatformPluginMonitor to handoff values to the SMC Keys.

class SMCAMDPlatformPlugin : public IOService {};
