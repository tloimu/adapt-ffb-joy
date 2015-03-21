# Related Background Information #

This page gathers interesting and potentially useful FFB and FFB joystick related links and information. Hopefully it gets anyone interested into a good start.

## USB HID ##

Here are links to some basic background information:
  * [Device Class Definition for Human Interface Devices (HID)](http://www.usb.org/developers/hidpage/HID1_11.pdf)
  * For force feedback USB devices the official HID document [USB Physical Interface Device (PID) Class](http://www.usb.org/developers/hidpage/pid1_01.pdf)

and some projects and discussions:
  * [A promising discussion of a working FFB HID](https://www.microchip.com/forums/tm.aspx?m=320225&mpage=2&print=true) on Microchip's forums
  * [Jan Axelson's USB HID](http://janaxelson.com/hidpage.htm)
  * Discussion of [force feedback HID in Object Development Forums](http://forums.obdev.at/viewtopic.php?f=8&t=4313&start=15)
  * Another discussion at [Microship's forums](http://www.microchip.com/forums/m487478-print.aspx)
  * Yet one discussion [in ObjDev about non-USB AVRs and FFB HID](http://forums.obdev.at/viewtopic.php?f=8&t=4313)

## Windows USB HID Driver ##

Links and information that relate to Windows HID driver:
  * Discussion [at Microchip forum about Windows FFB driver registry settings](http://www.microchip.com/forums/tm.aspx?m=487478&mpage=1&key=&#498305)
  * [Yet another discussion at Microchip forums](http://www.microchip.com/forums/m320225-print.aspx)
  * Microsoft docs on [OEMForceFeedback registry settings](http://msdn.microsoft.com/en-us/library/ff542434(VS.85).aspx)
  * [XBox game pad driver](http://www.redcl0ud.com/xbcd.html) for Windows
  * [XBox game pad USB descriptors](http://euc.jp/periphs/xbox-controller.en.html)

## MS Sidewinder Joysticks ##

Links to information about Microsoft Sidewinder joystics (with and without FFB):
  * AVR-based USB-adapter for Microsoft Sidewinder 3DP, FFP and PP [3DPVert](http://code.google.com/p/sw3dprousb/)
  * Discussion about [force feedback adaptation at tech forums in DescentBB](http://www.descentbb.net/viewtopic.php?f=8&t=19061) about this kind of projects
  * Discussion thread at [Microsoft Answers](http://answers.microsoft.com/en-us/windows/forum/windows_other-hardware/midi-force-feedback-protocol/2d1f18cc-c47c-e011-9b4b-68b599b31bf5?msgId=c867a34b-397d-e011-9b4b-68b599b31bf5) about the topic
  * Source code of [Sidewinder support in Linux kernel as in sidewinder.c](http://www.cs.fsu.edu/~baker/devices/lxr/http/source/linux/drivers/input/joystick/sidewinder.c)
  * [Article on analog joystick interface](http://www.epanorama.net/documents/joystick/pc_joystick.html) on game port

## Tools for Force Feedback in Windows ##

  * [Microsoft DirectX 9.0 SDK that has FEdit tool](http://www.microsoft.com/downloads/en/confirmation.aspx?FamilyId=FD044A42-9912-42A3-9A9E-D857199F888E&displaylang=en). This is the latest SDK that still contains FEdit-tool - it has been removed from later SDKs. Use this tool to create and play FFB effects on FFB devices
  * [Copy of FFB tools](http://members.shaw.ca/bokinator/) in file ffbStuff.zip. The zip-file contains applications (including FEdit) that create and play FFB effects on FFB devices. Disclaimer: Since this stuff is not from the original manufacturer, please check for viruses, malware, licenses etc. yourself before using!
  * [Discussion about FFB tools for Windows](http://www.microchip.com/forums/m320225-p3-print.aspx) in Microchip's forum