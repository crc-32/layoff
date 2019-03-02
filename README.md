# Layoff
A replacement for the default Switch overlay (menu when home is held and the power menu)

**Not yet ready for end-users to install**
## Building

**At the time of writing, the libnx implementations required to build are not in a release, you have to clone master, build libnx and add the path to the nx folder in the `localconfig.mk` file)**

Clone the repository and just `make`, the scripts `makelayeredfs.bat` and `makelayeredfs.sh` generate the folder structure to replace the overlay via atmosphere layeredfs for laziness' sake
Run `git update-index --assume-unchanged localconfig.mk` to avoid annoyances with the file showing as modified

## Features and (currently) planned features
  - Custom overlay menu with the built in old overlay's features
  - Window based UI
  - Custom notifications replacing the built in notifications (for example, low battery, screenshot taken) as well as allowing sysmodules to post notifications
  - Plugin system for additional custom features and implementations by the community
  
  ## Contributors
  - [exelix11](https://github.com/exelix11/)
  - [crc-32](https://github.com/crc-32/)
