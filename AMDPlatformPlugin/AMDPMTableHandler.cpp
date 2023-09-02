//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#include "AMDPMTableHandler.hpp"

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
