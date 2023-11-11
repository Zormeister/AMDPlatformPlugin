//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#include "AMDPlatformPluginSMUServices.hpp"
#include "AMDPMTableServices.hpp"
#include "AMDPlatformPlugin.hpp"
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
    amdpp->log(0, "%s: Sending command to SMU, MSG: %x, Args: 0: %x, 1: %x, 2: %x, 3: %x, 4: %x, 5: %x",
        __PRETTY_FUNCTION__, cmd->msg, cmd->args[0], cmd->args[1], cmd->args[2], cmd->args[3], cmd->args[4],
        cmd->args[5]);
    uint32_t tmp;
    uint32_t retries = 500;

    UInt32 argsAddr;
    UInt32 msgAddr;
    UInt32 rspAddr;
    // this is to fixup values left behind by both funcs in the case we
    // need to update Mailbox params.
    switch (cmd->toMailbox) {
        case SMUMailbox::MP1:
            argsAddr = platform->mp1->arguments;
            msgAddr = platform->mp1->command;
            rspAddr = platform->mp1->response;
        case SMUMailbox::RSMU:
            argsAddr = platform->rsmu->arguments;
            msgAddr = platform->rsmu->command;
            rspAddr = platform->rsmu->response;
        case SMUMailbox::HSMP:
            argsAddr = platform->hsmp->arguments;
            msgAddr = platform->hsmp->command;
            rspAddr = platform->hsmp->response;
    }
    do { tmp = readFromSmu(&this->smu, rspAddr); } while (tmp == 0 && retries--);
    if (!tmp && !retries) {
        amdpp->log(2, "%s: 	Timed out whilst waiting for the mailbox to be available", __PRETTY_FUNCTION__);
        return SMUReturnFailed;
    }
    writeToSmu(smu, rspAddr, 0);
    for (int i = 0; i < 6; i++) { writeToSmu(smu, argsAddr + (i * 4), cmd->args[i]); }
    writeToSmu(smu, msgAddr, cmd->msg);
    retries = 500;
    do { tmp = readFromSmu(&this->smu, rspAddr); } while (tmp == 0 && retries--);
    if (tmp != SMUReturnOK && !retries) {
        if (!tmp) {
            amdpp->log(2, "%s: 	Timed out whilst waiting for the SMU to respond", __PRETTY_FUNCTION__);
            return SMUReturnFailed;
        }
        amdpp->log(2, "%s: SMU response was not SMUReturnOK! Response: %xh", __PRETTY_FUNCTION__, tmp);
    }
    for (int i = 0; i < 6; i++) { cmd->args[i] = readFromSmu(smu, argsAddr + (i * 4)); }
    amdpp->log(0, "%s: SMU has responded!", __PRETTY_FUNCTION__);
    amdpp->log(0, "%s: SMU response: MSG: Args: 0: %x, 1: %x, 2: %x, 3: %x 4: %x, 5: %x", __PRETTY_FUNCTION__, cmd->msg,
        cmd->args[0], cmd->args[1], cmd->args[2], cmd->args[3], cmd->args[4], cmd->args[5]);
    return SMUReturnOK;
}

void AMDPlatformPluginSMUServices::nullAllArgs(SMUCmd *cmd) {
    for (int i = 1; i < 6; i++) { cmd->args[i] = 0; }
}

uint64_t AMDPlatformPluginSMUServices::getSmuDramBaseAddr(SMU *smu) {
    amdpp->log(0, "%s: grabbing SMU DRAM base address", __PRETTY_FUNCTION__);
    uint32_t ret, part[2], cmds[3];
    SMUCmd cmd;

    cmds[0] = 0;
    cmds[1] = 0;
    cmds[2] = 0;

    switch (amdpp->apuPlatform) {
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
    switch (amdpp->desktopPlatform) {
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
    switch (amdpp->hedtPlatform) {
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
    cmd.toMailbox = SMUMailbox::RSMU;    // no equiv on MP1? needs investigation.
    if (cmds[2]) {
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
    } else if (cmds[1]) {
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
    } else if (cmds[0]) {
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
    amdpp->log(0, "%s: grabbing SMU version", __PRETTY_FUNCTION__);
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
    amdpp->log(0, "  SmuServicesHaveStarted: %d", this->SmuServicesHaveStarted);
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
    amdpp->log(0, "    Last Returned (String): %s", SMUReturn2String(this->smu.lastReturned));
    amdpp->log(0, "    Last Returned (Hex): 0x%x", this->smu.lastReturned);
    amdpp->log(0, "    DRAM Base Address: 0x%x", this->smu.dramBaseAddr);
    amdpp->log(0, "  Platform:");
    amdpp->log(0, "    Current: %s", AMDCurrentPlatform2String(amdpp->currentPlatform));
    amdpp->log(0, "    APU: %s", AMDAPUPlatforms2String(amdpp->apuPlatform));
    amdpp->log(0, "    Desktop: %s", AMDDesktopPlatforms2String(amdpp->desktopPlatform));
    amdpp->log(0, "    HEDT: %s", AMDHEDTPlatforms2String(amdpp->hedtPlatform));
}

SMUReturn AMDPlatformPluginSMUServices::setupSmuServices(SMU *smu) {
    SMUReturn ret;

    amdpp->log(0, "%s: setting up SMU Services", __PRETTY_FUNCTION__);
    if (!amdpp->setPlatform()) { return SMUReturnFailed; }
    if (amdpp->currentPlatform == kAMDPlatformAPU ||
        (amdpp->desktopPlatform == kDesktopPlatformMatisse || amdpp->desktopPlatform == kDesktopPlatformVermeer)) {
        amdpp->log(0, "%s: activating PM table services", __PRETTY_FUNCTION__);
        pmTbl = AMDPMTableServices::createPMTableServices();
        pmTbl->setupPmTableServices();
    }
    UInt32 smuv = getSmuVersion(smu);
    if (smuv == 0xFF) {
        amdpp->log(2, "%s: grabbing SMU version failed!", __PRETTY_FUNCTION__);
        return SMUReturnFailed;
    } else {
        this->smu.smuVer = smuv;
    }
    ret = SMUReturnOK;
    return ret;
}

bool AMDPlatformPluginSMUServices::start(IOService *provider) {
    IOService::start(provider);
    OSDictionary *res;
    do { res = IOService::serviceMatching("AMDPlatformPlugin"); } while (!res);
    auto iter = IOService::getMatchingServices(res);
    this->amdpp = OSDynamicCast(AMDPlatformPlugin, iter->getNextObject());
    iter->release();

    if (setupSmuServices(&this->smu) == SMUReturnFailed) {
        amdpp->log(2, "%s: Failed to setup SMU Services!", __PRETTY_FUNCTION__);
        dumpServicesState();
        return false;
    }
    this->SmuServicesHaveStarted = true;

    return true;
}

AMDPMTableServices *AMDPlatformPluginSMUServices::getPMTableServices() { return this->pmTbl; }
