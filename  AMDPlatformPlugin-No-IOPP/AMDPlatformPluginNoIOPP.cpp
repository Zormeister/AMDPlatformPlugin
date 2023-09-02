//  Copyright © 2022-2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#include "AMDPlatformPluginNoIOPP.hpp"
#include "AMDPlatformPluginSMUServicesNoIOPP.hpp"
#include <i386/cpuid.h>
#include <i386/pmCPU.h>
#include <i386/proc_reg.h>
#include <IOKit/IOService.h>

OSDefineMetaClassAndStructors(AMDPlatformPluginNoIOPP, IOService);

constexpr static const char *LogLevel[] = {"Info", "Warning", "ERROR", "ERROR UNRECOVERABLE"};

void AMDPlatformPluginNoIOPP::log(uint32_t logLevel, const char *fmt, ...) {
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

bool AMDPlatformPluginNoIOPP::start(IOService *provider) {
    IOService::start(provider);
    const char *vendor[16];
    uint32_t reg[4];
    do_cpuid(0, reg);
    bcopy((char *)&reg[ebx], &vendor[0], 4);
    bcopy((char *)&reg[ecx], &vendor[8], 4);
    bcopy((char *)&reg[edx], &vendor[4], 4);
    log(0, "%s: CPU vendor detected as %s", __PRETTY_FUNCTION__, vendor);

    return true;
}

bool AMDPlatformPluginNoIOPP::waitForSmuServicesRunning() {
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

bool AMDPlatformPluginNoIOPP::setPlatform() {
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
                            this->currentPlatform = HEDT;
                            this->hedtPlatform = Whitehaven;
                        } else {
                            this->currentPlatform = Desktop;
                            this->desktopPlatform = SummitRidge;
                        }
                    } else if (cpuBModel == 0x8) {
                        if (pkgType == 7) {
                            this->currentPlatform = HEDT;
                            this->hedtPlatform = Colfax;
                        } else {
                            this->currentPlatform = Desktop;
                            this->desktopPlatform = PinnacleRidge;
                        }
                    }
                    break;
                }
                case 0x1: {
					this->currentPlatform = APU;
                    if (cpuBModel == 0x1) {
                        this->apuPlatform = Raven;
                    } else if (cpuBModel == 0x8) {
                        if (pkgType == 2) {
                            this->apuPlatform = Raven2;
                        } else {
                            this->apuPlatform = Picasso;
                        }
                    }
                    break;
                }
                case 0x2: {
                    if (cpuBModel == 0) {
                        this->currentPlatform = APU;
                        this->apuPlatform = Dali;
                    }
					break;
                }
                case 0x3: {
                    if (cpuBModel == 0x1) {
                        this->currentPlatform = HEDT;
                        this->hedtPlatform = CastlePeak;
                    }
					break;
                }
                case 0x6: {
					this->currentPlatform = APU;
                    if (cpuBModel == 0x0) {
                        this->apuPlatform = Renoir;
                    } else if (cpuBModel == 0x8) {
                        this->apuPlatform = Lucienne;
                    }
					break;
                }
                case 0x7: {
                    if (cpuBModel == 0x1) {
                        this->currentPlatform = Desktop;
                        this->desktopPlatform = Matisse;
                    }
					break;
                }
                case 0x9: {
                    if (cpuBModel == 0x0) {
                        this->currentPlatform = APU;
                        this->apuPlatform = VanGogh;
                    }
					break;
                }
                case 0xA: {
                    if (cpuBModel == 0x0) {
                        this->currentPlatform = APU;
                        this->apuPlatform = Mendocino;
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
                        this->currentPlatform = Desktop;
                        this->desktopPlatform = Vermeer;
                    }
					break;
                }
                case 0x4: {
                    if (cpuBModel == 0) {
                        this->currentPlatform = APU;
                        this->apuPlatform = Rembrandt;
                        break;
                    }
                }
                case 0x5: {
                    if (cpuBModel == 0) {
                        this->currentPlatform = APU;
                        this->apuPlatform = Cezanne;
                    }
					break;
                }
            }
			break;
        }
    }
    return true;
}
