//  Copyright © 2022-2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#include "AMDPlatformPlugin.hpp"
#include <i386/cpuid.h>
#include <i386/pmCPU.h>
#include <IOKit/IOService.h>

constexpr static const char *LogLevel[] = {"Info", "Warning", "ERROR", "ERROR UNRECOVERABLE"};

void AMDPlatformPlugin::log(uint32_t logLevel, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	auto *ns = new char[0x10000];
	vsnprintf(ns, 0x10000, fmt, args);
	va_end(args);
	const char *logLevelStr = sizeof(LogLevel) > logLevel ? LogLevel[logLevel] : "Unknown";
	kprintf("AMDPlatformPlugin:  %s: %s", logLevelStr, ns);
	IOLog("AMDPlatformPlugin:  %s: %s", logLevelStr, ns);
	if (logLevel == 3) {
		panic("AMDPlatformPlugin: %s: %s", logLevelStr, ns);
	}
	delete[] ns;
}

bool AMDPlatformPlugin::start(IOService *provider) {
	uint32_t cpufam = cpuid_family();
	if (cpufam == 0x17 || cpufam == 0x19) {
		kprintf("AMDPlatformPlugin: %s: supported CPU found!", __PRETTY_FUNCTION__);
	} else {
		return false;
	}
	return true;
}

