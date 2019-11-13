This folder contains the needed files to code and build Layoff as a windows exe with Visual Studio \
The WinStubs directory should contain code only targeted to windows builds \
Instead of including `switch.h` you should include `CrossSwitch.h`, libnx in this mode is only used for intellisense, all functions must be implemented as stubs in WinStubs.