//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#pragma once
#include "../IOPlatformPluginFamily/IOPlatformPluginFamilyPriv.hpp"    // will change on final release
#include <IOKit/IOService.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>

class AMDPlatformPluginCPUStateServices;
class AMDPlatformPluginSMUServices;
class AMDPlatformPluginTemperatureServices;
class AMDPMTableServices;

enum CPPCVersions : UInt32 {
    kCPPCVersion2 = 2,
    kCPPCVersion3,
};

static struct CPCReg {
    UInt8 desc;
    UInt16 length;
    UInt8 spaceId;
    UInt8 bitWidth;
    UInt8 bitOff;
    UInt8 accessWidth;
    UInt64 addr;
} PACKED;

struct CPPCPerformanceCaps {
    UInt32 energyPerf;
    UInt32 guaranteedPerf;
    UInt32 highestPerf;
    UInt32 lowestFreq;
    UInt32 lowestNonlinearPerf;
    UInt32 lowestPerf;
    UInt32 nominalFreq;
    UInt32 nominalPerf;
    bool autoSelEnable;
};

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
        case kEPPModeDefault:
            return "Default";
        case kEPPModePerformance:
            return "Performance";
        case kEPPModeBalancePerformance:
            return "Balanced Performance";
        case kEPPModeBalancePowerSavings:
            return "Balanced Power Savings";
        case kEPPModePowerSavings:
            return "Power Savings";
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

enum struct AMDPlatformCodename {
    SummitRidge = 0,    // Ryzen 1XXX
    Naples,             // First-gen EPYC
    RavenRidge,         // Ryzen 1XXX APUs
    Whitehaven,         // Threadripper 1XXX
    PinnacleRidge,      // Ryzen 2XXX
    Colfax,             // Threadripper 2XXX
    Picasso,            // Ryzen 3XXX APUs
    RavenRidge2,
    Dali,
    Matisse,       // Ryzen 3XXX
    Rome,          // Second-gen EPYC
    CastlePeak,    // Threadripper 3XXX
    Renoir,        // Ryzen 4XXX APUs
    Vermeer,       // Ryzen 5XXX
    Milan,
    Cezanne,
    Lucienne,
    VanGogh,
    Rembrandt,
    Chagall,
    Raphael,
    Genoa,    //----- UNSUPPORTED BEYOND THIS LINE -----//
    Phoenix,
    DragonRange,
    Mendocino,
    StormPeak,
};

struct SMUAddressCollection {
    UInt32 command;
    UInt32 arguments;
    UInt32 response;
};

SMUAddressCollection kRSMUAddresses1 = {0x3B1051C, 0x3B10590, 0x3B10568};
SMUAddressCollection kRSMUAddresses2 = {0x3B10524, 0x3B10A40, 0x3B10570};
SMUAddressCollection kRSMUAddresses3 = {0x3B10A20, 0x3B10A80, 0x3B10A88};

SMUAddressCollection kMP1Addresses1 = {0x3B10528, 0x3B10598, 0x3B10564};
SMUAddressCollection kMP1Addresses2 = {0x3B10528, 0x3B10998, 0x3B10564};
SMUAddressCollection kMP1Addresses3 = {0x3B10530, 0x3B109C4, 0x3B1057C};

SMUAddressCollection kHSMPAddresses1 = {0x3B10534, 0x3B109E0, 0x3B10980};

struct AMDPlatform {
    UInt32 familyId;
    UInt32 model;
    UInt32 pkgType;
    AMDPlatformCodename codename;
    SMUAddressCollection *rsmu;
    SMUAddressCollection *hsmp;
    SMUAddressCollection *mp1;
};

constexpr UInt32 PKG_TYPE_DONT_CARE = 0xFF;
constexpr UInt32 PLATFORM_TABLE_END = 0xFFFFFFFF;

AMDPlatform platformTable[] = {
    {0x17, 0x01, 0x02, AMDPlatformCodename::SummitRidge, &kRSMUAddresses1, nullptr, nullptr},
    {0x17, 0x01, 0x04, AMDPlatformCodename::Naples, &kRSMUAddresses1, nullptr, nullptr},
    {0x17, 0x01, 0x07, AMDPlatformCodename::Whitehaven, &kRSMUAddresses1, nullptr, nullptr},
    {0x17, 0x08, 0x02, AMDPlatformCodename::PinnacleRidge, &kRSMUAddresses1, nullptr, nullptr},
    {0x17, 0x08, 0x04, AMDPlatformCodename::Colfax, &kRSMUAddresses1, nullptr, nullptr},
    {0x17, 0x08, 0x07, AMDPlatformCodename::Colfax, &kRSMUAddresses1, nullptr, nullptr},
    {0x17, 0x11, PKG_TYPE_DONT_CARE, AMDPlatformCodename::RavenRidge, &kRSMUAddresses3, nullptr, nullptr},
    {0x17, 0x18, 0x02, AMDPlatformCodename::RavenRidge2, &kRSMUAddresses3, nullptr, nullptr},
    {0x17, 0x11, PKG_TYPE_DONT_CARE, AMDPlatformCodename::Picasso, &kRSMUAddresses3, nullptr, nullptr},
    {0x17, 0x20, PKG_TYPE_DONT_CARE, AMDPlatformCodename::Dali, &kRSMUAddresses3, nullptr, nullptr},
    {0x17, 0x31, PKG_TYPE_DONT_CARE, AMDPlatformCodename::CastlePeak, &kRSMUAddresses2, nullptr, &kHSMPAddresses1},
    {0x17, 0x60, PKG_TYPE_DONT_CARE, AMDPlatformCodename::Renoir, &kRSMUAddresses3, nullptr, nullptr},
    {0x17, 0x68, PKG_TYPE_DONT_CARE, AMDPlatformCodename::Lucienne, &kRSMUAddresses3, nullptr, nullptr},
    {0x17, 0x71, PKG_TYPE_DONT_CARE, AMDPlatformCodename::Matisse, &kRSMUAddresses2, nullptr, &kHSMPAddresses1},
    {0x17, 0x90, PKG_TYPE_DONT_CARE, AMDPlatformCodename::VanGogh, nullptr, nullptr, nullptr},
    {0x19, 0x01, PKG_TYPE_DONT_CARE, AMDPlatformCodename::Milan, &kRSMUAddresses2, nullptr, &kHSMPAddresses1},
    {0x19, 0x08, PKG_TYPE_DONT_CARE, AMDPlatformCodename::Chagall, &kRSMUAddresses2, nullptr, &kHSMPAddresses1},
    {0x19, 0x20, PKG_TYPE_DONT_CARE, AMDPlatformCodename::Vermeer, &kRSMUAddresses2, nullptr, &kHSMPAddresses1},
    {0x19, 0x21, PKG_TYPE_DONT_CARE, AMDPlatformCodename::Vermeer, &kRSMUAddresses2, nullptr, &kHSMPAddresses1},
    {0x19, 0x40, PKG_TYPE_DONT_CARE, AMDPlatformCodename::Rembrandt, nullptr, nullptr, nullptr},
    {0x19, 0x50, PKG_TYPE_DONT_CARE, AMDPlatformCodename::Cezanne, &kRSMUAddresses3, nullptr, nullptr},
    {0x19, 0x61, PKG_TYPE_DONT_CARE, AMDPlatformCodename::Raphael, &kRSMUAddresses2, nullptr, &kHSMPAddresses1},

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
            return "Van Gogh";
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
    "Mac-0CFF9C7C2B63DF8D",    // MacBookAir9,1
    "Mac-226CB3C6A851A671",    // MacBookAir8,2
    "Mac-827FAC58A8FDFA22",    // MacBookAir8,1
    "Mac-937CB26E2E02BB01",    // MacBookAir7,2
    "Mac-9F18E312C5C2BF0B",    // MacBookAir7,1
    "Mac-7DF21CB3ED6977E5",    // MacBookAir6,2
    "Mac-35C1E88140C3E6CF",    // MacBookAir6,1
    "Mac-2E6FAB96566FE58C",    // MacBookAir5,2
    "Mac-66F35F19FE2A0D05",    // MacBookAir5,1
    "Mac-A61BADE1FDAD7B05",    // MacBookPro16,4
    "Mac-5F9802EFE386AA28",    // MacBookPro16,3
    "Mac-E7203C0F68AA0004",    // MacBookPro16,2
    "Mac-E1008331FDC96864",    // MacBookPro16,1
    "Mac-53FDB3D8DB8CA971",    // MacBookPro15,4
    "Mac-1E7E29AD0135F9BC",    // MacBookPro15,3
    "Mac-827FB448E656EC26",    // MacBookPro15,2
    "Mac-937A206F2EE63C01",    // MacBookPro15,1
    "Mac-551B86E5744E2388",    // MacBookPro14,3
    "Mac-CAD6701F7CEA0921",    // MacBookPro14,2
    "Mac-B4831CEBD52A0C4C",    // MacBookPro14,1
    "Mac-A5C67F76ED83108C",    // MacBookPro13,3
    "Mac-66E35819EE2D0D05",    // MacBookPro13,2
    "Mac-473D31EABEB93F9B",    // MacBookPro13,1
    "Mac-E43C1C25D4880AD6",    // MacBookPro12,1
    "Mac-06F11F11946D27C5",    // MacBookPro11,5
    "Mac-06F11FD93F0323C5",    // MacBookPro11,4
    "Mac-2BD1B31983FE1663",    // MacBookPro11,3
    "Mac-3CBD00234E554E41",    // MacBookPro11,2
    "Mac-189A3D4F975D5FFC",    // MacBookPro11,1
    "Mac-AFD8A9D944EA4843",    // MacBookPro10,2
    "Mac-C3EC7CD22292981F",    // MacBookPro10,1
    "Mac-6F01561E16C75D06",    // MacBookPro9,2
    "Mac-4B7AC7E43945597E",    // MacBookPro9,1
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
    MobileSystem,     // 02, found on MBP & MBA SMBIOSes
};

struct AMDPlatformPluginServices {
    bool needsCpuStateServices = false;    // ACPI0007 devices need it.
    AMDPlatformPluginCPUStateServices *cpuStateServices;
    AMDPlatformPluginSMUServices *smuServices;
    AMDPlatformPluginTemperatureServices *tempServices;
    AMDPMTableServices *pmTblServices;
};

class AMDPlatformPlugin : public IOPlatformPluginFamilyPriv {
    OSDeclareDefaultStructors(AMDPlatformPlugin);
    virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual IOService *probe(IOService *provider, SInt32 *score) APPLE_KEXT_OVERRIDE;

    public:
    void log(uint32_t logLevel, const char *fmt, ...);
    bool waitForSmuServicesRunning();
    bool setPlatform();
    bool setEPPModeBias();
    OSData *getBoardID();
    bool setupAmdPlatformPlugin();
    void dumpAmdPPState();
    AMDAPUPlatforms apuPlatform;
    AMDHEDTPlatforms hedtPlatform;
    AMDDesktopPlatforms desktopPlatform;
    AMDCurrentPlatform currentPlatform;

    private:
    AMDPlatformPluginServices AMDServices;
    SystemType sysType;
    bool hasCPPC = false;
};
