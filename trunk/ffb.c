/*
  Force Feedback Joystick
  Joystick model specific code for handling force feedback data.
  This code is for Microsoft Sidewinder Force Feedback Pro joystick.

  Copyright 2012  Tero Loimuneva (tloimu [at] gmail [dot] com)

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

#include "ffb.h"

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <LUFA/Drivers/Board/LEDs.h>

#include "joystick.h"
#include "debug.h"
#include "3DPro.h"

#define USART_BAUD 31250

// Effect management
volatile uint8_t nextEID = 2;	// FFP effect indexes starts from 2 (yes, we waste memory for two effects...)
volatile USB_FFBReport_PIDStatus_Input_Data_t pidState;	// For holding device status flags

void SendPidStateForEffect(uint8_t eid, uint8_t effectState);
void SendPidStateForEffect(uint8_t eid, uint8_t effectState)
	{
	pidState.effectBlockIndex = effectState;

	pidState.effectBlockIndex = 0;
	}

const uint16_t USB_DURATION_INFINITE = 0x7FFF;
const uint16_t MIDI_DURATION_INFINITE = 0;

// Bit-masks for effect states
const uint8_t MEffectState_Free = 0x00;
const uint8_t MEffectState_Allocated = 0x01;
const uint8_t MEffectState_Playing = 0x02;
const uint8_t MEffectState_SentToJoystick = 0x04;

typedef struct {
	uint8_t state;	// see constants <MEffectState_*>
	uint16_t usb_duration, usb_fadeTime;	// used to calculate fadeTime to MIDI, since in USB it is given as time difference from the end while in MIDI it is given as time from start
	// These are used to calculate effects of USB gain to MIDI data
	uint8_t usb_gain, usb_offset, usb_attackLevel, usb_fadeLevel;
	uint8_t usb_magnitude;
	FFP_MIDI_Effect_Basic	data;	// For FFP, this is enough for all types of effects - cast for other effect types when necessary
	} TEffectState;

static volatile TEffectState gEffectStates[MAX_EFFECTS+1];	// one for each effect (array index 0 is unused to simplify things)

volatile TDisabledEffectTypes gDisabledEffects;

// All effects data start with this data
static uint8_t sCommonEffectHeader[] = {0x00, 0x01, 0x0a, 0x01};

uint8_t GetNextFreeEffect(void);
void StartEffect(uint8_t id);
void StopEffect(uint8_t id);
void StopAllEffects(void);
void FreeEffect(uint8_t id);
void FreeAllEffects(void);

uint8_t GetNextFreeEffect(void)
	{
	if (nextEID == MAX_EFFECTS)
		return 0;

	uint8_t id = nextEID++;

	// Find the next free effect ID for next time
	while (gEffectStates[nextEID].state != 0)
		{
		if (nextEID >= MAX_EFFECTS)
			break;	// the last spot was taken
		nextEID++;
		}

	gEffectStates[id].state = MEffectState_Allocated;
	memset((void*) &gEffectStates[id].data, 0, sizeof(gEffectStates[id].data));
		
	return id;
	}

void StopAllEffects(void)
	{
	for (uint8_t id = 1; id <= MAX_EFFECTS; id++)
		StopEffect(id);
	}

void StartEffect(uint8_t id)
	{
	if (id > MAX_EFFECTS)
		return;
	gEffectStates[id].state |= MEffectState_Playing;
	}

void StopEffect(uint8_t id)
	{
	if (id > MAX_EFFECTS)
		return;
	gEffectStates[id].state &= ~MEffectState_Playing;
	}

void FreeEffect(uint8_t id)
	{
	if (id > MAX_EFFECTS)
		return;

	gEffectStates[id].state = 0;
	if (id < nextEID)
		nextEID = id;
	}

void FreeAllEffects(void)
	{
	nextEID = 2;
	memset((void*) gEffectStates, 0, sizeof(gEffectStates));
	}

// Utilities

// Function: CalculateSysExChecksum
//	Checksum for MIDI SysEx message content.
//	See e.g. algorhitm used by Roland
//		- sum all bytes (after "address" i.e. 4 bytes)
//		- reminder from division by 128
//		- subtract the reminder from 128 => checksum
uint8_t CalculateSysExChecksum(uint8_t *data, uint8_t len);
uint8_t CalculateSysExChecksum(uint8_t *data, uint8_t len)
	{
	uint8_t cs = 0;
	for (uint8_t i = 0; i < len; i++)	// skip the "address" part
		{
		cs += data[i];	// let them overflow, which effectively leaves the reminder part
		}
	cs &= 0x7F;	// take the potential final 128 off

	if (cs != 0)
		cs = 0x80 - cs;

	return cs;
	}

uint16_t UsbUint16ToMidiUint14(uint16_t inUsbValue)
	{
	if (inUsbValue == 0xFFFF)
		return 0x0000;

	return (inUsbValue & 0x7F00) + ((inUsbValue & 0x00FF) >> 1);	// loss of the MSB-bit!
	}

int16_t UsbInt8ToMidiInt14(int8_t inUsbValue)
	{
	int16_t value;
	if (inUsbValue < 0)
		{
		value = inUsbValue;
		value += 0x7f80;
		}
	else
		value = inUsbValue;

	return value;
	}

// Calculates the final value of the given <value> when taking in given <gain> into account.
// Returns MIDI value (i.e. max 0..7f).
uint8_t CalcGain(uint8_t usbValue, uint8_t gain)
	{
	int16_t v = usbValue;
	return (((v * gain) / 256) >> 2 ) & 0x7f;
	}


void FfbSendSysEx(void *midi_data, uint16_t len)
	{
	uint8_t mark = 0xF0;	// SysEx Start
	FfbSendData(&mark, 1);

	FfbSendData(sCommonEffectHeader, sizeof(sCommonEffectHeader));
	FfbSendData((uint8_t*) midi_data, len);

	uint8_t checksum = CalculateSysExChecksum((uint8_t*) midi_data, len);
	FfbSendData(&checksum, 1);

	mark = 0xF7;	// SysEx End
	FfbSendData(&mark, 1);
	}


// Send to MIDI Control Change "B5"
void FfbSendEffectOper(uint8_t effectId, uint8_t operation)
	{
	uint8_t midi_cmd[3];
	midi_cmd[0] = 0xB5;
	midi_cmd[1] = operation;
	midi_cmd[2] = effectId;
	FfbSendData(midi_cmd, 3);
	}


// Send to MIDI effect data modification to the given address of the given effect
void FfbSendModify(uint8_t effectId, uint8_t address, uint16_t value)
	{
	// Modify + Address
	uint8_t midi_cmd[3];
	midi_cmd[0] = 0xB5;
	midi_cmd[1] = address;
	midi_cmd[2] = effectId;
	FfbSendData(midi_cmd, 3);

	// New value
	midi_cmd[0] = 0xA5;
	midi_cmd[1] = value & 0x7F;
	midi_cmd[2] = (value & 0x7F00) >> 8;
	FfbSendData(midi_cmd, 3);
	}

// Lengths of each report type
const uint16_t OutReportSize[] = {
	sizeof(USB_FFBReport_SetEffect_Output_Data_t),		// 1
	sizeof(USB_FFBReport_SetEnvelope_Output_Data_t),	// 2
	sizeof(USB_FFBReport_SetCondition_Output_Data_t),	// 3
	sizeof(USB_FFBReport_SetPeriodic_Output_Data_t),	// 4
	sizeof(USB_FFBReport_SetConstantForce_Output_Data_t),	// 5
	sizeof(USB_FFBReport_SetRampForce_Output_Data_t),	// 6
	sizeof(USB_FFBReport_SetCustomForceData_Output_Data_t),	// 7
	sizeof(USB_FFBReport_SetDownloadForceSample_Output_Data_t),	// 8
	0,	// 9
	sizeof(USB_FFBReport_EffectOperation_Output_Data_t),	// 10
	sizeof(USB_FFBReport_BlockFree_Output_Data_t),	// 11
	sizeof(USB_FFBReport_DeviceControl_Output_Data_t),	// 12
	sizeof(USB_FFBReport_DeviceGain_Output_Data_t),	// 13
	sizeof(USB_FFBReport_SetCustomForce_Output_Data_t),	// 14
	};


void FfbSendData(uint8_t *data, uint16_t len);
void FfbSendEnableInterrupts(void);

void FfbHandle_SetEffect(USB_FFBReport_SetEffect_Output_Data_t *data);
void FfbHandle_SetEnvelope(USB_FFBReport_SetEnvelope_Output_Data_t *data);
void FfbHandle_SetCondition(USB_FFBReport_SetCondition_Output_Data_t *data);
void FfbHandle_SetPeriodic(USB_FFBReport_SetPeriodic_Output_Data_t *data);
void FfbHandle_SetConstantForce(USB_FFBReport_SetConstantForce_Output_Data_t *data);
void FfbHandle_SetRampForce(USB_FFBReport_SetRampForce_Output_Data_t *data);
void FfbHandle_SetCustomForceData(USB_FFBReport_SetCustomForceData_Output_Data_t *data);
void FfbHandle_SetDownloadForceSample(USB_FFBReport_SetDownloadForceSample_Output_Data_t *data);
void FfbHandle_EffectOperation(USB_FFBReport_EffectOperation_Output_Data_t *data);
void FfbHandle_BlockFree(USB_FFBReport_BlockFree_Output_Data_t *data);
void FfbHandle_DeviceControl(USB_FFBReport_DeviceControl_Output_Data_t *data);
void FfbHandle_DeviceGain(USB_FFBReport_DeviceGain_Output_Data_t *data);
void FfbHandle_SetCustomForce(USB_FFBReport_SetCustomForce_Output_Data_t *data);

// Handle incoming data from USB and convert it to MIDI data to joystick
void FfbOnUsbData(uint8_t *data, uint16_t len)
	{
	// Parse incoming USB data and convert it to MIDI data for the joystick
	LEDs_SetAllLEDs(LEDS_ALL_LEDS);

	if (gDebugMode & DEBUG_DETAIL)
		LogReport("<= FfbData:", OutReportSize, data, len);

	switch (data[0])	// reportID
		{
		case 1:
			FfbHandle_SetEffect((USB_FFBReport_SetEffect_Output_Data_t*) data);
			break;
		case 2:
			FfbHandle_SetEnvelope((USB_FFBReport_SetEnvelope_Output_Data_t*) data);
			break;
		case 3:
			FfbHandle_SetCondition((USB_FFBReport_SetCondition_Output_Data_t*) data);
			break;
		case 4:
			FfbHandle_SetPeriodic((USB_FFBReport_SetPeriodic_Output_Data_t*) data);
			break;
		case 5:
			FfbHandle_SetConstantForce((USB_FFBReport_SetConstantForce_Output_Data_t*) data);
			break;
		case 6:
			FfbHandle_SetRampForce((USB_FFBReport_SetRampForce_Output_Data_t*) data);
			break;
		case 7:
			FfbHandle_SetCustomForceData((USB_FFBReport_SetCustomForceData_Output_Data_t*) data);
			break;
		case 8:
			FfbHandle_SetDownloadForceSample((USB_FFBReport_SetDownloadForceSample_Output_Data_t*) data);
			break;
		case 9:
			break;
		case 10:
			FfbHandle_EffectOperation((USB_FFBReport_EffectOperation_Output_Data_t*) data);
			break;
		case 11:
			FfbHandle_BlockFree((USB_FFBReport_BlockFree_Output_Data_t *) data);
			break;
		case 12:
			FfbHandle_DeviceControl((USB_FFBReport_DeviceControl_Output_Data_t*) data);
			break;
		case 13:
			FfbHandle_DeviceGain((USB_FFBReport_DeviceGain_Output_Data_t*) data);
			break;
		case 14:
			FfbHandle_SetCustomForce((USB_FFBReport_SetCustomForce_Output_Data_t*) data);
			break;
		default:
			break;
		};

	LEDs_SetAllLEDs(LEDS_NO_LEDS);
	}


// Define and play constant force to left
uint8_t leftData[] =
	{
		0xf0,	// define
			0x00, 0x01, 0x0a, 0x01, 0x23, 0x12, 0x7f, 0x5a, 0x19, 0x00, 0x00, 0x5a, 0x00, 0x7f, 0x64, 0x00, 0x10, 0x4e, 0x7f, 0x00, 0x00, 0x7f, 0x5a, 0x19, 0x7f, 0x01, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x4e,
		0xf7,
//		0xb5, 0x20, 0x02	// play
	};

// Constant, Ramp, Square, Sine, Triange, SawtoothDown, SawtoothUp, Spring, Damper, Inertia, Friction, Custom?
uint8_t usbToMidiEffectType[] = { 0x12,0x06,0x05,0x02,0x08,0x0A,0x0B,0x0D,0x0E,0x0F,0x10,0x01 };


void FfbOnCreateNewEffect(USB_FFBReport_CreateNewEffect_Feature_Data_t* inData, USB_FFBReport_PIDBlockLoad_Feature_Data_t *outData)
	{
	LogTextP(PSTR("Create New Effect:\n  "));
	LogBinaryLf(inData, sizeof(USB_FFBReport_CreateNewEffect_Feature_Data_t));

/*
USB effect data:
	uint8_t		reportId;	// =1
	uint8_t	effectType;	// Enum (1..12): ET 26,27,30,31,32,33,34,40,41,42,43,28
	uint16_t	byteCount;	// 0..511	- only valid with Custom Force
*/
	outData->reportId = 6;
	outData->effectBlockIndex = GetNextFreeEffect();
	if (outData->effectBlockIndex == 0)
		outData->loadStatus = 2;	// 1=Success,2=Full,3=Error
	else
		{
		outData->loadStatus = 1;	// 1=Success,2=Full,3=Error

		// Set defaults to the effect data
		volatile TEffectState *effect_state = &gEffectStates[outData->effectBlockIndex];
		effect_state->usb_duration = USB_DURATION_INFINITE;
		effect_state->usb_fadeTime = USB_DURATION_INFINITE;
		effect_state->usb_gain = 0xFF;
		effect_state->usb_offset = 0;
		effect_state->usb_attackLevel = 0xFF;
		effect_state->usb_fadeLevel = 0xFF;

		volatile FFP_MIDI_Effect_Basic *midi_data = &effect_state->data;

		midi_data->magnitude = 0x7f;
		midi_data->waveLength = 0x01;
		midi_data->waveForm = usbToMidiEffectType[inData->effectType - 1];
		midi_data->attackLevel = 0x00;
		midi_data->attackTime = 0x0000;
		midi_data->fadeLevel = 0x00;
		midi_data->fadeTime = 0x0000;

		// Constants
		midi_data->command = 0x23;
		midi_data->unknown1 = 0x7F;
		midi_data->unknown2 = 0x0000;
		midi_data->unknown3[0] = 0x7F;
		midi_data->unknown3[1] = 0x64;
		midi_data->unknown3[2] = 0x00;
		midi_data->unknown3[3] = 0x10;
		midi_data->unknown3[4] = 0x4E;

		if (midi_data->waveForm == 0x12)	// constant
			midi_data->param2 = 0x0000;
		else
			midi_data->param2 = 0x0101;
		}

	outData->ramPoolAvailable = 0xFFFF;	// =0 or 0xFFFF - don't really know what this is used for?

	LogDataLf("  => Usb:", outData->reportId, outData, sizeof(USB_FFBReport_PIDBlockLoad_Feature_Data_t));

	_delay_ms(5);
	}


void FfbOnPIDPool(USB_FFBReport_PIDPool_Feature_Data_t *data)
	{
	LogTextP(PSTR("GetReport PID Pool Feature:\n  => Usb:"));

	FreeAllEffects();

	data->reportId = 7;
	data->ramPoolSize = 0xFFFF;
	data->maxSimultaneousEffects = 0x0A;	// FFP supports playing up to 10 simultaneous effects
	data->memoryManagement = 3;

	LogBinaryLf(data, sizeof(USB_FFBReport_PIDPool_Feature_Data_t));
	}


void FfbHandle_SetEffect(USB_FFBReport_SetEffect_Output_Data_t *data)
	{
	uint8_t eid = data->effectBlockIndex;

/*	LogTextP(PSTR("Set Effect:"));
	LogBinaryLf(data, sizeof(USB_FFBReport_SetEffect_Output_Data_t));
	LogTextP(PSTR("  id  =")); LogBinaryLf(&eid, sizeof(eid));
	LogTextP(PSTR("  type=")); LogBinaryLf(&data->effectType, sizeof(data->effectType));
	LogTextP(PSTR("  gain=")); LogBinaryLf(&data->gain, sizeof(data->gain));
	LogTextP(PSTR("  dura=")); LogBinaryLf(&data->duration, sizeof(data->duration));
	if (data->enableAxis)
		{
		LogTextP(PSTR("  X=")); LogBinaryLf(&data->directionX, sizeof(data->directionX));
		LogTextP(PSTR("  Y=")); LogBinaryLf(&data->directionY, sizeof(data->directionY));
		}
	if (data->triggerRepeatInterval)
		{
		LogTextP(PSTR("  repeat=")); LogBinaryLf(&data->triggerRepeatInterval, sizeof(data->triggerRepeatInterval));
		}
	if (data->triggerButton)
		{
		LogTextP(PSTR("  button=")); LogBinaryLf(&data->triggerButton, sizeof(data->triggerButton));
		}
*/

/*
USB effect data:
	uint8_t	reportId;	// =1
	uint8_t	effectBlockIndex;	// 1..40
	uint8_t	effectType;	// 1..12 (effect usages: 26,27,30,31,32,33,34,40,41,42,43,28)
	uint16_t	duration; // 0..32767 ms
	uint16_t	triggerRepeatInterval; // 0..32767 ms
	uint16_t	samplePeriod;	// 0..32767 ms
	uint8_t	gain;	// 0..255	 (physical 0..10000)
	uint8_t	triggerButton;	// button ID (0..8)
	uint8_t	enableAxis; // bits: 0=X, 1=Y, 2=DirectionEnable
	uint8_t	directionX;	// angle (0=0 .. 180=0..360deg)
	uint8_t	directionY;	// angle (0=0 .. 180=0..360deg)
*/

	// Fill in data that is common to all effect MIDI data types
	volatile FFP_MIDI_Effect_Basic *common_midi_data = &gEffectStates[eid].data;

	if (data->duration == USB_DURATION_INFINITE)
		common_midi_data->duration = MIDI_DURATION_INFINITE;
	else
		common_midi_data->duration = UsbUint16ToMidiUint14(data->duration);	// MIDI unit is 2ms
	gEffectStates[eid].usb_duration = data->duration;	// store for later calculation of <fadeTime>

	if (gEffectStates[eid].state & MEffectState_SentToJoystick)
		{	// Send update
		FfbSendModify(eid, 0x40, common_midi_data->duration);
		}

	uint8_t midi_data_len = sizeof(FFP_MIDI_Effect_Basic); 	// default MIDI data size

	bool is_periodic = false;

	// Fill in the effect type specific data
	switch (data->effectType)
		{
		case 3:	// square (midi: 5)
		case 4:	// sine (midi: 2) or cosine (midi: 3)


		case 5:	// triangle (midi: 8)
		case 6: // sawtooth up (midi: 0x0a)
		case 7: // sawtooth down (midi: 0x0b)
			is_periodic = true;
		case 1:	// constant force (midi: 0x12)
		case 2:	// ramp up & down (midi: 6 or 7)
			{
			/*
			MIDI effect data:
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
				uint16_t param1;	// Constant: positive=7f 00, negative=01 01, Other effects: 01 01
				uint16_t param2;	// Constant: 00 00, Other effects 01 01
			*/

			volatile TEffectState *effect = &gEffectStates[eid];
			volatile FFP_MIDI_Effect_Basic *midi_data = &effect->data;

			// Convert direction
			uint16_t usbdir = data->directionX;
			usbdir = usbdir * 2;
			uint16_t dir = (usbdir & 0x7F) + ( (usbdir & 0x0180) << 1 );
			midi_data->direction = dir;

			// Recalculate fadeTime for MIDI since change to duration changes the fadeTime too
			if (data->duration == USB_DURATION_INFINITE)
				midi_data->fadeTime = MIDI_DURATION_INFINITE;
			else
				{
				if (effect->usb_fadeTime == USB_DURATION_INFINITE)
					midi_data->fadeTime = MIDI_DURATION_INFINITE;
				else
					{
					if (effect->usb_duration > effect->usb_fadeTime)	// add some safety and special case handling
						midi_data->fadeTime = UsbUint16ToMidiUint14(effect->usb_duration - effect->usb_fadeTime);
					else
						midi_data->fadeTime = midi_data->duration;
					}
				}

			// Gain and its effects (magnitude and envelope levels)
			bool gain_changed = (gEffectStates[eid].usb_gain != data->gain);
			if (gain_changed)
				{
//				LogTextP(PSTR("  New gain:"));
//				LogBinary(&data->gain, 1);

				gEffectStates[eid].usb_gain = data->gain;
				midi_data->attackLevel = CalcGain(effect->usb_attackLevel, data->gain);
				midi_data->fadeLevel = CalcGain(effect->usb_fadeLevel, data->gain);

				if (is_periodic)
					{
					// Calculate min-max from magnitude and offset, since magnitude may be affected by gain we must calc them here too for periodic effects
					uint8_t magnitude = CalcGain(effect->usb_magnitude, effect->usb_gain);	// already at MIDI-level i.e. 1/2 of USB level!
					midi_data->param1 = UsbInt8ToMidiInt14(effect->usb_offset + magnitude); // max
					midi_data->param2 = UsbInt8ToMidiInt14(effect->usb_offset - magnitude); // min
					if (effect->state & MEffectState_SentToJoystick)
						{
						FfbSendModify(eid, 0x74, midi_data->param1);
						FfbSendModify(eid, 0x78, midi_data->param2);
						}
					}
				else
					midi_data->magnitude = CalcGain(effect->usb_magnitude, data->gain);
				}

			// Send data to MIDI
			if (gEffectStates[eid].state & MEffectState_SentToJoystick)
				{	// Send update
				FfbSendModify(eid, 0x48, midi_data->direction);
				FfbSendModify(eid, 0x60, midi_data->fadeTime);
				if (gain_changed)
					{
					FfbSendModify(eid, 0x6C, midi_data->fadeLevel);	// might have changed due gain
					FfbSendModify(eid, 0x64, midi_data->attackLevel);	// might have changed due gain
					if (!is_periodic)
						FfbSendModify(eid, 0x74, midi_data->magnitude);	// might have changed due gain
					}
				}
			else
				{
				FfbSendSysEx((uint8_t*) midi_data, sizeof(FFP_MIDI_Effect_Basic));

				effect->state |= MEffectState_SentToJoystick;
				}
			}
			break;
		case 8:	// spring (midi: 0x0d)
		case 9:	// damper (midi: 0x0e)
		case 10:	// inertia (midi: 0x0f)
			{
			/*
			MIDI effect data:
				uint8_t command;	// always 0x23	-- start counting checksum from here
				uint8_t waveForm;	// 2=sine, 5=Square, 6=RampUp, 7=RampDown, 8=Triange, 0x12=Constant
				uint8_t unknown1;	// ? always 0x7F
				uint16_t duration;	// unit=2ms
				uint16_t unknown2;	// ? always 0x0000
				uint16_t coeffAxis0;
				uint16_t coeffAxis1;
				uint16_t offsetAxis0;
				uint16_t offsetAxis1;
			*/
//			volatile FFP_MIDI_Effect_Spring_Inertia_Damper *midi_data = (FFP_MIDI_Effect_Spring_Inertia_Damper *) &gEffectStates[eid].data;
			midi_data_len = sizeof(FFP_MIDI_Effect_Spring_Inertia_Damper);

			// Send data to MIDI
			if (gEffectStates[eid].state & MEffectState_SentToJoystick)
				{	// Send update
				}
			}
			break;
		case 11:	// friction (midi: 0x10)
			{
			/*
			MIDI effect data:
				uint8_t command;	// always 0x23	-- start counting checksum from here
				uint8_t waveForm;	// 2=sine, 5=Square, 6=RampUp, 7=RampDown, 8=Triange, 0x12=Constant
				uint8_t unknown1;	// ? always 0x7F
				uint16_t duration;	// unit=2ms
				uint16_t unknown2;	// ? always 0x0000
				uint16_t coeffAxis0;
				uint16_t coeffAxis1;
			*/
//			volatile FFP_MIDI_Effect_Friction *midi_data = (FFP_MIDI_Effect_Friction *) &gEffectStates[eid].data;
			midi_data_len = sizeof(FFP_MIDI_Effect_Friction);

			// Send data to MIDI
			if (gEffectStates[eid].state & MEffectState_SentToJoystick)
				{	// Send update
				}
			}
			break;
		case 12:	// custom (midi: ? does FFP support custom forces?)
			{
			}
			break;
		default:
			break;
		}

	// Send full effect data to MIDI if this effect has not been sent yet
	if (!(gEffectStates[eid].state & MEffectState_SentToJoystick))
		{
		FfbSendSysEx((uint8_t*) common_midi_data, midi_data_len);

		gEffectStates[eid].state |= MEffectState_SentToJoystick;
		}

	}



void FfbHandle_SetEnvelope(USB_FFBReport_SetEnvelope_Output_Data_t *data)
	{
	uint8_t eid = data->effectBlockIndex;
	volatile TEffectState *effect = &gEffectStates[eid];

/*	LogTextP(PSTR("Set Envelope:"));
	LogBinaryLf(data, sizeof(USB_FFBReport_SetEnvelope_Output_Data_t));
	LogTextP(PSTR("  id    =")); LogBinaryLf(&eid, sizeof(eid));
	LogTextP(PSTR("  attack=")); LogBinaryLf(&data->attackLevel, sizeof(data->attackLevel));
	LogTextP(PSTR("  fade  =")); LogBinaryLf(&data->fadeLevel, sizeof(data->fadeLevel));
	LogTextP(PSTR("  attackTime=")); LogBinaryLf(&data->attackTime, sizeof(data->attackTime));
	LogTextP(PSTR("  fadeTime  =")); LogBinaryLf(&data->fadeTime, sizeof(data->fadeTime));
*/
//	FlushDebugBuffer();

/*
USB effect data:
	uint8_t	reportId;	// =2
	uint8_t	effectBlockIndex;	// 1..40
	uint8_t attackLevel;
	uint8_t	fadeLevel;
	uint16_t	attackTime;	// ms
	uint16_t	fadeTime;	// ms

MIDI effect data:
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
	uint16_t param1;	// Constant: positive=7f 00, negative=01 01, Other effects: 01 01
	uint16_t param2;	// Constant: 00 00, Other effects 01 01
*/
	volatile FFP_MIDI_Effect_Basic *midi_data = &effect->data;

	effect->usb_attackLevel = data->attackLevel;
	effect->usb_fadeLevel = data->fadeLevel;
	effect->usb_fadeTime = data->fadeTime;

	midi_data->attackLevel = CalcGain(data->attackLevel, effect->usb_gain);
	midi_data->fadeLevel = CalcGain(data->fadeLevel, effect->usb_gain);

	midi_data->attackTime = UsbUint16ToMidiUint14(data->attackTime);

	if (data->fadeTime == USB_DURATION_INFINITE)
		midi_data->fadeTime = MIDI_DURATION_INFINITE;
	else
		midi_data->fadeTime = UsbUint16ToMidiUint14(gEffectStates[eid].usb_duration - gEffectStates[eid].usb_fadeTime);

	if (gEffectStates[eid].state & MEffectState_SentToJoystick)
		{	// Send update
		FfbSendModify(eid, 0x60, midi_data->fadeTime);
		FfbSendModify(eid, 0x5C, midi_data->attackTime);
		FfbSendModify(eid, 0x6C, midi_data->fadeLevel);
		FfbSendModify(eid, 0x64, midi_data->attackLevel);
		}
	}


void FfbHandle_SetCondition(USB_FFBReport_SetCondition_Output_Data_t *data)
	{
	uint8_t eid = data->effectBlockIndex;
	volatile FFP_MIDI_Effect_Basic *common_midi_data = &gEffectStates[eid].data;


/*	LogTextP(PSTR("Set Condition:"));
	LogBinaryLf(data, sizeof(USB_FFBReport_SetCondition_Output_Data_t));
	LogTextP(PSTR("  id   =")); LogBinaryLf(&eid, sizeof(eid));
	LogTextP(PSTR("  block =")); LogBinaryLf(&data->parameterBlockOffset, sizeof(data->parameterBlockOffset));
	LogTextP(PSTR("  offset=")); LogBinaryLf(&data->cpOffset, sizeof(data->cpOffset));
	LogTextP(PSTR("  coeff+=")); LogBinaryLf(&data->positiveCoefficient, sizeof(data->positiveCoefficient));
*/
//	FlushDebugBuffer();

/*
USB effect data:
	uint8_t	effectBlockIndex;	// 1..40
	uint8_t	parameterBlockOffset;	// bits: 0..3=parameterBlockOffset, 4..5=instance1, 6..7=instance2
	int8_t cpOffset;	// -128..127
	uint8_t	positiveCoefficient;	// 0..255

MIDI effect data:
	uint16_t coeffAxis0;
	uint16_t coeffAxis1;
	uint16_t offsetAxis0; // not in friction
	uint16_t offsetAxis1; // not in friction
*/

	switch (common_midi_data->waveForm)
		{	
		case 0x0d:	// spring (midi: 0x0d)
		case 0x0e:	// damper (midi: 0x0e)
		case 0x0f:	// inertia (midi: 0x0f)
			{
			volatile FFP_MIDI_Effect_Spring_Inertia_Damper *midi_data = (FFP_MIDI_Effect_Spring_Inertia_Damper *) &gEffectStates[eid].data;

			if (data->parameterBlockOffset == 0)
				{
				midi_data->coeffAxis0 = UsbInt8ToMidiInt14(data->positiveCoefficient);
				midi_data->offsetAxis0 = UsbInt8ToMidiInt14(data->cpOffset);
				}
			else
				{
				midi_data->coeffAxis1 = UsbInt8ToMidiInt14(data->positiveCoefficient);
				if (data->cpOffset == 0x80)
					midi_data->offsetAxis1 = 0x007f;
				else
					midi_data->offsetAxis1 = UsbInt8ToMidiInt14(-data->cpOffset);
				}

			// Send data to MIDI
			if (gEffectStates[eid].state & MEffectState_SentToJoystick)
				{	// Send update
				if (data->parameterBlockOffset == 0)
					{
					FfbSendModify(eid, 0x48, midi_data->coeffAxis0);
					FfbSendModify(eid, 0x50, midi_data->offsetAxis0);
					}
				else
					{
					FfbSendModify(eid, 0x4C, midi_data->coeffAxis1);
					FfbSendModify(eid, 0x54, midi_data->offsetAxis1);
					}
				}
			}
			break;
		case 0x10:	// friction (midi: 0x10)
			{
			volatile FFP_MIDI_Effect_Friction *midi_data = (FFP_MIDI_Effect_Friction *) &gEffectStates[eid].data;

			if (data->parameterBlockOffset == 0)
				midi_data->coeffAxis0 = UsbInt8ToMidiInt14(data->positiveCoefficient);
			else
				midi_data->coeffAxis1 = UsbInt8ToMidiInt14(data->positiveCoefficient);

			// Send data to MIDI
			if (gEffectStates[eid].state & MEffectState_SentToJoystick)
				{	// Send update
				if (data->parameterBlockOffset == 0)
					FfbSendModify(eid, 0x48, midi_data->coeffAxis0);
				else
					FfbSendModify(eid, 0x4C, midi_data->coeffAxis1);
				}
			}
			break;
		default:
			break;
		}
	}



void FfbHandle_SetPeriodic(USB_FFBReport_SetPeriodic_Output_Data_t *data)
	{
	uint8_t eid = data->effectBlockIndex;

	LogTextP(PSTR("Set Periodic:"));
	LogBinaryLf(data, sizeof(USB_FFBReport_SetPeriodic_Output_Data_t));
	LogTextP(PSTR("  id=")); LogBinaryLf(&eid, sizeof(eid));
	LogTextP(PSTR("  magnitude=")); LogBinaryLf(&data->magnitude, sizeof(data->magnitude));
	LogTextP(PSTR("  offset   =")); LogBinaryLf(&data->offset, sizeof(data->offset));
	LogTextP(PSTR("  phase    =")); LogBinaryLf(&data->phase, sizeof(data->phase));
	LogTextP(PSTR("  period   =")); LogBinaryLf(&data->period, sizeof(data->period));
//	FlushDebugBuffer();

/*
USB effect data:
	uint8_t	reportId;	// =4
	uint8_t	effectBlockIndex;	// 1..40
	uint8_t magnitude;
	int8_t	offset;
	uint8_t	phase;	// 0..255 (=0..359, exp-2)
	uint16_t	period;	// 0..32767 ms

MIDI effect data:

	Offset values other than zero do not work and thus it is ignored on FFP
*/
	volatile TEffectState *effect = &gEffectStates[eid];
	volatile FFP_MIDI_Effect_Basic *midi_data = &effect->data;

	effect->usb_magnitude = data->magnitude;

	midi_data->param1 = 0x007f;
	midi_data->param2 = 0x0101;

	// Calculate waveLength (in MIDI it is in units of 1/Hz and can have value 0x6F..0x01)
	if (data->period >= 1000)
		midi_data->waveLength = 0x01;
	else if (data->period <= 9)
		midi_data->waveLength = 0x6F;
	else
		midi_data->waveLength = (1000 / data->period) & 0x7F;

	// Check phase if relevant (+90 phase for sine makes it a cosine)
	if (midi_data->waveForm == 2 || midi_data->waveForm == 3)	// sine
		{
		if (data->phase >= 32 && data->phase <= 224)
			{
			midi_data->waveForm = 3;	// cosine
			}
		else
			{
			midi_data->waveForm = 2;	// sine
			}

		// Calculate min-max from magnitude and offset
		uint8_t magnitude = CalcGain(data->magnitude, effect->usb_gain);	// already at MIDI-level i.e. 1/2 of USB level!
		midi_data->param1 = UsbInt8ToMidiInt14(data->offset / 2 + magnitude); // max
		midi_data->param2 = UsbInt8ToMidiInt14(data->offset / 2 - magnitude); // min
		if (effect->state & MEffectState_SentToJoystick)
			{
			FfbSendModify(eid, 0x74, midi_data->param1);
			FfbSendModify(eid, 0x78, midi_data->param2);
			}
		}

	if (effect->state & MEffectState_SentToJoystick)
		{
		// FfbSendModify(eid, 0x74, midi_data->magnitude); // FFP does not actually support changing magnitude on-fly here
		FfbSendModify(eid, 0x70, midi_data->waveLength);
		}
	}



void FfbHandle_SetConstantForce(USB_FFBReport_SetConstantForce_Output_Data_t *data)
	{
	uint8_t eid = data->effectBlockIndex;
	volatile TEffectState *effect_data = &gEffectStates[eid];

/*	LogTextP(PSTR("Set Constant Force:"));
	LogBinaryLf(data, sizeof(USB_FFBReport_SetConstantForce_Output_Data_t));
	LogTextP(PSTR("  id=")); LogBinaryLf(&eid, sizeof(eid));
	LogTextP(PSTR("  magnitude=")); LogBinaryLf(&data->magnitude, sizeof(data->magnitude));
*/
//	FlushDebugBuffer();
/*
USB data:
	uint8_t	reportId;	// =5
	uint8_t	effectBlockIndex;	// 1..40
	int16_t magnitude;	// -255..255

MIDI effect data:
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
	uint16_t param1;	// Constant: positive=7f 00, negative=01 01, Other effects: 01 01
	uint16_t param2;	// Constant: 00 00, Other effects 01 01
*/
	volatile FFP_MIDI_Effect_Basic *midi_data = &effect_data->data;

	effect_data->usb_magnitude = data->magnitude;

	if (data->magnitude >= 0)
		{
		midi_data->magnitude = CalcGain(data->magnitude, effect_data->usb_gain);
		midi_data->param1 = 0x007f;
		}
	else
		{
		midi_data->magnitude = CalcGain(-(data->magnitude+1), effect_data->usb_gain);
		midi_data->param1 = 0x0101;
		}

	midi_data->param2 = 0x0000;

	if (gEffectStates[eid].state & MEffectState_SentToJoystick)
		{	// Send update
		FfbSendModify(eid, 0x74, midi_data->magnitude);
		FfbSendModify(eid, 0x7C, midi_data->param1);
		}
	}

void FfbHandle_SetRampForce(USB_FFBReport_SetRampForce_Output_Data_t *data)
	{
	uint8_t eid = data->effectBlockIndex;

	LogTextP(PSTR("Set Ramp Force:"));
	LogBinaryLf(data, sizeof(USB_FFBReport_SetRampForce_Output_Data_t));
	LogTextP(PSTR("  id=")); LogBinaryLf(&eid, sizeof(eid));
	LogTextP(PSTR("  start=")); LogBinaryLf(&data->start, sizeof(data->start));
	LogTextP(PSTR("  end  =")); LogBinaryLf(&data->end, sizeof(data->end));
//	FlushDebugBuffer();

	// FFP supports only ramp up from MIN to MAX and ramp down from MAX to MIN?
/*
USB effect data:
	uint8_t	reportId;	// =6
	uint8_t	effectBlockIndex;	// 1..40
	int8_t start;
	int8_t	end;
*/
	
	volatile FFP_MIDI_Effect_Basic *midi_data = &gEffectStates[eid].data;
	if (data->start < 0)
		midi_data->param1 = 0x0100 | (-(data->start+1));
	else
		midi_data->param1 = data->start;

	midi_data->param2 = UsbInt8ToMidiInt14(data->end);

	if (gEffectStates[eid].state & MEffectState_SentToJoystick)
		{	// Send update
		FfbSendModify(eid, 0x78, midi_data->param1);
		FfbSendModify(eid, 0x74, midi_data->param2);
		}
	}



void FfbHandle_SetCustomForceData(USB_FFBReport_SetCustomForceData_Output_Data_t *data)
	{
	LogTextLf("Set Custom Force Data");
	}



void FfbHandle_SetDownloadForceSample(USB_FFBReport_SetDownloadForceSample_Output_Data_t *data)
	{
	LogTextLf("Set Download Force Sample");
	}



void FfbHandle_EffectOperation(USB_FFBReport_EffectOperation_Output_Data_t *data)
	{
	uint8_t eid = data->effectBlockIndex;

//	LogTextP(PSTR("Effect Operation:"));
	LogBinary(&eid, 1);
//	LogBinary(data, sizeof(USB_FFBReport_EffectOperation_Output_Data_t));

	if (eid == 0xFF)
		eid = 0x7F;	// All effects

	if (data->operation == 1)
		{	// Start
		LogTextLfP(PSTR(" Start"));
		LogBinaryLf(&eid, 1);

		StartEffect(data->effectBlockIndex);
		if (!gDisabledEffects.effectId[eid])
			FfbSendEffectOper(eid, 0x20);
		}
	else if (data->operation == 2)
		{	// StartSolo
		LogTextLfP(PSTR(" StartSolo"));

		// Stop all first
		StopAllEffects();
		if (!gDisabledEffects.effectId[eid])
			FfbSendEffectOper(0x7F, 0x30);

		// Then start the given effect
		StartEffect(data->effectBlockIndex);

		if (!gDisabledEffects.effectId[eid])
			FfbSendEffectOper(0x7F, 0x20);
		}
	else if (data->operation == 3)
		{	// Stop
		LogTextLfP(PSTR(" Stop"));

		StopEffect(data->effectBlockIndex);

		if (!gDisabledEffects.effectId[eid])
			FfbSendEffectOper(eid, 0x30);
		}
	else
		LogBinaryLf(&data->operation, sizeof(data->operation));
	}


void FfbHandle_BlockFree(USB_FFBReport_BlockFree_Output_Data_t *data)
	{
	uint8_t eid = data->effectBlockIndex;

	LogTextP(PSTR("Block Free: "));
	LogBinaryLf(&eid, 1);

	if (eid == 0xFF)
		{	// all effects
		FreeAllEffects();
		FfbSendEffectOper(0x7F, 0x10);
		}
	else
		{
		FreeEffect(eid);
		FfbSendEffectOper(eid, 0x10);
		}
	}


// Commands to disable and enable auto-center spring
uint8_t disableAutoCenterFfbData_1[] =
{
	0xc5, 0x01
};
// Wait 75 ms
uint8_t disableAutoCenterFfbData_2[] =
{
	0xb5, 0x7c, 0x7f,
	0xa5, 0x7f, 0x00,
	0xc5, 0x06
};

uint8_t enableAutoCenterFfbData_1[] =
{
	0xc5, 0x01
};

uint8_t enableAutoCenterFfbData_2[] =
{
	0xc5, 0x07
};

uint8_t enableAutoCenterFfbData_3[] =
{
	0xb0, 0x40, 0x00,
	0xb1, 0x40, 0x00,
	0xb2, 0x40, 0x00,
	0xb3, 0x40, 0x00,
	0xb4, 0x40, 0x00,
	0xb5, 0x40, 0x00,
	0xb6, 0x40, 0x00,
	0xb7, 0x40, 0x00,
	0xb8, 0x40, 0x00,
	0xb9, 0x40, 0x00,
	0xba, 0x40, 0x00,
	0xbb, 0x40, 0x00,
	0xbc, 0x40, 0x00,
	0xbd, 0x40, 0x00,
	0xbe, 0x40, 0x00,
	0xbf, 0x40, 0x00,
	0xb0, 0x40, 0x00,
	0xb1, 0x40, 0x00,
	0xb2, 0x40, 0x00,
	0xb3, 0x40, 0x00,
	0xb4, 0x40, 0x00,
	0xb5, 0x40, 0x00,
	0xb6, 0x40, 0x00,
	0xb7, 0x40, 0x00,
	0xb8, 0x40, 0x00,
	0xb9, 0x40, 0x00,
	0xba, 0x40, 0x00,
	0xbb, 0x40, 0x00,
	0xbc, 0x40, 0x00,
	0xbd, 0x40, 0x00,
	0xbe, 0x40, 0x00,
	0xbf, 0x40, 0x00
};


void FfbHandle_DeviceControl(USB_FFBReport_DeviceControl_Output_Data_t *data)
	{
//	LogTextP(PSTR("Device Control: "));

	uint8_t control = data->control;
	// 1=Enable Actuators, 2=Disable Actuators, 3=Stop All Effects, 4=Reset, 5=Pause, 6=Continue

// PID State Report:
//	uint8_t	reportId;	// =2
//	uint8_t	status;	// Bits: 0=Device Paused,1=Actuators Enabled,2=Safety Switch,3=Actuator Override Switch,4=Actuator Power
//	uint8_t	effectBlockIndex;	// Bit7=Effect Playing, Bit0..7=EffectId (1..40)

	pidState.reportId = 2;
	pidState.status |= 1 << 2;
	pidState.status |= 1 << 4;
	pidState.effectBlockIndex = 0;

	if (control == 0x01)
		{
		LogTextLf("Disable Actuators");
		pidState.status = (pidState.status & 0xFE);
		}
	else if (control == 0x02)
		{
		LogTextLf("Enable Actuators");
		pidState.status |= 1 << 2;
		}
	else if (control == 0x03)
		{
		// Stop all effects (e.g. FFB-application to foreground)
		LogTextLf("Stop All Effects");

		// Disable auto-center spring and stop all effects
//	???? The below would take too long?
		FfbSendData(disableAutoCenterFfbData_1, sizeof(disableAutoCenterFfbData_1));
		_delay_ms(35);
		_delay_ms(40);
		FfbSendData(disableAutoCenterFfbData_2, sizeof(disableAutoCenterFfbData_2));

		pidState.effectBlockIndex = 0;
		}
	else if (control == 0x04)
		{
		LogTextLf("Reset");
		// Reset (e.g. FFB-application out of focus)
		// Enable auto-center spring and stop all effects
		FfbSendData(enableAutoCenterFfbData_1, sizeof(enableAutoCenterFfbData_1));
		_delay_ms(35);
		_delay_ms(40);
/*		FfbSendData(enableAutoCenterFfbData_2, sizeof(enableAutoCenterFfbData_2));
		_delay_ms(10);
		FfbSendData(enableAutoCenterFfbData_3, sizeof(enableAutoCenterFfbData_3));*/
		FreeAllEffects();
//		FfbSendEffectOper(0x7F, 0x10);	// remove all
		}
	else if (control == 0x05)
		{
		LogTextLf("Pause");
		}
	else if (control == 0x06)
		{
		LogTextLf("Continue");
		}
	else if (control  & (0xFF-0x3F))
		{
		LogTextP(PSTR("Other "));
		LogBinaryLf(&data->control, 1);
		}

	// Send response
	
	}



void
FfbHandle_DeviceGain(USB_FFBReport_DeviceGain_Output_Data_t *data)
	{
	LogTextP(PSTR("Device Gain: "));
	LogBinaryLf(&data->gain, 1);
	}


void
FfbHandle_SetCustomForce(USB_FFBReport_SetCustomForce_Output_Data_t *data)
	{
	LogTextLf("Set Custom Force");
//	LogBinary(&data, sizeof(USB_FFBReport_SetCustomForce_Output_Data_t));
	}


//------------------------------------------------------------------------------
// Startup MIDI data procedure
uint8_t startupFfbData_0[] =
{
    0xc5, 0x01        // <ProgramChange> 0x01
};
// Wait for 10-20 ms (20)
uint8_t startupFfbData_1[] =
{
    0xf0,
    0x00, 0x01, 0x0a, 0x01, 0x10, 0x05, 0x6b,  // ???? - reset all?
    0xf7
};
// Wait for 56 ms
uint8_t startupFfbData_2[] =
{
    0xb5, 0x40, 0x7f,  // <ControlChange>(Modify, 0x7f)
    0xa5, 0x72, 0x57,  // offset 0x72 := 0x57
    0xb5, 0x44, 0x7f,
    0xa5, 0x3c, 0x43,
    0xb5, 0x48, 0x7f,
    0xa5, 0x7e, 0x00,
    0xb5, 0x4c, 0x7f,
    0xa5, 0x04, 0x00,
    0xb5, 0x50, 0x7f,
    0xa5, 0x02, 0x00,
    0xb5, 0x54, 0x7f,
    0xa5, 0x02, 0x00,
    0xb5, 0x58, 0x7f,
    0xa5, 0x00, 0x7e,
    0xb5, 0x5c, 0x7f,
    0xa5, 0x3c, 0x00,
    0xb5, 0x60, 0x7f
};

uint8_t startupFfbData_3[] =
{
    0xa5, 0x14, 0x65,
    0xb5, 0x64, 0x7f,
    0xa5, 0x7e, 0x6b,
    0xb5, 0x68, 0x7f,
    0xa5, 0x36, 0x00,
    0xb5, 0x6c, 0x7f,
    0xa5, 0x28, 0x00,
    0xb5, 0x70, 0x7f,
    0xa5, 0x66, 0x4c,
    0xb5, 0x74, 0x7f,
    0xa5, 0x7e, 0x01,
    0xc5, 0x01
};
// Wait for 69 ms
uint8_t startupFfbData_4[] =
{
    0xb5, 0x7c, 0x7f,
    0xa5, 0x7f, 0x00,
    0xc5, 0x06
};


//------------------------------------------------------------------------------
// Trigger the stick

#define X1_pull()	__WRAP__( {				\
				clr_bit( TRGDDR, TRGX1BIT ) ;	\
				clr_bit( TRGDDR, TRGY2BIT ) ;	\
			} )
#define X1_rel()	__WRAP__( {				\
				set_bit( TRGDDR, TRGX1BIT ) ;	\
				set_bit( TRGDDR, TRGY2BIT ) ;	\
			} )


static void FA_NOINLINE( PulseX1 ) ( void )
{
    X1_pull() ;

	_delay_us(30);
	_delay_us(20);

    X1_rel() ;

	_delay_us(20);
	_delay_us(30);
	_delay_us(20);
	_delay_us(30);

	_delay_us(20);
	_delay_us(30);

}

void WaitMs(int ms)
	{
	wdt_reset() ;

	int left = ms;
	while (left > 0)
		{
		if (left > 15)
	  		{
			_delay_ms(15);
			left = left - 15;
			}
		else
			{
			_delay_ms(left);
			return;
			}
		}
	}


void FfbSendEnableInterrupts(void)
	{
    WaitMs(100);

	// -- Read
	PulseX1();

    _delay_ms(7);

	// --- 4-pulse

	PulseX1();
	PulseX1();
	PulseX1();
	PulseX1();

	WaitMs(35);

	// --- 3-pulse
	PulseX1();
	PulseX1();
	PulseX1();

	WaitMs(14);

	// --- 2-pulse
	PulseX1();
	PulseX1();

	// delay of 78 ms

	WaitMs(78);

	// -----------------------

	// --- 2-pulse
	PulseX1();
	PulseX1();

    _delay_ms(4);

	// --- 3-pulse
	PulseX1();
	PulseX1();
	PulseX1();

	WaitMs(59);

	// --- 2-pulse
	PulseX1();
	PulseX1();

	// -- START MIDI
	FfbSendData(startupFfbData_0, sizeof(startupFfbData_0));	// Program change

	WaitMs(20);

	FfbSendData(startupFfbData_1, sizeof(startupFfbData_1));	// Init

	WaitMs(57);

	FfbSendData(startupFfbData_2, sizeof(startupFfbData_2));	// Initialize effects data memory
	FfbSendData(startupFfbData_3, sizeof(startupFfbData_3));	// Initialize effects data memory and enable auto-center spring

    WaitMs(70);

	FfbSendData(startupFfbData_4, sizeof(startupFfbData_4));	// Disable auto-center...?

	WaitMs(70);
	}



// Initializes and enables MIDI to joystick using USART1 TX
void FfbInitMidi()
	{
	// Initialize some states
	memset((void*) &gDisabledEffects, 0, sizeof(gDisabledEffects));

	// Check TX-pin (PD3) settings
	DDRD = DDRD | 0b00001000;
	
	// Set baud rate
	UCSR1A = 0;	
	UBRR1 = ((F_CPU/(31250ul<<4))-1);

	// Set frame format to 8 data bits, no parity, 1 stop bit, 1 start bit
	UCSR1C = (1<<7)|(1<<UCSZ11)|(1<<UCSZ10);
	// Enable transmitter only
	//UCSR1B = (1 << TXCIE1) | (1<<TXEN1);
	UCSR1B = (1<<TXEN1);

	UDR1 = 0;	// write something to get things going

	memset((void*) gEffectStates, 0, sizeof(gEffectStates));
	memset((void*) &pidState, 0, sizeof(pidState));
	nextEID = 2;

	FfbSendEnableInterrupts();
	}


void FfbSendByte(uint8_t data);

void FfbSendData(uint8_t *data, uint16_t len)
	{
	if (gDebugMode)
		{
		LogTextP(PSTR(" => Midi:")); LogBinaryLf(data, len);
		}

	uint16_t i = 0;
	for (i = 0; i < len; i++)
		FfbSendByte(data[i]);
	}

// ----------------------------------------------
// Ring buffer for sending MIDI data to joystick
// ----------------------------------------------

// Buffer for sending data to MIDI
//#define MIDI_BUFFER_SIZE 128

#ifndef MIDI_BUFFER_SIZE

// Non-buffered MIDI
void FfbSendByte(uint8_t data)
	{
	// Wait if a byte is being transmitted
	while((UCSR1A & (1<<UDRE1)) == 0);
	// Transmit data
	UDR1 = data;
	}

#else

// Buffered MIDI

volatile uint8_t gMidiBuffer[MIDI_BUFFER_SIZE];
volatile unit8_t *midi_buffer_head = gMidiBuffer;
volatile unit8_t *midi_buffer_tail = gMidiBuffer;

void FfbSendByte(uint8_t data)
	{
	cli();

	*gMidiBufferHead++ = data;

	if (gMidiBufferHead == gMidiBufferTail)
		{	// Ouch - buffer overflown!

		return;
		}

	if (gMidiBufferHead - gMidiBuffer >= MIDI_BUFFER_SIZE)
		gMidiBufferHead = gMidiBuffer;

	sei();
	}


ISR(USART1_UDRE_vect)
	{
	cli();

	uint8_t i;

	if (gMidiBufferHead == gMidiBufferTail)
		{
		// Buffer is empty, disable transmit interrupt
		UCSR1B = (1<<TXCIE1) | (1<<TXEN1);
		}
	else 
		{
		i = gMidiBufferTail + 1;
		if (i >= MIDI_BUFFER_SIZE) i = 0;
		UDR1 = gMidiBuffer[i];
		gMidiBufferTail = i;
		}

	sei();
	}
#endif // MIDI_BUFFER_SIZE

// ----------------------------------------------
// Debug and other settings
// ----------------------------------------------


// Send "enable FFB" to joystick
void FfbSendEnable()
	{
	}

// Send "disable FFB" to joystick
void FfbSendDisable()
	{
	}

/*
typedef struct {
	uint8_t state;	// see constants <MEffectState_*>
	uint16_t usb_duration, usb_fadeTime;	// used to calculate fadeTime to MIDI, since in USB it is given as time difference from the end while in MIDI it is given as time from start
	// These are used to calculate effects of USB gain to MIDI data
	uint8_t usb_gain, usb_offset, usb_attackLevel, usb_fadeLevel;
	uint8_t usb_magnitude;
	FFP_MIDI_Effect_Basic	data;	// For FFP, this is enough for all types of effects - cast for other effect types when necessary
	} TEffectState;

const uint8_t MEffectState_Allocated = 0x01;
const uint8_t MEffectState_Playing = 0x02;
const uint8_t MEffectState_SentToJoystick = 0x04;
*/

uint8_t FfbDebugListEffects(uint8_t *index)
	{
	if (*index == 0)
		*index = 2;

//	if (*index >= nextEID)
	if (*index >= MAX_EFFECTS)
		return 0;

	TEffectState *e = (TEffectState*) &gEffectStates[*index];

	LogBinary(index, 1);
	if (e->state == MEffectState_Allocated)
		LogTextP(PSTR(" Allocated"));
	else if (e->state == MEffectState_Playing)
		LogTextP(PSTR(" Playing\n"));
	else if (e->state == MEffectState_SentToJoystick)
		LogTextP(PSTR(" Sent"));
	else
		LogTextP(PSTR(" Free"));

	if (gDisabledEffects.effectId[*index])
		LogTextP(PSTR(" (Disabled)\n"));
	else
		LogTextP(PSTR(" (Enabled)\n"));

	if (e->state)
		{
		LogTextP(PSTR("  duration="));
		LogBinary(&e->usb_duration, 2);
		LogTextP(PSTR("\n  fadeTime="));
		LogBinary(&e->usb_fadeTime, 2);
		LogTextP(PSTR("\n  gain="));
		LogBinary(&e->usb_gain, 1);
		}

	*index = *index + 1;

	return 1;
	}


void FfbEnableSprings(uint8_t inEnable)
	{
	gDisabledEffects.springs = !inEnable;
	}

void FfbEnableConstants(uint8_t inEnable)
	{
	gDisabledEffects.constants = !inEnable;
	}

void FfbEnableTriangles(uint8_t inEnable)
	{
	gDisabledEffects.triangles = !inEnable;
	}

void FfbEnableSines(uint8_t inEnable)
	{
	gDisabledEffects.sines = !inEnable;
	}

void FfbEnableEffectId(uint8_t inId, uint8_t inEnable)
	{
	gDisabledEffects.effectId[inId] = !inEnable;

	if (gEffectStates[inId].state == MEffectState_Playing)
		{
		LogTextP(PSTR("Stop manual:"));
		LogBinaryLf(&inId, 1);
		StopEffect(inId);
		FfbSendEffectOper(inId, 0x30);
		}
	}
