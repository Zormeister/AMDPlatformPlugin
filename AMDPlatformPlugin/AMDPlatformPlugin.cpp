//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#include "AMDPlatformPlugin.hpp"
#include "AMDPlatformPluginSMUServices.hpp"
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

bool AMDPlatformPlugin::setupAmdPlatformPlugin() { return true; }

bool AMDPlatformPlugin::start(IOService *provider) {
    IOPlatformPluginFamilyPriv::start(provider);
    callback = this;
    const char *vendor[16];
    uint32_t reg[4];
    do_cpuid(0, reg);
    bcopy((char *)&reg[ebx], &vendor[0], 4);
    bcopy((char *)&reg[ecx], &vendor[8], 4);
    bcopy((char *)&reg[edx], &vendor[4], 4);
    log(0, "%s: CPU vendor detected as %s", __PRETTY_FUNCTION__, vendor);

    return true;
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

#define bit32(n)            (1U << (n))
#define bitmask32(h, l)     ((bit32(h) | (bit32(h) - 1)) & ~(bit32(l) - 1))
#define bitfield32(x, h, l) ((((x)&bitmask32(h, l)) >> l))

bool AMDPlatformPlugin::setPlatform() {
    uint32_t reg[4];
    do_cpuid(1, reg);
    uint32_t cpuFamily = bitfield32(reg[eax], 11, 8) + bitfield32(reg[eax], 27, 20);
    uint32_t cpuBModel = bitfield32(reg[eax], 7, 4);
    uint32_t cpuEModel = bitfield32(reg[eax], 27, 20);
    do_cpuid(0x80000001, reg);
    uint32_t pkgType = bitfield32(reg[ebx], 31, 28);
    switch (cpuFamily) {
        case 0x17: {
            switch (cpuEModel) {
                case 0x0: {
                    if (cpuBModel == 0x1) {
                        if (pkgType == 7) {
                            this->currentPlatform = kAMDPlatformHEDT;
                            this->hedtPlatform = kHEDTPlatformWhitehaven;
                        } else {
                            this->currentPlatform = kAMDPlatformDesktop;
                            this->desktopPlatform = kDesktopPlatformSummitRidge;
                        }
                    } else if (cpuBModel == 0x8) {
                        if (pkgType == 7) {
                            this->currentPlatform = kAMDPlatformHEDT;
                            this->hedtPlatform = kHEDTPlatformColfax;
                        } else {
                            this->currentPlatform = kAMDPlatformDesktop;
                            this->desktopPlatform = kDesktopPlatformPinnacleRidge;
                        }
                    }
                    break;
                }
                case 0x1: {
                    this->currentPlatform = kAMDPlatformAPU;
                    if (cpuBModel == 0x1) {
                        this->apuPlatform = kAPUPlatformRaven;
                    } else if (cpuBModel == 0x8) {
                        if (pkgType == 2) {
                            this->apuPlatform = kAPUPlatformRaven2;
                        } else {
                            this->apuPlatform = kAPUPlatformPicasso;
                        }
                    }
                    break;
                }
                case 0x2: {
                    if (cpuBModel == 0) {
                        this->currentPlatform = kAMDPlatformAPU;
                        this->apuPlatform = kAPUPlatformDali;
                    }
                    break;
                }
                case 0x3: {
                    if (cpuBModel == 0x1) {
                        this->currentPlatform = kAMDPlatformHEDT;
                        this->hedtPlatform = kHEDTPlatformCastlePeak;
                    }
                    break;
                }
                case 0x6: {
                    this->currentPlatform = kAMDPlatformAPU;
                    if (cpuBModel == 0x0) {
                        this->apuPlatform = kAPUPlatformRenoir;
                    } else if (cpuBModel == 0x8) {
                        this->apuPlatform = kAPUPlatformLucienne;
                    }
                    break;
                }
                case 0x7: {
                    if (cpuBModel == 0x1) {
                        this->currentPlatform = kAMDPlatformDesktop;
                        this->desktopPlatform = kDesktopPlatformMatisse;
                    }
                    break;
                }
                case 0x9: {
                    if (cpuBModel == 0x0) {
                        this->currentPlatform = kAMDPlatformAPU;
                        this->apuPlatform = kAPUPlatformVanGogh;
                    }
                    break;
                }
                case 0xA: {
                    if (cpuBModel == 0x0) {
                        this->currentPlatform = kAMDPlatformAPU;
                        this->apuPlatform = kAPUPlatformMendocino;
                    }
                    break;
                }
            }
            break;
        }
        case 0x19: {
            switch (cpuEModel) {
                case 0x2: {
                    if (cpuBModel == 0x0 || cpuBModel == 0x1) {
                        this->currentPlatform = kAMDPlatformDesktop;
                        this->desktopPlatform = kDesktopPlatformVermeer;
                    }
                    break;
                }
                case 0x4: {
                    if (cpuBModel == 0) {
                        this->currentPlatform = kAMDPlatformAPU;
                        this->apuPlatform = kAPUPlatformRembrandt;
                        break;
                    }
                }
                case 0x5: {
                    if (cpuBModel == 0) {
                        this->currentPlatform = kAMDPlatformAPU;
                        this->apuPlatform = kAPUPlatformCezanne;
                    }
                    break;
                }
				case 0x8: {
                    if (cpuBModel == 0) {
                        this->currentPlatform = kAMDPlatformAPU;
                        this->apuPlatform = kAPUPlatformBarcelo;
                    }
                    break;
                }
            }
            break;
        }
    }
    return true;
}
