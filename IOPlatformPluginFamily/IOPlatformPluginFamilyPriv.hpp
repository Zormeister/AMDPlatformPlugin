//  Copyright © 2022-2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5. See LICENSE for
//  details.

#pragma once
#include "IOPlatformPluginFamily.hpp"

enum {
	kIOPPFSleepFactorSSD,
	kIOPPFSleepFactorHibernateModeOn,
	kIOPPFSleepFactorHibernateModeSleep,
	kIOPPFSleepFactorSleepTimerWake,
	kIOPPFSleepFactorLastSleepS3,
	kIOPPFSleepFactorLastSleepStandby,
	kIOPPFSleepFactorLastSleepErP,
	kIOPPFSleepFactorLastSleepHibernate,
	kIOPPFSleepFactorAtMostTwoExternalHIDDevices,
	kIOPPFSleepFactorAtMostOneExternalDisplay,
	kIOPPFSleepFactorErPPolling,
	kIOPPFSleepFactorErPNoDelay,
	kIOPPFSleepFactorRTCAlarmScheduled,
	kIOPPFSleepFactorLastSleepDeepIdle,
};

class IOPlatformPluginFamilyPriv : public IOPlatformPluginFamily {
	OSDeclareAbstractStructors(IOPlatformPluginFamilyPriv);
	
	virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
	virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;
	IOReturn initSymbols();
	IOReturn setProperties(OSObject *object) APPLE_KEXT_OVERRIDE;
	void releaseSymbols();
	IOReturn pluginGetDiagProperty(OSSymbol *sym, OSSymbol *something);
	IOReturn pluginSetDiagProperty(OSSymbol *sym, OSSymbol *something, OSObject *plugin);
	void pluginRemoveDiagProperty(OSSymbol *sym, OSSymbol *something);
};
