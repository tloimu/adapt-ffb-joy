/*
  Force Feedback Joystick
  Joystick model specific code for handling force feedback data.
  This code is for Microsoft Sidewinder Force Feedback Pro joystick.

  Copyright 2012  Tero Loimuneva (tloimu [at] gmail [dot] com)
  Copyright 2013  Saku Kekkonen

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

#include "Joystick.h"
#include "debug.h"
#include "3DPro.h"

#include "ffb-pro.h"
#include "ffb-wheel.h"

#define USART_BAUD 31250

const FFB_Driver ffb_drivers[2] =
	{
		{
		.EnableInterrupts = FfbproEnableInterrupts,
		.GetSysExHeader = FfbproGetSysExHeader,
		.SetAutoCenter = FfbproSetAutoCenter,
		.UsbToMidiEffectType = FfbproUsbToMidiEffectType,
		.StartEffect = FfbproStartEffect,
		.StopEffect = FfbproStopEffect,
		.FreeEffect = FfbproFreeEffect,
		.CreateNewEffect = FfbproCreateNewEffect,
		.SetEnvelope = FfbproSetEnvelope,
		.SetCondition = FfbproSetCondition,
		.SetPeriodic = FfbproSetPeriodic,
		.SetConstantForce = FfbproSetConstantForce,
		.SetRampForce = FfbproSetRampForce,
		.SetEffect = FfbproSetEffect,
		.ModifyDuration = FfbproModifyDuration,
		},
		{
		.EnableInterrupts = FfbwheelEnableInterrupts,
		.GetSysExHeader = FfbwheelGetSysExHeader,
		.SetAutoCenter = FfbwheelSetAutoCenter,
		.UsbToMidiEffectType = FfbwheelUsbToMidiEffectType,
		.StartEffect = FfbwheelStartEffect,
		.StopEffect = FfbwheelStopEffect,
		.FreeEffect = FfbwheelFreeEffect,
		.CreateNewEffect = FfbwheelCreateNewEffect,
		.SetEnvelope = FfbwheelSetEnvelope,
		.SetCondition = FfbwheelSetCondition,
		.SetPeriodic = FfbwheelSetPeriodic,
		.SetConstantForce = FfbwheelSetConstantForce,
		.SetRampForce = FfbwheelSetRampForce,
		.SetEffect = FfbwheelSetEffect,
		.ModifyDuration = FfbwheelModifyDuration,
		}
	};

static const FFB_Driver* ffb;

// Effect management
volatile uint8_t nextEID = 2;	// FFP effect indexes starts from 2 (yes, we waste memory for two effects...)
volatile USB_FFBReport_PIDStatus_Input_Data_t pidState;	// For holding device status flags

void SendPidStateForEffect(uint8_t eid, uint8_t effectState);
void SendPidStateForEffect(uint8_t eid, uint8_t effectState)
	{
	pidState.effectBlockIndex = effectState;

	pidState.effectBlockIndex = 0;
	}

static volatile TEffectState gEffectStates[MAX_EFFECTS+1];	// one for each effect (array index 0 is unused to simplify things)

volatile TDisabledEffectTypes gDisabledEffects;

uint8_t GetNextFreeEffect(void);
void StartEffect(uint8_t id);
void StopEffect(uint8_t id);
void StopAllEffects(void);
void FreeEffect(uint8_t id);
void FreeAllEffects(void);

void FfbSetDriver(uint8_t id)
{
	ffb = &ffb_drivers[id];
}

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
	if (!gDisabledEffects.effectId[id])
		ffb->StopEffect(id);
	}

void FreeEffect(uint8_t id)
	{
	if (id > MAX_EFFECTS)
		return;

	gEffectStates[id].state = 0;
	if (id < nextEID)
		nextEID = id;
		
	ffb->FreeEffect(id);
	}

void FreeAllEffects(void)
	{
	nextEID = 2;
	memset((void*) gEffectStates, 0, sizeof(gEffectStates));
	}

// Utilities

void FfbSendSysEx(const uint8_t* midi_data, uint8_t len)
{	
	uint8_t hdr_len;
	const uint8_t*	hdr = ffb->GetSysExHeader(&hdr_len); // header includes the first 0xF0
	FfbSendData(hdr, hdr_len);
	
	FfbSendData((uint8_t*) midi_data, len);
	
	uint8_t checksum = 0;
	while (len--)
		checksum += *midi_data++;
	checksum = (0x80-checksum) & 0x7f;
	FfbSendData(&checksum, 1);

	uint8_t mark = 0xF7;	// SysEx End
	FfbSendData(&mark, 1);
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

void FfbHandle_EffectOperation(USB_FFBReport_EffectOperation_Output_Data_t *data);
void FfbHandle_BlockFree(USB_FFBReport_BlockFree_Output_Data_t *data);
void FfbHandle_DeviceControl(USB_FFBReport_DeviceControl_Output_Data_t *data);
void FfbHandle_DeviceGain(USB_FFBReport_DeviceGain_Output_Data_t *data);
void FfbHandle_SetCustomForceData(USB_FFBReport_SetCustomForceData_Output_Data_t* data);
void FfbHandle_SetDownloadForceSample(USB_FFBReport_SetDownloadForceSample_Output_Data_t* data);
void FfbHandle_SetCustomForce(USB_FFBReport_SetCustomForce_Output_Data_t* data);
void FfbHandle_SetEffect(USB_FFBReport_SetEffect_Output_Data_t *data);

// Handle incoming data from USB and convert it to MIDI data to joystick
void FfbOnUsbData(uint8_t *data, uint16_t len)
	{
	// Parse incoming USB data and convert it to MIDI data for the joystick
	LEDs_SetAllLEDs(LEDS_ALL_LEDS);

	LogReport(PSTR("Usb  =>"), OutReportSize, data, len);

	uint8_t effectId = data[1]; // effectBlockIndex is always the second byte.
		
	switch (data[0])	// reportID
		{
		case 1:
			FfbHandle_SetEffect((USB_FFBReport_SetEffect_Output_Data_t *) data);
			break;
		case 2:
			ffb->SetEnvelope((USB_FFBReport_SetEnvelope_Output_Data_t*) data, &gEffectStates[effectId]);
			break;
		case 3:
			ffb->SetCondition((USB_FFBReport_SetCondition_Output_Data_t*) data, &gEffectStates[effectId]);
			break;
		case 4:
			ffb->SetPeriodic((USB_FFBReport_SetPeriodic_Output_Data_t*) data, &gEffectStates[effectId]);
			break;
		case 5:
			ffb->SetConstantForce((USB_FFBReport_SetConstantForce_Output_Data_t*) data, &gEffectStates[effectId]);
			break;
		case 6:
			ffb->SetRampForce((USB_FFBReport_SetRampForce_Output_Data_t*)data, &gEffectStates[effectId]);
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

void FfbOnCreateNewEffect(USB_FFBReport_CreateNewEffect_Feature_Data_t* inData, USB_FFBReport_PIDBlockLoad_Feature_Data_t *outData)
{
	outData->reportId = 6;
	outData->effectBlockIndex = GetNextFreeEffect();
	
	if (outData->effectBlockIndex == 0) {
		outData->loadStatus = 2;	// 1=Success,2=Full,3=Error
	} else {
		outData->loadStatus = 1;	// 1=Success,2=Full,3=Error
		
		volatile TEffectState* effect = &gEffectStates[outData->effectBlockIndex];
		
		effect->usb_duration = USB_DURATION_INFINITE;
		effect->usb_fadeTime = USB_DURATION_INFINITE;
		effect->usb_gain = 0xFF;
		effect->usb_offset = 0;
		effect->usb_attackLevel = 0xFF;
		effect->usb_fadeLevel = 0xFF;

		((midi_data_common_t*)effect->data)->waveForm = ffb->UsbToMidiEffectType(inData->effectType - 1);
		
		ffb->CreateNewEffect(inData, effect);
	}
	
	outData->ramPoolAvailable = 0xFFFF;	// =0 or 0xFFFF - don't really know what this is used for?

	if (DoDebug(DEBUG_DETAIL))
		{
		LogTextP(PSTR("Create New Effect => id="));
		LogBinary(&outData->effectBlockIndex, 1);
		LogTextP(PSTR(", status="));
		LogBinaryLf(&outData->loadStatus, 1);
		FlushDebugBuffer();
		}

	LogDataLf("Usb <=", outData->reportId, outData, sizeof(USB_FFBReport_PIDBlockLoad_Feature_Data_t));

	WaitMs(5);
}

void FfbHandle_SetEffect(USB_FFBReport_SetEffect_Output_Data_t *data)
{
	volatile TEffectState* effect = &gEffectStates[data->effectBlockIndex];
	
	if (DoDebug(DEBUG_DETAIL))
		{
		LogTextP(PSTR("Set Effect:"));
		LogBinaryLf(data, sizeof(USB_FFBReport_SetEffect_Output_Data_t));
		LogTextP(PSTR("  id  =")); LogBinaryLf(&data->effectBlockIndex, sizeof(data->effectBlockIndex));
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
		FlushDebugBuffer();
		}

	midi_data_common_t* midi_data = (midi_data_common_t*)effect->data;
	
	if (data->duration == USB_DURATION_INFINITE) {
		midi_data->duration = MIDI_DURATION_INFINITE;
	} else {
		midi_data->duration = UsbUint16ToMidiUint14(data->duration); // MIDI unit is 2ms
	}
	effect->usb_duration = data->duration;	// store for later calculation of <fadeTime>

	if (effect->state & MEffectState_SentToJoystick)
		ffb->ModifyDuration(data->effectBlockIndex, midi_data->duration);

	uint8_t midi_data_len = ffb->SetEffect((USB_FFBReport_SetEffect_Output_Data_t *) data, effect);
	
	// Send full effect data to MIDI if this effect has not been sent yet
	if (!(effect->state & MEffectState_SentToJoystick)) {
		FfbSendSysEx((const uint8_t*)effect->data, midi_data_len);
		effect->state |= MEffectState_SentToJoystick;
	}

}

void FfbOnPIDPool(USB_FFBReport_PIDPool_Feature_Data_t *data)
	{
	if (DoDebug(DEBUG_DETAIL))
		{
		LogTextLfP(PSTR("GetReport PID Pool Feature"));
		}

	FreeAllEffects();

	data->reportId = 7;
	data->ramPoolSize = 0xFFFF;
	data->maxSimultaneousEffects = 0x0A;	// FFP supports playing up to 10 simultaneous effects
	data->memoryManagement = 3;
	}

void FfbHandle_SetCustomForceData(USB_FFBReport_SetCustomForceData_Output_Data_t *data)
	{
	if (DoDebug(DEBUG_DETAIL))
		LogTextLf("Set Custom Force Data");
	}



void FfbHandle_SetDownloadForceSample(USB_FFBReport_SetDownloadForceSample_Output_Data_t *data)
	{
	if (DoDebug(DEBUG_DETAIL))
		LogTextLf("Set Download Force Sample");
	}



void FfbHandle_EffectOperation(USB_FFBReport_EffectOperation_Output_Data_t *data)
	{
	uint8_t eid = data->effectBlockIndex;

	if (DoDebug(DEBUG_DETAIL))
		{
		LogTextP(PSTR("Effect Operation:"));
		LogBinary(&eid, 1);
		}

	if (eid == 0xFF)
		eid = 0x7F;	// All effects

	if (data->operation == 1)
		{	// Start
		if (DoDebug(DEBUG_DETAIL))
			LogTextLfP(PSTR(" Start"));

		StartEffect(data->effectBlockIndex);
		if (!gDisabledEffects.effectId[eid])
			ffb->StartEffect(eid);
		}
	else if (data->operation == 2)
		{	// StartSolo
		if (DoDebug(DEBUG_DETAIL))
			LogTextLfP(PSTR(" StartSolo"));

		// Stop all first
		StopAllEffects();
		if (!gDisabledEffects.effectId[eid])
			ffb->StopEffect(0x7F); // TODO: wheel ?

		// Then start the given effect
		StartEffect(data->effectBlockIndex);

		if (!gDisabledEffects.effectId[eid])
			ffb->StartEffect(0x7F);	// TODO: wheel ?
		}
	else if (data->operation == 3)
		{	// Stop
		if (DoDebug(DEBUG_DETAIL))
			LogTextLfP(PSTR(" Stop"));

		StopEffect(data->effectBlockIndex);
		}
	else
		{
		if (DoDebug(DEBUG_DETAIL))
			{
			LogTextLfP(PSTR(" Unknown operation"));
			LogBinaryLf(&data->operation, sizeof(data->operation));
			}
		}
	}


void FfbHandle_BlockFree(USB_FFBReport_BlockFree_Output_Data_t *data)
	{
	uint8_t eid = data->effectBlockIndex;

	if (DoDebug(DEBUG_DETAIL))
		{
		LogTextP(PSTR("Block Free: "));
		LogBinaryLf(&eid, 1);
		}

	if (eid == 0xFF)
		{	// all effects
		FreeAllEffects();
		ffb->FreeEffect(0x7f); // TODO: does this work with the wheel?
		}
	else
		{
		FreeEffect(eid);
		}
	}

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
		ffb->SetAutoCenter(0);
		pidState.effectBlockIndex = 0;
		}
	else if (control == 0x04)
		{
		LogTextLf("Reset");
		// Reset (e.g. FFB-application out of focus)
		// Enable auto-center spring and stop all effects
		ffb->SetAutoCenter(1);
		WaitMs(75);
		FreeAllEffects();
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
// Trigger the stick

#define X1_pull()	__WRAP__( {				\
				clr_bit( TRGDDR, TRGX1BIT ) ;	\
				clr_bit( TRGDDR, TRGY2BIT ) ;	\
			} )
#define X1_rel()	__WRAP__( {				\
				set_bit( TRGDDR, TRGX1BIT ) ;	\
				set_bit( TRGDDR, TRGY2BIT ) ;	\
			} )

void _delay_us10(uint8_t delay)
{
	while (delay--) {
		_delay_us(9); // .. compensate for loop handling
	}
}

void FA_NOINLINE( FfbPulseX1 ) ( void )
{
    X1_pull() ;
	_delay_us10(5);
    X1_rel() ;
	_delay_us10(1);
}

void WaitMs(int ms)
	{
	while (ms--)
		{
		wdt_reset() ;
		_delay_ms(1);
		}
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

	ffb->EnableInterrupts();
	}

void FfbSendByte(uint8_t data);

void FfbSendData(const uint8_t *data, uint16_t len)
	{
	if (gDebugMode)
		{
		LogTextP(PSTR(" => Midi:")); LogBinaryLf(data, len);
		}

	uint16_t i = 0;
	for (i = 0; i < len; i++)
		FfbSendByte(data[i]);
	}
	
void FfbSendPackets(const uint8_t *data, uint16_t len)
	{
	uint16_t i = 0;
	while (i < len)
		{
		WaitMs(1);
		uint8_t count = data[i++];
		while (count--)
			FfbSendByte(data[i++]);
		}
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
		}
	}
