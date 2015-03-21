# Introduction #

Version: 3 (11.3.2012)

A completely new HID descriptor is in use and fixes all of the issues below. The new source code is of version "proto 2" and available at the download page.

# Details #

This is tested in a Teensy 2 (ATmega32U4) with WinAVR-20100110 (you need this or newer version) and LUFA-120219-BETA (need this or newer version too).

If debugging has been enabled (in debug.c, set the constant const <i>uint8_t FFB_ENABLE_DEBUG = 1</i>) the device sends debugging data to UART TX pin in a format that allows string and binary data dumps:
  * Strings start with byte value "0" and end to a terminating NULL
  * Binary data block starts with non-zero byte that indicates how many bytes of binary data is included in the following bytes
  * All sent and received reports data is printed except INPUT Report ID 1 i.e. the joystick position report (because it is very verbose and content is trivial and non-interesting in terms of FFB)

Enabling debugging messes up the data going to the joystick and thus, you will not see the joystick working well then.

# Descriptors and Code #

Here are the descriptors and code. Code is based on LUFA's joystick class device sample and USB.org's HID PID descriptor with minor modifications as to adhere to Microsoft recommendations (e.g. position axis logical and physical minimums and maximums should not be negative).

### Makefile ###

The sample's makefile needs a couple of changes to enable support for large HIDs required by FFB. See that the compiler options are:

```
# Place -D or -U options here for C sources
CDEFS  = -DF_CPU=$(F_CPU)UL
CDEFS += -DF_USB=$(F_USB)UL
# Start of changes -->
CDEFS += -DHID_USAGE_STACK_DEPTH=16
CDEFS += -DHID_MAX_COLLECTIONS=35
CDEFS += -DHID_MAX_REPORT_IDS=20
CDEFS += -DHID_MAX_REPORTITEMS=35
# <-- End of changes
CDEFS += -DBOARD=BOARD_$(BOARD) -DARCH=ARCH_$(ARCH)
CDEFS += $(LUFA_OPTS)
```

and that 16MHz clock speed is selected.

### Device and HID Descriptors ###

See the source code for "proto 2" in the download section.