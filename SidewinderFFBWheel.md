## Effects(SysEx) ##

### Data bytes ###

Data byte format is the same as with FFB Pro protocol. Data bytes are 7-bit (bit 7 always 0). 14-bit values are presented with 2 bytes in LSB, MSB order.

### Header and footer ###

Common parts for all SysEx messages are first 11 bytes:
```
f0
   00 01 0a 15  header
   20	        always 0x20
   xx           effect type
   7f		always 0x7f
   xx xx	duration
   xx 		direction (angle*128/360)
```
and the end of message:
```
   xx		checksum
f7
```

Difference to pro protocol is 0x20 byte (0x23 for pro) and that direction is followed directly after duration.

Checksum is calculated the same way as with pro protocol.

### waveforms ###
```
f0
   00 01 0a 15  header
   20	        always 0x20
   xx           effect type (sine=2, square=3, triangle=4, sawtooth/ramp=5)
   7f		always 7f
   xx xx	duration
   xx 		direction (angle*128/360)
   ??	        precise direction indicator ? 7f=precise, 7d=not
   00 40	periodic x-offset, default 00 40
   7f		envelope y1, default 7f
   00 00	envelope x1, default 00 00
   7f		periodic amplitude
   65 12	envelope x2
   7f		envelope y2
   74 03	periodic x-hz
   3e		periodic y-offset
   xx		checksum
f7
```
Ramp has same structure as sine etc but with different default values:
```
f0
   00 01 0a 15
   20
   05			effect type: ramp
   7f			
   xx xx 		duration
   xx			ramp direction, increasing=0x40, decreasing=0x00 (in direction view 0x40 <180 deg, 0x00 >= 180).
   7f			???
   00 00		???
   xx			envelope y1, 0x00=middle, 7f=top/bottom, default 7f.
   xx xx		envelope x1, 00 00 = left, 6e 1e (3950) = right, default left
   xx			ramp delta (7f = max diff, 00=same level, default 7f)
   xx xx		envelope x2, 00 00 = left, 6e 1e = right, default right
   xx			envelope y2, 0x00=middle, 7f=top/bottom, default 7f
   6e 1e		???
   xx			ramp midpoint y-offset 00..7f. 00=top, 7d=bottom
   xx 			checksum
f7
```
NOTE:
ramp delta = abs(end-start)
ramp direction = (end < start) ? 0x40 : 0x00
ramp midpoint = abs(start - (ramp delta/2))

### constant force ###
```
f0
   00 01 0a 15 20
   06			effect type: constant force
   7f			
   xx xx		duration
   00			direction, not used, always 00
   7f			unknown/always 7f
   xx			envelope y1 00-7f, default 7f
   xx xx		envelope x1 (0x6e 0x1e = right, 0x00 0x00 = left. default left)
   xx			force 00-7f, default 7f.
   xx xx		envelope x2 (0x00 0x00 = left, 0x6e 0x1e = right. default right)
   xx 			envelope y2 (0x00 = bottom, 0x7f=top. default top)
   xx			force direction 00=ccw, 7d=cw
   xx			checksum
f7
```
### friction ###
```
f0
   00 01 0a 15 20	header
   0b			effect type: friction
   7f			always 7f
   xx xx		duration
   00			direction, not used, always 00
   7e			friction axis0 positive coef (0x00=-10000, 0x3f = 0, 0x7e=10000)
   xx			checksum
f7
```
### inertia ###
```
f0
   00 01 0a 15 20	header
   0a			effect type: inertia
   7f			always 7f
   xx xx		duration
   00			direction, not used, always 00
   00			?? always 00			
   7d			negative coef 0x7d=-10000, 0x3e=0, 0x00=+10000		
   3e 3f		?? always 3e 3f
   3e 3f		?? always 3e 3f
   7d			?? always 7d
   00			positive coef 0x7d=-10000, 0x3e=0, 0x00=+10000
   xx			checksum
f7
```
### damper ###
```
f0
   00 01 0a 15 20	header
   09			effect type: damper
   7f			always 7f
   xx xx		duration
   00			direction, not used, always 00
   00			pos sat
   7d			neg coef & pos sat & deadband
   3e			deadband
   3f			???
   3e			deadband
   3f			???
   7d			neg sat
   00			pos coef & net sat & deadband
   57
f7
```
Coefficients, deadband and saturation values affect multiple locations, formula not known.

### spring ###
```
f0
   00 01 0a 15 20 	header
   08			effect type: spring
   7f			always 7f
   xx xx		duration
   xx			direction, not used, always 00
   xx			positive saturation 3e=1, 00=10000
   xx			positive saturation 3e=1, 7d=10000
   3e			
   xx			positive saturation
   3e
   xx			negative saturation 3e=1, 2f=10000
   xx			negative saturation 3e=1, 7d=10000 (0x3e+(0x7d-0x3e)*sat/10000)
   xx			negative saturation 3e=1, 00=10000 (0x3e-sat/10000).
   xx			checksum
f7
```
Saturation values are stored to multiple locations, might be mixed if both pos+neg are modified (not tested).

## Effect modification ##

### Command structure ###

```
f1
xx     checksum of all bytes, omitting 6th bit (0x40) of attribute byte			
da     default indicator 0x00/0x40 | attribute/address
ii     effect index
nn nn  14-bit value LSB, MSB (some attributes use only LSB, MSB=0)
```

"default indicator" seem to be set to 0x00 by ForceEditor when given value is the same as default one when effect is created. 0x40 for non-default values.

### data format ###
```
uu 14 : unsigned 14-bit
ii 14 : signed 14-bit
u7 00 : unsigned 7-bit, in first (LSB) byte.

signed 14-bit limits:
00 40 = 0x2000	-> 0010 0000 0000 0000
		-> 1110 0000 0000 0000 = -8192
7f 3f = 0x1fff	-> 0001 1111 1111 1111 = 8191
```
### attributes ###
```
duration				f1 -- -0 -- uu 14	25 .. 5000 (50ms .. 10000ms)	
direction				f1 -- -2 -- u7 00	0 .. 127 (deg 0 .. ~359)
period, 1/Hz:				f1 -- -a -- uu 14	0 .. 1000
period, x-offset:			f1 -- -4 -- ii 14	-8192 .. +8191
period, y-offset:			f1 -- -b -- u7 00	0 .. 125
period, amplitude:			f1 -- -7 -- u7 00	0 .. 127
envelope, start amplitude:		f1 -- -5 -- u7 00	distance from middle 0 .. 127
envelope, end amplitude:		f1 -- -9 -- u7 00	distance from middle 0 .. 127
envelope, x1:				f1 -- -6 -- uu 14	0 .. 1000
envelope, x2:				f1 -- -8 -- uu 14	0 .. 1000
envelope, (const force) start y:	f1 -- -4 -- u7 00	distance from middle 0 .. 127
envelope, (const force) end y:		f1 -- -8 -- u7 00	distance from middle 0 .. 127
envelope, (const force) x1:		f1 -- -5 -- uu 14	0 .. 1000
envelope, (const force) x2:		f1 -- -7 -- uu 14	0 .. 1000
constant force constant:		f1 -- -6 -- u7 00	0 .. 127 (0=no force, 127=max)
constant force direction:		f1 -- -9 -- u7 00	00/7d. 00=ccw, 7d=cw.
ramp start				f1 -- -7 -- u7 00	0 .. 127 (top .. bottom)
ramp end				f1 -- -b -- u7 00	0 .. 127 (top .. bottom)
offset					f1 -- -a -- u7 00	0 .. 126 (-10000 .. +10000)
positive coefficient			f1 -- -6 -- 7d u7	0 .. 127 (-10000 .. +10000)
negative coefficient			f1 -- -3 -- 00 u7	0 .. 125 (-10000 .. +10000)
positive saturation			f1 -- -3 -- uu 14	0 .. 8000 (-10000 .. +10000)
negative saturation			f1 -- -6 -- uu 14	0 .. 8000 (-10000 .. +10000)
```
**Envelope x1 < x2, command for setting x2=x1 is sent when x1 goes over x2 and vice versa.** Const force env x2 changes are not sent if x1 > x2. modifying const force env x1 changes also x2 to the same value as x1.
**Changing duration may also change envlope x-values.** Constant force direction is sent when force direction changes, otherwise just constant is sent when force constant changes.
**Adjusting deadband for spring&damper changes attributes 2-6.** Adjusting ramp start changes also end and vice versa.
**Coefficient changes MSB, LSB is always 0 for negative and 7d for positive coef.** Changing positive coefs & neg sat modify the same attibutes, same for neg coef & pos sat.
**Changing direction may also change attributes 3 & 4 for ramp (and possibly others)**

## Effect command ##
Effect command messages are 3 bytes long:
```
f2
ec effect command and xor checksum.
ii effect index
```
effect commands: 0x20=play, 0x30=stop, 0x10=delete.
xor checksum is calculated by xorring all nibbles (0xf ^ 0x2 ...).

## Initialization ##

Wheel does not require any X-pulses for entering FFB mode. Following midi byte sequences are sent at midi (as captured from FEdit communication):

```
0xf3, 0x1d
0xf1 ,0x0e ,0x43 ,0x01 ,0x00 ,0x7d,
0xf1 ,0x7e ,0x04 ,0x01 ,0x3e ,0x4e,
0xf1 ,0x1c ,0x45 ,0x01 ,0x3e ,0x2f,
0xf1 ,0x0b ,0x46 ,0x01 ,0x7d ,0x00,
```
Followed by auto center off:
```
0xf3, 0x1d,
0xf1, 0x10, 0x40, 0x00, 0x7f, 0x00,
0xf3, 0x6a
```

Auto center is activated with
```
0xf3, 0x1d
```