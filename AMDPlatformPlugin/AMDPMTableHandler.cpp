//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#include "AMDPMTableHandler.hpp"
#include "AMDPlatformPluginSMUServices.hpp"

bool AMDPMTableHandler::init() {
	callback = this;
	return true;
}

AMDPMTableHandler *AMDPMTableHandler::createPMTableHandler() {
	auto PM = AMDPMTableHandler::operator new(sizeof(AMDPMTableHandler));
	auto pmTbl = (AMDPMTableHandler *)PM;
	if (pmTbl == nullptr) {
		return nullptr;
	}
	if (!pmTbl->init()) {
		pmTbl->release();
	}
	return (AMDPMTableHandler *)PM;
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
	AMDPlatformPluginSMUServices::callback->sendCmdToSmu(&AMDPlatformPluginSMUServices::callback->smu, &cmd);
	
	return cmd.args[0];
}

bool AMDPMTableHandler::setupPmTableServices() {
	uint64_t dramBaseAddr = AMDPlatformPluginSMUServices::callback->getSmuDramBaseAddr(&AMDPlatformPluginSMUServices::callback->smu);
	if (dramBaseAddr == 0xFF) { return SMUReturnFailed; }
	else if (dramBaseAddr == 0xFB) { AMDPlatformPlugin::callback->log(1, "DRAM Base address grabbing appears to be unsupported."); }
	return true;
}
