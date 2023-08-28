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

class IOPlatformPluginFamilyPriv : public IOPlatformPluginFamily {};
