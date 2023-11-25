//! Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5.
//! See LICENSE for details.

#include "AMDPlatformPluginACPIInterface.hpp"

OSDefineMetaClassAndStructors(AMDPlatformPluginACPIInterface, IOService);

bool AMDPlatformPluginACPIInterface::start(IOService *provider) {
	this->cpu = OSDynamicCast(IOACPIPlatformDevice, provider);
	if (!this->cpu) {
		IOLog("ERROR: %s: failed to cast provider!", __BASE_FILE__);
		return false;
	}
	
	if (this->cpu->validateObject("_CPC")) {
		IOLog("ERROR: %s: _CPC object could not be found!", __BASE_FILE__);
		return false;
	}
	
	OSObject *obj = nullptr;
	
	this->cpu->evaluateObject("_CPC", &obj);
	
	OSArray *cpc = OSDynamicCast(OSArray, obj);
	if (!cpc) {
		IOLog("ERROR: %s: _CPC did not return a package!", __BASE_FILE__);
		OSSafeReleaseNULL(obj);
		return false;
	}
	
	OSData *entries = OSDynamicCast(OSData, cpc->getObject(0));
	OSData *rev = OSDynamicCast(OSData, cpc->getObject(1));
	OSNumber *highPerf = OSDynamicCast(OSNumber, cpc->getObject(2));
	OSNumber *nomPerf = OSDynamicCast(OSNumber, cpc->getObject(3));
	OSNumber *lowNonLinPerf = OSDynamicCast(OSNumber, cpc->getObject(4));
	OSNumber *lowPerf = OSDynamicCast(OSNumber, cpc->getObject(5));
	OSNumber *guaranteedPerfReg = OSDynamicCast(OSNumber, cpc->getObject(6));
	OSNumber *desiredPerfReg = OSDynamicCast(OSNumber, cpc->getObject(7));
	OSNumber *minPerfReg = OSDynamicCast(OSNumber, cpc->getObject(8));
	OSNumber *maxPerfReg = OSDynamicCast(OSNumber, cpc->getObject(9));
	OSNumber *perfRedTolReg = OSDynamicCast(OSNumber, cpc->getObject(10));
	OSNumber *timeWndwReg = OSDynamicCast(OSNumber, cpc->getObject(11));
	OSNumber *ctrWrapTime = OSDynamicCast(OSNumber, cpc->getObject(12));
	OSNumber *refPerfCtrReg = OSDynamicCast(OSNumber, cpc->getObject(13));
	OSNumber *delivPerfCtrReg = OSDynamicCast(OSNumber, cpc->getObject(14));
	OSNumber *perfLimitedReg = OSDynamicCast(OSNumber, cpc->getObject(15));
	OSNumber *enableReg = OSDynamicCast(OSNumber, cpc->getObject(16));
	OSNumber *autoSelEnable = OSDynamicCast(OSNumber, cpc->getObject(17));
	OSNumber *autoActivityWndwReg = OSDynamicCast(OSNumber, cpc->getObject(18));
	OSNumber *energyPerfPrefReg = OSDynamicCast(OSNumber, cpc->getObject(19));
	OSNumber *refPerf = OSDynamicCast(OSNumber, cpc->getObject(20));
	OSNumber *lowestFreq = OSDynamicCast(OSNumber, cpc->getObject(21));
	OSNumber *nomFreq = OSDynamicCast(OSNumber, cpc->getObject(22));
	
	OSDictionary *dict = OSDictionary::withCapacity(23);
	dict->setObject("Entries", entries);
	dict->setObject("CPC Revision", rev);
	dict->setObject("Highest Performance", highPerf);
	dict->setObject("Nominal Performance", nomPerf);
	dict->setObject("Lowest Nonlinear Performance", lowNonLinPerf);
	dict->setObject("Lowest Performance", lowPerf);
	dict->setObject("Guaranteed Performance Register", guaranteedPerfReg);
	dict->setObject("Desired Performance Register", desiredPerfReg);
	dict->setObject("Minimum Performance Register", minPerfReg);
	dict->setObject("Maximum Performance register", maxPerfReg);
	dict->setObject("Performance Reduction Tolerance Register", perfRedTolReg);
	dict->setObject("Time Window Register", timeWndwReg);
	dict->setObject("Counter Wraparound Time", ctrWrapTime);
	dict->setObject("Reference Performance Counter Register", refPerfCtrReg);
	dict->setObject("Delivered Performance Counter Register", delivPerfCtrReg);
	dict->setObject("Performance Limited Register", perfLimitedReg);
	dict->setObject("CPPC Enable Register", enableReg);
	dict->setObject("Autonomous Select Enable", autoSelEnable);
	dict->setObject("Autonomous Activity Window Register", autoActivityWndwReg);
	dict->setObject("Energy Performance Preference Register", energyPerfPrefReg);
	dict->setObject("Reference Performance", refPerf);
	dict->setObject("Lowest Frequency", lowestFreq);
	dict->setObject("Nominal Frequency", nomFreq);
	
	provider->setProperty("CPC Package Dict", dict);
	provider->setProperty("CPC Array", cpc);
	
	return true;
}
