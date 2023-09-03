//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#pragma once
#include "AMDPlatformPlugin.hpp"
#include "AMDPMTableHandler.hpp"
#include <IOKit/pci/IOPCIDevice.h>

enum SMUReturn : uint32_t {
    SMUReturnOK = 0x01,
    SMUReturnFailed = 0xFF,
    SMUReturnUnknownCmd = 0xFE,
    SMUReturnCmdRejectedPrereq = 0xFD,
    SMUReturnCmdRejectedBusy = 0xFC,
	SMUReturnUnsupported = 0xFB,
	SMUReturnUnknownReturn = 0xFA,
};

constexpr const char *SMUReturn2String(SMUReturn smuret) {
	if (smuret == 0x01) {return "SMUReturnOK"; }
	if (smuret == 0xFF) {return "SMUReturnFailed"; }
	if (smuret == 0xFE) {return "SMUReturnUnknownCmd"; }
	if (smuret == 0xFD) {return "SMUReturnCmdRejectedPrereq"; }
	if (smuret == 0xFC) {return "SMUReturnCmdRejectedBusy"; }
	if (smuret == 0xFB) {return "SMUReturnUnsupported"; }
	return "SMUReturnUnknownReturn";
}

enum SMUMailboxType : uint32_t {
	kSMUMBTypeRSMU = 0,
	kSMUMBTypeMP1,
};

constexpr const char *SMUMBType2String(SMUMailboxType mbType) {
	switch (mbType) {
		case kSMUMBTypeRSMU: return "RSMU";
		case kSMUMBTypeMP1: return "MP1";
	}
}

enum MP1InterfaceVersion : uint32_t {
	kMP1InterfaceVNotMP1Mailbox = 0,
	kMP1InterfaceV9 = 9,
	kMP1InterfaceV10,
	kMP1InterfaceV11,
	kMP1InterfaceV12,
	kMP1InterfaceV13,
};

struct SMUInterface {
	MP1InterfaceVersion ifver;
	SMUMailboxType mbType;
	uint32_t addrMbCmd;
	uint32_t addrMbRsp;
	uint32_t addrMbArgs;
};

struct SMU {
    IOPCIDevice *smu;
	SMUInterface smuIf;
	SMUMailboxType mbType;
	UInt32 smuVer;
	SMUReturn lastReturned;
	uint64_t dramBaseAddr;
};

struct SMUCmd {
	uint32_t args[6];
	uint32_t msg;
	uint32_t rsp;
	bool sendToRSMU = false;
};

class AMDPlatformPluginSMUServices : public IOService {
	OSDeclareDefaultStructors(AMDPlatformPluginSMUServices);
	
	virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
	virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;

    UInt32 readFromSmu(SMU *smu, uint32_t addr);
	SMUReturn writeToSmu(SMU *smu, uint32_t addr, uint32_t val);
	SMUReturn getAddrForMB(SMU *smu);
	SMUReturn getAddrForRsmuMB(SMU *smu);
    SMUReturn setupSmuServices(SMU *smu);
	void dumpServicesState();

	SMUCmd lastProcessedCmd;
	static AMDPMTableHandler *pmTbl;
	public:
	static AMDPlatformPluginSMUServices *callback;
	SMU smu;
	bool SmuServicesHaveStarted = false;
	void nullAllArgs(SMUCmd *cmd);
	SMUReturn sendCmdToSmu(SMU *smu, SMUCmd *cmd);
	UInt32 getSmuVersion(SMU *smu);
	uint64_t getSmuDramBaseAddr(SMU *smu);
};
