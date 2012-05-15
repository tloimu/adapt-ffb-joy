/*
  Force Feedback Joystick
  USB HID descriptors for a force feedback joystick.  

  This code is for Microsoft Sidewinder Force Feedback Pro joystick.
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

#include <avr/io.h>


/* Type Defines: */
/** Type define for the joystick HID report structure, for creating and sending HID reports to the host PC.
 *  This mirrors the layout described to the host in the HID report descriptor, in Descriptors.c.
 */

// ---- Input

typedef struct
	{
	uint8_t	reportId;	// =2
	uint8_t	status;	// Bits: 0=Device Paused,1=Actuators Enabled,2=Safety Switch,3=Actuator Override Switch,4=Actuator Power
	uint8_t	effectBlockIndex;	// Bit7=Effect Playing, Bit0..7=EffectId (1..40)
	} USB_FFBReport_PIDStatus_Input_Data_t;

// ---- Output

typedef struct
	{ // FFB: Set Effect Output Report
	uint8_t	reportId;	// =1
	uint8_t	effectBlockIndex;	// 1..40
	uint8_t	effectType;	// 1..12 (effect usages: 26,27,30,31,32,33,34,40,41,42,43,28)
	uint16_t	duration; // 0..32767 ms
	uint16_t	triggerRepeatInterval; // 0..32767 ms
	uint16_t	samplePeriod;	// 0..32767 ms
	uint8_t	gain;	// 0..255	 (physical 0..10000)
	uint8_t	triggerButton;	// button ID (0..8)
	uint8_t	enableAxis; // bits: 0=X, 1=Y, 2=DirectionEnable
	uint8_t	directionX;	// angle (0=0 .. 255=360deg)
	uint8_t	directionY;	// angle (0=0 .. 255=360deg)
//	uint16_t	startDelay;	// 0..32767 ms
	} USB_FFBReport_SetEffect_Output_Data_t;

typedef struct
	{ // FFB: Set Envelope Output Report
	uint8_t	reportId;	// =2
	uint8_t	effectBlockIndex;	// 1..40
	uint8_t attackLevel;
	uint8_t	fadeLevel;
	uint16_t	attackTime;	// ms
	uint16_t	fadeTime;	// ms
	} USB_FFBReport_SetEnvelope_Output_Data_t;

typedef struct
	{ // FFB: Set Condition Output Report
	uint8_t	reportId;	// =3
	uint8_t	effectBlockIndex;	// 1..40
	uint8_t	parameterBlockOffset;	// bits: 0..3=parameterBlockOffset, 4..5=instance1, 6..7=instance2
	uint8_t cpOffset;	// 0..255
	int8_t	positiveCoefficient;	// -128..127
//	int8_t	negativeCoefficient;	// -128..127
//	uint8_t	positiveSaturation;	// -128..127
//	uint8_t	negativeSaturation;	// -128..127
//	uint8_t	deadBand;	// 0..255
	} USB_FFBReport_SetCondition_Output_Data_t;

typedef struct
	{ // FFB: Set Periodic Output Report
	uint8_t	reportId;	// =4
	uint8_t	effectBlockIndex;	// 1..40
	uint8_t magnitude;
	int8_t	offset;
	uint8_t	phase;	// 0..255 (=0..359, exp-2)
	uint16_t	period;	// 0..32767 ms
	} USB_FFBReport_SetPeriodic_Output_Data_t;

typedef struct
	{ // FFB: Set ConstantForce Output Report
	uint8_t	reportId;	// =5
	uint8_t	effectBlockIndex;	// 1..40
	int16_t magnitude;	// -255..255
	} USB_FFBReport_SetConstantForce_Output_Data_t;

typedef struct
	{ // FFB: Set RampForce Output Report
	uint8_t	reportId;	// =6
	uint8_t	effectBlockIndex;	// 1..40
	int8_t start;
	int8_t	end;
	} USB_FFBReport_SetRampForce_Output_Data_t;

typedef struct
	{ // FFB: Set CustomForceData Output Report
	uint8_t	reportId;	// =7
	uint8_t	effectBlockIndex;	// 1..40
	uint8_t dataOffset;
	int8_t	data[12];
	} USB_FFBReport_SetCustomForceData_Output_Data_t;

typedef struct
	{ // FFB: Set DownloadForceSample Output Report
	uint8_t	reportId;	// =8
	int8_t	x;
	int8_t	y;
	} USB_FFBReport_SetDownloadForceSample_Output_Data_t;

typedef struct
	{ // FFB: Set EffectOperation Output Report
	uint8_t	reportId;	// =10
	uint8_t effectBlockIndex;	// 1..40
	uint8_t operation; // 1=Start, 2=StartSolo, 3=Stop
	uint8_t	loopCount;
	} USB_FFBReport_EffectOperation_Output_Data_t;

typedef struct
	{ // FFB: Block Free Output Report
	uint8_t	reportId;	// =11
	uint8_t effectBlockIndex;	// 1..40
	} USB_FFBReport_BlockFree_Output_Data_t;

typedef struct
	{ // FFB: Device Control Output Report
	uint8_t	reportId;	// =12
	uint8_t control;	// 1=Enable Actuators, 2=Disable Actuators, 4=Stop All Effects, 8=Reset, 16=Pause, 32=Continue
	} USB_FFBReport_DeviceControl_Output_Data_t;

typedef struct
	{ // FFB: DeviceGain Output Report
	uint8_t	reportId;	// =13
	uint8_t gain;
	} USB_FFBReport_DeviceGain_Output_Data_t;

typedef struct
	{ // FFB: Set Custom Force Output Report
	uint8_t		reportId;	// =14
	uint8_t effectBlockIndex;	// 1..40
	uint8_t	sampleCount;
	uint16_t	samplePeriod;	// 0..32767 ms
	} USB_FFBReport_SetCustomForce_Output_Data_t;

// ---- Features

typedef struct
	{ // FFB: Create New Effect Feature Report
	uint8_t		reportId;	// =1
	uint8_t	effectType;	// Enum (1..12): ET 26,27,30,31,32,33,34,40,41,42,43,28
	uint16_t	byteCount;	// 0..511
	} USB_FFBReport_CreateNewEffect_Feature_Data_t;

typedef struct
	{ // FFB: PID Block Load Feature Report
	uint8_t	reportId;	// =2
	uint8_t effectBlockIndex;	// 1..40
	uint8_t	loadStatus;	// 1=Success,2=Full,3=Error
	uint16_t	ramPoolAvailable;	// =0 or 0xFFFF?
	} USB_FFBReport_PIDBlockLoad_Feature_Data_t;

typedef struct
	{ // FFB: PID Pool Feature Report
	uint8_t	reportId;	// =3
	uint16_t	ramPoolSize;	// ?
	uint8_t		maxSimultaneousEffects;	// ?? 40?
	uint8_t		memoryManagement;	// Bits: 0=DeviceManagedPool, 1=SharedParameterBlocks
	} USB_FFBReport_PIDPool_Feature_Data_t;


// ----------------------------------
// Microsoft Sidewinder Force Feedback Pro FFB structures

typedef struct
	{
	uint8_t command;	// always 0x23	-- start counting checksum from here
	uint8_t waveForm;	// 2=sine, 5=Square, 6=RampUp, 7=RampDown, 8=Triange, 0x12=Constant
	uint8_t unknown1;	// ? always 0x7F
	uint16_t duration;	// unit=2ms
	uint16_t unknown2;	// ? always 0x0000
	uint16_t direction;
	uint8_t	unknown3[5];	// ? always 7f 64 00 10 4e
	uint8_t attackLevel;
	uint16_t	attackTime;
	uint8_t		magnitude;
	uint16_t	fadeTime;
	uint8_t	fadeLevel;
	uint8_t	waveLength;	// 0x6F..0x01 => 1/Hz
	uint8_t unknown5;	// ? always 0x00
	uint16_t param1;	// Varies by effect type; Constant: positive=7f 00, negative=01 01, Other effects: 01 01
	uint16_t param2;	// Varies by effect type; Constant: 00 00, Other effects 01 01
	} FFP_MIDI_Effect_Basic;

typedef struct
	{
	uint8_t command;	// always 0x23	-- start counting checksum from here
	uint8_t waveForm;	// 0xd=Spring, 0x0e=Damper, 0xf=Inertia
	uint8_t unknown1;	// ? always 0x7F
	uint16_t duration;	// unit=2ms
	uint16_t unknown2;	// ? always 0x0000
	uint16_t coeffAxis0;
	uint16_t coeffAxis1;
	uint16_t offsetAxis0;
	uint16_t offsetAxis1;
	} FFP_MIDI_Effect_Spring_Inertia_Damper;


typedef struct
	{
	uint8_t command;	// always 0x23	-- start counting checksum from here
	uint8_t waveForm;	// 0x10=Friction
	uint8_t unknown1;	// ? always 0x7F
	uint16_t duration;	// unit=2ms
	uint16_t unknown2;	// ? always 0x0000
	uint16_t coeffAxis0;
	uint16_t coeffAxis1;
	} FFP_MIDI_Effect_Friction;

// Lengths of each report type
extern const uint16_t OutReportSize[];

// Handles Force Feeback data manipulation from USB reports to joystick's MIDI channel

// Initializes and enables MIDI to joystick using USART1 TX
void FfbInitMidi(void);

// Send "enable FFB" to joystick
void FfbSendEnable(void);

// Send "disable FFB" to joystick
void FfbSendDisable(void);

// Handle incoming data from USB
void FfbOnUsbData(uint8_t *data, uint16_t len);

// Handle incoming feature requests
void FfbOnCreateNewEffect(USB_FFBReport_CreateNewEffect_Feature_Data_t* inData, USB_FFBReport_PIDBlockLoad_Feature_Data_t *outData);
void FfbOnPIDPool(USB_FFBReport_PIDPool_Feature_Data_t *data);

// Utility to wait any amount of milliseconds.
// Resets watchdog at the beginning of each call so
// if watchdog is enabled, don't make too long waits
// in once call.
void WaitMs(int ms);

// Send raw data to the
void FfbSendData(uint8_t *data, uint16_t len);

