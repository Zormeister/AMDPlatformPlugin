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

SMUReturn AMDPlatformPluginSMUServices::sendCmdToSmu(SMU *smu, SMUCmd *cmd) {
	amdpp->log(0, "%s: Sending command to SMU, MSG: %x, Args: 0: %x, 1: %x, 2: %x, 3: %x, 4: %x, 5: %x", __PRETTY_FUNCTION__, cmd->msg, cmd->args[0], cmd->args[1], cmd->args[2], cmd->args[3], cmd->args[4], cmd->args[5]);
	uint32_t tmp;
	uint32_t retries = 500;
	do { tmp = readFromSmu(&this->smu, smu->addrMbRsp); } while (tmp == 0 && retries--);
	if (!tmp && !retries) {
		amdpp->log(2, "%s: 	Timed out whilst waiting for the mailbox to be available", __PRETTY_FUNCTION__);
		return SMUReturnFailed;
	}
	writeToSmu(smu, smu->addrMbRsp, 0);
	for (int i = 0; i < 6; i++) { writeToSmu(smu, smu->addrMbArgs + (i * 4), cmd->args[i]); }
	writeToSmu(smu, smu->addrMbCmd, cmd->msg);
	retries = 500;
	do { tmp = readFromSmu(&this->smu, smu->addrMbRsp); } while (tmp == 0 && retries--);
	if (tmp != SMUReturnOK && !retries) {
		if (!tmp) {
			amdpp->log(2, "%s: 	Timed out whilst waiting for the SMU to respond", __PRETTY_FUNCTION__);
			return SMUReturnFailed;
		}
		amdpp->log(2, "%s: SMU response was not SMUReturnOK! Response: %xh", __PRETTY_FUNCTION__, tmp);
	}
	for (int i = 0; i < 6; i++) { cmd->args[i] = readFromSmu(smu, smu->addrMbArgs + (i * 4)); }
	amdpp->log(0, "%s: SMU has responded!", __PRETTY_FUNCTION__);
	amdpp->log(0, "%s: SMU response: MSG: Args: 0: %x, 1: %x, 2: %x, 3: %x 4: %x, 5: %x", __PRETTY_FUNCTION__, cmd->msg, cmd->args[0], cmd->args[1], cmd->args[2], cmd->args[3], cmd->args[4], cmd->args[5]);
	return SMUReturnOK;
}

void AMDPlatformPluginSMUServices::nullAllArgs(SMUCmd *cmd) {
	for (int i = 1; i < 6; i++) { cmd->args[i] = 0; }
}

SMUReturn AMDPlatformPluginSMUServices::getAddrForMB(SMU *smu) {
	switch (amdpp->apuPlatform) {
		case Raven:
		case Raven2:
		case Picasso:
		case Dali:
			smu->ifver = V10;
			smu->addrMbArgs = 0x3B10998;
			smu->addrMbCmd = 0x3B10528;
			smu->addrMbRsp = 0x3B10564;
			break;
		case Renoir:
		case Lucienne:
		case Cezanne:
			smu->ifver = V12;
			smu->addrMbArgs = 0x3B10998;
			smu->addrMbCmd = 0x3B10528;
			smu->addrMbRsp = 0x3B10564;
			break;
		case VanGogh:
		case Rembrandt:
		case Mendocino:
			smu->ifver = V13;
			smu->addrMbArgs = 0x3B10998;
			smu->addrMbCmd = 0x3B10528;
			smu->addrMbRsp = 0x3B10564;
			break;
		default:
			break;
	}
	switch (amdpp->desktopPlatform) {
		case SummitRidge:
		case PinnacleRidge:
			smu->ifver = V9;
			smu->addrMbArgs = 0x3B10598;
			smu->addrMbCmd = 0x3B10528;
			smu->addrMbRsp = 0x3B10564;
			break;
		case Matisse:
		case Vermeer:
			smu->ifver = V11;
			smu->addrMbArgs = 0x3B109C4;
			smu->addrMbCmd = 0x3B10530;
			smu->addrMbRsp = 0x3B1057C;
			break;
		default:
			break;
	}
	switch (amdpp->hedtPlatform) {
		case Whitehaven:
		case Colfax:
			smu->ifver = V9;
			smu->addrMbArgs = 0x3B10598;
			smu->addrMbCmd = 0x3B10528;
			smu->addrMbRsp = 0x3B10564;
			break;
		case CastlePeak:
			smu->ifver = V11;
			smu->addrMbArgs = 0x3B109C4;
			smu->addrMbCmd = 0x3B10530;
			smu->addrMbRsp = 0x3B1057C;
			break;
		default:
			break;
	}
	if (smu->addrMbArgs == 0 || smu->addrMbCmd == 0 || smu->addrMbRsp == 0) {
		amdpp->log(2, "%s: one of our Addresses are null!", __PRETTY_FUNCTION__);
		return SMUReturnFailed;
	}
	return SMUReturnOK;
}

UInt32 AMDPlatformPluginSMUServices::getSmuVersion(SMU *smu) {
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
	amdpp->log(0, "SmuServices State:");
	amdpp->log(0, "  SmuServicesHaveStarted: 0x%x", this->SmuServicesHaveStarted);
	amdpp->log(0, "  Last Processed Command:");
	amdpp->log(0, "    Arg 0: 0x%x", this->lastProcessedCmd.args[0]);
	amdpp->log(0, "    Arg 1: 0x%x", this->lastProcessedCmd.args[1]);
	amdpp->log(0, "    Arg 2: 0x%x", this->lastProcessedCmd.args[2]);
	amdpp->log(0, "    Arg 3: 0x%x", this->lastProcessedCmd.args[3]);
	amdpp->log(0, "    Arg 4: 0x%x", this->lastProcessedCmd.args[4]);
	amdpp->log(0, "    Arg 5: 0x%x", this->lastProcessedCmd.args[5]);
	amdpp->log(0, "    MSG: 0x%x", this->lastProcessedCmd.msg);
	amdpp->log(0, "    Response: 0x%x", this->lastProcessedCmd.rsp);
	amdpp->log(0, "  SMU:");
	amdpp->log(0, "    Version: 0x%x", this->smu.smuVer);
	amdpp->log(0, "    Interface version: %d", this->smu.ifver);
	amdpp->log(0, "    Mailbox Addresses:");
	amdpp->log(0, "      Arguments Address: 0x%x", this->smu.addrMbArgs);
	amdpp->log(0, "      Command Address: 0x%x", this->smu.addrMbCmd);
	amdpp->log(0, "      Response Address: 0x%x", this->smu.addrMbRsp);
	amdpp->log(0, "    Last Returned (String): %s", SMUReturn2String(this->smu.lastReturned));
	amdpp->log(0, "    Last Returned (Hex): %x", this->smu.lastReturned);
	amdpp->log(0, "  Platforms:");
	amdpp->log(0, "    Current: %s", AMDCurrentPlatform2String(amdpp->currentPlatform));
	amdpp->log(0, "    APU: %s", AMDAPUPlatforms2String(amdpp->apuPlatform));
	amdpp->log(0, "    Desktop: %s", AMDDesktopPlatforms2String(amdpp->desktopPlatform));
	amdpp->log(0, "    HEDT: %s", AMDHEDTPlatforms2String(amdpp->hedtPlatform));
}

SMUReturn AMDPlatformPluginSMUServices::setupSmuServices(SMU *smu) {
	SMUReturn ret;
	
    amdpp->log(0, "%s: setting up SMU Services", __PRETTY_FUNCTION__);
	if (!amdpp->setPlatform()) { return SMUReturnFailed; }
	ret = getAddrForMB(smu);
	if (amdpp->currentPlatform == APU) {
		amdpp->log(0, "%s: activating PM table services", __PRETTY_FUNCTION__);
	}
	UInt32 smuv = getSmuVersion(smu);
	if (smuv == 0xFF) {
		amdpp->log(2, "%s: grabbing SMU version failed!", __PRETTY_FUNCTION__);
		return SMUReturnFailed;
	} else {
		this->smu.smuVer = smuv;
	}
	return ret;
}

bool AMDPlatformPluginSMUServices::start(IOService *provider) {
	IOService::start(provider);
	auto PP = IOService::serviceMatching("AMDPlatformPlugin");
	this->amdpp = OSDynamicCast(AMDPlatformPlugin, PP);
	if (setupSmuServices(&this->smu) == SMUReturnFailed) {
		amdpp->log(2, "%s: Failed to setup SMU Services!", __PRETTY_FUNCTION__);
		dumpServicesState();
		return false;
	}
	this->SmuServicesHaveStarted = true;
	
	
	return true;
}
