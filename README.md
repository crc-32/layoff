# Layoff
A replacement for the default Switch overlay (menu when home is held and the power menu)

**Not yet ready for end-users to install**
## Building
Clone the repository and run for first time setup:
```
git submodule init
git submodule update --recursive --remote
cd libnx
make
cd ..
```
Then just `make`, the scripts `makelayeredfs.bat` and `makelayeredfs.sh` generate the folder structure to replace the overlay via atmosphere layeredfs for laziness' sake

## Features and (currently) planned features
  - Custom overlay menu with the built in old overlay's features reimplemented
  - Custom notifications replacing the built in notifications (for example, low battery, screenshot taken) as well as allowing sysmodules to post notifications
  - Plugin system for additional custom features and implementations by the community, such as controlling and displaying information from sysmodules
  
  ## Contributors
  - [exelix11](https://github.com/exelix11/)
  - [crc-32](https://github.com/crc-32/)
