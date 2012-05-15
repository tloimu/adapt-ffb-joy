/*
  Force Feedback Joystick
  Basic debugging utilities.

  This code is for Microsoft Sidewinder Force Feedback Pro joystick
  with some room for additional extra controls.

  Copyright 2012  Tero Loimuneva (tloimu [at] gmail [dot] com)
  MIT License.

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

#include "debug.h"
#include "ffb.h"

// If defined, enables sending debug data to MIDI OUT channel.
// If not defined, discards all debug data.
const uint8_t FFB_ENABLE_DEBUG = 0;

void LogSendByte(uint8_t data)
	{
	if (!FFB_ENABLE_DEBUG)
		{
		_delay_us(300);
		return;
		}
	// Wait if a byte is being transmitted
	while((UCSR1A & (1<<UDRE1)) == 0);

	UDR1 = data;
	}


void LogSendData(uint8_t *data, uint16_t len)
	{
	uint16_t i = 0;
	for (i = 0; i < len; i++)
		LogSendByte(data[i]);
	}


void LogText(const char *text)
	{
	uint8_t temp = 0;	// Start text
	LogSendData(&temp, 1);
	LogSendData((uint8_t*) text , strlen(text) + 1);	// Include terminating NULL
	}

void LogBinary(const void *data, uint16_t len)
	{
    uint8_t temp = (uint8_t) (len & 0xFF);
	if (temp > 0)
		{
		LogSendData(&temp, 1);
		LogSendData((uint8_t*) data, temp);
		}
	}

void LogData(const char *text, uint8_t reportId, const void *data, uint16_t len)
	{
	LogText(text);
	LogBinary(&reportId, 1);
	LogBinary(data, len);
	}

// Log all reports found in the given data (may have one or more)
void LogReport(const char *text, const uint16_t *reportSizeArray, uint8_t *data, uint16_t len)
	{
	LogText(text);

	uint8_t *p = data;

	while (p < data + len)
		{
		uint8_t replen = reportSizeArray[p[0]-1];
		LogBinary(p, 1);
		if (replen > 1)
			LogBinary(&p[1], replen-1);
		p += replen;
		}
	}
