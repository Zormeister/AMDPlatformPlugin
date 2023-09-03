//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#include "AMDPlatformPluginSMUServices.hpp"
#include "AMDPMTableHandler.hpp"
#include <i386/cpuid.h>

OSDefineMetaClassAndStructors(AMDPlatformPluginSMUServices, IOService);

UInt32 AMDPlatformPluginSMUServices::readFromSmu(SMU *smu, uint32_t addr) {
	smu->smu->configWrite32(0xC4, addr);
	return smu->smu->configRead32(0xC8);
}

SMUReturn AMDPlatformPluginSMUServices::writeToSmu(SMU *smu, uint32_t addr, uint32_t val) {
	smu->smu->configWrite32(0xC4, addr);
	smu->smu->configWrite32(0xC8, val);
	return SMUReturnOK;
}

SMUReturn AMDPlatformPluginSMUServices::getAddrForRsmuMB(SMU *smu) {
	switch (AMDPlatformPlugin::callback->apuPlatform) {
		case kAPUPlatformRaven:
		case kAPUPlatformRaven2:
		case kAPUPlatformPicasso:
		case kAPUPlatformDali:
		case kAPUPlatformRenoir:
		case kAPUPlatformLucienne:
		case kAPUPlatformCezanne:
			smu->smuIf.ifver = kMP1InterfaceVNotMP1Mailbox;
			smu->mbType = kSMUMBTypeRSMU;
			smu->smuIf.addrMbArgs = 0x3B10A88;
			smu->smuIf.addrMbCmd = 0x3B10A20;
			smu->smuIf.addrMbRsp = 0x3B10A80;
			break;
		case kAPUPlatformVanGogh:
		case kAPUPlatformRembrandt:
		case kAPUPlatformMendocino:
			// doesn't support RSMU
			smu->smuIf.ifver = kMP1InterfaceV13;
			smu->mbType = kSMUMBTypeMP1;
			smu->smuIf.addrMbArgs = 0x3B10998;
			smu->smuIf.addrMbCmd = 0x3B10528;
			smu->smuIf.addrMbRsp = 0x3B10564;
			break;
		default:
			break;
	}
	switch (AMDPlatformPlugin::callback->desktopPlatform) {
		case kDesktopPlatformSummitRidge:
		case kDesktopPlatformPinnacleRidge:
			smu->smuIf.ifver = kMP1InterfaceV9;
			smu->smuIf.addrMbArgs = 0x3B10598;
			smu->smuIf.addrMbCmd = 0x3B10528;
			smu->smuIf.addrMbRsp = 0x3B10564;
			break;
		case kDesktopPlatformMatisse:
		case kDesktopPlatformVermeer:
			smu->smuIf.ifver = kMP1InterfaceV11;
			smu->smuIf.addrMbArgs = 0x3B109C4;
			smu->smuIf.addrMbCmd = 0x3B10530;
			smu->smuIf.addrMbRsp = 0x3B1057C;
			break;
		default:
			break;
	}
	switch (AMDPlatformPlugin::callback->hedtPlatform) {
		case kHEDTPlatformWhitehaven:
		case kHEDTPlatformColfax:
			smu->smuIf.ifver = kMP1InterfaceV9;
			smu->smuIf.addrMbArgs = 0x3B10598;
			smu->smuIf.addrMbCmd = 0x3B10528;
			smu->smuIf.addrMbRsp = 0x3B10564;
			break;
		case kHEDTPlatformCastlePeak:
			smu->smuIf.ifver = kMP1InterfaceV11;
			smu->smuIf.addrMbArgs = 0x3B109C4;
			smu->smuIf.addrMbCmd = 0x3B10530;
			smu->smuIf.addrMbRsp = 0x3B1057C;
			break;
		default:
			break;
	}
	if (smu->smuIf.addrMbArgs == 0 || smu->smuIf.addrMbCmd == 0 || smu->smuIf.addrMbRsp == 0) {
		AMDPlatformPlugin::callback->log(2, "%s: one of our Addresses are null!", __PRETTY_FUNCTION__);
		return SMUReturnFailed;
	}
	return SMUReturnOK;
}

SMUReturn AMDPlatformPluginSMUServices::sendCmdToSmu(SMU *smu, SMUCmd *cmd) {
	AMDPlatformPlugin::callback->log(0, "%s: Sending command to SMU, MSG: %x, Args: 0: %x, 1: %x, 2: %x, 3: %x, 4: %x, 5: %x", __PRETTY_FUNCTION__, cmd->msg, cmd->args[0], cmd->args[1], cmd->args[2], cmd->args[3], cmd->args[4], cmd->args[5]);
	uint32_t tmp;
	uint32_t retries = 500;
	if (cmd->sendToRSMU) {
		
	}
	do { tmp = readFromSmu(&this->smu, smu->smuIf.addrMbRsp); } while (tmp == 0 && retries--);
	if (!tmp && !retries) {
		AMDPlatformPlugin::callback->log(2, "%s: 	Timed out whilst waiting for the mailbox to be available", __PRETTY_FUNCTION__);
		return SMUReturnFailed;
	}
	writeToSmu(smu, smu->smuIf.addrMbRsp, 0);
	for (int i = 0; i < 6; i++) { writeToSmu(smu, smu->smuIf.addrMbArgs + (i * 4), cmd->args[i]); }
	writeToSmu(smu, smu->smuIf.addrMbCmd, cmd->msg);
	retries = 500;
	do { tmp = readFromSmu(&this->smu, smu->smuIf.addrMbRsp); } while (tmp == 0 && retries--);
	if (tmp != SMUReturnOK && !retries) {
		if (!tmp) {
			AMDPlatformPlugin::callback->log(2, "%s: 	Timed out whilst waiting for the SMU to respond", __PRETTY_FUNCTION__);
			return SMUReturnFailed;
		}
		AMDPlatformPlugin::callback->log(2, "%s: SMU response was not SMUReturnOK! Response: %xh", __PRETTY_FUNCTION__, tmp);
	}
	for (int i = 0; i < 6; i++) { cmd->args[i] = readFromSmu(smu, smu->smuIf.addrMbArgs + (i * 4)); }
	AMDPlatformPlugin::callback->log(0, "%s: SMU has responded!", __PRETTY_FUNCTION__);
	AMDPlatformPlugin::callback->log(0, "%s: SMU response: MSG: Args: 0: %x, 1: %x, 2: %x, 3: %x 4: %x, 5: %x", __PRETTY_FUNCTION__, cmd->msg, cmd->args[0], cmd->args[1], cmd->args[2], cmd->args[3], cmd->args[4], cmd->args[5]);
	return SMUReturnOK;
}

void AMDPlatformPluginSMUServices::nullAllArgs(SMUCmd *cmd) {
	for (int i = 1; i < 6; i++) { cmd->args[i] = 0; }
}

SMUReturn AMDPlatformPluginSMUServices::getAddrForMB(SMU *smu) {
	switch (AMDPlatformPlugin::callback->apuPlatform) {
		case kAPUPlatformRaven:
		case kAPUPlatformRaven2:
		case kAPUPlatformPicasso:
		case kAPUPlatformDali:
			smu->smuIf.ifver = kMP1InterfaceV10;
			smu->mbType = kSMUMBTypeMP1;
			smu->smuIf.addrMbArgs = 0x3B10998;
			smu->smuIf.addrMbCmd = 0x3B10528;
			smu->smuIf.addrMbRsp = 0x3B10564;
			break;
		case kAPUPlatformRenoir:
		case kAPUPlatformLucienne:
		case kAPUPlatformCezanne:
			smu->smuIf.ifver = kMP1InterfaceV12;
			smu->mbType = kSMUMBTypeRSMU;
			smu->smuIf.addrMbArgs = 0x3B10998;
			smu->smuIf.addrMbCmd = 0x3B10528;
			smu->smuIf.addrMbRsp = 0x3B10564;
			break;
		case kAPUPlatformVanGogh:
		case kAPUPlatformRembrandt:
		case kAPUPlatformMendocino:
			smu->smuIf.ifver = kMP1InterfaceV13;
			smu->mbType = kSMUMBTypeMP1;
			smu->smuIf.addrMbArgs = 0x3B10998;
			smu->smuIf.addrMbCmd = 0x3B10528;
			smu->smuIf.addrMbRsp = 0x3B10564;
			break;
		default:
			break;
	}
	switch (AMDPlatformPlugin::callback->desktopPlatform) {
		case kDesktopPlatformSummitRidge:
		case kDesktopPlatformPinnacleRidge:
			smu->smuIf.ifver = kMP1InterfaceV9;
			smu->smuIf.addrMbArgs = 0x3B10598;
			smu->smuIf.addrMbCmd = 0x3B10528;
			smu->smuIf.addrMbRsp = 0x3B10564;
			break;
		case kDesktopPlatformMatisse:
		case kDesktopPlatformVermeer:
			smu->smuIf.ifver = kMP1InterfaceV11;
			smu->smuIf.addrMbArgs = 0x3B109C4;
			smu->smuIf.addrMbCmd = 0x3B10530;
			smu->smuIf.addrMbRsp = 0x3B1057C;
			break;
		default:
			break;
	}
	switch (AMDPlatformPlugin::callback->hedtPlatform) {
		case kHEDTPlatformWhitehaven:
		case kHEDTPlatformColfax:
			smu->smuIf.ifver = kMP1InterfaceV9;
			smu->smuIf.addrMbArgs = 0x3B10598;
			smu->smuIf.addrMbCmd = 0x3B10528;
			smu->smuIf.addrMbRsp = 0x3B10564;
			break;
		case kHEDTPlatformCastlePeak:
			smu->smuIf.ifver = kMP1InterfaceV11;
			smu->smuIf.addrMbArgs = 0x3B109C4;
			smu->smuIf.addrMbCmd = 0x3B10530;
			smu->smuIf.addrMbRsp = 0x3B1057C;
			break;
		default:
			break;
	}
	if (smu->smuIf.addrMbArgs == 0 || smu->smuIf.addrMbCmd == 0 || smu->smuIf.addrMbRsp == 0) {
		AMDPlatformPlugin::callback->log(2, "%s: one of our Addresses are null!", __PRETTY_FUNCTION__);
		return SMUReturnFailed;
	}
	return SMUReturnOK;
}

uint64_t AMDPlatformPluginSMUServices::getSmuDramBaseAddr(SMU *smu) {
	AMDPlatformPlugin::callback->log(0, "%s: grabbing SMU DRAM base address", __PRETTY_FUNCTION__);
	uint32_t ret, part[2], cmds[3];
	SMUCmd cmd;
	switch (AMDPlatformPlugin::callback->apuPlatform) {
		case kAPUPlatformRaven:
		case kAPUPlatformRaven2:
		case kAPUPlatformPicasso:
		case kAPUPlatformDali:
			cmds[0] = 0x0A;
			cmds[1] = 0x3D;
			cmds[2] = 0x03;
			break;
		case kAPUPlatformRenoir:
		case kAPUPlatformLucienne:
		case kAPUPlatformCezanne:
			cmds[0] = 0x66;
			cmds[1] = 0;
			cmds[2] = 0;
			break;
		default:
			break;
	}
	switch (AMDPlatformPlugin::callback->desktopPlatform) {
		case kDesktopPlatformPinnacleRidge:
			cmds[0] = 0x0B;
			cmds[1] = 0x0C;
			cmds[2] = 0;
			break;
		case kDesktopPlatformMatisse:
		case kDesktopPlatformVermeer:
			cmds[0] = 0x06;
			cmds[1] = 0;
			cmds[2] = 0;
			break;
		default:
			break;
	}
	switch (AMDPlatformPlugin::callback->hedtPlatform) {
		case kHEDTPlatformColfax:
			cmds[0] = 0x0B;
			cmds[1] = 0x0C;
			cmds[2] = 0;
			break;
		case kHEDTPlatformCastlePeak:
			cmds[0] = 0x06;
			cmds[1] = 0;
			cmds[2] = 0;
			break;
		default:
			break;
	}
	cmd.sendToRSMU = true;
	if (cmds[2] != 0) {
		cmd.args[0] = 3;
		cmd.msg = cmds[0];
		ret = sendCmdToSmu(smu, &cmd);
		this->lastProcessedCmd = cmd;
		if (ret != SMUReturnOK) { return SMUReturnFailed; }
		nullAllArgs(&cmd);
		cmd.args[0] = 3;
		cmd.msg = cmds[2];
		ret = sendCmdToSmu(smu, &cmd);
		this->lastProcessedCmd = cmd;
		if (ret != SMUReturnOK) { return SMUReturnFailed; }
		part[0] = cmd.args[0];
		
		nullAllArgs(&cmd);
		cmd.args[0] = 3;
		cmd.msg = cmds[1];
		ret = sendCmdToSmu(smu, &cmd);
		this->lastProcessedCmd = cmd;
		if (ret != SMUReturnOK) { return SMUReturnFailed; }
		nullAllArgs(&cmd);
		cmd.args[0] = 5;
		cmd.msg = cmds[0];
		ret = sendCmdToSmu(smu, &cmd);
		this->lastProcessedCmd = cmd;
		if (ret != SMUReturnOK) { return SMUReturnFailed; }
		nullAllArgs(&cmd);
		cmd.args[0] = 5;
		cmd.msg = cmds[2];
		ret = sendCmdToSmu(smu, &cmd);
		this->lastProcessedCmd = cmd;
		if (ret != SMUReturnOK) { return SMUReturnFailed; }
		part[1] = cmd.args[0];
		return (uint64_t)part[1] << 32 | part[0];
	}  else if (cmds[1] != 0) {
		cmd.msg = cmds[0];
		ret = sendCmdToSmu(smu, &cmd);
		this->lastProcessedCmd = cmd;
		if (ret != SMUReturnOK) { return SMUReturnFailed; }
		nullAllArgs(&cmd);
		cmd.args[0] = 0;
		cmd.msg = cmds[1];
		ret = sendCmdToSmu(smu, &cmd);
		this->lastProcessedCmd = cmd;
		return ret != SMUReturnOK ? ret : cmd.args[0];
	} else if (cmds[0] != 0) {
		cmd.msg = cmds[0];
		cmd.args[0] = 1;
		cmd.args[1] = 1;
		ret = sendCmdToSmu(smu, &cmd);
		this->lastProcessedCmd = cmd;
		return ret != SMUReturnOK ? ret : cmd.args[0] | ((uint64_t)cmd.args[1] << 32);
	} else {
		return SMUReturnUnsupported;
	}
}

UInt32 AMDPlatformPluginSMUServices::getSmuVersion(SMU *smu) {
	AMDPlatformPlugin::callback->log(0, "%s: grabbing SMU version", __PRETTY_FUNCTION__);
	SMUCmd cmd;
	uint32_t ret;
	cmd.msg = 0x02;
	nullAllArgs(&cmd);
	cmd.args[0] = 1;
	ret = sendCmdToSmu(smu, &cmd);
	if (ret != SMUReturnOK) { return ret; }
	return cmd.args[0];
}

void AMDPlatformPluginSMUServices::dumpServicesState() {
	AMDPlatformPlugin::callback->log(0, "SmuServices State:");
	AMDPlatformPlugin::callback->log(0, "  SmuServicesHaveStarted: 0x%x", this->SmuServicesHaveStarted);
	AMDPlatformPlugin::callback->log(0, "  Last Processed Command:");
	AMDPlatformPlugin::callback->log(0, "    Arg 0: 0x%x", this->lastProcessedCmd.args[0]);
	AMDPlatformPlugin::callback->log(0, "    Arg 1: 0x%x", this->lastProcessedCmd.args[1]);
	AMDPlatformPlugin::callback->log(0, "    Arg 2: 0x%x", this->lastProcessedCmd.args[2]);
	AMDPlatformPlugin::callback->log(0, "    Arg 3: 0x%x", this->lastProcessedCmd.args[3]);
	AMDPlatformPlugin::callback->log(0, "    Arg 4: 0x%x", this->lastProcessedCmd.args[4]);
	AMDPlatformPlugin::callback->log(0, "    Arg 5: 0x%x", this->lastProcessedCmd.args[5]);
	AMDPlatformPlugin::callback->log(0, "    MSG: 0x%x", this->lastProcessedCmd.msg);
	AMDPlatformPlugin::callback->log(0, "    Response: 0x%x", this->lastProcessedCmd.rsp);
	AMDPlatformPlugin::callback->log(0, "  SMU:");
	AMDPlatformPlugin::callback->log(0, "    Version: 0x%x", this->smu.smuVer);
	AMDPlatformPlugin::callback->log(0, "    Interface version: %d", this->smu.smuIf.ifver);
	AMDPlatformPlugin::callback->log(0, "    Currently in use Mailbox: %s", SMUMBType2String(this->smu.smuIf.mbType));
	AMDPlatformPlugin::callback->log(0, "    Mailbox Addresses:");
	AMDPlatformPlugin::callback->log(0, "      Arguments Address: 0x%x", this->smu.smuIf.addrMbArgs);
	AMDPlatformPlugin::callback->log(0, "      Command Address: 0x%x", this->smu.smuIf.addrMbCmd);
	AMDPlatformPlugin::callback->log(0, "      Response Address: 0x%x", this->smu.smuIf.addrMbRsp);
	AMDPlatformPlugin::callback->log(0, "    Last Returned (String): %s", SMUReturn2String(this->smu.lastReturned));
	AMDPlatformPlugin::callback->log(0, "    Last Returned (Hex): 0x%x", this->smu.lastReturned);
	AMDPlatformPlugin::callback->log(0, "    DRAM Base Address: 0x%x", this->smu.dramBaseAddr);
	AMDPlatformPlugin::callback->log(0, "  Platforms:");
	AMDPlatformPlugin::callback->log(0, "    Current: %s", AMDCurrentPlatform2String(AMDPlatformPlugin::callback->currentPlatform));
	AMDPlatformPlugin::callback->log(0, "    APU: %s", AMDAPUPlatforms2String(AMDPlatformPlugin::callback->apuPlatform));
	AMDPlatformPlugin::callback->log(0, "    Desktop: %s", AMDDesktopPlatforms2String(AMDPlatformPlugin::callback->desktopPlatform));
	AMDPlatformPlugin::callback->log(0, "    HEDT: %s", AMDHEDTPlatforms2String(AMDPlatformPlugin::callback->hedtPlatform));
}

SMUReturn AMDPlatformPluginSMUServices::setupSmuServices(SMU *smu) {
	SMUReturn ret;
	
    AMDPlatformPlugin::callback->log(0, "%s: setting up SMU Services", __PRETTY_FUNCTION__);
	if (!AMDPlatformPlugin::callback->setPlatform()) { return SMUReturnFailed; }
	ret = getAddrForMB(smu);
	if (AMDPlatformPlugin::callback->currentPlatform == kAMDPlatformAPU || (AMDPlatformPlugin::callback->desktopPlatform == kDesktopPlatformMatisse || AMDPlatformPlugin::callback->desktopPlatform == kDesktopPlatformVermeer)) {
		AMDPlatformPlugin::callback->log(0, "%s: activating PM table services", __PRETTY_FUNCTION__);
		pmTbl = AMDPMTableHandler::createPMTableHandler();
		pmTbl->setupPmTableServices();
	}
	UInt32 smuv = getSmuVersion(smu);
	if (smuv == 0xFF) {
		AMDPlatformPlugin::callback->log(2, "%s: grabbing SMU version failed!", __PRETTY_FUNCTION__);
		return SMUReturnFailed;
	} else {
		this->smu.smuVer = smuv;
	}
	return ret;
}

bool AMDPlatformPluginSMUServices::start(IOService *provider) {
	IOService::start(provider);
	callback = this;
	if (setupSmuServices(&this->smu) == SMUReturnFailed) {
		AMDPlatformPlugin::callback->log(2, "%s: Failed to setup SMU Services!", __PRETTY_FUNCTION__);
		dumpServicesState();
		return false;
	}
	this->SmuServicesHaveStarted = true;
	
	return true;
}
