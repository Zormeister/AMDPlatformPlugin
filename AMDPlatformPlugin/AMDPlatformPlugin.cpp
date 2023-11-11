//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#include "AMDPlatformPlugin.hpp"
#include "AMDPMTableServices.hpp"
#include "AMDPlatformPluginCPUStateServices.hpp"
#include "AMDPlatformPluginSMUServices.hpp"
#include "AMDPlatformPluginTemperatureServices.hpp"
#include <IOKit/IOService.h>
#include <i386/cpuid.h>
#include <i386/pmCPU.h>
#include <i386/proc_reg.h>

OSDefineMetaClassAndStructors(AMDPlatformPlugin, IOPlatformPluginFamilyPriv);

constexpr static const char *LogLevel[] = {"Info", "Warning", "ERROR", "ERROR UNRECOVERABLE"};

void AMDPlatformPlugin::log(uint32_t logLevel, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    auto *ns = new char[0x10000];
    vsnprintf(ns, 0x10000, fmt, args);
    va_end(args);
    bool doLog;
    PE_parse_boot_argn("-amdpplog", &doLog, sizeof(doLog));
    const char *logLevelStr = sizeof(LogLevel) > logLevel ? LogLevel[logLevel] : "Unknown";
    if (doLog) {
        kprintf("AMDPlatformPlugin:  %s: %s", logLevelStr, ns);
        IOLog("AMDPlatformPlugin:  %s: %s", logLevelStr, ns);
    } else if (logLevel >= 2) {
        kprintf("AMDPlatformPlugin:  %s: %s", logLevelStr, ns);
        IOLog("AMDPlatformPlugin:  %s: %s", logLevelStr, ns);
    }
    if (logLevel == 3) { panic("AMDPlatformPlugin: %s: %s", logLevelStr, ns); }
    delete[] ns;
}

OSData *AMDPlatformPlugin::getBoardID() {
    auto plat = IOService::getPlatform();
    if (plat) {
        auto bid = plat->getProperty("board-id");
        if (bid) {
            OSData *boardid = OSDynamicCast(OSData, bid);
            if (boardid) { return boardid; }
        }
    }
    return 0;
}

bool AMDPlatformPlugin::setupAmdPlatformPlugin() {
    this->AMDServices.tempServices = AMDPlatformPluginTemperatureServices::createTemperatureServices();
    return true;
}

bool AMDPlatformPlugin::start(IOService *provider) {
    IOPlatformPluginFamilyPriv::start(provider);
    const char *vendor[16];
    uint32_t reg[4];
    do_cpuid(0, reg);
    bcopy((char *)&reg[ebx], &vendor[0], 4);
    bcopy((char *)&reg[ecx], &vendor[8], 4);
    bcopy((char *)&reg[edx], &vendor[4], 4);
    log(0, "%s: CPU vendor detected as %s", __PRETTY_FUNCTION__, vendor);

    return true;
}

IOService *AMDPlatformPlugin::probe(IOService *provider, SInt32 *score) {
    OSDictionary *res = IOService::serviceMatching("AMDPlatformPluginSMUServices");
    if (res) {
        auto iter = getMatchingServices(res);
        res->release();
        this->AMDServices.smuServices = OSDynamicCast(AMDPlatformPluginSMUServices, iter->getNextObject());
        iter->release();
        res = IOService::serviceMatching("AMDPlatformPluginCPUStateServices");
        if (res) {
            auto iter = getMatchingServices(res);
            res->release();
            this->AMDServices.cpuStateServices =
                OSDynamicCast(AMDPlatformPluginCPUStateServices, iter->getNextObject());
            iter->release();
        }
        // CPUStateServices is non-essential, so we can skip it in-case something happens
        return IOService::probe(provider, score);
    }
    log(2, "%s: failed probing!", __PRETTY_FUNCTION__);
    return nullptr;
}

bool AMDPlatformPlugin::waitForSmuServicesRunning() {
    OSDictionary *SmuServ;
    AMDPlatformPluginSMUServices *SmuServices;
    do {
        SmuServ = IOService::serviceMatching("AMDPlatformPluginSMUServices");
        SmuServices = OSDynamicCast(AMDPlatformPluginSMUServices, SmuServ);
    } while (!SmuServices);
    return true;
}

bool AMDPlatformPlugin::setPlatform() {
    uint32_t reg[4];
    do_cpuid(0x1, reg);
    UInt32 cpufamily = ((reg[eax] & 0xf00) >> 8) + ((reg[eax] & 0xff00000) >> 20);
    UInt32 cpumodel = ((reg[eax] & 0xf0000) >> 12) + ((reg[eax] & 0xf0) >> 4);
    do_cpuid(0x80000001, reg);
    uint32_t pkgType = (reg[ebx] >> 28);
    return true;
}
