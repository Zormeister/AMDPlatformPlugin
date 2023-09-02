//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#pragma once
#include "AMDPlatformPluginSMUServices.hpp"
#include "../IOPlatformPluginFamily/IOPlatformPluginFamilyPriv.hpp"    // will change on final release
#include <IOKit/IOService.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>

enum AMDEPPMode : uint32_t {
    kEPPModePerformance = 0,
    kEPPModeBalancePerformance,
    kEPPModeBalancePowerSavings,
    kEPPModePowerSavings,
};
// Ryzen Mobile/Desktop, APUs in general.
enum AMDAPUPlatforms : uint32_t {
    kNotAPUPlatform = 0,
    kAPUPlatformRaven,
    kAPUPlatformRaven2,
    kAPUPlatformPicasso,
    kAPUPlatformDali,
    kAPUPlatformRenoir,
    kAPUPlatformLucienne,
    kAPUPlatformCezanne,
    kAPUPlatformVanGogh,      // here for reference
    kAPUPlatformRembrandt,    // here for reference
    kAPUPlatformMendocino,
    kAPUPlatformPhoenix,    // here for reference
};

constexpr const char *AMDAPUPlatforms2String(AMDAPUPlatforms apuplat) {
	switch (apuplat) {
		case 0:
			return "Not an APU platform";
		case 1:
			return "Raven";
		case 2:
			return "Raven2";
		case 3:
			return "Picasso";
		case 4:
			return "Dali";
		case 5:
			return "Renoir";
		case 6:
			return "Lucienne";
		case 7:
			return "Cezanne";
		case 8:
			return "VanGogh";
		case 9:
			return "Rembrandt";
		case 10:
			return "Mendocino";
		case 11:
			return "Phoenix";
	}
}

// Ryzen Desktop
enum AMDDesktopPlatforms : uint32_t {
    NotDesktopPlatform = 0,
    kDesktopPlatformSummitRidge,
    kDesktopPlatformPinnacleRidge,
    kDesktopPlatformMatisse,
    kDesktopPlatformVermeer,
    kDesktopPlatformRaphael,
};

constexpr const char *AMDDesktopPlatforms2String(AMDDesktopPlatforms deskplat) {
	switch (deskplat) {
		case 0:
			return "Not a desktop platform";
		case 1:
			return "SummitRidge";
		case 2:
			return "PinnacleRidge";
		case 3:
			return "Matisse";
		case 4:
			return "Vermeer";
		case 5:
			return "Raphael";
	}
}

// Threadripper
enum AMDHEDTPlatforms : uint32_t {
    kNotHEDTPlatform = 0,
    kHEDTPlatformWhitehaven,
    kHEDTPlatformColfax,
    kHEDTPlatformCastlePeak,
    kHEDTPlatformChagall,
};

constexpr const char *AMDHEDTPlatforms2String(AMDHEDTPlatforms hedtplat) {
	switch (hedtplat) {
		case 0:
			return "Not a HEDT platform";
		case 1:
			return "Whitehaven";
		case 2:
			return "Colfax";
		case 3:
			return "CastlePeak";
		case 4:
			return "Chagall";
	}
}

enum AMDCurrentPlatform : uint32_t {
    Undetermined = 0,
    APU,
    Desktop,
    HEDT,
};

constexpr const char *AMDCurrentPlatform2String(AMDCurrentPlatform currentplat) {
	switch (currentplat) {
		case 0:
			return "Undetermined";
		case 1:
			return "APU";
		case 2:
			return "Desktop";
		case 3:
			return "HEDT";
	}
}

enum SystemType : uint32_t {
    UNKNOWN = 0,
    DesktopSystem,    // 01, observed on Mac Pro
    MobileSystem,     // 02, found on MBP SMBIOSes
};

class AMDPlatformPlugin : public IOPlatformPluginFamilyPriv {
	OSDeclareDefaultStructors(AMDPlatformPlugin);
    virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;

    public:
    void log(uint32_t logLevel, const char *fmt, ...);
    bool waitForSmuServicesRunning();
    bool setPlatform();
    AMDAPUPlatforms apuPlatform;
    AMDHEDTPlatforms hedtPlatform;
    AMDDesktopPlatforms desktopPlatform;
    AMDCurrentPlatform currentPlatform;

	static AMDPlatformPlugin *callback;
    private:
    SystemType sysType;
};
