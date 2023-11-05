# What is Adapt-FFB-Joy #

Adapt-FFB-Joy is an AVR microcontroller based device that looks like a joystick with advanced force feedback features in a Windows machine without need for installing any device drivers to PC.

This project contains the software for the AVR microcontroller as well as basic instructions for [building the hardware](https://github.com/tloimu/adapt-ffb-joy/blob/wiki/HowToBuild.md).

Currently, it allows connecting a Microsoft Sidewinder Force Feedback Pro (FFP) joystick (with a game port connector) to various MS Windows versions as a standard **USB joystick with force feedback** and **no need to install any device drivers**. The adapter also allows to solder a few additional trim pots to work e.g. as elevator trims, aileron trims and rudder pedals in your favorite simulator game.

For more information, see [Adapt-ffb-joy Wiki](https://github.com/tloimu/adapt-ffb-joy/blob/wiki/README.md)

The firmware software project is configured to compile for ATmega32U4 with WinAVR-20100110 or newer version.

## Firmware Version History ##
[v0.5.0beta1](https://github.com/tloimu/adapt-ffb-joy/releases/tag/0.5.0beta1) - Upgrades to FFP Force Feedback capability for wider compatibility with games 11/2023; currently in beta

v0.4.0 (=r74 on original Google Code site) - Allows for future support for the Sidewinder FF Wheel and changes the USB debug protocol. Not released here, but code used for many other forks.

[v0.3.0 (=r54)](https://github.com/tloimu/adapt-ffb-joy/blob/master/downloads/adaptffbjoy-0.3.0(r54).hex) - Released _r54 hex
