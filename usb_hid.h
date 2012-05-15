/*******************************************************************************
 * File Name	: usb_hid.h
 * Project	: 3DP-Vert, Microsoft Sidewinder 3D Pro/PP/FFP to USB converter
 * Date		: 2009/06/27
 * Version      : 1.0
 * Target MCU	: AT90USB162/82, AT90USB646/1286, ATMEGA16U4/32U4
 * Tool Chain	: Atmel AVR Studio 4.18 716 / WinAVR 20100110
 * Author       : Detlef "Grendel" Mueller
 *                detlef@gmail.com
 * Release Notes:
 *
 *	================
 *	WARNING: This version has been heavily modified to only support
 *           Force Feedback Pro and ATmega32U4.
 *           Do not use except for testing purposes!
 *
 *             -- tloimu
 *	================
 *
 * $Id: usb_hid.h 1.5 2009/10/26 07:34:30 Detlef Exp Detlef $
 ******************************************************************************/

#ifndef __usb_hid_h__
#define __usb_hid_h__

//------------------------------------------------------------------------------
// Standard control endpoint request types

#define GET_STATUS		0
#define CLEAR_FEATURE		1
#define SET_FEATURE		3
#define SET_ADDRESS		5
#define GET_DESCRIPTOR		6
#define GET_CONFIGURATION	8
#define SET_CONFIGURATION	9
#define GET_INTERFACE		10
#define SET_INTERFACE		11

//------------------------------------------------------------------------------
// HID (human interface device)

#define HID_GET_REPORT		1
#define HID_GET_IDLE		2
#define HID_GET_PROTOCOL	3
#define HID_SET_REPORT		9
#define HID_SET_IDLE		10
#define HID_SET_PROTOCOL	11

//------------------------------------------------------------------------------

#define LSB( n )		( (uint16_t)(n)       & 0xFF)
#define MSB( n )		(((uint16_t)(n) >> 8) & 0xFF)

#define	LVAL( n )		LSB( n ), MSB( n )

#endif // __usb_hid_h__

//------------------------------------------------------------------------------
