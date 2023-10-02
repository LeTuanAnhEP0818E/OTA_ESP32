
## What?
- This is a tool/library for remote (over the air) configure device's parameters and auto update firmware from a remote repo (like github) for esp32 devices
- the main code of this tool is in configOTASecure.h & configOTASecure.cpp
- It can connect to a remote repo over HTTP or HTTPS without checking the certificate of the site
- The security is done by using self sign RSA signature of the config.json --> config.img, and firmware.bin --> firmware.img 
- The public-key for each signature was stored in the devices and can be update later
- Config params and public keys are stored in NVS (non volite storage - in flash memory of the devices)

## Why?
- IoT devices need the ability of OTA firmware update and update the configuration parameters over the air in a convenient and secured way
- The build in lib in arduino core (Update.h) does not have the secure features and the version compare to auto-update over the air
- The other lib like chrisjoyce911's FOTA are not secured enough (some memory leak) and not robust enough, also not includ the secure config feature

## How to use?
- All the code of this tool/library is in ./src folder. 
- The code in main.cpp file is a example use case
- You need to modify the device params, initial/default config params in configOTASecure.h file before compile

