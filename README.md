# AMDPlatformPlugin ![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/NootInc/AMDPlatformPlugin/main.yml?branch=master&logo=github&style=for-the-badge)

An AMD power management driver.

The AMDPlatformPlugin derivative work/project is licensed under the `Thou Shalt Not Profit License version 1.5`. See `LICENSE`

Thanks to [Acidanthera](https://github.com/acidanthera) for the [WhateverGreen](https://github.com/acidanthera/WhateverGreen) kern_agdc and kern_rad code used in kern_support

You almost certainly need to [add this custom lilu build to your system](https://github.com/Zorm-Industries/Lilu) to avoid kernel panics on 'No iGPU' as for some reason, Lilu's quirk **fails to trigger on older systems**

## FAQ

### Which Processors does this kext support?

It supports all processors compatible with the AMD P-state driver.

### On which macOS versions am I able to use this on?

Right now, most tests are being conducted on macOS Sonoma
