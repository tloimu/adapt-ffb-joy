# Microsoft Sidewinder Force Feedback Pro Joystick details #

Here are some technical details about the FFP joystick to help making your own reverse engineering with it.

The cable from game port connector comes into the joystick's motherboard to a 11-pin header. Below is the connectors pins related to the DB15 game port pins. The motherboard header pins are numbered so that pin #1 is closest to the motors and the camera at the base of the actual stick.

| FFP motherboard header | DB15 | Desc |
|:-----------------------|:-----|:-----|
| 1 | 1 | +5V |
| 2 | 4 | GND |
| 3 | 12 | MIDI to joystick (for FFB data) |
| 4 | NC | NC |
| 5 | 2 | Button 1 |
| 6 | 7 | Button 2 |
| 7 | 10 | Button 4 |
| 8 | 14 |  Button 3 |
| 9 | 11 | X2 |
| 10 | Shield |  |
| 11 | 3 | X1 (for OverDrive interrupt) |

The Digital OverDrive protocol towards joystick happens in pin X1 (DB15 pin 3).

# Microsoft Sidewinder Force Feedback 2 Joystick HID Descriptor #

Compliments to Grendel for seeing the trouble to extract these from an actual FF2 device.

```
 Device Descriptor           Radix:  
bLength   18
bDescriptorType   DEVICE (0x01)
bcdUSB   1.10 (0x0110)
bDeviceClass   Defined in Interface (0x00)
bDeviceSubClass   Defined in Interface (0x00)
bDeviceProtocol   Defined in Interface (0x00)
bMaxPacketSize0   8
idVendor   0x045e
idProduct   0x001b
bcdDevice   a.00 (0x0a00)
iManufacturer   Not Requested (1)
iProduct   SideWinder Force Feedback 2 Joystick (2)
iSerialNumber   None (0)
bNumConfigurations   1

  Configuration Descriptor           Radix:  
bLength   9
bDescriptorType   CONFIGURATION (0x02)
wTotalLength   41
bNumInterfaces   1
bConfigurationValue   1
iConfiguration   None (0)
bmAttributes.Reserved   0
bmAttributes.RemoteWakeup   RemoteWakeup Supported (0b1)
bmAttributes.SelfPowered   Bus Powered (0b0)
bMaxPower   100mA (0x32)

 Interface Descriptor           Radix:  
bLength   9
bDescriptorType   INTERFACE (0x04)
bInterfaceNumber   0
bAlternateSetting   0
bNumEndpoints   2
bInterfaceClass   Human Interface Device (0x03)
bInterfaceSubClass   None (0x00)
bInterfaceProtocol   None (0x00)
iInterface   None (0)

 HID Descriptor           Radix:  
bLength   9
bDescriptorType   HID (33)
bcdHID   1.00 (0x0100)
bCountryCode   0x00
bNumDescriptors   1
bDescriptorType   REPORT (34)
wDescriptorLength   1343

  Endpoint Descriptor           Radix:  
bLength   7
bDescriptorType   ENDPOINT (0x05)
bEndpointAddress   1 IN (0b10000001)
bmAttributes.TransferType   Interrupt (0b11)
wMaxPacketSize.PacketSize   64
wMaxPacketSize.Transactions   One transaction per microframe if HS (0b00)
bInterval   1

  Endpoint Descriptor           Radix:  
bLength   7
bDescriptorType   ENDPOINT (0x05)
bEndpointAddress   2 OUT (0b00000010)
bmAttributes.TransferType   Interrupt (0b11)
wMaxPacketSize.PacketSize   16
wMaxPacketSize.Transactions   One transaction per microframe if HS (0b00)
bInterval   4
```

The HID report descriptor:

```
Get Report Descriptor
 General           Radix:  
Timestamp   0:09.269.305.483
Duration   169.997.583 ms
Length   1343 Bytes
  Report Descriptor           Radix:  
Usage Page   Generic Desktop Controls (0x01)
Usage   Joystick (0x04)
Collection   Application (0x01)
  Report ID   1
  Usage   Pointer (0x01)
  Collection   Physical (0x00)
    Unit   0
    Unit Exponent   0
    Logical Minimum   -512
    Logical Maximum   511
    Physical Minimum   0
    Physical Maximum   1023
    Report Count   1
    Report Size   10
    Usage   X (0x30)
    Input   Data (0x02)
    Report Size   6
    Input   Constant (0x01)
    Usage   Y (0x31)
    Report Size   10
    Input   Data (0x02)
    Report Size   6
    Report Count   1
    Input   Constant (0x01)
    Logical Minimum   -32
    Logical Maximum   31
    Physical Minimum   0
    Physical Maximum   63
    Unit   20
    Usage   Rz (0x35)
    Input   Data (0x02)
    Report Size   2
    Input   Constant (0x01)
    Unit   0
  End Collection   
  
  Report Size   7
  Report Count   1
  Logical Minimum   0
  Logical Maximum   127
  Physical Minimum   0
  Physical Maximum   127
  Usage   Slider (0x36)
  Input   Data (0x02)
  Report Size   1
  Input   Constant (0x01)
  Usage   Hat switch (0x39)
  Logical Minimum   0
  Logical Maximum   7
  Physical Minimum   0
  Physical Maximum   315
  Unit   20
  Report Size   4
  Report Count   1
  Input   Data (0x42)
  Report Count   1
  Input   Constant (0x01)
  Unit   0
  Usage Page   Button (0x09)
  Usage Minimum   Button 1 (primary/trigger) (0x01)
  Usage Maximum   Button 8 (0x08)
  Logical Minimum   0
  Logical Maximum   1
  Physical Minimum   0
  Physical Maximum   1
  Report Count   8
  Report Size   1
  Input   Data (0x02)
  
  Report Count   4
  Report Size   8
  Input   Constant (0x01)
  
 ----> Here is the force feedback stuff ---->

  Usage Page   PID Page (0x0f)
  Usage   PID State Report (0x92)
  Collection   Logical (0x02)
    Report ID   2
    Usage   Device Paused (0x9f)
    Usage   Actuators Enabled (0xa0)
    Usage   Safety Switch (0xa4)
    Usage   Actuator Override Switch (0xa5)
    Usage   Actuator Power (0xa6)
    Logical Minimum   0
    Logical Maximum   1
    Physical Minimum   0
    Physical Maximum   1
    Report Size   1
    Report Count   5
    Input   Data (0x02)
	
    Report Count   3
    Input   Constant (0x03)
	
    Usage   Effect Playing (0x94)
    Logical Minimum   0
    Logical Maximum   1
    Physical Minimum   0
    Physical Maximum   1
    Report Size   1
    Report Count   1
    Input   Data (0x02)
	
    Usage   Effect Block Index (0x22)
    Logical Minimum   1
    Logical Maximum   40
    Physical Minimum   1
    Physical Maximum   40
    Report Size   7
    Report Count   1
    Input   Data (0x02)
  End Collection   
  
  Usage   Set Effect Report (0x21)
  Collection   Logical (0x02)
    Report ID   1
    Usage    Effect Block Inde (0x22)
    Logical Minimum   1
    Logical Maximum   40
    Physical Minimum   1
    Physical Maximum   40
    Report Size   8
    Report Count   1
    Output   Data (0x02)
    Usage   Effect Type (0x25)
    Collection   Logical (0x02)
      Usage   ET Constant Force (0x26)
      Usage   ET Ramp (0x27)
      Usage   ET Square (0x30)
      Usage   ET Sine (0x31)
      Usage   ET Triangle (0x32)
      Usage   ET Sawtooth Up (0x33)
      Usage   ET Sawtooth Down (0x34)
      Usage   ET Spring (0x40)
      Usage   ET Damper (0x41)
      Usage   ET Inertia (0x42)
      Usage   ET Friction (0x43)
      Usage   ET Custom Force Data (0x28)
      Logical Maximum   12
      Logical Minimum   1
      Physical Minimum   1
      Physical Maximum   12
      Report Size   8
      Report Count   1
      Output   Data (0x00)
    End Collection   
	
    Usage   Duration (0x50)
    Usage   Trigger Repeat Interval (0x54)
    Usage   Sample Period (0x51)
    Logical Minimum   0
    Logical Maximum   32767
    Physical Minimum   0
    Physical Maximum   32767
    Unit   4099
    Unit Exponent   -3
    Report Size   16
    Report Count   3
    Output   Data (0x02)
	
    Unit Exponent   0
    Unit   0
    Usage   Gain (0x52)
    Logical Minimum   0
    Logical Maximum   255
    Physical Minimum   0
    Physical Maximum   10000
    Report Size   8
    Report Count   1
    Output   Data (0x02)
    Usage   Trigger Button (0x53)
    Logical Minimum   1
    Logical Maximum   8
    Physical Minimum   1
    Physical Maximum   8
    Report Size   8
    Report Count   1
    Output   Data (0x02)
    Usage   Axes Enable (0x55)
    Collection   Logical (0x02)
      Usage Page   Generic Desktop Controls (0x01)
      Usage   X (0x30)
      Usage   Y (0x31)
      Logical Minimum   0
      Logical Maximum   1
      Report Size   1
      Report Count   2
      Output   Data (0x02)
    End Collection   
	
    Usage Page   PID Page (0x0f)
    Usage   Direction Enable (0x56)
    Report Count   1
    Output   Data (0x02)
    Report Count   5
    Output   Constant (0x03)
    Usage   Direction (0x57)
    Collection   Logical (0x02)
      Usage   Instance 1 (0x000a0001)
      Usage   Instance 2 (0x000a0002)
      Unit   20
      Unit Exponent   -2
      Logical Minimum   0
      Logical Maximum   255
      Physical Minimum   0
      Physical Maximum   0
      Unit   0
      Report Size   8
      Report Count   2
      Output   Data (0x02)
      Unit Exponent   0
      Unit   0
    End Collection   
	
    Usage Page   PID Page (0x0f)
    Usage   Start Delay (0xa7)
    Unit   4099
    Unit Exponent   -3
    Logical Minimum   0
    Logical Maximum   32767
    Physical Minimum   0
    Physical Maximum   32767
    Report Size   16
    Report Count   1
    Output   Data (0x02)
    Unit   0
    Unit Exponent   0
  End Collection   
  
  Usage Page   PID Page (0x0f)
  Usage   Set Envelope Report (0x5a)
  Collection   Logical (0x02)
    Report ID   2
    Usage    Effect Block Inde (0x22)
    Logical Minimum   1
    Logical Maximum   40
    Physical Minimum   1
    Physical Maximum   40
    Report Size   8
    Report Count   1
    Output   Data (0x02)
    Usage   Attack Level (0x5b)
    Usage   Fade Level (0x5d)
    Logical Minimum   0
    Logical Maximum   255
    Physical Minimum   0
    Physical Maximum   10000
    Report Count   2
    Output   Data (0x02)
    Usage   Attack Time (0x5c)
    Usage   Fade Time (0x5e)
    Unit   4099
    Unit Exponent   -3
    Logical Maximum   32767
    Physical Maximum   32767
    Report Size   16
    Output   Data (0x02)
    Physical Maximum   0
    Unit   0
    Unit Exponent   0
  End Collection   
  
  Usage   Set Condition Report (0x5f)
  Collection   Logical (0x02)
    Report ID   3
    Usage   Effect Block Index (0x22)
    Logical Minimum   1
    Logical Maximum   40
    Physical Minimum   1
    Physical Maximum   40
    Report Size   8
    Report Count   1
    Output   Data (0x02)
    Usage   Parameter Block Offset (0x23)
    Logical Minimum   0
    Logical Maximum   1
    Physical Minimum   0
    Physical Maximum   1
    Report Size   4
    Report Count   1
    Output   Data (0x02)
    Usage   Type Specific Block Offset (0x58)
    Collection   Logical (0x02)
      Usage   Instance 1 (0x000a0001)
      Usage   Instance 2 (0x000a0002)
      Report Size   2
      Report Count   2
      Output   Data (0x02)
    End Collection   
    Logical Minimum   -128
    Logical Maximum   127
    Physical Minimum   -10000
    Physical Maximum   10000
    Usage   CP Offset (0x60)
    Report Size   8
    Report Count   1
    Output   Data (0x02)
    Physical Minimum   -10000
    Physical Maximum   10000
    Usage   Positive Coefficient (0x61)
    Usage   Negative Coefficient (0x62)
    Report Count   2
    Output   Data (0x02)
    Logical Minimum   0
    Logical Maximum   255
    Physical Minimum   0
    Physical Maximum   10000
    Usage   Positive Saturation (0x63)
    Usage   Negative Saturation (0x64)
    Report Size   8
    Report Count   2
    Output   Data (0x02)
    Usage   Dead Band (0x65)
    Physical Maximum   10000
    Report Count   1
    Output   Data (0x02)
  End Collection   
  
  Usage   Set Periodic Report (0x6e)
  Collection   Logical (0x02)
    Report ID   4
    Usage   Effect Block Index (0x22)
    Logical Minimum   1
    Logical Maximum   40
    Physical Minimum   1
    Physical Maximum   40
    Report Size   8
    Report Count   1
    Output   Data (0x02)
    Usage   Magnitude (0x70)
    Logical Minimum   0
    Logical Maximum   255
    Physical Minimum   0
    Physical Maximum   10000
    Report Size   8
    Report Count   1
    Output   Data (0x02)
    Usage   Offset (0x6f)
    Logical Minimum   -128
    Logical Maximum   127
    Physical Minimum   -10000
    Physical Maximum   10000
    Report Count   1
    Output   Data (0x02)
    Usage   Phase (0x71)
    Unit   20
    Unit Exponent   -2
    Logical Minimum   0
    Logical Maximum   255
    Physical Minimum   0
    Physical Maximum   0
    Output   Data (0x02)
    Usage   Period	(0x72)
    Logical Maximum   32767
    Physical Maximum   32767
    Unit   4099
    Unit Exponent   -3
    Report Size   16
    Report Count   1
    Output   Data (0x02)
    Unit   0
    Unit Exponent   0
  End Collection  
  
  Usage   Set Constant Force Report (0x73)
  Collection   Logical (0x02)
    Report ID   5
    Usage   Effect Block Index (0x22)
    Logical Minimum   1
    Logical Maximum   40
    Physical Minimum   1
    Physical Maximum   40
    Report Size   8
    Report Count   1
    Output   Data (0x02)
    Usage   Magnitude (0x70)
    Logical Minimum   -255
    Logical Maximum   255
    Physical Minimum   -10000
    Physical Maximum   10000
    Report Size   16
    Report Count   1
    Output   Data (0x02)
  End Collection   
  
  Usage   Set Ramp Force Report (0x74)
  Collection   Logical (0x02)
    Report ID   6
    Usage   Effect Block Index (0x22)
    Logical Minimum   1
    Logical Maximum   40
    Physical Minimum   1
    Physical Maximum   40
    Report Size   8
    Report Count   1
    Output   Data (0x02)
    Usage   Ramp Start (0x75)
    Usage   Ramp End (0x76)
    Logical Minimum   -128
    Logical Maximum   127
    Physical Minimum   -10000
    Physical Maximum   10000
    Report Size   8
    Report Count   2
    Output   Data (0x02)
  End Collection   
  
  Usage   Custom Force Data Report (0x68)
  Collection   Logical (0x02)
    Report ID   7
    Usage   Effect Block Index (0x22)
    Logical Minimum   1
    Logical Maximum   40
    Physical Minimum   1
    Physical Maximum   40
    Report Size   8
    Report Count   1
    Output   Data (0x02)
    Usage   Custom Force Data Offset (0x6c)
    Logical Minimum   0
    Logical Maximum   10000
    Physical Minimum   0
    Physical Maximum   10000
    Report Size   16
    Report Count   1
    Output   Data (0x02)
    Usage   Custom Force Data (0x69)
    Logical Minimum   -127
    Logical Maximum   127
    Physical Minimum   0
    Physical Maximum   255
    Report Size   8
    Report Count   12
    Output   Data (0x0102)
  End Collection   
  
  Usage   Download Force Sample (0x66)
  Collection   Logical (0x02)
    Report ID   8
    Usage Page   Generic Desktop Controls (0x01)
    Usage   X (0x30)
    Usage   Y (0x31)
    Logical Minimum   -127
    Logical Maximum   127
    Physical Minimum   0
    Physical Maximum   255
    Report Size   8
    Report Count   2
    Output   Data (0x02)
  End Collection   
  
  Usage Page   PID Page (0x0f)
  Usage   Effect Operation Report (0x77)
  Collection   Logical (0x02)
    Report ID   10
    Usage   Effect Block Index (0x22)
    Logical Minimum   1
    Logical Maximum   40
    Physical Minimum   1
    Physical Maximum   40
    Report Size   8
    Report Count   1
    Output   Data (0x02)
    Usage   Effect Operation (0x78)
    Collection   Logical (0x02)
      Usage   Op Effect Start (0x79)
      Usage   Op Effect Start Solo (0x7a)
      Usage   Op Effect Stop (0x7b)
      Logical Minimum   1
      Logical Maximum   3
      Report Size   8
      Report Count   1
      Output   Data (0x00)
    End Collection   
    Usage   Loop Count (0x7c)
    Logical Minimum   0
    Logical Maximum   255
    Physical Minimum   0
    Physical Maximum   255
    Output   Data (0x02)
  End Collection   
  
  Usage   PID Block Free Report (0x90)
  Collection   Logical (0x02)
    Report ID   11
    Usage   Effect Block Index (0x22)
    Logical Maximum   40
    Logical Minimum   1
    Physical Minimum   1
    Physical Maximum   40
    Report Size   8
    Report Count   1
    Output   Data (0x02)
  End Collection   
  
  Usage   PID Device Control (0x96)
  Collection   Logical (0x02)
    Report ID   12
    Usage   DC Enable Actuators (0x97)
    Usage   DC Disable Actuators (0x98)
    Usage   DC Stop All Effects (0x99)
    Usage   DC Device Reset (0x9a)
    Usage   DC Device Pause (0x9b)
    Usage   DC Device Continue (0x9c)
    Logical Minimum   1
    Logical Maximum   6
    Report Size   8
    Report Count   1
    Output   Data (0x00)
  End Collection   
  
  Usage   Device Gain Report (0x7d)
  Collection   Logical (0x02)
    Report ID   13
    Usage   Device Gain (0x7e)
    Logical Minimum   0
    Logical Maximum   255
    Physical Minimum   0
    Physical Maximum   10000
    Report Size   8
    Report Count   1
    Output   Data (0x02)
  End Collection   
  
  Usage   Set Custom Force Report (0x6b)
  Collection   Logical (0x02)
    Report ID   14
    Usage   Effect Block Index (0x22)
    Logical Minimum   1
    Logical Maximum   40
    Physical Minimum   1
    Physical Maximum   40
    Report Size   8
    Report Count   1
    Output   Data (0x02)
    Usage   Sample Count (0x6d)
    Logical Minimum   0
    Logical Maximum   255
    Physical Minimum   0
    Physical Maximum   255
    Report Size   8
    Report Count   1
    Output   Data (0x02)
    Usage   Sample Period (0x51)
    Unit   4099
    Unit Exponent   -3
    Logical Minimum   0
    Logical Maximum   32767
    Physical Minimum   0
    Physical Maximum   32767
    Report Size   16
    Report Count   1
    Output   Data (0x02)
    Unit Exponent   0
    Unit   0
  End Collection   
  
  Usage   Create New Effect Report (0xab)
  Collection   Logical (0x02)
    Report ID   1
    Usage   Effect Type (0x25)
    Collection   Logical (0x02)
      Usage   ET Constant Force (0x26)
      Usage   ET Ramp (0x27)
      Usage   ET Square (0x30)
      Usage   ET Sine (0x31)
      Usage   ET Triangle (0x32)
      Usage   ET Sawtooth Up (0x33)
      Usage   ET Sawtooth Down (0x34)
      Usage   ET Spring (0x40)
      Usage   ET Damper (0x41)
      Usage   ET Inertia (0x42)
      Usage   ET Friction (0x43)
      Usage   ET Custom Force Data (0x28)
      Logical Maximum   12
      Logical Minimum   1
      Physical Minimum   1
      Physical Maximum   12
      Report Size   8
      Report Count   1
      Feature   Data (0x00)
    End Collection   
    Usage Page   Generic Desktop Controls (0x01)
    Usage   Byte Count (0x3b)
    Logical Minimum   0
    Logical Maximum   511
    Physical Minimum   0
    Physical Maximum   511
    Report Size   10
    Report Count   1
    Feature   Data (0x02)
    Report Size   6
    Feature   Constant (0x01)
  End Collection   
  
  Usage Page   PID Page (0x0f)
  Usage   PID Block Load Report (0x89)
  Collection   Logical (0x02)
    Report ID   2
    Usage   Effect Block Index (0x22)
    Logical Maximum   40
    Logical Minimum   1
    Physical Minimum   1
    Physical Maximum   40
    Report Size   8
    Report Count   1
    Feature   Data (0x02)
    Usage   Block Load Status (0x8b)
    Collection   Logical (0x02)
      Usage   Block Load Success (0x8c)
      Usage   Block Load Full (0x8d)
      Usage   Block Load Error (0x8e)
      Logical Maximum   3
      Logical Minimum   1
      Physical Minimum   1
      Physical Maximum   3
      Report Size   8
      Report Count   1
      Feature   Data (0x00)
    End Collection   
    Usage   RAM Pool Available (0xac)
    Logical Minimum   0
    Logical Maximum   0
    Physical Minimum   0
    Physical Maximum   0
    Report Size   16
    Report Count   1
    Feature   Data (0x00)
  End Collection   
  
  Usage   PID Pool Report (0x7f)
  Collection   Logical (0x02)
    Report ID   3
    Usage   RAM Pool Size (0x80)
    Report Size   16
    Report Count   1
    Logical Minimum   0
    Physical Minimum   0
    Logical Maximum   0
    Physical Maximum   0
    Feature   Data (0x02)
    Usage   Simultaneous Effects Max (0x83)
    Logical Maximum   255
    Physical Maximum   255
    Report Size   8
    Report Count   1
    Feature   Data (0x02)
    Usage   Device Managed Pool (0xa9)
    Usage   Shared Parameter Blocks (0xaa)
    Report Size   1
    Report Count   2
    Logical Minimum   0
    Logical Maximum   1
    Physical Minimum   0
    Physical Maximum   1
    Feature   Data (0x02)
    Report Size   6
    Report Count   1
    Feature   Constant (0x03)
  End Collection   
End Collection   
```

Sample reports (not very useful tho):

```
  Sample Input Report [1]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (10b)   X
Byte 3.0 (10b)   Y
Byte 5.0 (6b)   Rz
Byte 6.0 (7b)   Pointer
Byte 7.0 (4b)   Hat switch
Byte 8.0 (1b)   Button 1 (primary/trigger)
Byte 8.1 (1b)   Button 2 (secondary)
Byte 8.2 (1b)   Button 3 (tertiary)
Byte 8.3 (1b)   Button 4
Byte 8.4 (1b)   Button 5
Byte 8.5 (1b)   Button 6
Byte 8.6 (1b)   Button 7
Byte 8.7 (1b)   Button 8
  Sample Input Report [2]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (1b)   Undefined
Byte 1.1 (1b)   Undefined
Byte 1.2 (1b)   Undefined
Byte 1.3 (1b)   Undefined
Byte 1.4 (1b)   Undefined
Byte 2.0 (1b)   Undefined
Byte 2.1 (7b)   Undefined

  Sample Output Report [1]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (8b)   Undefined
Byte 2.0 (8b)   PID Page Array
Byte 3.0 (16b)   Undefined
Byte 5.0 (16b)   Undefined
Byte 7.0 (16b)   Undefined
Byte 9.0 (8b)   Undefined
Byte 10.0 (8b)   Undefined
Byte 11.0 (1b)   X
Byte 11.1 (1b)   Y
Byte 11.2 (1b)   Undefined
Byte 12.0 (8b)   Instance 1
Byte 13.0 (8b)   Instance 2
Byte 14.0 (16b)   Undefined
 Sample Output Report [2]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (8b)   Undefined
Byte 2.0 (8b)   Undefined
Byte 3.0 (8b)   Undefined
Byte 4.0 (16b)   Undefined
Byte 6.0 (16b)   Undefined
 Sample Output Report [3]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (8b)   Undefined
Byte 2.0 (4b)   Undefined
Byte 2.4 (2b)   Instance 1
Byte 2.6 (2b)   Instance 2
Byte 3.0 (8b)   Undefined
Byte 4.0 (8b)   Undefined
Byte 5.0 (8b)   Undefined
Byte 6.0 (8b)   Undefined
Byte 7.0 (8b)   Undefined
Byte 8.0 (8b)   Undefined
 Sample Output Report [4]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (8b)   Undefined
Byte 2.0 (8b)   Undefined
Byte 3.0 (8b)   Undefined
Byte 4.0 (8b)   Undefined
Byte 5.0 (16b)   Undefined
 Sample Output Report [5]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (8b)   Undefined
Byte 2.0 (16b)   Undefined
 Sample Output Report [6]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (8b)   Undefined
Byte 2.0 (8b)   Undefined
Byte 3.0 (8b)   Undefined
 Sample Output Report [7]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (8b)   Undefined
Byte 2.0 (16b)   Undefined
Byte 4.0 (8b)   Undefined
Byte 5.0 (8b)   Undefined
Byte 6.0 (8b)   Undefined
Byte 7.0 (8b)   Undefined
Byte 8.0 (8b)   Undefined
Byte 9.0 (8b)   Undefined
Byte 10.0 (8b)   Undefined
Byte 11.0 (8b)   Undefined
Byte 12.0 (8b)   Undefined
Byte 13.0 (8b)   Undefined
Byte 14.0 (8b)   Undefined
Byte 15.0 (8b)   Undefined
 Sample Output Report [8]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (8b)   X
Byte 2.0 (8b)   Y
 Sample Output Report [10]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (8b)   Undefined
Byte 2.0 (8b)   PID Page Array
Byte 3.0 (8b)   Undefined
 Sample Output Report [11]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (8b)   Undefined
 Sample Output Report [12]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (8b)   PID Page Array
 Sample Output Report [13]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (8b)   Undefined
 Sample Output Report [14]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (8b)   Undefined
Byte 2.0 (8b)   Undefined
Byte 3.0 (16b)   Undefined

  Sample Feature Report [1]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (8b)   PID Page Array
Byte 2.0 (10b)   Reserved
 Sample Feature Report [2]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (8b)   Undefined
Byte 2.0 (8b)   PID Page Array
Byte 3.0 (16b)   PID Page Array
  Sample Feature Report [3]           Radix:  
Byte 0.0 (8b)   Report ID
Byte 1.0 (16b)   Undefined
Byte 3.0 (8b)   Undefined
Byte 4.0 (1b)   Undefined
Byte 4.1 (1b)   Undefined
```

And the HID report descriptor as pure data in hexadecimal:

```
05 01 09 04 A1 01 85 01 09 01 A1 00 66 00 00 55
00 16 00 FE 26 FF 01 35 00 46 FF 03 95 01 75 0A
09 30 81 02 75 06 81 01 09 31 75 0A 81 02 75 06
95 01 81 01 15 E0 25 1F 35 00 45 3F 66 14 00 09
35 81 02 75 02 81 01 66 00 00 C0 75 07 95 01 15
00 25 7F 35 00 45 7F 09 36 81 02 75 01 81 01 09
39 15 00 25 07 35 00 46 3B 01 66 14 00 75 04 95
01 81 42 95 01 81 01 66 00 00 05 09 19 01 29 08
15 00 25 01 35 00 45 01 95 08 75 01 81 02 95 04
75 08 81 01 05 0F 09 92 A1 02 85 02 09 9F 09 A0
09 A4 09 A5 09 A6 15 00 25 01 35 00 45 01 75 01
95 05 81 02 95 03 81 03 09 94 15 00 25 01 35 00
45 01 75 01 95 01 81 02 09 22 15 01 25 28 35 01
45 28 75 07 95 01 81 02 C0 09 21 A1 02 85 01 09
22 15 01 25 28 35 01 45 28 75 08 95 01 91 02 09
25 A1 02 09 26 09 27 09 30 09 31 09 32 09 33 09
34 09 40 09 41 09 42 09 43 09 28 25 0C 15 01 35
01 45 0C 75 08 95 01 91 00 C0 09 50 09 54 09 51
15 00 26 FF 7F 35 00 46 FF 7F 66 03 10 55 FD 75
10 95 03 91 02 55 00 66 00 00 09 52 15 00 26 FF
00 35 00 46 10 27 75 08 95 01 91 02 09 53 15 01
25 08 35 01 45 08 75 08 95 01 91 02 09 55 A1 02
05 01 09 30 09 31 15 00 25 01 75 01 95 02 91 02
C0 05 0F 09 56 95 01 91 02 95 05 91 03 09 57 A1
02 0B 01 00 0A 00 0B 02 00 0A 00 66 14 00 55 FE
15 00 26 FF 00 35 00 47 A0 8C 00 00 66 00 00 75
08 95 02 91 02 55 00 66 00 00 C0 05 0F 09 A7 66
03 10 55 FD 15 00 26 FF 7F 35 00 46 FF 7F 75 10
95 01 91 02 66 00 00 55 00 C0 05 0F 09 5A A1 02
85 02 09 22 15 01 25 28 35 01 45 28 75 08 95 01
91 02 09 5B 09 5D 15 00 26 FF 00 35 00 46 10 27
95 02 91 02 09 5C 09 5E 66 03 10 55 FD 26 FF 7F
46 FF 7F 75 10 91 02 45 00 66 00 00 55 00 C0 09
5F A1 02 85 03 09 22 15 01 25 28 35 01 45 28 75
08 95 01 91 02 09 23 15 00 25 01 35 00 45 01 75
04 95 01 91 02 09 58 A1 02 0B 01 00 0A 00 0B 02
00 0A 00 75 02 95 02 91 02 C0 15 80 25 7F 36 F0
D8 46 10 27 09 60 75 08 95 01 91 02 36 F0 D8 46
10 27 09 61 09 62 95 02 91 02 15 00 26 FF 00 35
00 46 10 27 09 63 09 64 75 08 95 02 91 02 09 65
46 10 27 95 01 91 02 C0 09 6E A1 02 85 04 09 22
15 01 25 28 35 01 45 28 75 08 95 01 91 02 09 70
15 00 26 FF 00 35 00 46 10 27 75 08 95 01 91 02
09 6F 15 80 25 7F 36 F0 D8 46 10 27 95 01 91 02
09 71 66 14 00 55 FE 15 00 26 FF 00 35 00 47 A0
8C 00 00 91 02 09 72 26 FF 7F 46 FF 7F 66 03 10
55 FD 75 10 95 01 91 02 66 00 00 55 00 C0 09 73
A1 02 85 05 09 22 15 01 25 28 35 01 45 28 75 08
95 01 91 02 09 70 16 01 FF 26 FF 00 36 F0 D8 46
10 27 75 10 95 01 91 02 C0 09 74 A1 02 85 06 09
22 15 01 25 28 35 01 45 28 75 08 95 01 91 02 09
75 09 76 15 80 25 7F 36 F0 D8 46 10 27 75 08 95
02 91 02 C0 09 68 A1 02 85 07 09 22 15 01 25 28
35 01 45 28 75 08 95 01 91 02 09 6C 15 00 26 10
27 35 00 46 10 27 75 10 95 01 91 02 09 69 15 81
25 7F 35 00 46 FF 00 75 08 95 0C 92 02 01 C0 09
66 A1 02 85 08 05 01 09 30 09 31 15 81 25 7F 35
00 46 FF 00 75 08 95 02 91 02 C0 05 0F 09 77 A1
02 85 0A 09 22 15 01 25 28 35 01 45 28 75 08 95
01 91 02 09 78 A1 02 09 79 09 7A 09 7B 15 01 25
03 75 08 95 01 91 00 C0 09 7C 15 00 26 FF 00 35
00 46 FF 00 91 02 C0 09 90 A1 02 85 0B 09 22 25
28 15 01 35 01 45 28 75 08 95 01 91 02 C0 09 96
A1 02 85 0C 09 97 09 98 09 99 09 9A 09 9B 09 9C
15 01 25 06 75 08 95 01 91 00 C0 09 7D A1 02 85
0D 09 7E 15 00 26 FF 00 35 00 46 10 27 75 08 95
01 91 02 C0 09 6B A1 02 85 0E 09 22 15 01 25 28
35 01 45 28 75 08 95 01 91 02 09 6D 15 00 26 FF
00 35 00 46 FF 00 75 08 95 01 91 02 09 51 66 03
10 55 FD 15 00 26 FF 7F 35 00 46 FF 7F 75 10 95
01 91 02 55 00 66 00 00 C0 09 AB A1 02 85 01 09
25 A1 02 09 26 09 27 09 30 09 31 09 32 09 33 09
34 09 40 09 41 09 42 09 43 09 28 25 0C 15 01 35
01 45 0C 75 08 95 01 B1 00 C0 05 01 09 3B 15 00
26 FF 01 35 00 46 FF 01 75 0A 95 01 B1 02 75 06
B1 01 C0 05 0F 09 89 A1 02 85 02 09 22 25 28 15
01 35 01 45 28 75 08 95 01 B1 02 09 8B A1 02 09
8C 09 8D 09 8E 25 03 15 01 35 01 45 03 75 08 95
01 B1 00 C0 09 AC 15 00 27 FF FF 00 00 35 00 47
FF FF 00 00 75 10 95 01 B1 00 C0 09 7F A1 02 85
03 09 80 75 10 95 01 15 00 35 00 27 FF FF 00 00
47 FF FF 00 00 B1 02 09 83 26 FF 00 46 FF 00 75
08 95 01 B1 02 09 A9 09 AA 75 01 95 02 15 00 25
01 35 00 45 01 B1 02 75 06 95 01 B1 03 C0 C0
```