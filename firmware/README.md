# Firmware
This directory contains various firmware resources for the CH32V003 LCD NAV board.
It relies on CNLohr's CH32Fun build system and you'll need to initialize it with
the following commands:

```
git submodule init
git submodule update
```
which will pull in the required repository. Then just drop down into the projects
you're interested in and build them:
```
cd nl_irscope
make
```

## USB
In order to use the USB functionality you'll need to build code in the
rv003usb repo. I have a fork + branch of this with demo code showing how to
use the 5-way nav switch as a HID gamepad. Look for it here:

[rv003usb repo](https://github.com/emeb/rv003usb/tree/bubble/nl_gamepad)
