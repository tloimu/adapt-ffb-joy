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

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "main.h"

// Method of debugging
extern const uint8_t DEBUG_TO_NONE;
extern const uint8_t DEBUG_TO_MIDI;
extern const uint8_t DEBUG_TO_USB;
extern const uint8_t DEBUG_DETAIL;

extern volatile uint8_t gDebugMode;

// If below are defined, code for respective debug target is included into build
#define DEBUG_ENABLE_MIDI
#define DEBUG_ENABLE_USB

#define DEBUG_BUFFER_SIZE 512

// Debugging utilities

// The basic debug data sending method used by all other methods.
// Implement this to send debug data to desired destination.
void LogSendByte(uint8_t data);

// Send out the given null terminated text
void LogText(const char *text);
void LogTextLf(const char *text);	// Adds linefeed
void LogTextP(const char *text);	// From program memory
void LogTextLfP(const char *text);	// From program memory, adds linefeed

// Send out the given binary data
void LogBinary(const void *data, uint16_t len);
void LogBinaryLf(const void *data, uint16_t len);	// Adds linefeed

// Send out data with a prefix of text and an integer
void LogData(const char *text, uint8_t reportId, const void *data, uint16_t len);
void LogDataLf(const char *text, uint8_t reportId, const void *data, uint16_t len);	// Adds linefeed

// Log all reports found in the given data (may have one or more)
void LogReport(const char *text, const uint16_t *reportSizeArray, uint8_t *data, uint16_t len);

// Debugging utils for USB-serial debugging
void FlushDebugBuffer(void);

#endif // _DEBUG_H_
