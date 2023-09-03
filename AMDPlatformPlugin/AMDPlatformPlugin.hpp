//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#pragma once
#include "AMDPlatformPluginSMUServices.hpp"
#include "../IOPlatformPluginFamily/IOPlatformPluginFamilyPriv.hpp"    // will change on final release
#include <IOKit/IOService.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>

enum AMDEPPMode : uint32_t {
	kEPPModeDefault = 0,
    kEPPModePerformance,
    kEPPModeBalancePerformance,
    kEPPModeBalancePowerSavings,
    kEPPModePowerSavings,
};
/**
 * `AMDEPPMode Biases`
 * Default is used when starting up the driver, once we've started, the value gets set
 * depending on the platform SystemType enum
 * MacBook Air & MacBook boards automatically switch to kEPPModePowerSavings.
 * MacBook Pro boards automatically switch to kEPPModeBalancePowerSavings,howver
 * it can be overridden by `amdppmode=<AMDEPPMode as string>`
 * in the event that the AC is detected as plugged in, both of these SMBIOSes will jump to
 * kEPPModePerformance.
 * Desktop SMBIOSes automatically get kEPPModePerformance.
 * also applies to iMacPro1,1
 */

constexpr const char *AMDEPPMode2String(AMDEPPMode eppmode) {
	switch (eppmode) {
		case kEPPModeDefault: return "Default";
		case kEPPModePerformance: return "Performance";
		case kEPPModeBalancePerformance: return "Balanced Performance";
		case kEPPModeBalancePowerSavings: return "Balanced Power Savings";
		case kEPPModePowerSavings: return "Power Savings";
	}
}

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
    kAMDPlatformUndetermined = 0,
    kAMDPlatformAPU,
    kAMDPlatformDesktop,
    kAMDPlatformHEDT,
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
// SMBIOS order is from newest to oldest supported by Catalina 10.15.7.

static constexpr const char *kMacBookModelBoardIDs[] = {
	"Mac-0CFF9C7C2B63DF8D", // MacBookAir9,1
	"Mac-226CB3C6A851A671", // MacBookAir8,2
	"Mac-827FAC58A8FDFA22", // MacBookAir8,1
	"Mac-937CB26E2E02BB01", // MacBookAir7,2
	"Mac-9F18E312C5C2BF0B", // MacBookAir7,1
	"Mac-7DF21CB3ED6977E5", // MacBookAir6,2
	"Mac-35C1E88140C3E6CF", // MacBookAir6,1
	"Mac-2E6FAB96566FE58C", // MacBookAir5,2
	"Mac-66F35F19FE2A0D05", // MacBookAir5,1
	"Mac-A61BADE1FDAD7B05", // MacBookPro16,4
	"Mac-5F9802EFE386AA28", // MacBookPro16,3
	"Mac-E7203C0F68AA0004", // MacBookPro16,2
	"Mac-E1008331FDC96864", // MacBookPro16,1
	"Mac-53FDB3D8DB8CA971", // MacBookPro15,4
	"Mac-1E7E29AD0135F9BC", // MacBookPro15,3
	"Mac-827FB448E656EC26", // MacBookPro15,2
	"Mac-937A206F2EE63C01", // MacBookPro15,1
	"Mac-551B86E5744E2388", // MacBookPro14,3
	"Mac-CAD6701F7CEA0921", // MacBookPro14,2
	"Mac-B4831CEBD52A0C4C", // MacBookPro14,1
	"Mac-A5C67F76ED83108C", // MacBookPro13,3
	"Mac-66E35819EE2D0D05", // MacBookPro13,2
	"Mac-473D31EABEB93F9B", // MacBookPro13,1
	"Mac-E43C1C25D4880AD6", // MacBookPro12,1
	"Mac-06F11F11946D27C5", // MacBookPro11,5
	"Mac-06F11FD93F0323C5", // MacBookPro11,4
	"Mac-2BD1B31983FE1663", // MacBookPro11,3
	"Mac-3CBD00234E554E41", // MacBookPro11,2
	"Mac-189A3D4F975D5FFC", // MacBookPro11,1
	"Mac-AFD8A9D944EA4843", // MacBookPro10,2
	"Mac-C3EC7CD22292981F", // MacBookPro10,1
	"Mac-6F01561E16C75D06", // MacBookPro9,2
	"Mac-4B7AC7E43945597E", // MacBookPro9,1
};


enum AppleModelType : uint32_t {
	kAppleModelTypeiMac = 1,
	kAppleModelTypeMacBook,
	kAppleModelTypeMacBookAir,
	kAppleModelTypeMacBookPro,
	kAppleModelTypeMacMini,
	kAppleModelTypeMacPro,
};

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
	bool setEPPModeBias();
	OSData *getBoardID();
	bool setupAmdPlatformPlugin();
    AMDAPUPlatforms apuPlatform;
    AMDHEDTPlatforms hedtPlatform;
    AMDDesktopPlatforms desktopPlatform;
    AMDCurrentPlatform currentPlatform;

	static AMDPlatformPlugin *callback;
    private:
    SystemType sysType;
};
