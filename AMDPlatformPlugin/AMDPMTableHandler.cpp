//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#include "AMDPMTableHandler.hpp"
#include "AMDPlatformPluginSMUServices.hpp"
#include <IOKit/IOTypes.h>

bool AMDPMTableHandler::init() {
    callback = this;
    return true;
}

AMDPMTableHandler *AMDPMTableHandler::createPMTableHandler() {
	auto *pm = new AMDPMTableHandler{};
    if (!pm) { return nullptr; }
    if (!pm->init()) { return pm; }
	pm->release();
    return nullptr;
}

uint32_t AMDPMTableHandler::getPmTableVersion() {
    SMUCmd cmd;
    switch (AMDPlatformPlugin::callback->apuPlatform) {
        case kAPUPlatformRaven:
        case kAPUPlatformRaven2:
        case kAPUPlatformPicasso:
        case kAPUPlatformDali:
            cmd.msg = 0x0C;
            break;
        case kAPUPlatformRenoir:
        case kAPUPlatformCezanne:
        case kAPUPlatformLucienne:
            cmd.msg = 0x06;
            break;
        default:
            break;
    }
    switch (AMDPlatformPlugin::callback->desktopPlatform) {
        case kDesktopPlatformMatisse:
        case kDesktopPlatformVermeer:
            cmd.msg = 0x08;
            break;
        default:
            break;
    }
    AMDPlatformPluginSMUServices::callback->nullAllArgs(&cmd);
    cmd.args[0] = 0;
    cmd.sendToRSMU = true;
    AMDPlatformPluginSMUServices::callback->sendCmdToSmu(&AMDPlatformPluginSMUServices::callback->smu, &cmd);

    return cmd.args[0];
}

uint32_t AMDPMTableHandler::sendPmTableToDram() {
    SMUCmd cmd;
    cmd.msg = 0;
    AMDPlatformPluginSMUServices::callback->nullAllArgs(&cmd);
    cmd.args[0] = 0;

    switch (AMDPlatformPlugin::callback->apuPlatform) {
        case kAPUPlatformRaven:
        case kAPUPlatformRaven2:
        case kAPUPlatformPicasso:
        case kAPUPlatformDali:
            cmd.msg = 0x3D;
            cmd.args[0] = 3;
            break;
        case kAPUPlatformRenoir:
            cmd.msg = 0x65;
            cmd.args[0] = 3;
            break;
        case kAPUPlatformCezanne:
        case kAPUPlatformLucienne:
            cmd.msg = 0x65;
            break;
        default:
            break;
    }
    switch (AMDPlatformPlugin::callback->desktopPlatform) {
        case kDesktopPlatformMatisse:
        case kDesktopPlatformVermeer:
            cmd.msg = 0x05;
            break;
        default:
            break;
    }
    cmd.sendToRSMU = true;

    if (cmd.msg == 0) { return SMUReturnUnsupported; }

    return AMDPlatformPluginSMUServices::callback->sendCmdToSmu(&AMDPlatformPluginSMUServices::callback->smu, &cmd);
}

uint32_t AMDPMTableHandler::getPmTableSize() {
    switch (AMDPlatformPlugin::callback->apuPlatform) {
        case kAPUPlatformRaven:
        case kAPUPlatformRaven2:
        case kAPUPlatformPicasso:
        case kAPUPlatformDali:
            this->pmTbl.dramMapSize2 = 0xA4;
            this->pmTbl.dramMapSize = 0x608 + this->pmTbl.dramMapSize2;
            this->dramBaseAddr2 = this->dramBaseAddr >> 32;
            this->dramBaseAddr &= 0xFFFFFFFF;
            break;
        case kAPUPlatformRenoir:
            switch (this->pmTbl.pmTableVersion) {
                case 0x37000000:
                    this->pmTbl.dramMapSize = 0x794;
                    break;
                case 0x370001:
                    this->pmTbl.dramMapSize = 0x884;
                    break;
                case 0x370002:
                case 0x370003:
                    this->pmTbl.dramMapSize = 0x88C;
                    break;
                case 0x370004:
                    this->pmTbl.dramMapSize = 0x8AC;
                    break;
                case 0x370005:
                    this->pmTbl.dramMapSize = 0x8C8;
                    break;
                default:
                    return SMUReturnUnsupported;
            }
            break;
        case kAPUPlatformCezanne:
            switch (this->pmTbl.pmTableVersion) {
                case 0x400005:
                    this->pmTbl.dramMapSize = 0x944;
                    break;
                default:
                    return SMUReturnUnsupported;
            }
            break;
        default:
			return SMUReturnUnsupported;
    }
    switch (AMDPlatformPlugin::callback->desktopPlatform) {
        case kDesktopPlatformMatisse:
            switch (this->pmTbl.pmTableVersion) {
                case 0x240902:
                    this->pmTbl.dramMapSize = 0x514;
                    break;
                case 0x240903:
                    this->pmTbl.dramMapSize = 0x518;
                    break;
                case 0x240802:
                    this->pmTbl.dramMapSize = 0x7E0;
                    break;
                case 0x240803:
                    this->pmTbl.dramMapSize = 0x7E4;
                    break;
                default:
                    return SMUReturnUnsupported;
            }
            break;
        case kDesktopPlatformVermeer:
            switch (this->pmTbl.pmTableVersion) {
                case 0x2D0903:
                    this->pmTbl.dramMapSize = 0x594;
                    break;
                case 0x380904:
                    this->pmTbl.dramMapSize = 0x5A4;
                    break;
                case 0x380905:
                    this->pmTbl.dramMapSize = 0x5D0;
                    break;
                case 0x2D0803:
                    this->pmTbl.dramMapSize = 0x894;
                    break;
                case 0x380804:
                    this->pmTbl.dramMapSize = 0x8A4;
                    break;
                case 0x380805:
                    this->pmTbl.dramMapSize = 0x8F0;
                    break;
                default:
                    return SMUReturnUnsupported;
            }
            break;
        default:
			return SMUReturnUnsupported;
    }
    return 0;
}

uint32_t AMDPMTableHandler::setupPmTableServices() {
    uint64_t dramBaseAddr =
        AMDPlatformPluginSMUServices::callback->getSmuDramBaseAddr(&AMDPlatformPluginSMUServices::callback->smu);
    if (dramBaseAddr == 0xFF) {
        return SMUReturnFailed;
    } else if (dramBaseAddr == 0xFB) {
        AMDPlatformPlugin::callback->log(1, "DRAM Base address grabbing appears to be unsupported.");
        return SMUReturnUnsupported;
    }
    this->dramBaseAddr = dramBaseAddr;
    this->pmTbl.pmTableVersion = getPmTableVersion();
    sendPmTableToDram();
	if (!getPmTableSize()) {
		AMDPlatformPlugin::callback->log(0, "%s: preparing PM table", __PRETTY_FUNCTION__);
		if (this->pmTbl.dramMapPtr == nullptr) {
			AMDPlatformPlugin::callback->log(0, "%s: mapping DRAM", __PRETTY_FUNCTION__);
		}
	}
    return true;
}
