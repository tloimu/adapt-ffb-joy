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

const uint8_t DEBUG_TO_NONE = 0;
const uint8_t DEBUG_TO_MIDI = 1;
const uint8_t DEBUG_TO_USB = 2;
const uint8_t DEBUG_DETAIL = 4;

volatile uint8_t gDebugMode = 2;

#ifdef DEBUG_ENABLE_USB

volatile char debug_buffer[DEBUG_BUFFER_SIZE];
volatile uint16_t debug_buffer_used = 0;

#endif


#define DEBUG_DATA_AS_HEX

void LogSendData(uint8_t *data, uint16_t len)
	{
	_delay_us(300);

	uint16_t i = 0;
	for (i = 0; i < len; i++)
		{
#ifdef DEBUG_DATA_AS_HEX
		static const char *hexmap = "0123456789ABCDEF";
		LogSendByte( ' ' );
		LogSendByte( hexmap[(data[i] >> 4)]);
		LogSendByte( hexmap[(data[i] & 0x0F)]);
#else
		LogSendByte(data[i]);
#endif
		}
	}


void LogText(const char *text)
	{
	while (1)
		{
		char c = *text++;
		if (c == 0)
			break;
		if (c == '\n')
			LogSendByte('\r');	// CR+LF
		LogSendByte(c);
		}
	}

void LogTextLf(const char *text)
	{
	LogText(text);
	LogSendByte('\r');	// CR+LF
	LogSendByte('\n');	// CR+LF
	}


void LogTextP(const char *text)
	{
	while (1)
		{
		char c = pgm_read_byte(text++);
		if (c == 0)
			break;
		if (c == '\n')
			LogSendByte('\r');	// CR+LF
		LogSendByte(c);
		}
	}

void LogTextLfP(const char *text)
	{
	LogTextP(text);
	LogSendByte('\r');	// CR+LF
	LogSendByte('\n');	// CR+LF
	}


void LogBinary(const void *data, uint16_t len)
	{
    uint8_t temp = (uint8_t) (len & 0xFF);
	if (temp > 0)
		{
		LogSendData((uint8_t*) data, temp);
		}
	}

void LogBinaryLf(const void *data, uint16_t len)
	{
    uint8_t temp = (uint8_t) (len & 0xFF);
	if (temp > 0)
		{
		LogSendData((uint8_t*) data, temp);
		}
	LogSendByte('\r');	// CR+LF
	LogSendByte('\n');	// CR+LF
	}

void LogData(const char *text, uint8_t reportId, const void *data, uint16_t len)
	{
	LogText(text);
	LogBinary(&reportId, 1);
	LogBinary(data, len);
	}

void LogDataLf(const char *text, uint8_t reportId, const void *data, uint16_t len)
	{
	LogText(text);
	LogBinary(&reportId, 1);
	LogBinaryLf(data, len);
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
	LogSendByte('\r');	// CR+LF
	LogSendByte('\n');	// CR+LF
	}


// --------------------


void LogSendByte(uint8_t data)
	{
#ifdef DEBUG_ENABLE_USB

	if (gDebugMode & DEBUG_TO_USB)
		{
		if (debug_buffer_used >= DEBUG_BUFFER_SIZE)
			return;	// overflow - discard

		debug_buffer[debug_buffer_used++] = data;
		}
#endif

#ifdef DEBUG_ENABLE_MIDI
	if (gDebugMode & DEBUG_TO_MIDI)
		{
		// Wait if a byte is being transmitted
		while((UCSR1A & (1<<UDRE1)) == 0);

		UDR1 = data;
		}
	else
		{
		_delay_us(300);	// Gludge - FFP requires some delay when sending data to it at some point - find out where and remove this
		}
#else
	_delay_us(300);	// Gludge - FFP requires some delay when sending data to it at some point - find out where and remove this
#endif // DEBUG_ENABLE_MIDI
	}


void FlushDebugBuffer(void)
	{
#ifdef DEBUG_ENABLE_USB
	uint16_t len = debug_buffer_used;	// use this to lessen chance of value changing in the middle of sending it - e.g. in interrupt

	if (len == 0)
		return;

	debug_buffer_used = 0;

	// Select the Serial Tx Endpoint
	Endpoint_SelectEndpoint(CDC1_TX_EPNUM);

	// Write the String to the Endpoint
	Endpoint_Write_Stream_LE(&debug_buffer, len, NULL);

	// Finalize the stream transfer to send the last packet
	Endpoint_ClearIN();

	// Wait until the endpoint is ready for another packet
	Endpoint_WaitUntilReady();

	// Send an empty packet to ensure that the host does not buffer data sent to it
	Endpoint_ClearIN();
#endif // DEBUG_ENABLE_USB
	}

