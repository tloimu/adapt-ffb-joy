# Enable FFB #

The below procedure enables the FFB mode in the joystick. It also disables the auto-center spring effect.

It is sufficient to send pulses to X1-line. A single pulse has a form of 50us high and 150us low. A single "command" is composed of one or more pulses in series. Between commands, there is usually a pause of several milliseconds.

```
1 pulse
    (wait 7 ms)
4 pulses
    (wait 24-41 ms)
3 pulses
    (wait 15 ms)
2 pulses
    (wait 78 ms)
2 pulses
    (wait 4 ms)
3 pulses
    (wait 59 ms)
2 pulses
```

I.e. omitting the timings, the command sequence would be "1-4-3-2-2-3-2".

The first MIDI bytes "0xC5 0x01..." can be sent immediately after all of the above sequence pulse commands have been completed. The full MIDI data is as follows:

```
0xc5, 0x01        // <ProgramChange> 0x01
    (wait for 20 ms)
0xf0,
0x00, 0x01, 0x0a, 0x01, 0x10, 0x05, 0x6b,
0xf7
    (wait for 56 ms)
0xb5, 0x40, 0x7f,  // <ControlChange>(Modify, 0x7f)
0xa5, 0x72, 0x57,  // offset 0x72 := 0x57
0xb5, 0x44, 0x7f,  // ...
0xa5, 0x3c, 0x43,
0xb5, 0x48, 0x7f,
0xa5, 0x7e, 0x00,
0xb5, 0x4c, 0x7f,
0xa5, 0x04, 0x00,
0xb5, 0x50, 0x7f,
0xa5, 0x02, 0x00,
0xb5, 0x54, 0x7f,
0xa5, 0x02, 0x00,
0xb5, 0x58, 0x7f,
0xa5, 0x00, 0x7e,
0xb5, 0x5c, 0x7f,
0xa5, 0x3c, 0x00,
0xb5, 0x60, 0x7f,
0xa5, 0x14, 0x65,
0xb5, 0x64, 0x7f,
0xa5, 0x7e, 0x6b,
0xb5, 0x68, 0x7f,
0xa5, 0x36, 0x00,
0xb5, 0x6c, 0x7f,
0xa5, 0x28, 0x00,
0xb5, 0x70, 0x7f,
0xa5, 0x66, 0x4c,
0xb5, 0x74, 0x7f,
0xa5, 0x7e, 0x01,
0xc5, 0x01        // <ProgramChange> 0x01
    (wait for 69 ms)
0xb5, 0x7c, 0x7f,
0xa5, 0x7f, 0x00,
0xc5, 0x06        // <ProgramChange> 0x06
```

The joystick also sends an acknowledgement of the mode switch to the PC. If there is a problem in a sequence, Windows will notify that it cannot find a force feedback device and the above commands are not carried thru, but end only as e.g. sequence "1-4-3-2". The acknowledgement is signaled using the button-lines.

# Old Background Information #

An AVR based tester connected to real game port's X1-line (in 3DPVert this would go to pin B4 on Teensy 2.0) prints the below interrupt sequences at enumeration and startup of an FFB application. The X1-line is connected to tester's D0 (to get interrupts on changes) with a serial 330k resistor. Using no resistor produced quite erratic results probably due the AVRs interference in the process.

The tester can clock up to 4us accuracy but additional distortion is always possible since it is based on Arduino code, which doesn't give you all the control easily.

The tester prints out time differences between changes in pin value in micro seconds and the value the pin changed to. Results are quite consistent.

Looking at the tester report, there seems to be a pattern where a "command" is a varying number of about 2,5kHz pulses and each "command" is separated by a pause of varying amount of milliseconds. If each "command" could be identified with a number of pulses in it, the startup sequence looks like this:

```
1 pulse
    (wait 7 ms)
4 pulses
    (wait 24-41 ms)
3 pulses
    (wait 15 ms)
2 pulses
    (wait 78 ms)
2 pulses
    (wait 4 ms)
3 pulses
    (wait 59 ms)
2 pulses
```

I.e. omitting the timings, the command sequence would be "1-4-3-2-2-3-2".

I don't know how exact the timings between the commands have to be or how the timing coincides with other signalling (joystick reports via the button lines, MIDI line or the X2-line interrupts).

Some interrupts are still sent in X1-line after and along the very first MIDI commands. But after the startup sequence is completed, issuing effects data happens on the MIDI-line alone.

The first MIDI commands "0xC5 0x01" are sent around the time after the first "commands" ("1-4-3") have been sent.

The joystick also seems to somehow acknowledge the switch to the PC since if there is a problem in a sequence, Windows will notify that it cannot find a force feedback device and the above commands are not carried thru, but end only as e.g. sequence "1-4-3-2".


---


In this sample, the values of buttons, X1 and X2 -lines are also visible. Buttons and X2 values do not have timing information and there is a change that the order or printing is slightly off, but this should give a better view:

Open FEdit, which enumerates the device, enables FFB and switches off the auto-center-spring effect (see the [FFB MIDI-protocol description](http://code.google.com/p/adapt-ffb-joy/wiki/SidewinderFFBMIDI) for more on that). The MIDI data is also printed here.

Some "commands" also include pulsing of X2 in addition to X1. These are longer pulses of about 330..430us. Some commands also return data to PC in button-lines.

Here's some more detailed info about the "enable FFB"-sequence
```
Command A: (i.e. "Read")
  X1: pulse 1 time for 230us
  X2: delay 50us and pulse 420us 
  Return ~16 clocks of data to PC

(pause 6 ms)

Command B:
  X1: pulse 4 times (200us intervals) with pulse widths 230, 220, 220 and 290us
  X2: delay 50us, pulse 2 times (pulse 330us, wait 520us, pulse 400us)
  Return ~60 clocks of data to PC

(pause 35 ms)

Command C:
  X1: pulse 3 times (200us intervals) with pulse widths 230, 220 and  290us
  X2: delay 50us, pulse 2 times (pulse 330us, wait 490us, pulse 420us)
  No data to PC
  
(pause 14 ms)

Command D:
  X1 for 2 times (200us intervals) with pulse widths 230 and 290us
  X2: delay 50us and pulse 310us
  Return ~60 clocks of data to PC

(pause 78 ms)

Command E:
  X1 for 2 times (200us intervals) with pulse widths 230 and 290us
  No X2 activity
  No data to PC

(pause 3.5 ms)

Command F: (like "Command C", but no X2 activity)
  X1: pulse 3 times (200us intervals) with pulse widths 230, 220 and  290us
  No X2 activity
  No data to PC

..Somewhere here, start sending MIDI data starting with 0xC5 0x01...

(pause 59 ms)

Command G: (like "Command E" but returns data)
  X1 for 2 times (200us intervals) with pulse widths 230 and 290us
  No X2 activity
  Return ~28 clocks of data to PC

...complete setup sequence in MIDI...
```

The "Command A" is also repeated as such when a Game Controllers panel is opened and when it polls every 5 seconds the stick to display its status (Connected/Not connected) or when the positional data is read from the stick.

Below is a raw sample of log (not the only one tho) to support the above picture.

How to interpret the log? Lines starting with a large number have a timing information in micro seconds (the time value "1000000000" is added only to get easy formatting directly in AVR).

Lines with "X1 high" or "X1 low" indicate change in X1-line state. The number after "+"-sign is the microseconds since the last X1-change occurred. Value before "+"-sign is offset from beginning of a "command", which is here defined to start if previous X1-changes was longer than 900us ago.

Lines starting with "Buttons" indicate that a buttons-line or X2-line has changed. Values printed are But1, But2, But3, But4 and X2=?. The X1-value is copied from the time of pin change interrupt for the button values. (Yes, I know, the But1 is the clock and one could already decipher the actual data sent, but as my time slot for doing this is out now - I'll just leave the raw data here.)

Any hexadecimal data is MIDI data received by the joystick. Their timings are also measured and are related to the start of the previous MIDI-command (MIDI command starts with byte with MSB set to 1 or with F0 - see the MIDI doc mentioned above for more).

So, here is the raw data from the tester (the first line indicates that the X1 went high 7.68 seconds since X1 has last changed).

```
1007680012: X1 high (0) @ 0 +7680012
Buttons: 0 1 1 1 X2=0 X1=0
Buttons: 1 1 1 1 X2=0 X1=1
Buttons: 0 1 1 1 X2=0 X1=1
Buttons: 0 1 0 1 X2=0 X1=1
Buttons: 0 1 1 0 X2=0 X1=1
Buttons: 1 1 1 0 X2=0 X1=1
Buttons: 1 1 1 1 X2=0 X1=1
1000000228: X1 low  (0) @ 228 +228
Buttons: 1 1 0 1 X2=1 X1=0
Buttons: 0 0 1 1 X2=0 X1=0
Buttons: 0 1 0 1 X2=1 X1=0
Buttons: 1 1 0 1 X2=1 X1=0
Buttons: 0 1 1 1 X2=1 X1=0
Buttons: 0 1 1 1 X2=1 X1=0
Buttons: 1 1 1 1 X2=1 X1=0
Buttons: 0 0 0 0 X2=1 X1=0
Buttons: 1 0 0 1 X2=1 X1=0
Buttons: 1 0 0 1 X2=1 X1=0
Buttons: 0 0 0 0 X2=1 X1=0
Buttons: 1 0 0 0 X2=1 X1=0
Buttons: 1 0 1 1 X2=1 X1=0
Buttons: 1 0 1 1 X2=1 X1=0
Buttons: 1 0 1 1 X2=1 X1=0
Buttons: 1 0 0 1 X2=1 X1=0
Buttons: 0 0 0 1 X2=1 X1=1
Buttons: 0 0 0 1 X2=1 X1=1
Buttons: 0 0 0 1 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
1000006800: X1 high (0) @ 0 +6800
1000000224: X1 low  (1) @ 224 +224
1000000204: X1 high (2) @ 428 +204
1000000208: X1 low  (3) @ 636 +208
1000000196: X1 high (4) @ 832 +196
1000000012: X1 high (5) @ 844 +12
1000000008: X1 high (6) @ 852 +8
1000000204: X1 low  (7) @ 1056 +204
1000000196: X1 high (8) @ 1252 +196
1000000276: X1 low  (9) @ 1528 +276
Buttons: 0 1 1 1 X2=0 X1=0
Buttons: 1 1 1 1 X2=0 X1=1
Buttons: 1 1 1 1 X2=0 X1=1
Buttons: 0 1 0 1 X2=0 X1=1
Buttons: 0 1 1 0 X2=0 X1=1
Buttons: 1 1 1 0 X2=0 X1=1
Buttons: 1 1 1 1 X2=0 X1=1
Buttons: 0 0 1 1 X2=0 X1=1
Buttons: 0 1 0 1 X2=0 X1=1
Buttons: 1 1 0 1 X2=1 X1=1
Buttons: 1 0 1 1 X2=1 X1=1
Buttons: 0 1 0 1 X2=1 X1=1
Buttons: 0 1 1 1 X2=1 X1=1
Buttons: 0 1 1 1 X2=1 X1=1
Buttons: 1 1 1 1 X2=1 X1=1
Buttons: 1 0 0 0 X2=1 X1=1
Buttons: 0 0 0 1 X2=1 X1=1
Buttons: 0 0 0 0 X2=1 X1=1
Buttons: 1 0 0 0 X2=1 X1=1
Buttons: 1 0 1 1 X2=1 X1=1
Buttons: 1 0 1 1 X2=1 X1=1
Buttons: 1 0 0 1 X2=1 X1=1
Buttons: 1 0 0 1 X2=1 X1=1
Buttons: 0 0 0 1 X2=1 X1=0
Buttons: 0 0 0 1 X2=1 X1=0
Buttons: 0 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 0 0 0 0 X2=0 X1=0
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 0 0 1 1 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 0 0 0 1 X2=0 X1=1
Buttons: 0 0 0 1 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=1 X1=1
Buttons: 0 0 0 1 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=1 X1=1
Buttons: 1 0 0 1 X2=1 X1=1
Buttons: 0 0 0 1 X2=1 X1=0
Buttons: 0 0 0 1 X2=1 X1=0
Buttons: 0 0 0 1 X2=0 X1=1
Buttons: 0 0 1 1 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
1000024712: X1 high (0) @ 0 +24712
1000000228: X1 low  (1) @ 228 +228
1000000188: X1 high (2) @ 416 +188
1000000032: X1 high (3) @ 448 +32
1000000192: X1 low  (4) @ 640 +192
1000000192: X1 high (5) @ 832 +192
1000000016: X1 high (6) @ 848 +16
1000000272: X1 low  (7) @ 1120 +272
1000002788: X1 high (8) @ 0 +2788
1000000236: X1 low  (9) @ 236 +236
1000000184: X1 high (10) @ 420 +184
1000000032: X1 high (11) @ 452 +32
1000000260: X1 low  (12) @ 712 +260
Buttons: 0 1 1 1 X2=0 X1=0
Buttons: 1 1 1 1 X2=0 X1=0
Buttons: 1 1 1 1 X2=0 X1=0
Buttons: 0 1 1 1 X2=0 X1=0
Buttons: 0 1 1 0 X2=0 X1=0
Buttons: 1 1 1 0 X2=0 X1=0
Buttons: 1 1 1 1 X2=0 X1=0
Buttons: 0 0 1 0 X2=0 X1=0
Buttons: 0 0 1 1 X2=0 X1=0
Buttons: 1 0 1 1 X2=0 X1=0
Buttons: 1 0 1 1 X2=1 X1=0
Buttons: 0 1 0 1 X2=1 X1=0
Buttons: 0 1 1 1 X2=1 X1=0
Buttons: 0 1 1 1 X2=1 X1=0
Buttons: 1 1 1 1 X2=1 X1=0
Buttons: 1 0 0 0 X2=1 X1=0
Buttons: 0 0 0 1 X2=1 X1=0
Buttons: 0 0 0 0 X2=1 X1=0
Buttons: 1 0 0 0 X2=1 X1=0
Buttons: 1 0 1 1 X2=1 X1=0
Buttons: 1 0 1 1 X2=1 X1=0
Buttons: 1 0 0 0 X2=1 X1=0
Buttons: 1 0 0 0 X2=1 X1=0
Buttons: 0 0 0 0 X2=1 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 1 1 1 X2=0 X1=0
Buttons: 1 1 1 1 X2=0 X1=0
Buttons: 1 1 1 0 X2=0 X1=0
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 1 1 X2=0 X1=1
Buttons: 1 0 1 1 X2=0 X1=1
Buttons: 1 1 1 1 X2=0 X1=1
Buttons: 0 1 1 1 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 1 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 0 0 0 0 X2=0 X1=0
Buttons: 0 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=1 X1=0
Buttons: 0 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=1 X1=0
Buttons: 0 0 0 0 X2=1 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 0 1 1 1 X2=0 X1=0
Buttons: 1 1 1 1 X2=0 X1=0
Buttons: 1 1 1 1 X2=0 X1=0
Buttons: 0 1 1 1 X2=0 X1=0
Buttons: 1 1 1 0 X2=0 X1=0
Buttons: 0 1 1 1 X2=0 X1=0
Buttons: 0 0 1 0 X2=0 X1=0
Buttons: 1 0 1 0 X2=0 X1=0
Buttons: 1 0 1 1 X2=1 X1=0
Buttons: 0 0 1 1 X2=1 X1=0
Buttons: 0 1 0 1 X2=1 X1=0
Buttons: 1 1 0 1 X2=1 X1=0
Buttons: 1 1 1 1 X2=1 X1=0
Buttons: 1 1 1 1 X2=1 X1=0
Buttons: 0 0 0 0 X2=1 X1=0
Buttons: 0 0 0 1 X2=1 X1=0
Buttons: 1 0 0 1 X2=1 X1=0
Buttons: 1 0 0 0 X2=1 X1=0
Buttons: 0 0 0 0 X2=1 X1=0
Buttons: 1 0 0 0 X2=1 X1=0
Buttons: 0 0 0 0 X2=1 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 1 1 1 X2=0 X1=0
Buttons: 1 1 1 1 X2=0 X1=0
Buttons: 1 1 1 0 X2=0 X1=0
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
1004615824:  c5
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0 01
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
1000091088: X1 high (0) @ 0 +91088
Buttons: 0 1 1 1 X2=0 X1=0
Buttons: 1 1 1 1 X2=0 X1=0
Buttons: 1 1 1 1 X2=0 X1=0
Buttons: 0 1 1 1 X2=0 X1=0
Buttons: 0 1 1 0 X2=0 X1=0
Buttons: 0 1 1 1 X2=0 X1=0
Buttons: 1 1 1 1 X2=0 X1=0
Buttons: 0 0 1 0 X2=0 X1=0
Buttons: 0 0 1 1 X2=0 X1=0
Buttons: 0 0 1 1 X2=0 X1=0
Buttons: 1 0 1 1 X2=1 X1=0
Buttons: 1 1 0 1 X2=1 X1=0
Buttons: 0 1 1 1 X2=1 X1=0
Buttons: 0 1 1 1 X2=1 X1=0
Buttons: 1 1 1 1 X2=1 X1=0
Buttons: 1 0 0 0 X2=1 X1=0
Buttons: 0 0 0 1 X2=1 X1=0
Buttons: 0 0 0 0 X2=1 X1=0
Buttons: 1 0 0 0 X2=1 X1=0
Buttons: 1 0 1 1 X2=1 X1=0
Buttons: 1 0 1 1 X2=1 X1=0
Buttons: 1 0 0 0 X2=1 X1=0
1000024352:  f0

1000000224: X1 low  (0) @ 224 +224
1000000200: X1 high (1) @ 424 +200
1000000280: X1 low  (2) @ 704 +280
1000003200: X1 high (3) @ 0 +3200
1000000224: X1 low  (4) @ 224 +224
1000000196: X1 high (5) @ 420 +196
1000000220: X1 low  (6) @ 640 +220
1000000192: X1 high (7) @ 832 +192
1000000016: X1 high (8) @ 848 +16
1000000276: X1 low  (9) @ 1124 +276
Buttons: 0 0 0 0 X2=1 X1=0
Buttons: 0 0 0 0 X2=1 X1=0
Buttons: 0 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 0 0 1 1 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 0 1 1 1 X2=0 X1=0
Buttons: 1 1 1 1 X2=0 X1=0
Buttons: 1 1 1 1 X2=0 X1=0
Buttons: 0 1 1 1 X2=0 X1=0
Buttons: 0 1 1 0 X2=0 X1=0
Buttons: 1 1 1 0 X2=0 X1=0
Buttons: 1 1 1 1 X2=0 X1=0
Buttons: 0 0 1 0 X2=0 X1=0
Buttons: 0 0 1 1 X2=0 X1=0
Buttons: 1 0 1 1 X2=0 X1=0
Buttons: 1 0 1 1 X2=1 X1=0
Buttons: 0 1 0 1 X2=1 X1=0
Buttons: 0 1 1 1 X2=1 X1=0
Buttons: 0 1 1 1 X2=1 X1=0
Buttons: 1 1 1 1 X2=1 X1=0
Buttons: 1 0 0 0 X2=1 X1=0
Buttons: 0 0 0 1 X2=1 X1=0
Buttons: 0 0 0 0 X2=1 X1=0
Buttons: 1 0 0 0 X2=1 X1=0
Buttons: 1 0 1 1 X2=1 X1=0
Buttons: 1 0 1 1 X2=1 X1=0
Buttons: 1 0 0 0 X2=1 X1=0
Buttons: 0 0 0 0 X2=1 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 0 0 0 1 X2=0 X1=1
Buttons: 0 0 1 1 X2=0 X1=1
Buttons: 1 0 1 1 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=0
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 0 0 0 0 X2=0 X1=0
Buttons: 0 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=1 X1=0
Buttons: 1 0 0 1 X2=1 X1=0
Buttons: 0 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=1 X1=0
Buttons: 0 0 0 0 X2=1 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 1 X2=0 X1=1
Buttons: 0 0 0 1 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
 00 01 0a 01 10 05 6b
1000041644:  f7
1000059424: X1 high (0) @ 0 +59424
Buttons: 0 1 1 1 X2=0 X1=1
Buttons: 1 1 1 1 X2=0 X1=1
Buttons: 1 1 1 1 X2=0 X1=1
Buttons: 0 1 1 1 X2=0 X1=1
Buttons: 0 1 1 0 X2=0 X1=1
Buttons: 1 1 1 0 X2=0 X1=1
Buttons: 1 1 1 1 X2=0 X1=1
Buttons: 0 0 1 0 X2=0 X1=1
Buttons: 0 0 1 1 X2=0 X1=1
Buttons: 0 0 1 1 X2=0 X1=1
Buttons: 1 0 1 1 X2=1 X1=1
Buttons: 1 1 0 1 X2=1 X1=1
Buttons: 0 1 1 1 X2=1 X1=1
Buttons: 0 1 1 1 X2=1 X1=1
Buttons: 1 1 1 1 X2=1 X1=1
Buttons: 1 0 0 0 X2=1 X1=1
Buttons: 0 0 0 1 X2=1 X1=1
Buttons: 0 0 0 0 X2=1 X1=1
Buttons: 1 0 0 0 X2=1 X1=1
Buttons: 1 0 0 0 X2=1 X1=1
1000023096:  b5
1000000232: X1 low  (0) @ 232 +232
1000000204: X1 high (1) @ 436 +204
1000000220: X1 low  (2) @ 656 +220
Buttons: 0 0 0 0 X2=1 X1=1
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 0 0 0 0 X2=0 X1=0
Buttons: 0 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 0 0 0 0 X2=0 X1=0
Buttons: 0 0 0 0 X2=0 X1=0
Buttons: 0 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 0 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 0 0 1 0 X2=0 X1=0
Buttons: 0 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 0 0 0 0 X2=0 X1=1
Buttons: 0 0 0 1 X2=0 X1=1
Buttons: 0 0 0 1 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
Buttons: 1 0 0 1 X2=0 X1=1
Buttons: 1 0 0 0 X2=0 X1=1
 40
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0 7f
1000039616:  a5 72 57
1000000972:  b5 44 7f
1000000968:  a5 3c 43
1000000968:  b5 48 7f
1000000964:  a5 7e 00
1000000968:  b5 4c 7f
1000000968:  a5 04 00
1000000968:  b5 50 7f
1000000968:  a5 02 00
1000000968:  b5 54 7f
1000000964:  a5 02 00
1000000972:  b5 58 7f
1000000968:  a5 00 7e
1000000968:  b5 5c 7f
1000000968:  a5 3c 00
1000000968:  b5 60 7f
1000000960:  a5 14 65
1000000968:  b5 64 7f
1000000968:  a5 7e 6b
1000000964:  b5 68 7f
1000000968:  a5
1000029636:  b5
Buttons: 1 0 0 1 X2=0 X1=0 7c 7f
1000001304:  a5 7f
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 1 X2=0 X1=0 00
Buttons: 1 0 0 0 X2=0 X1=0
1000001844:  c5 06
Buttons: 1 0 0 1 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
```

Then close Fedit-application:

```
1001521624:  c5
Buttons: 1 0 0 1 X2=0 X1=0 01
Buttons: 1 0 0 0 X2=0 X1=0
Buttons: 1 0 0 0 X2=0 X1=0
1000073472:  c5
Buttons: 1 0 0 1 X2=0 X1=0 07
1000010200:  b0 40 00
1000001044:  b1 40 00
1000001048:  b2 40 00
1000001064:  b3 40 00
1000001044:  b4 40 00
1000001060:  b5 40 00
1000001048:  b6 40 00
1000001036:  b7 40 00
1000001068:  b8 40 00
1000001048:  b9 40 00
1000001064:  ba 40 00
1000001044:  bb 40 00
1000001040:  bc 40 00
1000001068:  bd 40 00
1000001040:  be 40 00
1000001064:  bf 40 00
1000001040:  b0 40 00
1000001068:  b1 40 00
1000001044:  b2 40 00
1000001060:  b3 40 00
1000001048:  b4 40 00
1000001048:  b5 40 00
1000001064:  b6 40 00
1000001044:  b7 40 00
1000001064:  b8 40 00
1000001044:  b9 40 00
1000001040:  ba 40 00
1000001068:  bb 40 00
1000001040:  bc 40 00
1000001064:  bd 40 00
1000001040:  be 40 00
1000001068:  bf 40 00
```