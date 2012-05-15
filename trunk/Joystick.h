/*
	Copyright 2012  Tero Loimuneva (tloimu [at] gmail [dot] com)

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/** \file
 *
 *  Header file for Joystick.c.
 */

#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include "main.h"

extern const uint8_t INPUT_REPORTID_ALL;

// Data structures for input reports from the joystick positions

typedef struct
	{
	// Joystick Input Report
	uint8_t	reportId;	// =1
	int16_t  X;
	int16_t  Y;
	int16_t  Z;
	int8_t  Rz, Rx, Ry;
	uint8_t  Rudder;
	uint8_t  Throttle;
	uint16_t Button;
	uint8_t Hat;
	} USB_JoystickReport_Data_t;

// Functions that form the inferface from the generic parts of the code
// to joystick model specific parts.


// Gets called at very beginning to allow joystick model specific
// initializations of the hardware and software to occur.
void Joystick_Init(void);

// Gets called to check connection to the joystick at startup
// or after disconnect.
// If initialization succeeds, the function should return true.
// If it fails, return false and this functions gets called again
// until further progress is made (e.g. waiting for the actual
// joystick to be connected).
int Joystick_Connect(void);

// Gets called when input report of joysticks position, buttons etc. are
// requested. Data written to <outReportData> is sent to host if the function
// returns true. If false is returned, nothing is sent.
// If <inReportId> has value INPUT_REPORTID_ALL, all input report IDs should
// generated.
int Joystick_CreateInputReport(uint8_t inReportId, USB_JoystickReport_Data_t* const outReportData);

#endif

