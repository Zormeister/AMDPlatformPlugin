//  Copyright © 2022-2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#pragma once
#include <IOKit/IOService.h>

class IOPlatformPluginFamily : public IOService {
	virtual IOReturn setProperties(OSObject *object) APPLE_KEXT_OVERRIDE;
	virtual IOReturn platformASPMEnable(IOService *service);
	virtual IOReturn platformMaxBusDelay(IOService *service, int delay);
	
	OSMetaClassDeclareReservedUnused(IOPlatformPluginFamily, 0);
	OSMetaClassDeclareReservedUnused(IOPlatformPluginFamily, 1);
	OSMetaClassDeclareReservedUnused(IOPlatformPluginFamily, 2);
	OSMetaClassDeclareReservedUnused(IOPlatformPluginFamily, 3);
	OSMetaClassDeclareReservedUnused(IOPlatformPluginFamily, 4);
	OSMetaClassDeclareReservedUnused(IOPlatformPluginFamily, 5);
	OSMetaClassDeclareReservedUnused(IOPlatformPluginFamily, 6);
	OSMetaClassDeclareReservedUnused(IOPlatformPluginFamily, 7);
	OSMetaClassDeclareReservedUnused(IOPlatformPluginFamily, 8);
	OSMetaClassDeclareReservedUnused(IOPlatformPluginFamily, 9);
	OSMetaClassDeclareReservedUnused(IOPlatformPluginFamily, 10);
	OSMetaClassDeclareReservedUnused(IOPlatformPluginFamily, 11);
	OSMetaClassDeclareReservedUnused(IOPlatformPluginFamily, 12);
};
