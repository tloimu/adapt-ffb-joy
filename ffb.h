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

#ifndef _FFB_
#define _FFB_

#include <avr/io.h>


/* Type Defines: */
/** Type define for the joystick HID report structure, for creating and sending HID reports to the host PC.
 *  This mirrors the layout described to the host in the HID report descriptor, in Descriptors.c.
 */

// Maximum number of parallel effects in memory
#define MAX_EFFECTS 20
	
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

// Lengths of each report type
extern const uint16_t OutReportSize[];

// Handles Force Feeback data manipulation from USB reports to joystick's MIDI channel

void FfbSetDriver(uint8_t id);

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
// Resets watchdog for each 1ms wait.
void WaitMs(int ms);

// delay_us has max limits and the wait time must be known at compile time.
// function for making 10us delays that don't have be known at compile time.
// max delay 2560us.
void _delay_us10(uint8_t delay);

// Send raw data to the
void FfbSendData(const uint8_t *data, uint16_t len);
void FfbSendPackets(const uint8_t *data, uint16_t len);
void FfbPulseX1( void );

// Debugging
//	<index> should be pointer to an index variable whose value should be set to 0 to start iterating.
//	Returns 0 when no more effects
uint8_t FfbDebugListEffects(uint8_t *index);

// Effect manipulations

typedef struct 
	{
	uint8_t midi;	// disables all MIDI-traffic
	uint8_t springs;
	uint8_t constants;
	uint8_t triangles;
	uint8_t sines;
	uint8_t effectId[MAX_EFFECTS];
	} TDisabledEffectTypes;

extern volatile TDisabledEffectTypes gDisabledEffects;

void FfbSendSysEx(const uint8_t* midi_data, uint8_t len);
uint16_t UsbUint16ToMidiUint14_Time(uint16_t inUsbValue);
uint16_t UsbUint16ToMidiUint14(uint16_t inUsbValue);
int16_t UsbInt8ToMidiInt14(int8_t inUsbValue);
uint8_t CalcGain(uint8_t usbValue, uint8_t gain);

void FfbEnableSprings(uint8_t inEnable);
void FfbEnableConstants(uint8_t inEnable);
void FfbEnableTriangles(uint8_t inEnable);
void FfbEnableSines(uint8_t inEnable);
void FfbEnableEffectId(uint8_t inId, uint8_t inEnable);

// Bit-masks for effect states
#define MEffectState_Free			0x00
#define MEffectState_Allocated		0x01
#define MEffectState_Playing		0x02
#define MEffectState_SentToJoystick	0x04

#define USB_DURATION_INFINITE	0x7FFF
#define MIDI_DURATION_INFINITE	0

#define USB_EFFECT_CONSTANT		0x01
#define USB_EFFECT_RAMP			0x02
#define USB_EFFECT_SQUARE 		0x03
#define USB_EFFECT_SINE 		0x04
#define USB_EFFECT_TRIANGLE		0x05
#define USB_EFFECT_SAWTOOTHDOWN	0x06
#define USB_EFFECT_SAWTOOTHUP	0x07
#define USB_EFFECT_SPRING		0x08
#define USB_EFFECT_DAMPER		0x09
#define USB_EFFECT_INERTIA		0x0A
#define USB_EFFECT_FRICTION		0x0B
#define USB_EFFECT_CUSTOM		0x0C

#define MAX_MIDI_MSG_LEN 27 /* enough to hold longest midi message data part, FFP_MIDI_Effect_Basic */

/* start of midi data common for both pro and wheel protocols */
typedef struct {
	uint8_t command;	// 0x23 for pro, 0x20 for wheel
	uint8_t waveForm;	// different enumeration for pro/wheel
	uint8_t unknown1;	// always 0x7F
	uint16_t duration;	// unit=2ms
} midi_data_common_t;

typedef struct {
	uint8_t state;	// see constants <MEffectState_*>
	uint16_t usb_duration, usb_fadeTime;	// used to calculate fadeTime to MIDI, since in USB it is given as time difference from the end while in MIDI it is given as time from start
	// These are used to calculate effects of USB gain to MIDI data
	uint8_t usb_gain, usb_offset, usb_attackLevel, usb_fadeLevel;
	uint8_t usb_magnitude;
	volatile uint8_t	data[MAX_MIDI_MSG_LEN];
	} TEffectState;

typedef struct
	{
	void (*EnableInterrupts)(void);
	const uint8_t* (*GetSysExHeader)(uint8_t* hdr_len);
	void (*SetAutoCenter)(uint8_t enable);
	uint8_t (*UsbToMidiEffectType)(uint8_t usb_effect_type);
	
	void (*StartEffect)(uint8_t eid);
	void (*StopEffect)(uint8_t eid);
	void (*FreeEffect)(uint8_t eid);
	
	void (*ModifyDuration)(uint8_t effectId, uint16_t duration);
	
	void (*CreateNewEffect)(USB_FFBReport_CreateNewEffect_Feature_Data_t* inData, volatile TEffectState* effect);
	void (*SetEnvelope)(USB_FFBReport_SetEnvelope_Output_Data_t* data, volatile TEffectState* effect);
	void (*SetCondition)(USB_FFBReport_SetCondition_Output_Data_t* data, volatile TEffectState* effect);
	void (*SetPeriodic)(USB_FFBReport_SetPeriodic_Output_Data_t* data, volatile TEffectState* effect);
	void (*SetConstantForce)(USB_FFBReport_SetConstantForce_Output_Data_t* data, volatile TEffectState* effect);
	void (*SetRampForce)(USB_FFBReport_SetRampForce_Output_Data_t* data, volatile TEffectState* effect);
	int  (*SetEffect)(USB_FFBReport_SetEffect_Output_Data_t* data, volatile TEffectState* effect);
	} FFB_Driver;

#endif // _FFB_PRO_