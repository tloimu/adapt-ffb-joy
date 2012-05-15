/*
  Force Feedback Joystick
  USB HID descriptors for a force feedback joystick.  

  This code is for Microsoft Sidewinder Force Feedback Pro joystick
  with some room for additional extra controls.

  Copyright 2012  Tero Loimuneva (tloimu [at] gmail [dot] com)
  MIT License.

  Some code is based on LUFA Library, for which uses MIT license:

  Copyright 2012  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/


/** \file
 *
 *  USB Device Descriptors, for library use when in USB device mode. Descriptors are special
 *  computer-readable structures which the host requests upon device enumeration, to determine
 *  the device's capabilities and functions.
 */

#include "Descriptors.h"
#include "3DPro.h"

/** HID class report descriptor. This is a special descriptor constructed with values from the
 *  USBIF HID class specification to describe the reports and capabilities of the HID device. This
 *  descriptor is parsed by the host and its contents used to determine what data (and in what encoding)
 *  the device will send, and what it may be sent back from the host. Refer to the HID specification for
 *  more details on HID report descriptors.
 */
const USB_Descriptor_HIDReport_Datatype_t PROGMEM JoystickReport[] =
{
0x05,0x01,	// USAGE_PAGE (Generic Desktop)
0x09,0x04,	// USAGE (04)
0xA1,0x01,	// COLLECTION (Application)
	0x85,0x01,	// REPORT_ID (1)

	// FFP input descriptor:
    HID_RI_USAGE(8, 0x01), // Pointer
    HID_RI_COLLECTION(8, 0x00), // Physical
        HID_RI_USAGE(8, 0x30), // Usage X
        HID_RI_USAGE(8, 0x31), // Usage Y
        HID_RI_USAGE(8, 0x32), // Usage Z
        HID_RI_LOGICAL_MINIMUM(16, -512),
        HID_RI_LOGICAL_MAXIMUM(16, 511),
        HID_RI_PHYSICAL_MINIMUM(8, 0),
        HID_RI_PHYSICAL_MAXIMUM(16, 1023),
        HID_RI_REPORT_COUNT(8, 0x03),
        HID_RI_REPORT_SIZE(8, 0x10),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),

		0x09, 0x35,		//     USAGE (Rz)
        HID_RI_LOGICAL_MINIMUM(8, 0),
        HID_RI_LOGICAL_MAXIMUM(8, 63),
        HID_RI_PHYSICAL_MINIMUM(8, 0),
        HID_RI_PHYSICAL_MAXIMUM(8, 46),
        HID_RI_REPORT_COUNT(8, 0x01),
        HID_RI_REPORT_SIZE(8, 0x06),
		0x81, 0x02,		//     INPUT (Data,Var,Abs)		 6b Rz

		0x75, 0x02,		//     REPORT_SIZE (2)
		0x95, 0x01,		//     REPORT_COUNT (1)
		0x81, 0x01,		//     INPUT (Cnst,Ary,Abs)		 2b Fill


		0x65, 0x00,		//     UNIT (None)

		0x09, 0x33,     //   USAGE (Rx)
		0x09, 0x34,		//   USAGE (Ry)
	    HID_RI_LOGICAL_MINIMUM(8, 0),
	    HID_RI_LOGICAL_MAXIMUM(8, 255),
        HID_RI_PHYSICAL_MAXIMUM(8, 255),
	    HID_RI_REPORT_SIZE(8, 0x08),
        HID_RI_REPORT_SIZE(8, 0x08),
	    HID_RI_REPORT_COUNT(8, 0x02),
		HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),

    HID_RI_END_COLLECTION(0),

	0x09, 0x36,		//   USAGE (Rudder - hmm...actual rudder code 0xBA does not seem to work in Windows - so use slider)
    HID_RI_LOGICAL_MINIMUM(8, -128),
    HID_RI_LOGICAL_MAXIMUM(8, 127),
    HID_RI_PHYSICAL_MAXIMUM(8, 255),
    HID_RI_REPORT_SIZE(8, 0x08),
    HID_RI_REPORT_COUNT(8, 0x01),
	HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),

	0x05, 0x02,                    //   USAGE_PAGE (Simulation Controls)

	0x09, 0xbb,		//   USAGE (Throttle)
    HID_RI_LOGICAL_MINIMUM(8, -64),
    HID_RI_LOGICAL_MAXIMUM(8, 63),
    HID_RI_PHYSICAL_MAXIMUM(8, 127),
    HID_RI_REPORT_SIZE(8, 0x08),
    HID_RI_REPORT_COUNT(8, 0x01),
	HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),		 // Throttle

    HID_RI_USAGE_PAGE(8, 0x09),
    HID_RI_USAGE_MINIMUM(8, 0x01),
    HID_RI_USAGE_MAXIMUM(8, 0x08),
    HID_RI_LOGICAL_MINIMUM(8, 0x00),
    HID_RI_LOGICAL_MAXIMUM(8, 0x01),
	0x45, 0x01,		//     PHYSICAL_MAXIMUM (1)
    HID_RI_REPORT_SIZE(8, 0x01),
    HID_RI_REPORT_COUNT(8, 0x08),
    HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),

    HID_RI_USAGE_MINIMUM(8, 0x09),
    HID_RI_USAGE_MAXIMUM(8, 0x10),
    HID_RI_REPORT_SIZE(8, 0x01),
    HID_RI_REPORT_COUNT(8, 0x08),
    HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),

	HID_RI_USAGE_PAGE(8, 0x01), // Generic Desktop
	0x09, 0x39,		//     USAGE (Hat switch)
	0x95, 0x01,		//     REPORT_COUNT (1)
	0x75, 0x04,		//     REPORT_SIZE (4)
	0x15, 0x00,		//     LOGICAL_MINIMUM (0)
	0x25, 0x07,		//     LOGICAL_MAXIMUM (7)
	0x46, 0x3B, 0x01,	//     PHYSICAL_MAXIMUM (315)
	0x65, 0x14,		//     UNIT (Eng Rot:Angular Pos)
	0x81, 0x42,		//     INPUT (Data,Var,Abs,Null)	 4b Hat

	0x75, 0x04,		//     REPORT_SIZE (4)
	0x95, 0x01,		//     REPORT_COUNT (1)
	0x81, 0x01,		//     INPUT (Cnst,Ary,Abs)		 4b Fill
	
	0x55, 0x00, 	// ( UNIT_EXPONENT ( 0))
	0x65, 0x00, 	// ( UNIT ( None))
/*
// FF2 input descriptor:

	0x09,0x01,	// USAGE (01)
	0xA1,0x00,	// COLLECTION (Physical)
		0x66,0x00,0x00,	// UNIT (None)
		0x55,0x00,	// UNIT_EXPONENT (00)
		0x16,0x00,0xFE,	// LOGICAL_MINIMUM (-512)
		0x26,0xFF,0x01,	// LOGICAL_MAXIMUM (512)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x46,0xFF,0x03,	// PHYSICAL_MAXIMUM (1023)
		0x95,0x01,	// REPORT_COUNT (01)
		0x75,0x0A,	// REPORT_SIZE (0A)
		0x09,0x30,	// USAGE (X)
		0x81,0x02,	// INPUT (Data,Var,Abs)
		0x75,0x06,	// REPORT_SIZE (06)
		0x81,0x01,	// INPUT (Constant,Ary,Abs)
		0x09,0x31,	// USAGE (Y)
		0x75,0x0A,	// REPORT_SIZE (0A)
		0x81,0x02,	// INPUT (Data,Var,Abs)
		0x75,0x06,	// REPORT_SIZE (06)
		0x95,0x01,	// REPORT_COUNT (01)
		0x81,0x01,	// INPUT (Constant,Ary,Abs)
		0x15,0xE0,	// LOGICAL_MINIMUM (-32)
		0x25,0x1F,	// LOGICAL_MAXIMUM (31)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x45,0x3F,	// PHYSICAL_MAXIMUM (63)
		0x66,0x14,0x00,	// UNIT (Eng Rot:Angular Pos)
		0x09,0x35,	// USAGE (Rz)
		0x81,0x02,	// INPUT (Data,Var,Abs)
		0x75,0x02,	// REPORT_SIZE (02)
		0x81,0x01,	// INPUT (Constant,Ary,Abs)
		0x66,0x00,0x00,	// UNIT (None)
	0xC0,	// END COLLECTION ()
	0x75,0x07,	// REPORT_SIZE (07)
	0x95,0x01,	// REPORT_COUNT (01)
	0x15,0x00,	// LOGICAL_MINIMUM (00)
	0x25,0x7F,	// LOGICAL_MAXIMUM (7F)
	0x35,0x00,	// PHYSICAL_MINIMUM (00)
	0x45,0x7F,	// PHYSICAL_MAXIMUM (7F)
	0x09,0x36,	// USAGE (Slider)
	0x81,0x02,	// INPUT (Data,Var,Abs)
	0x75,0x01,	// REPORT_SIZE (01)
	0x81,0x01,	// INPUT (Constant,Ary,Abs)
	0x09,0x39,	// USAGE (Hat switch)
	0x15,0x00,	// LOGICAL_MINIMUM (00)
	0x25,0x07,	// LOGICAL_MAXIMUM (07)
	0x35,0x00,	// PHYSICAL_MINIMUM (00)
	0x46,0x3B,0x01,	// PHYSICAL_MAXIMUM (315)
	0x66,0x14,0x00,	// UNIT (Eng Rot:Angular Pos)
	0x75,0x04,	// REPORT_SIZE (04)
	0x95,0x01,	// REPORT_COUNT (01)
	0x81,0x42,	// INPUT (Data,Var,Abs,Null)
	0x95,0x01,	// REPORT_COUNT (01)
	0x81,0x01,	// INPUT (Constant,Ary,Abs)
	0x66,0x00,0x00,	// UNIT (None)
	0x05,0x09,	// USAGE_PAGE (Button)
	0x19,0x01,	// USAGE_MINIMUM (Button 1)
	0x29,0x08,	// USAGE_MAXIMUM (Button 8)
	0x15,0x00,	// LOGICAL_MINIMUM (00)
	0x25,0x01,	// LOGICAL_MAXIMUM (01)
	0x35,0x00,	// PHYSICAL_MINIMUM (00)
	0x45,0x01,	// PHYSICAL_MAXIMUM (01)
	0x95,0x08,	// REPORT_COUNT (08)
	0x75,0x01,	// REPORT_SIZE (01)
	0x81,0x02,	// INPUT (Data,Var,Abs)
	0x95,0x04,	// REPORT_COUNT (04)
	0x75,0x08,	// REPORT_SIZE (08)
	0x81,0x01,	// INPUT (Constant,Ary,Abs)
*/
	
	0x05,0x0F,	// USAGE_PAGE (Physical Interface)
	0x09,0x92,	// USAGE (PID State Report)
	0xA1,0x02,	// COLLECTION (Logical)
		0x85,0x02,	// REPORT_ID (02)
		0x09,0x9F,	// USAGE (Device Paused)
		0x09,0xA0,	// USAGE (Actuators Enabled)
		0x09,0xA4,	// USAGE (Safety Switch)
		0x09,0xA5,	// USAGE (Actuator Override Switch)
		0x09,0xA6,	// USAGE (Actuator Power)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x25,0x01,	// LOGICAL_MINIMUM (01)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x45,0x01,	// PHYSICAL_MAXIMUM (01)
		0x75,0x01,	// REPORT_SIZE (01)
		0x95,0x05,	// REPORT_COUNT (05)
		0x81,0x02,	// INPUT (Data,Var,Abs)
		0x95,0x03,	// REPORT_COUNT (03)
		0x81,0x03,	// INPUT (Constant,Var,Abs)
		0x09,0x94,	// USAGE (Effect Playing)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x25,0x01,	// LOGICAL_MAXIMUM (01)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x45,0x01,	// PHYSICAL_MAXIMUM (01)
		0x75,0x01,	// REPORT_SIZE (01)
		0x95,0x01,	// REPORT_COUNT (01)
		0x81,0x02,	// INPUT (Data,Var,Abs)
		0x09,0x22,	// USAGE (Effect Block Index)
		0x15,0x01,	// LOGICAL_MINIMUM (01)
		0x25,0x28,	// LOGICAL_MAXIMUM (28)
		0x35,0x01,	// PHYSICAL_MINIMUM (01)
		0x45,0x28,	// PHYSICAL_MAXIMUM (28)
		0x75,0x07,	// REPORT_SIZE (07)
		0x95,0x01,	// REPORT_COUNT (01)
		0x81,0x02,	// INPUT (Data,Var,Abs)
	0xC0,	// END COLLECTION ()
	
	0x09,0x21,	// USAGE (Set Effect Report)
	0xA1,0x02,	// COLLECTION (Logical)
		0x85,0x01,	// REPORT_ID (01)
		0x09,0x22,	// USAGE (Effect Block Index)
		0x15,0x01,	// LOGICAL_MINIMUM (01)
		0x25,0x28,	// LOGICAL_MAXIMUM (28)
		0x35,0x01,	// PHYSICAL_MINIMUM (01)
		0x45,0x28,	// PHYSICAL_MAXIMUM (28)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x25,	// USAGE (25)
		0xA1,0x02,	// COLLECTION (Logical)
			0x09,0x26,	// USAGE (26)
			0x09,0x27,	// USAGE (27)
			0x09,0x30,	// USAGE (30)
			0x09,0x31,	// USAGE (31)
			0x09,0x32,	// USAGE (32)
			0x09,0x33,	// USAGE (33)
			0x09,0x34,	// USAGE (34)
			0x09,0x40,	// USAGE (40)
			0x09,0x41,	// USAGE (41)
			0x09,0x42,	// USAGE (42)
			0x09,0x43,	// USAGE (43)
			0x09,0x28,	// USAGE (28)
			0x25,0x0C,	// LOGICAL_MAXIMUM (0C)
			0x15,0x01,	// LOGICAL_MINIMUM (01)
			0x35,0x01,	// PHYSICAL_MINIMUM (01)
			0x45,0x0C,	// PHYSICAL_MAXIMUM (0C)
			0x75,0x08,	// REPORT_SIZE (08)
			0x95,0x01,	// REPORT_COUNT (01)
			0x91,0x00,	// OUTPUT (Data)
		0xC0,	// END COLLECTION ()
		0x09,0x50,	// USAGE (Duration)
		0x09,0x54,	// USAGE (Trigger Repeat Interval)
		0x09,0x51,	// USAGE (Sample Period)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x26,0xFF,0x7F,	// LOGICAL_MAXIMUM (7F FF)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x46,0xFF,0x7F,	// PHYSICAL_MAXIMUM (7F FF)
		0x66,0x03,0x10,	// UNIT (Eng Lin:Time)
		0x55,0xFD,	// UNIT_EXPONENT (-3)
		0x75,0x10,	// REPORT_SIZE (10)
		0x95,0x03,	// REPORT_COUNT (03)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x55,0x00,	// UNIT_EXPONENT (00)
		0x66,0x00,0x00,	// UNIT (None)
		0x09,0x52,	// USAGE (Gain)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x53,	// USAGE (Trigger Button)
		0x15,0x01,	// LOGICAL_MINIMUM (01)
		0x25,0x08,	// LOGICAL_MAXIMUM (08)
		0x35,0x01,	// PHYSICAL_MINIMUM (01)
		0x45,0x08,	// PHYSICAL_MAXIMUM (08)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x55,	// USAGE (Axes Enable)
		0xA1,0x02,	// COLLECTION (Logical)
			0x05,0x01,	// USAGE_PAGE (Generic Desktop)
			0x09,0x30,	// USAGE (X)
			0x09,0x31,	// USAGE (Y)
			0x15,0x00,	// LOGICAL_MINIMUM (00)
			0x25,0x01,	// LOGICAL_MAXIMUM (01)
			0x75,0x01,	// REPORT_SIZE (01)
			0x95,0x02,	// REPORT_COUNT (02)
			0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0xC0,	// END COLLECTION ()
		0x05,0x0F,	// USAGE_PAGE (Physical Interface)
		0x09,0x56,	// USAGE (Direction Enable)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x95,0x05,	// REPORT_COUNT (05)
		0x91,0x03,	// OUTPUT (Constant,Var,Abs)
		0x09,0x57,	// USAGE (Direction)
		0xA1,0x02,	// COLLECTION (Logical)
			0x0B,0x01,0x00,0x0A,0x00,
			0x0B,0x02,0x00,0x0A,0x00,
			0x66,0x14,0x00,	// UNIT (Eng Rot:Angular Pos)
			0x55,0xFE,	// UNIT_EXPONENT (FE)
			0x15,0x00,	// LOGICAL_MINIMUM (00)
			0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
			0x35,0x00,	// PHYSICAL_MINIMUM (00)
			0x47,0xA0,0x8C,0x00,0x00,	// PHYSICAL_MAXIMUM (00 00 8C A0)
			0x66,0x00,0x00,	// UNIT (None)
			0x75,0x08,	// REPORT_SIZE (08)
			0x95,0x02,	// REPORT_COUNT (02)
			0x91,0x02,	// OUTPUT (Data,Var,Abs)
			0x55,0x00,	// UNIT_EXPONENT (00)
			0x66,0x00,0x00,	// UNIT (None)
		0xC0,	// END COLLECTION ()
		0x05,0x0F,	// USAGE_PAGE (Physical Interface)
//		0x09,0xA7,	// USAGE (Start Delay)
		0x66,0x03,0x10,	// UNIT (Eng Lin:Time)
		0x55,0xFD,	// UNIT_EXPONENT (-3)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x26,0xFF,0x7F,	// LOGICAL_MAXIMUM (7F FF)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x46,0xFF,0x7F,	// PHYSICAL_MAXIMUM (7F FF)
		0x75,0x10,	// REPORT_SIZE (10)
		0x95,0x01,	// REPORT_COUNT (01)
//		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x66,0x00,0x00,	// UNIT (None)
		0x55,0x00,	// UNIT_EXPONENT (00)
	0xC0,	// END COLLECTION ()
	
	0x05,0x0F,	// USAGE_PAGE (Physical Interface)
	0x09,0x5A,	// USAGE (Set Envelope Report)
	0xA1,0x02,	// COLLECTION (Logical)
		0x85,0x02,	// REPORT_ID (02)
		0x09,0x22,	// USAGE (Effect Block Index)
		0x15,0x01,	// LOGICAL_MINIMUM (01)
		0x25,0x28,	// LOGICAL_MAXIMUM (28)
		0x35,0x01,	// PHYSICAL_MINIMUM (01)
		0x45,0x28,	// PHYSICAL_MAXIMUM (28)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x5B,	// USAGE (Attack Level)
		0x09,0x5D,	// USAGE (Fade Level)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
		0x95,0x02,	// REPORT_COUNT (02)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x5C,	// USAGE (5C)
		0x09,0x5E,	// USAGE (5E)
		0x66,0x03,0x10,	// UNIT (Eng Lin:Time)
		0x55,0xFD,	// UNIT_EXPONENT (-3)
		0x26,0xFF,0x7F,	// LOGICAL_MAXIMUM (7F FF)
		0x46,0xFF,0x7F,	// PHYSICAL_MAXIMUM (7F FF)
		0x75,0x10,	// REPORT_SIZE (10)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x45,0x00,	// PHYSICAL_MAXIMUM (00)
		0x66,0x00,0x00,	// UNIT (None)
		0x55,0x00,	// UNIT_EXPONENT (00)
	0xC0,	// END COLLECTION ()
	
	0x09,0x5F,	// USAGE (Set Condition Report)
	0xA1,0x02,	// COLLECTION (Logical)
		0x85,0x03,	// REPORT_ID (03)
		0x09,0x22,	// USAGE (Effect Block Index)
		0x15,0x01,	// LOGICAL_MINIMUM (01)
		0x25,0x28,	// LOGICAL_MAXIMUM (28)
		0x35,0x01,	// PHYSICAL_MINIMUM (01)
		0x45,0x28,	// PHYSICAL_MAXIMUM (28)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x23,	// USAGE (Parameter Block Offset)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x25,0x01,	// LOGICAL_MAXIMUM (01)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x45,0x01,	// PHYSICAL_MAXIMUM (01)
		0x75,0x04,	// REPORT_SIZE (04)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x58,	// USAGE (Type Specific Block Offset)
		0xA1,0x02,	// COLLECTION (Logical)
			0x0B,0x01,0x00,0x0A,0x00,	// USAGE (Instance 1)
			0x0B,0x02,0x00,0x0A,0x00,	// USAGE (Instance 2)
			0x75,0x02,	// REPORT_SIZE (02)
			0x95,0x02,	// REPORT_COUNT (02)
			0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0xC0,	// END COLLECTION ()
		0x15,0x80,	// LOGICAL_MINIMUM (80)
		0x25,0x7F,	// LOGICAL_MAXIMUM (7F)
		0x36,0xF0,0xD8,	// PHYSICAL_MINIMUM (-10000)
		0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
		0x09,0x60,	// USAGE (CP Offset)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x36,0xF0,0xD8,	// PHYSICAL_MINIMUM (-10000)
		0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
		0x09,0x61,	// USAGE (Positive Coefficient)
//		0x09,0x62,	// USAGE (Negative Coefficient)
		0x95,0x01,	// REPORT_COUNT (01)	// ???? WAS 2 with "negative coeff"
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
		0x09,0x63,	// USAGE (Positive Saturation)
		0x09,0x64,	// USAGE (Negative Saturation)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x02,	// REPORT_COUNT (02)
//		0x91,0x02,	// OUTPUT (Data,Var,Abs)
//		0x09,0x65,	// USAGE (Dead Band )
		0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
		0x95,0x01,	// REPORT_COUNT (01)
//		0x91,0x02,	// OUTPUT (Data,Var,Abs)
	0xC0,	// END COLLECTION ()
	
	0x09,0x6E,	// USAGE (Set Periodic Report)
	0xA1,0x02,	// COLLECTION (Logical)
		0x85,0x04,	// REPORT_ID (04)
		0x09,0x22,	// USAGE (Effect Block Index)
		0x15,0x01,	// LOGICAL_MINIMUM (01)
		0x25,0x28,	// LOGICAL_MAXIMUM (28)
		0x35,0x01,	// PHYSICAL_MINIMUM (01)
		0x45,0x28,	// PHYSICAL_MAXIMUM (28)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x70,	// USAGE (Magnitude)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x6F,	// USAGE (Offset)
		0x15,0x80,	// LOGICAL_MINIMUM (80)
		0x25,0x7F,	// LOGICAL_MAXIMUM (7F)
		0x36,0xF0,0xD8,	// PHYSICAL_MINIMUM (-10000)
		0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x71,	// USAGE (Phase)
		0x66,0x14,0x00,	// UNIT (Eng Rot:Angular Pos)
		0x55,0xFE,	// UNIT_EXPONENT (FE)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x47,0xA0,0x8C,0x00,0x00,	// PHYSICAL_MAXIMUM (00 00 8C A0)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x72,	// USAGE (Period)
		0x26,0xFF,0x7F,	// LOGICAL_MAXIMUM (7F FF)
		0x46,0xFF,0x7F,	// PHYSICAL_MAXIMUM (7F FF)
		0x66,0x03,0x10,	// UNIT (Eng Lin:Time)
		0x55,0xFD,	// UNIT_EXPONENT (-3)
		0x75,0x10,	// REPORT_SIZE (10)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x66,0x00,0x00,	// UNIT (None)
		0x55,0x00,	// UNIT_EXPONENT (00)
	0xC0,	// END COLLECTION ()
	
	0x09,0x73,	// USAGE (Set Constant Force Report)
	0xA1,0x02,	// COLLECTION (Logical)
		0x85,0x05,	// REPORT_ID (05)
		0x09,0x22,	// USAGE (Effect Block Index)
		0x15,0x01,	// LOGICAL_MINIMUM (01)
		0x25,0x28,	// LOGICAL_MAXIMUM (28)
		0x35,0x01,	// PHYSICAL_MINIMUM (01)
		0x45,0x28,	// PHYSICAL_MAXIMUM (28)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x70,	// USAGE (Magnitude)
		0x16,0x01,0xFF,	// LOGICAL_MINIMUM (-255)
		0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (255)
		0x36,0xF0,0xD8,	// PHYSICAL_MINIMUM (-10000)
		0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
		0x75,0x10,	// REPORT_SIZE (10)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
	0xC0,	// END COLLECTION ()
	
	0x09,0x74,	// USAGE (Set Ramp Force Report)
	0xA1,0x02,	// COLLECTION (Logical)
		0x85,0x06,	// REPORT_ID (06)
		0x09,0x22,	// USAGE (Effect Block Index)
		0x15,0x01,	// LOGICAL_MINIMUM (01)
		0x25,0x28,	// LOGICAL_MAXIMUM (28)
		0x35,0x01,	// PHYSICAL_MINIMUM (01)
		0x45,0x28,	// PHYSICAL_MAXIMUM (28)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x75,	// USAGE (Ramp Start)
		0x09,0x76,	// USAGE (Ramp End)
		0x15,0x80,	// LOGICAL_MINIMUM (-128)
		0x25,0x7F,	// LOGICAL_MAXIMUM (127)
		0x36,0xF0,0xD8,	// PHYSICAL_MINIMUM (-10000)
		0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x02,	// REPORT_COUNT (02)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
	0xC0,	// END COLLECTION ()
	
	0x09,0x68,	// USAGE (Custom Force Data Report)
	0xA1,0x02,	// COLLECTION (Logical)
		0x85,0x07,	// REPORT_ID (07)
		0x09,0x22,	// USAGE (Effect Block Index)
		0x15,0x01,	// LOGICAL_MINIMUM (01)
		0x25,0x28,	// LOGICAL_MAXIMUM (28)
		0x35,0x01,	// PHYSICAL_MINIMUM (01)
		0x45,0x28,	// PHYSICAL_MAXIMUM (28)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x6C,	// USAGE (Custom Force Data Offset)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x26,0x10,0x27,	// LOGICAL_MAXIMUM (10000)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
		0x75,0x10,	// REPORT_SIZE (10)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x69,	// USAGE (Custom Force Data)
		0x15,0x81,	// LOGICAL_MINIMUM (-127)
		0x25,0x7F,	// LOGICAL_MAXIMUM (127)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x46,0xFF,0x00,	// PHYSICAL_MAXIMUM (255)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x0C,	// REPORT_COUNT (0C)
		0x92,0x02,0x01,	// OUTPUT ( Data,Var,Abs,Buf)
	0xC0,	// END COLLECTION ()
	
	0x09,0x66,	// USAGE (Download Force Sample)
	0xA1,0x02,	// COLLECTION (Logical)
		0x85,0x08,	// REPORT_ID (08)
		0x05,0x01,	// USAGE_PAGE (Generic Desktop)
		0x09,0x30,	// USAGE (X)
		0x09,0x31,	// USAGE (Y)
		0x15,0x81,	// LOGICAL_MINIMUM (-127)
		0x25,0x7F,	// LOGICAL_MAXIMUM (127)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x46,0xFF,0x00,	// PHYSICAL_MAXIMUM (255)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x02,	// REPORT_COUNT (02)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
	0xC0,	// END COLLECTION ()
	
	0x05,0x0F,	// USAGE_PAGE (Physical Interface)
	0x09,0x77,	// USAGE (Effect Operation Report)
	0xA1,0x02,	// COLLECTION (Logical)
		0x85,0x0A,	// REPORT_ID (0A)
		0x09,0x22,	// USAGE (Effect Block Index)
		0x15,0x01,	// LOGICAL_MINIMUM (01)
		0x25,0x28,	// LOGICAL_MAXIMUM (28)
		0x35,0x01,	// PHYSICAL_MINIMUM (01)
		0x45,0x28,	// PHYSICAL_MAXIMUM (28)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x78,	// USAGE (78)
		0xA1,0x02,	// COLLECTION (Logical)
			0x09,0x79,	// USAGE (Op Effect Start)
			0x09,0x7A,	// USAGE (Op Effect Start Solo)
			0x09,0x7B,	// USAGE (Op Effect Stop)
			0x15,0x01,	// LOGICAL_MINIMUM (01)
			0x25,0x03,	// LOGICAL_MAXIMUM (03)
			0x75,0x08,	// REPORT_SIZE (08)
			0x95,0x01,	// REPORT_COUNT (01)
			0x91,0x00,	// OUTPUT (Data,Ary,Abs)
		0xC0,	// END COLLECTION ()
		0x09,0x7C,	// USAGE (Loop Count)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x46,0xFF,0x00,	// PHYSICAL_MAXIMUM (00 FF)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
	0xC0,	// END COLLECTION ()
	
	0x09,0x90,	// USAGE (PID Block Free Report)
	0xA1,0x02,	// COLLECTION (Logical)
		0x85,0x0B,	// REPORT_ID (0B)
		0x09,0x22,	// USAGE (Effect Block Index)
		0x25,0x28,	// LOGICAL_MAXIMUM (28)
		0x15,0x01,	// LOGICAL_MINIMUM (01)
		0x35,0x01,	// PHYSICAL_MINIMUM (01)
		0x45,0x28,	// PHYSICAL_MAXIMUM (28)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
	0xC0,	// END COLLECTION ()
	
	0x09,0x96,	// USAGE (PID Device Control)
	0xA1,0x02,	// COLLECTION (Logical)
		0x85,0x0C,	// REPORT_ID (0C)
		0x09,0x97,	// USAGE (DC Enable Actuators)
		0x09,0x98,	// USAGE (DC Disable Actuators)
		0x09,0x99,	// USAGE (DC Stop All Effects)
		0x09,0x9A,	// USAGE (DC Device Reset)
		0x09,0x9B,	// USAGE (DC Device Pause)
		0x09,0x9C,	// USAGE (DC Device Continue)
		0x15,0x01,	// LOGICAL_MINIMUM (01)
		0x25,0x06,	// LOGICAL_MAXIMUM (06)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x00,	// OUTPUT (Data)
	0xC0,	// END COLLECTION ()
	
	0x09,0x7D,	// USAGE (Device Gain Report)
	0xA1,0x02,	// COLLECTION (Logical)
		0x85,0x0D,	// REPORT_ID (0D)
		0x09,0x7E,	// USAGE (Device Gain)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
	0xC0,	// END COLLECTION ()
	
	0x09,0x6B,	// USAGE (Set Custom Force Report)
	0xA1,0x02,	// COLLECTION (Logical)
		0x85,0x0E,	// REPORT_ID (0E)
		0x09,0x22,	// USAGE (Effect Block Index)
		0x15,0x01,	// LOGICAL_MINIMUM (01)
		0x25,0x28,	// LOGICAL_MAXIMUM (28)
		0x35,0x01,	// PHYSICAL_MINIMUM (01)
		0x45,0x28,	// PHYSICAL_MAXIMUM (28)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x6D,	// USAGE (Sample Count)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x46,0xFF,0x00,	// PHYSICAL_MAXIMUM (00 FF)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x09,0x51,	// USAGE (Sample Period)
		0x66,0x03,0x10,	// UNIT (Eng Lin:Time)
		0x55,0xFD,	// UNIT_EXPONENT (-3)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x26,0xFF,0x7F,	// LOGICAL_MAXIMUM (32767)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x46,0xFF,0x7F,	// PHYSICAL_MAXIMUM (32767)
		0x75,0x10,	// REPORT_SIZE (10)
		0x95,0x01,	// REPORT_COUNT (01)
		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0x55,0x00,	// UNIT_EXPONENT (00)
		0x66,0x00,0x00,	// UNIT (None)
	0xC0,	// END COLLECTION ()
	
	0x09,0xAB,	// USAGE (Create New Effect Report)
	0xA1,0x02,	// COLLECTION (Logical)
		0x85,0x05,	// REPORT_ID (05)
		0x09,0x25,	// USAGE (Effect Type)
		0xA1,0x02,	// COLLECTION (Logical)
			0x09,0x26,	// USAGE (26)
			0x09,0x27,	// USAGE (27)
			0x09,0x30,	// USAGE (30)
			0x09,0x31,	// USAGE (31)
			0x09,0x32,	// USAGE (32)
			0x09,0x33,	// USAGE (33)
			0x09,0x34,	// USAGE (34)
			0x09,0x40,	// USAGE (40)
			0x09,0x41,	// USAGE (41)
			0x09,0x42,	// USAGE (42)
			0x09,0x43,	// USAGE (43)
			0x09,0x28,	// USAGE (28)
			0x25,0x0C,	// LOGICAL_MAXIMUM (0C)
			0x15,0x01,	// LOGICAL_MINIMUM (01)
			0x35,0x01,	// PHYSICAL_MINIMUM (01)
			0x45,0x0C,	// PHYSICAL_MAXIMUM (0C)
			0x75,0x08,	// REPORT_SIZE (08)
			0x95,0x01,	// REPORT_COUNT (01)
			0xB1,0x00,	// FEATURE (Data)
		0xC0,	// END COLLECTION ()
		0x05,0x01,	// USAGE_PAGE (Generic Desktop)
		0x09,0x3B,	// USAGE (Byte Count)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x26,0xFF,0x01,	// LOGICAL_MAXIMUM (511)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x46,0xFF,0x01,	// PHYSICAL_MAXIMUM (511)
		0x75,0x0A,	// REPORT_SIZE (0A)
		0x95,0x01,	// REPORT_COUNT (01)
		0xB1,0x02,	// FEATURE (Data,Var,Abs)
		0x75,0x06,	// REPORT_SIZE (06)
		0xB1,0x01,	// FEATURE (Constant,Ary,Abs)
	0xC0,	// END COLLECTION ()
	
	0x05,0x0F,	// USAGE_PAGE (Physical Interface)
	0x09,0x89,	// USAGE (PID Block Load Report)
	0xA1,0x02,	// COLLECTION (Logical)
		0x85,0x06,	// REPORT_ID (06)
		0x09,0x22,	// USAGE (Effect Block Index)
		0x25,0x28,	// LOGICAL_MAXIMUM (28)
		0x15,0x01,	// LOGICAL_MINIMUM (01)
		0x35,0x01,	// PHYSICAL_MINIMUM (01)
		0x45,0x28,	// PHYSICAL_MAXIMUM (28)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0xB1,0x02,	// FEATURE (Data,Var,Abs)
		0x09,0x8B,	// USAGE (Block Load Status)
		0xA1,0x02,	// COLLECTION (Logical)
			0x09,0x8C,	// USAGE (Block Load Success)
			0x09,0x8D,	// USAGE (Block Load Full)
			0x09,0x8E,	// USAGE (Block Load Error)
			0x25,0x03,	// LOGICAL_MAXIMUM (03)
			0x15,0x01,	// LOGICAL_MINIMUM (01)
			0x35,0x01,	// PHYSICAL_MINIMUM (01)
			0x45,0x03,	// PHYSICAL_MAXIMUM (03)
			0x75,0x08,	// REPORT_SIZE (08)
			0x95,0x01,	// REPORT_COUNT (01)
			0xB1,0x00,	// FEATURE (Data)
		0xC0,	// END COLLECTION ()
		0x09,0xAC,	// USAGE (RAM Pool Available)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x27,0xFF,0xFF,0x00,0x00,	// LOGICAL_MAXIMUM (00 00 FF FF)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x47,0xFF,0xFF,0x00,0x00,	// PHYSICAL_MAXIMUM (00 00 FF FF)
		0x75,0x10,	// REPORT_SIZE (10)
		0x95,0x01,	// REPORT_COUNT (01)
		0xB1,0x00,	// FEATURE (Data)
	0xC0,	// END COLLECTION ()
	
	0x09,0x7F,	// USAGE (PID Pool Report)
	0xA1,0x02,	// COLLECTION (Logical)
		0x85,0x07,	// REPORT_ID (07)
		0x09,0x80,	// USAGE (RAM Pool Size)
		0x75,0x10,	// REPORT_SIZE (10)
		0x95,0x01,	// REPORT_COUNT (01)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x27,0xFF,0xFF,0x00,0x00,	// LOGICAL_MAXIMUM (00 00 FF FF)
		0x47,0xFF,0xFF,0x00,0x00,	// PHYSICAL_MAXIMUM (00 00 FF FF)
		0xB1,0x02,	// FEATURE (Data,Var,Abs)
		0x09,0x83,	// USAGE (Simultaneous Effects Max)
		0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
		0x46,0xFF,0x00,	// PHYSICAL_MAXIMUM (00 FF)
		0x75,0x08,	// REPORT_SIZE (08)
		0x95,0x01,	// REPORT_COUNT (01)
		0xB1,0x02,	// FEATURE (Data,Var,Abs)
		0x09,0xA9,	// USAGE (Device Managed Pool)
		0x09,0xAA,	// USAGE (Shared Parameter Blocks)
		0x75,0x01,	// REPORT_SIZE (01)
		0x95,0x02,	// REPORT_COUNT (02)
		0x15,0x00,	// LOGICAL_MINIMUM (00)
		0x25,0x01,	// LOGICAL_MAXIMUM (01)
		0x35,0x00,	// PHYSICAL_MINIMUM (00)
		0x45,0x01,	// PHYSICAL_MAXIMUM (01)
		0xB1,0x02,	// FEATURE (Data,Var,Abs)
		0x75,0x06,	// REPORT_SIZE (06)
		0x95,0x01,	// REPORT_COUNT (01)
		0xB1,0x03,	// FEATURE ( Cnst,Var,Abs)
	0xC0,	// END COLLECTION ()
0xC0,	// END COLLECTION ()
};
/** Device descriptor structure. This descriptor, located in FLASH memory, describes the overall
 *  device characteristics, including the supported USB version, control endpoint size and the
 *  number of device configurations. The descriptor is read out by the USB host when the enumeration
 *  process begins.
 */
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
	.Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

	.USBSpecification       = VERSION_BCD(01.10),
	.Class                  = USB_CSCP_NoDeviceClass,
	.SubClass               = USB_CSCP_NoDeviceSubclass,
	.Protocol               = USB_CSCP_NoDeviceProtocol,

	.Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

	.VendorID               = 0x03EB,
	.ProductID              = 0x2056,	// WAS 0x2043
	.ReleaseNumber          = VERSION_BCD(00.01),

	.ManufacturerStrIndex   = 0x01,
	.ProductStrIndex        = 0x02,
	.SerialNumStrIndex      = NO_DESCRIPTOR,

	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

/** Configuration descriptor structure. This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
	.Config =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

			.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
			.TotalInterfaces        = 1,

			.ConfigurationNumber    = 1,
			.ConfigurationStrIndex  = NO_DESCRIPTOR,

			.ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),

			.MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
		},

	.HID_Interface =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

			.InterfaceNumber        = 0x00,
			.AlternateSetting       = 0x00,

			.TotalEndpoints         = 2,

			.Class                  = HID_CSCP_HIDClass,
			.SubClass               = HID_CSCP_NonBootSubclass,
			.Protocol               = HID_CSCP_NonBootProtocol,

			.InterfaceStrIndex      = NO_DESCRIPTOR
		},

	.HID_JoystickHID =
		{
			.Header                 = {.Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID},

			.HIDSpec                = VERSION_BCD(01.11),
			.CountryCode            = 0x00,
			.TotalReportDescriptors = 1,
			.HIDReportType          = HID_DTYPE_Report,
			.HIDReportLength        = sizeof(JoystickReport)
		},


	.HID_ReportINEndpoint =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

			.EndpointAddress        = (ENDPOINT_DIR_IN | JOYSTICK_EPNUM),
			.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = JOYSTICK_EPSIZE,
			.PollingIntervalMS      = 0x01
		},

	.HID_ReportOUTEndpoint =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

			.EndpointAddress        = (ENDPOINT_DIR_OUT | FFB_EPNUM),
			.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = FFB_EPSIZE,
			.PollingIntervalMS      = 0x01
		}
};

/** Language descriptor structure. This descriptor, located in FLASH memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
const USB_Descriptor_String_t PROGMEM LanguageString =
{
	.Header                 = {.Size = USB_STRING_LEN(1), .Type = DTYPE_String},

	.UnicodeString          = {LANGUAGE_ID_ENG}
};

/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ManufacturerString =
{
	.Header                 = {.Size = USB_STRING_LEN(11), .Type = DTYPE_String},

	.UnicodeString          = L"Dean Camera"
};

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ProductString =
{
	.Header                 = {.Size = USB_STRING_LEN(18), .Type = DTYPE_String},

	.UnicodeString          = L"LUFA Joystick wFFB"
};

/** This function is called by the library when in device mode, and must be overridden (see library "USB Descriptors"
 *  documentation) by the application code so that the address and size of a requested descriptor can be given
 *  to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 *  is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 *  USB host.
 */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void** const DescriptorAddress)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	const void* Address = NULL;
	uint16_t    Size    = NO_DESCRIPTOR;

	switch (DescriptorType)
	{
		case DTYPE_Device:
			Address = &DeviceDescriptor;
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			Address = &ConfigurationDescriptor;
			Size    = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String:
			switch (DescriptorNumber)
			{
				case 0x00:
					Address = &LanguageString;
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case 0x01:
					Address = &ManufacturerString;
					Size    = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case 0x02:
					Address = &ProductString;
					Size    = pgm_read_byte(&ProductString.Header.Size);
					break;
			}

			break;
		case DTYPE_HID:
			Address = &ConfigurationDescriptor.HID_JoystickHID;
			Size    = sizeof(USB_HID_Descriptor_HID_t);
			break;
		case DTYPE_Report:
			Address = &JoystickReport;
			Size    = sizeof(JoystickReport);
			break;
	}

	*DescriptorAddress = Address;
	return Size;
}

