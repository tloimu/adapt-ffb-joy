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

// Debugging utilities

// Send out the given null terminated text
void LogText(const char *text);

// Send out the given binary data
void LogBinary(const void *data, uint16_t len);

// Send out data with a prefix of text and an integer
void LogData(const char *text, uint8_t reportId, const void *data, uint16_t len);

// Log all reports found in the given data (may have one or more)
void LogReport(const char *text, const uint16_t *reportSizeArray, uint8_t *data, uint16_t len);

#endif // _DEBUG_H_
