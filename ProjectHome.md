# What is Adapt-FFB-Joy #

Adapt-FFB-Joy is an AVR micro-controller based device that looks like a joystick with advanced force feedback features in a Windows machine without need for installing any device drivers to PC.

Now already, it allows connecting a Microsoft Sidewinder Force Feedback Pro (FFP) joystick (with a game port connector) to Windows XP/Vista/7/8 (32-bit and 64-bit) as a standard **USB joystick with force feedback** and **no need to install any device drivers**. The adapter also allows to solder a few additional trim pots to work e.g. as elevator trims, aileron trims and rudder pedals in your favorite simulator game.

The adapter has been extensively play tested to work with e.g. IL-2 combat flight simulator. It has also been seen working with other games like Mechwarrior, Rise of Flight, World of Warplanes and War Thunder.

Work is going on to extend the support to MS Sidewinder Force Feedback Wheel also.

# Build It Yourself #

[Here are the building instructions](http://code.google.com/p/adapt-ffb-joy/wiki/HowToBuild) with images and circuit diagrams. This project offers instructions only. If you want someone else to build it for you, you need to look for volunteers outside of these project pages - sorry.

# About the Project #

Documentation and code for adapting various force feedback (FFB) effect protocols to USB-devices. Useful for pointing devices like joysticks.

The project's Wiki contains:
  * [Project Plan](http://code.google.com/p/adapt-ffb-joy/wiki/Project) and to-do-list
  * [Links to related information](http://code.google.com/p/adapt-ffb-joy/wiki/RelatedInfo)  to get you started
  * [Document of MS Sidewinder force feedback MIDI protocol](http://code.google.com/p/adapt-ffb-joy/wiki/SidewinderFFBMIDI) as of been reverse engineered
  * [USB HID descriptor with force feedback](http://code.google.com/p/adapt-ffb-joy/wiki/USBFfbHid) development page

The first implementation targeted Microsoft Sidewinder Force Feedback Pro (FFP) joystick and integrated it with [3DP-Vert-project](http://code.google.com/p/sw3dprousb) to make it a full featured USB-adapter for the FFP. Thus, the microcontroller platform of choice is now AVR (Teensy 2.0, in fact), although most of the code and documentation should be made easily portable to other platforms too. There is already another project to make [the adapter as a library for Arduino](http://code.google.com/p/sidewinder-arduino/).

The secondary target is to create documentation and even some framework for adapting any force feedback joystick (and other input devices) to USB using built-in Windows drivers to make them a real plug-and-play device.

There is also a development effort put to make the device configurable by e.g. a standard serial terminal application. This is achieved by making the adapter device a composite  USB device having both joystick and virtual serial devices. Now the device can be configured to disable/enable certain force feedback effects. The adapter can also be somewhat debugged using the virtual serial line.

**As Google is now in progress of shutting down Google Code, this project will be moved to another location soon to be disclosed.**