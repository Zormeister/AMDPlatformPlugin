//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

// works with ACPI0007 devices, if not AMDPlatformPlugin should default to grabbing them from the processor definitions.

#pragma once
#include <IOKit/acpi/IOACPIPlatformDevice.h>

struct CPPCPerformanceCaps;

class AMDPlatformPluginCPUStateServices : public IOService {
    OSDeclareDefaultStructors(AMDPlatformPluginCPUStateServices);

    OSObject *grabCPPCTable();
    bool doesProcessorUseLegacyMethod();

    IOACPIPlatformDevice *cpu;
    CPPCPerformanceCaps *caps;
};
