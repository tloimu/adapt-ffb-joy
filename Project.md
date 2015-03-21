# Project Plan and Status #

This page contains the project's plan and status. It lists what is still missing.

# What works #

It is plug-n-play for Windows (i.e. doesn't need any non-built-in drivers) as it conforms to use Windows' PID built-in driver. Works all the way from 32-bit Windows XP to 32-bit and 64-bit Windows 7 and 8.

All effect types work using e.g. FEdit. Also, modifying effects already playing on-fly work almost as much as FFP allows them. There is an issue with ramps still and they are effectively played as constant forces.

Technology stack is using
  * AVR Teensy 2.0 for hardware
  * 3DPVert for reading Sidewinder joystick
  * LUFA for USB
  * USB descriptors for Human Interface Device with Physical Interface Device parts for force feedback compatible with Windows built-in HID and PID drivers

The USB descriptor has
  * Constant, Sine and Triangle waveforms
  * Duration parameter
  * Direction parameter(s)
  * Periodic parameters (include frequency and amplitude, exclude Phase)
  * Ramp and envelope parameters for attack and fade

# To Do #

Things to do still.

## Support for Sidewinder Force Feedback Wheel ##

The work is on-going and progressing quite well even tho there is still a lot of details to work on.

## Debugging and configuration of adapter via USB serial ##

A preliminary version is in subversion now. The adapt-ffb-joy has now two USB interfaces, one for the HID joystick and another for a virtual serial port.

Configuration via USB virtual serial line now only allows enabling/disabling effects by their effect ID. The serial line also allows printing current effects in the adapter.

Configurability should be extended to include
  * Store/load configuration to EEPROM for persistence
  * Multiple configuration sets or "profiles" in EEPROM
  * Allow using external pins to choose an active configuration set
  * Modify device gain for force feedback effects
  * Modify parameters of force feedback effects
  * Choose whether two analog inputs are used as a separate sliders or combined together to form a single rudder in Windows (now rudder-option is hard coded)
  * Enable/disable debugging
  * Enable/disable force feedback effects or certain type

## Code Cleanup for Modularity and Portability ##

Now that most of the code works, it should be refactored to be more modular and more easily portable to other platforms.