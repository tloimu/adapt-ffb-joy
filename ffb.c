/*
  Force Feedback Joystick
  Joystick model specific code for handling force feedback data.
  This code is for Microsoft Sidewinder Force Feedback Pro joystick.

  Copyright 2018  Tero Loimuneva (tloimu [at] gmail [dot] com)

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

#include "ffb-direct.h"

const FFB_Driver ffb_drivers[1] =
	{
		{
		.InitializeDriver = FfbDirect_InitializeDriver,
		.GetMaxSimultaneousEffects = FfbDirect_GetMaxSimultaneousEffects,
		.CreateNewEffect = FfbDirect_CreateNewEffect,
		.SetAutoCenter = FfbDirect_SetAutoCenter,
		.StartEffect = FfbDirect_StartEffect,
		.StopEffect = FfbDirect_StopEffect,
		.StopAllEffects = FfbDirect_StopAllEffects,
		.FreeEffect = FfbDirect_FreeEffect,
		.FreeAllEffects = FfbDirect_FreeAllEffects,
		.SetEnvelope = FfbDirect_SetEnvelope,
		.SetCondition = FfbDirect_SetCondition,
		.SetPeriodic = FfbDirect_SetPeriodic,
		.SetConstantForce = FfbDirect_SetConstantForce,
		.SetRampForce = FfbDirect_SetRampForce,
		.SetEffect = FfbDirect_SetEffect,
		.MaintainEffects = FfbDirect_MaintainEffects,
		}
	};

static const FFB_Driver* ffb;

void FfbInit()
{
	FfbDirect_InitializeDriver();
}

// Effect management
volatile USB_FFBReport_PIDStatus_Input_Data_t pidState;	// For holding device status flags

void FfbSetDriver(uint8_t id)
{
	ffb = &ffb_drivers[id];
	ffb->InitializeDriver();
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
	if (!ffb)
		return;

	switch (data[0])	// reportID
		{
		case 1:
			ffb->SetEffect((USB_FFBReport_SetEffect_Output_Data_t *) data);
			break;
		case 2:
			ffb->SetEnvelope((USB_FFBReport_SetEnvelope_Output_Data_t*) data);
			break;
		case 3:
			ffb->SetCondition((USB_FFBReport_SetCondition_Output_Data_t*) data);
			break;
		case 4:
			ffb->SetPeriodic((USB_FFBReport_SetPeriodic_Output_Data_t*) data);
			break;
		case 5:
			ffb->SetConstantForce((USB_FFBReport_SetConstantForce_Output_Data_t*) data);
			break;
		case 6:
			ffb->SetRampForce((USB_FFBReport_SetRampForce_Output_Data_t*)data);
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
	outData->ramPoolAvailable = 0xFFFF;	// =0 or 0xFFFF - don't really know what this is used for?
	
	uint8_t eid = ffb->CreateNewEffect(inData->effectType, inData->byteCount);
	outData->effectBlockIndex = eid;
	
	if (eid == 0)
		outData->loadStatus = 2;	// 1=Success,2=Full,3=Error
	else
		outData->loadStatus = 1;	// 1=Success,2=Full,3=Error		

	if (DoDebug(DEBUG_DETAIL))
		{
		LogTextP(PSTR("Create New Effect => id="));
		LogBinary(&outData->effectBlockIndex, 1);
		LogTextP(PSTR(", status="));
		LogBinaryLf(&outData->loadStatus, 1);
		FlushDebugBuffer();
		}

	// LogDataLf("Usb <=", outData->reportId, outData, sizeof(USB_FFBReport_PIDBlockLoad_Feature_Data_t));

	WaitMs(5); // Windows doesn't like fast answers
}

void FfbHandle_SetEffect(USB_FFBReport_SetEffect_Output_Data_t *data)
{	
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

	ffb->SetEffect((USB_FFBReport_SetEffect_Output_Data_t *) data);	
}

void FfbOnPIDPool(USB_FFBReport_PIDPool_Feature_Data_t *data)
	{
	if (DoDebug(DEBUG_DETAIL))
		{
		LogTextLfP(PSTR("GetReport PID Pool Feature"));
		}

	ffb->FreeAllEffects();

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

		ffb->StartEffect(eid);
		}
	else if (data->operation == 2)
		{	// StartSolo
		if (DoDebug(DEBUG_DETAIL))
			LogTextLfP(PSTR(" StartSolo"));

		if (ffb)
			{
			// Stop all first
			ffb->StopAllEffects();
			// Then start the given effect
			ffb->StartEffect(data->effectBlockIndex);
			}
		}
	else if (data->operation == 3)
		{	// Stop
		if (DoDebug(DEBUG_DETAIL))
			LogTextLfP(PSTR(" Stop"));

		if (ffb)
			ffb->StopEffect(data->effectBlockIndex);
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
		ffb->FreeAllEffects();
		}
	else
		{
		ffb->FreeEffect(eid);
		}
	}

void FfbHandle_DeviceControl(USB_FFBReport_DeviceControl_Output_Data_t *data)
	{
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
		ffb->FreeAllEffects();
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

void WaitMs(int ms)
	{
	while (ms--)
		{
		wdt_reset() ;
		_delay_ms(1);
		}
	}

// Send "enable FFB" to joystick
void FfbSendEnable()
	{
	}

// Send "disable FFB" to joystick
void FfbSendDisable()
	{
	}

void FfbSendRawData(uint8_t *data, uint16_t len)
{
	if (ffb)
		ffb->SendRawData(data, len);
}

// Continuously called
void FfbOnMaintainEffects(int16_t x, int16_t y, uint16_t dtMs)
{
	if (ffb)
		ffb->MaintainEffects(x, y, dtMs);
}
