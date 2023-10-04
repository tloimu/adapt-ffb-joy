/*
  Force Feedback Joystick
  Joystick model specific code for handling force feedback data.
  This code is for Microsoft Sidewinder Force Feedback Pro joystick.

  Copyright 2012  Tero Loimuneva (tloimu [at] gmail [dot] com)
  Copyright 2013  Saku Kekkonen
  Copyright 2023  Ed Wilkinson

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

#include "ffb-pro.h"
#include "ffb.h"

#include <util/delay.h>
#include "debug.h"

uint8_t FfbproUsbToMidiEffectType(uint8_t usb_effect_type)
{
	const uint8_t usbToMidiEffectType[] = {
		0x12,	// Constant, 
		0x06, 	// Ramp
		0x05, 	// Square
		0x02, 	// Sine
		0x08,	// Triangle
		0x0A,	// SawtoothDown
		0x0B,	// SawtoothUp
		0x0D,	// Spring
		0x0E,	// Damper
		0x0F,	// Inertia
		0x10,	// Friction
		0x01 	// Custom ?
	};
	
	if (usb_effect_type >= sizeof(usbToMidiEffectType))
		return 0;
		
	return usbToMidiEffectType[usb_effect_type];
}

static void FfbproInitPulses(uint8_t count)
{
	while (count--) {
		FfbPulseX1();
		_delay_us10(10);
	}
}

void FfbproEnableInterrupts(void)
{
	const uint8_t startupFfbData_0[] = {
		0xc5, 0x01        // <ProgramChange> 0x01
	};

	const uint8_t startupFfbData_1[] = {
		0xf0,
		0x00, 0x01, 0x0a, 0x01, 0x10, 0x05, 0x6b,  // ???? - reset all?
		0xf7
	};

	const uint8_t startupFfbData_2[] = {
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

	const uint8_t startupFfbData_3[] = {
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
	};

    WaitMs(100);

	FfbPulseX1();
    WaitMs(7);

	FfbproInitPulses(4);
	WaitMs(35);

	FfbproInitPulses(3);
	WaitMs(14);

	FfbproInitPulses(2);
	WaitMs(78);

	FfbproInitPulses(2);
    WaitMs(4);

	FfbproInitPulses(3);
	WaitMs(59);

	FfbproInitPulses(2);
	
	// -- START MIDI
	FfbSendData(startupFfbData_0, sizeof(startupFfbData_0));	// Program change
	WaitMs(20);

	FfbSendData(startupFfbData_1, sizeof(startupFfbData_1));	// Init
	WaitMs(57);

	FfbSendData(startupFfbData_2, sizeof(startupFfbData_2));	// Initialize effects data memory
	FfbSendData(startupFfbData_3, sizeof(startupFfbData_3));	// Initialize effects data memory

	FfbproDeviceControl(USB_DCTRL_RESET); // Leave auto centre on
	WaitMs(70);
	
	}

uint8_t FfbproDeviceControl(uint8_t usb_control)
{
	/*
	USB_DCTRL_ACTUATORS_ENABLE	0x01
	USB_DCTRL_ACTUATORS_DISABLE	0x02 
	USB_DCTRL_STOPALL			0x03 
	USB_DCTRL_RESET				0x04
	USB_DCTRL_PAUSE				0x05
	USB_DCTRL_CONTINUE			0x06
	*/
	const uint8_t usbToMidiControl[] = {
		0x02, 	// Enable Actuators
		0x03, 	// Disable Actuators (time stepping continues in background)		
		0x06, 	// Stop All (including stop auto centre)
		0x01,	// Reset  (stop all effects; free all effects; reset device gain to max; enable actuators; continue; enable auto spring centre)
		0x05,	// Pause (time stepping is paused)
		0x04,	// Continue
	};	
	
	if (usb_control < 1 || usb_control > 6)
		return 0; //not supported
	
	uint8_t command[2] = {0xc5};
	command[1] = usbToMidiControl[usb_control-1];
	FfbSendData(command, sizeof(command));
	//Is a wait needed here?
	return 1; //supported command
}

const uint8_t* FfbproGetSysExHeader(uint8_t* hdr_len)
{
	static const uint8_t header[] = {0xf0, 0x00, 0x01, 0x0a, 0x01};
	*hdr_len = sizeof(header);
	return header;
}

// effect operations ---------------------------------------------------------

static void FfbproSendEffectOper(uint8_t effectId, uint8_t operation)
{
	uint8_t midi_cmd[3];
	midi_cmd[0] = 0xB5;
	midi_cmd[1] = operation;
	midi_cmd[2] = effectId;
	FfbSendData(midi_cmd, 3);
}

void FfbproStartEffect(uint8_t effectId)
{
	FfbproSendEffectOper(effectId, 0x20);
}

void FfbproStopEffect(uint8_t effectId)
{
	FfbproSendEffectOper(effectId, 0x30);
}

void FfbproFreeEffect(uint8_t effectId)
{
	FfbproSendEffectOper(effectId, 0x10);
}

// modify operations ---------------------------------------------------------

// Send to MIDI effect data modification to the given address of the given effect
void FfbproSendModify(uint8_t effectId, uint8_t address, uint16_t value)
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

void FfbproModifyDuration(uint8_t effectState, uint16_t* midi_data_param, uint8_t effectId, uint16_t duration)
{
	FfbSetParamMidi_14bit(effectState, midi_data_param, effectId, 
							FFP_MIDI_MODIFY_DURATION, duration);
	//FfbproSendModify(effectId, 0x40, duration);
}

void FfbproModifyDeviceGain(uint8_t usb_gain)
{
	FfbproSendModify(0x7f, FFP_MIDI_MODIFY_DEVICEGAIN, (usb_gain >> 1) & 0x7f);
}

static uint16_t FfbproConvertDirection(uint8_t usbdir, uint8_t reciprocal)
{
	//Convert from USB 0..179 i.e. unit 2deg to MIDI uint_14 0..359 unit deg
	//Take reciprocal direction if arg not 0
	uint16_t direction = usbdir * 2;
	
	if (reciprocal)
		direction = (direction + 180) % 360;
	
	return (direction & 0x7F) + ( (direction & 0x0180) << 1 );
}

static uint8_t FfbproModifyParamRange(volatile TEffectState* effect, uint8_t effectId, int8_t offset)
{

	volatile FFP_MIDI_Effect_Basic *midi_data = (volatile FFP_MIDI_Effect_Basic *)&effect->data;
	
	int8_t param1, param2;
	uint8_t range;
	if (offset >= 0) {
		param1 = 127;
		param2 = -128 + offset * 2;
	} else {
		param1 = 127 + (offset + 1) * 2; // avoid overflow, but offset -1 has same effect as 0
		param2 = -128;
	} // Note range of 0 should not occur - this would cause /div0 in FFbproCalcLevel
	range = param1 - param2;
	
	if (effect->invert) //param1 is always set > param2 by MS drivers? Possible this inversion could cause some unexpected behaviour 
	{
		param2 = param1;
		param1 = param2 - range;
	}
	
	FfbSetParamMidi_14bit(effect->state, &(midi_data->param1), effectId, 
							FFP_MIDI_MODIFY_PARAM1, UsbInt8ToMidiInt14(param1));	
	FfbSetParamMidi_14bit(effect->state, &(midi_data->param2), effectId, 
							FFP_MIDI_MODIFY_PARAM2, UsbInt8ToMidiInt14(param2));	
		
	return range;
}

static uint8_t FfbproCalcLevel(uint8_t range, uint8_t usb_level)
{
	// Initial levels assume full range - but range is reduced by application of offset
	// So compensate by increasing levels (attack, magnitude or fade)
	
	uint16_t v = ((uint16_t) usb_level * 255) / range; //explicit cast was necessary here to avoid implicit to int16_t and overflow
	
	if (v > 255) {
		return 0x7f; //saturated
	} else {
		return (v >> 1) & 0x7f;
	}
}

static uint16_t FfbproCalcSampleRate(uint16_t usb_samplePeriod, uint16_t frequency)
{
	if (usb_samplePeriod == USB_SAMPLEPERIOD_DEFAULT) {
		if (frequency > (FFP_SAMPLERATE_DEFAULT / 4))
			return frequency * 4;	//This is needed to avoid aliasing or attenuation of peaks	
		else
			return FFP_SAMPLERATE_DEFAULT;
	} else {
		return UsbPeriodToFrequencyHz(usb_samplePeriod);
	}
}

void FfbproSetEnvelope(
	USB_FFBReport_SetEnvelope_Output_Data_t* data,
	volatile TEffectState* effect)
{
	uint8_t eid = data->effectBlockIndex;
	
	uint16_t midi_fadeTime;
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
		uint8_t unknown1;	// Overwrite an allocated effect 
		uint16_t duration;	// unit=2ms
		uint16_t triggerButton;	// Bitwise buttons 1 to 9 from LSB
		uint16_t direction;
		uint8_t gain;
		uint16_t sampleRate;	//default 0x64 0x00 = 100Hz
		uint16_t truncate;		//default 0x10 0x4e = 10000 for full waveform
		uint8_t attackLevel;
		uint16_t	attackTime;
		uint8_t		magnitude;
		uint16_t	fadeTime;
		uint8_t	fadeLevel;
		uint16_t frequency;	// unit=Hz; 1 for constant and ramps
		uint16_t param1;	// Varies by effect type; Constant: positive=7f 00, negative=01 01, Other effects: 01 01
		uint16_t param2;	// Varies by effect type; Constant: 00 00, Other effects 01 01
	*/
	
	if (DoDebug(DEBUG_DETAIL))
		{
		LogTextP(PSTR("Set Envelope:"));
		LogBinaryLf(data, sizeof(USB_FFBReport_SetEnvelope_Output_Data_t));
		LogTextP(PSTR("  id    =")); LogBinaryLf(&eid, sizeof(eid));
		LogTextP(PSTR("  attack=")); LogBinaryLf(&data->attackLevel, sizeof(data->attackLevel));
		LogTextP(PSTR("  fade  =")); LogBinaryLf(&data->fadeLevel, sizeof(data->fadeLevel));
		LogTextP(PSTR("  attackTime=")); LogBinaryLf(&data->attackTime, sizeof(data->attackTime));
		LogTextP(PSTR("  fadeTime  =")); LogBinaryLf(&data->fadeTime, sizeof(data->fadeTime));
		FlushDebugBuffer();
		}
		
	volatile FFP_MIDI_Effect_Basic *midi_data = (volatile FFP_MIDI_Effect_Basic *)&effect->data;

	effect->usb_attackLevel = data->attackLevel;
	effect->usb_fadeLevel = data->fadeLevel;
	effect->usb_fadeTime = data->fadeTime;

	if (data->fadeTime == USB_DURATION_INFINITE) // is this check needed? Only if duration is not INF but fadeTime is INF - can this occur?
		midi_fadeTime = MIDI_DURATION_INFINITE;
	else
		midi_fadeTime = UsbUint16ToMidiUint14_Time(effect->usb_duration - effect->usb_fadeTime); 

	FfbSetParamMidi_14bit(effect->state, &(midi_data->fadeTime), eid, 
							FFP_MIDI_MODIFY_FADETIME, midi_fadeTime);
	FfbSetParamMidi_14bit(effect->state, &(midi_data->attackTime), eid, 
							FFP_MIDI_MODIFY_ATTACKTIME, UsbUint16ToMidiUint14_Time(data->attackTime));
	FfbSetParamMidi_7bit(effect->state, &(midi_data->fadeLevel), eid, 
							FFP_MIDI_MODIFY_FADE, FfbproCalcLevel(effect->range, data->fadeLevel));
	FfbSetParamMidi_7bit(effect->state, &(midi_data->attackLevel), eid, 
							FFP_MIDI_MODIFY_ATTACK, FfbproCalcLevel(effect->range, data->attackLevel));
}

void FfbproSetCondition(
	USB_FFBReport_SetCondition_Output_Data_t* data,
	volatile TEffectState* effect)
{
	uint8_t eid = data->effectBlockIndex;
	volatile FFP_MIDI_Effect_Basic *common_midi_data = (volatile FFP_MIDI_Effect_Basic *)&effect->data;

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
	
	if (DoDebug(DEBUG_DETAIL))
		{
		LogTextP(PSTR("Set Condition:"));
		LogBinaryLf(data, sizeof(USB_FFBReport_SetCondition_Output_Data_t));
		LogTextP(PSTR("  id   =")); LogBinaryLf(&eid, sizeof(eid));
		LogTextP(PSTR("  block =")); LogBinaryLf(&data->parameterBlockOffset, sizeof(data->parameterBlockOffset));
		LogTextP(PSTR("  offset=")); LogBinaryLf(&data->cpOffset, sizeof(data->cpOffset));
		LogTextP(PSTR("  coeff+=")); LogBinaryLf(&data->positiveCoefficient, sizeof(data->positiveCoefficient));
		FlushDebugBuffer();
		}

	int8_t coeff = CalcGainCoeff(data->positiveCoefficient, effect->usb_gain); //Scale coefficients by gain since FFP conditional effects don't have gain parameter

	switch (common_midi_data->waveForm) {
		case 0x0d:	// spring (midi: 0x0d)
		case 0x0e:	// damper (midi: 0x0e)
		case 0x0f:	// inertia (midi: 0x0f)
		{
			volatile FFP_MIDI_Effect_Spring_Inertia_Damper *midi_data =
				(FFP_MIDI_Effect_Spring_Inertia_Damper *)&effect->data;
			
			uint16_t midi_offsetAxis1;
			
			if (data->parameterBlockOffset == 0) {
				effect->usb_coeffAxis0 = data->positiveCoefficient;
				FfbSetParamMidi_14bit(effect->state, &(midi_data->coeffAxis0), eid, 
										FFP_MIDI_MODIFY_COEFFAXIS0, UsbInt8ToMidiInt14(coeff));
				FfbSetParamMidi_14bit(effect->state, &(midi_data->offsetAxis0), eid, 
										FFP_MIDI_MODIFY_OFFSETAXIS0, UsbInt8ToMidiInt14(data->cpOffset));
				
			} else {
				effect->usb_coeffAxis1 = data->positiveCoefficient;
				FfbSetParamMidi_14bit(effect->state, &(midi_data->coeffAxis1), eid, 
										FFP_MIDI_MODIFY_COEFFAXIS1, UsbInt8ToMidiInt14(coeff));
				if (data->cpOffset == 0x80)
					midi_offsetAxis1 = 0x007f;
				else
					midi_offsetAxis1 = UsbInt8ToMidiInt14(-data->cpOffset);
				FfbSetParamMidi_14bit(effect->state, &(midi_data->offsetAxis1), eid, 
										FFP_MIDI_MODIFY_OFFSETAXIS1, midi_offsetAxis1);			
			}

		}
		break;
		
		case 0x10:	// friction (midi: 0x10)
		{
			volatile FFP_MIDI_Effect_Friction *midi_data =
					(FFP_MIDI_Effect_Friction *)&effect->data;

			if (data->parameterBlockOffset == 0) {
				effect->usb_coeffAxis0 = data->positiveCoefficient;
				FfbSetParamMidi_14bit(effect->state, &(midi_data->coeffAxis0), eid, 
										FFP_MIDI_MODIFY_COEFFAXIS0, UsbInt8ToMidiInt14(coeff));
			} else {
				effect->usb_coeffAxis1 = data->positiveCoefficient;
				FfbSetParamMidi_14bit(effect->state, &(midi_data->coeffAxis1), eid, 
										FFP_MIDI_MODIFY_COEFFAXIS1, UsbInt8ToMidiInt14(coeff));
			}
		}
		break;
		
		default:
			break;
	}
}

void FfbproSetPeriodic(
	USB_FFBReport_SetPeriodic_Output_Data_t* data,
	volatile TEffectState* effect)
{
	uint8_t eid = data->effectBlockIndex;

	/*
	USB effect data:
		uint8_t	reportId;	// =4
		uint8_t	effectBlockIndex;	// 1..40
		uint8_t magnitude;
		int8_t	offset;
		uint8_t	phase;	// 0..255 (=0..359, exp-2)
		uint16_t	period;	// 0..32767 ms

	MIDI effect data:

	*/
	
	if (DoDebug(DEBUG_DETAIL))
		{
		LogTextP(PSTR("Set Periodic:"));
		LogBinaryLf(data, sizeof(USB_FFBReport_SetPeriodic_Output_Data_t));
		LogTextP(PSTR("  id=")); LogBinaryLf(&eid, sizeof(eid));
		LogTextP(PSTR("  magnitude=")); LogBinaryLf(&data->magnitude, sizeof(data->magnitude));
		LogTextP(PSTR("  offset   =")); LogBinaryLf(&data->offset, sizeof(data->offset));
		LogTextP(PSTR("  phase    =")); LogBinaryLf(&data->phase, sizeof(data->phase));
		LogTextP(PSTR("  period   =")); LogBinaryLf(&data->period, sizeof(data->period));
		FlushDebugBuffer();
		}
	
	volatile FFP_MIDI_Effect_Basic *midi_data = (volatile FFP_MIDI_Effect_Basic *)&effect->data;

	uint16_t frequency = 0x0001; // 1Hz
	
	// Calculate frequency (in MIDI it is in units of Hz and can have value from 1 to 169Hz)
	if (data->period <= 13) { //Can actually play up to 169Hz, but this seems a more sensible limit to avoid motor damage, plus the freq steps get quite big 
		frequency = 77; //Hz
	} else if (data->period < 1000) {
		frequency = UsbPeriodToFrequencyHz(data->period); 
	}
	
	effect->frequency = frequency;
	
	uint16_t sampleRate = FfbproCalcSampleRate(effect->usb_samplePeriod, frequency); //Sample rate may need to change as a result of frequency if usb value set to default
	
	FfbSetParamMidi_14bit(effect->state, &(midi_data->frequency), eid, 
							FFP_MIDI_MODIFY_FREQUENCY, UsbUint16ToMidiUint14(frequency));

	FfbSetParamMidi_14bit(effect->state, &(midi_data->sampleRate), eid, 
							FFP_MIDI_MODIFY_SAMPLERATE, UsbUint16ToMidiUint14(sampleRate));							
							
	// Check phase and set closest waveform and sign only before effect is sent
	//   - don't allow changes on the fly - even where possible this would result in harsh steps
	if (!(effect->state & MEffectState_SentToJoystick))
	{
		if (midi_data->waveForm == 2 || midi_data->waveForm == 3) // sine or cosine
		{
			switch (data->phase / 32) //USB 255 = 2*pi or 360deg so 32 is 45deg
			{
				case 0: //0-44deg
				case 7:
				{
					midi_data->waveForm = 2;
					effect->invert = 0;
					break;
				}
				case 1:
				case 2:
				{
					midi_data->waveForm = 3;
					effect->invert = 0;
					break;
				}
				case 3:
				case 4:
				{
					midi_data->waveForm = 2;
					effect->invert = 1; //i.e. -sine
					break;
				}	
				case 5:
				case 6:
				{
					midi_data->waveForm = 3;
					effect->invert = 1;
					break;
				}
			}
		} else {
			if ((data->phase > 64) && (data->phase < 192)) { //for square, tri, sawtooth
				effect->invert = 1;
			} else {
				effect->invert = 0;
			}
		}
	}
	
	// Calculate min max and available range from offset. Then Modify. Invert if needed.
	uint8_t range = FfbproModifyParamRange(effect, eid, data->offset);
	
	// Calculate magnitude relative to available range
	FfbSetParamMidi_7bit(effect->state, &(midi_data->magnitude), eid, 
						FFP_MIDI_MODIFY_MAGNITUDE, FfbproCalcLevel(range, data->magnitude));
	
	// Check whether envelope levels need to be updated too
	if (range != effect->range)	
	{	
		effect->range = range;
		FfbSetParamMidi_7bit(effect->state, &(midi_data->fadeLevel), eid, 
								FFP_MIDI_MODIFY_FADE, FfbproCalcLevel(range, effect->usb_fadeLevel));
		FfbSetParamMidi_7bit(effect->state, &(midi_data->attackLevel), eid, 
								FFP_MIDI_MODIFY_ATTACK, FfbproCalcLevel(range, effect->usb_attackLevel));		
	}
								
}

void FfbproSetConstantForce(
	USB_FFBReport_SetConstantForce_Output_Data_t* data,
	volatile TEffectState* effect)
{
	uint8_t eid = data->effectBlockIndex;
	/*
	USB data:
		uint8_t	reportId;	// =5
		uint8_t	effectBlockIndex;	// 1..40
		int16_t magnitude;	// -255..255

	MIDI effect data:
		uint8_t command;	// always 0x23	-- start counting checksum from here
		uint8_t waveForm;	// 2=sine, 5=Square, 6=RampUp, 7=RampDown, 8=Triange, 0x12=Constant
		uint8_t unknown1;	// Overwrite an allocated effect 
		uint16_t duration;	// unit=2ms
		uint16_t triggerButton;	// Bitwise buttons 1 to 9 from LSB
		uint16_t direction;
		uint8_t gain;
		uint16_t sampleRate;	//default 0x64 0x00 = 100Hz
		uint16_t truncate;		//default 0x10 0x4e = 10000 for full waveform
		uint8_t attackLevel;
		uint16_t	attackTime;
		uint8_t		magnitude;
		uint16_t	fadeTime;
		uint8_t	fadeLevel;
		uint16_t frequency;	// unit=Hz; 1 for constant and ramps
		uint16_t param1;	// Varies by effect type; Constant: positive=7f 00, negative=01 01, Other effects: 01 01
		uint16_t param2;	// Varies by effect type; Constant: 00 00, Other effects 01 01

	*/
	
	if (DoDebug(DEBUG_DETAIL))
		{
		LogTextP(PSTR("Set Constant Force:"));
		LogBinaryLf(data, sizeof(USB_FFBReport_SetConstantForce_Output_Data_t));
		LogTextP(PSTR("  id=")); LogBinaryLf(&eid, sizeof(eid));
		LogTextP(PSTR("  magnitude=")); LogBinaryLf(&data->magnitude, sizeof(data->magnitude));
		FlushDebugBuffer();
		}
	
	volatile FFP_MIDI_Effect_Basic *midi_data = (volatile FFP_MIDI_Effect_Basic *)&effect->data;

	effect->usb_magnitude = data->magnitude;

	uint8_t midi_magnitude;

	if (data->magnitude >= 0) {
		midi_magnitude = (data->magnitude >> 1) & 0x7f;

	} else {
		midi_magnitude = (( -(data->magnitude + 1)) >> 1) & 0x7f;

	}
	
	FfbSetParamMidi_7bit(effect->state, &(midi_data->magnitude), eid, 
						FFP_MIDI_MODIFY_MAGNITUDE, midi_magnitude);
	FfbSetParamMidi_14bit(effect->state, &(midi_data->direction), eid, 
						FFP_MIDI_MODIFY_DIRECTION, FfbproConvertDirection(effect->usb_direction, (data->magnitude < 0))); 
							//reciprocal direction if -ve
						
	midi_data->param1 = 0x007F; // never again modified
	midi_data->param2 = 0x0000; // never again modified 
}

void FfbproSetRampForce(
	USB_FFBReport_SetRampForce_Output_Data_t* data,
	volatile TEffectState* effect)
{
	uint8_t eid = data->effectBlockIndex;
	if (DoDebug(DEBUG_DETAIL))
		{
		uint8_t eid = data->effectBlockIndex;
		LogTextP(PSTR("Set Ramp Force:"));
		LogBinaryLf(data, sizeof(USB_FFBReport_SetRampForce_Output_Data_t));
		LogTextP(PSTR("  id=")); LogBinaryLf(&eid, sizeof(eid));
		LogTextP(PSTR("  start=")); LogBinaryLf(&data->start, sizeof(data->start));
		LogTextP(PSTR("  end  =")); LogBinaryLf(&data->end, sizeof(data->end));
		FlushDebugBuffer();
		}

	/*
	USB effect data:
		uint8_t	reportId;	// =6
		uint8_t	effectBlockIndex;	// 1..40
		int8_t start;
		int8_t	end;
	*/
	
	volatile FFP_MIDI_Effect_Basic *midi_data = (volatile FFP_MIDI_Effect_Basic *)&effect->data;

	// Same approach as periodic waveforms
	int8_t offset = ((int16_t)data->start + (int16_t)data->end)/2; //Could be done more efficiently without casting
	uint8_t magnitude;
	
	if (data->start > data->end) {
		effect->invert = 1; //Ramp Down
		magnitude = data->start - data->end;
	} else {
		effect->invert = 0; //Ramp Up
		magnitude = data->end - data->start;
	}


	// Calculate min max and available range from offset. Then Modify. Invert if needed.
	uint8_t range = FfbproModifyParamRange(effect, eid, offset);
	
	
	// Calculate magnitude relative to available range
	FfbSetParamMidi_7bit(effect->state, &(midi_data->magnitude), eid, 
						FFP_MIDI_MODIFY_MAGNITUDE, FfbproCalcLevel(range, magnitude));
	
	// Check whether envelope levels need to be updated too
	if (range != effect->range)	
	{	
		effect->range = range;
		FfbSetParamMidi_7bit(effect->state, &(midi_data->fadeLevel), eid, 
								FFP_MIDI_MODIFY_FADE, FfbproCalcLevel(range, effect->usb_fadeLevel));
		FfbSetParamMidi_7bit(effect->state, &(midi_data->attackLevel), eid, 
								FFP_MIDI_MODIFY_ATTACK, FfbproCalcLevel(range, effect->usb_attackLevel));		
	}
		
	
/*
	uint16_t midi_param1;

	if (data->start < 0)
		midi_param1 = 0x0100 | (-(data->start+1));
	else
		midi_param1 = data->start;
	
	FfbSetParamMidi_14bit(effect->state, &(midi_data->param1), eid, 
						FFP_MIDI_MODIFY_PARAM1, midi_param1);	

	FfbSetParamMidi_14bit(effect->state, &(midi_data->param2), eid, 
						FFP_MIDI_MODIFY_PARAM2, UsbInt8ToMidiInt14(data->end));
						*/
}

int FfbproSetEffect(
	USB_FFBReport_SetEffect_Output_Data_t *data,
	volatile TEffectState* effect
)
{
	uint8_t eid = data->effectBlockIndex;

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

	volatile FFP_MIDI_Effect_Basic *midi_data = (volatile FFP_MIDI_Effect_Basic *)&effect->data;
	uint8_t midi_data_len = sizeof(FFP_MIDI_Effect_Basic); 	// default MIDI data size
	
	// Data applying to all effects
	uint16_t buttonBits = 0;
	if (data->triggerButton != USB_TRIGGERBUTTON_NULL)		
		buttonBits = (1 << data->triggerButton);
	//Buttons 1-9 from LSB
	FfbSetParamMidi_14bit(effect->state, &(midi_data->triggerButton), eid, 
							FFP_MIDI_MODIFY_TRIGGERBUTTON, (buttonBits & 0x7F) + ( (buttonBits & 0x0180) << 1 ));	

	// Fill in the effect type specific data
	switch (data->effectType)
	{
		case USB_EFFECT_SQUARE:
		case USB_EFFECT_SINE:
		case USB_EFFECT_TRIANGLE:
		case USB_EFFECT_SAWTOOTHDOWN:
		case USB_EFFECT_SAWTOOTHUP:
		case USB_EFFECT_CONSTANT:
		case USB_EFFECT_RAMP:
		{
			/*
			MIDI effect data:
				uint8_t command;	// always 0x23	-- start counting checksum from here
				uint8_t waveForm;	// 2=sine, 5=Square, 6=RampUp, 7=RampDown, 8=Triange, 0x12=Constant
				uint8_t unknown1;	// Overwrite an allocated effect 
				uint16_t duration;	// unit=2ms
				uint16_t triggerButton;	// Bitwise buttons 1 to 9 from LSB
				uint16_t direction;
				uint8_t gain;
				uint16_t sampleRate;	//default 0x64 0x00 = 100Hz
				uint16_t truncate;		//default 0x10 0x4e = 10000 for full waveform
				uint8_t attackLevel;
				uint16_t	attackTime;
				uint8_t		magnitude;
				uint16_t	fadeTime;
				uint8_t	fadeLevel;
				uint16_t frequency;	// unit=Hz; 1 for constant and ramps
				uint16_t param1;	// Varies by effect type; Constant: positive=7f 00, negative=01 01, Other effects: 01 01
				uint16_t param2;	// Varies by effect type; Constant: 00 00, Other effects 01 01

			*/
			// Effect Gain
			FfbSetParamMidi_7bit(effect->state, &(midi_data->gain), eid, 
								FFP_MIDI_MODIFY_GAIN, (data->gain >> 1) & 0x7f);			
			
			// Convert direction
			effect->usb_direction = data->directionX;
			FfbSetParamMidi_14bit(effect->state, &(midi_data->direction), eid, 
								FFP_MIDI_MODIFY_DIRECTION, FfbproConvertDirection(data->directionX, (effect->usb_magnitude < 0)));
				//reciprocal only if -ve constant force
			
			
			// Recalculate fadeTime for MIDI since change to duration changes the fadeTime too
			uint16_t midi_fadeTime;
			if (data->duration == USB_DURATION_INFINITE) {
				midi_fadeTime = MIDI_DURATION_INFINITE;
			} else {
				if (effect->usb_fadeTime == USB_DURATION_INFINITE) {
					midi_fadeTime = MIDI_DURATION_INFINITE;
				} else {
					if (effect->usb_duration > effect->usb_fadeTime) {
						// add some safety and special case handling
						midi_fadeTime = UsbUint16ToMidiUint14_Time(effect->usb_duration - effect->usb_fadeTime);
					} else {
						midi_fadeTime = midi_data->duration;
					}
				}
			}			
			FfbSetParamMidi_14bit(effect->state, &(midi_data->fadeTime), eid, 
								FFP_MIDI_MODIFY_FADETIME, midi_fadeTime);
			
			effect->usb_samplePeriod = data->samplePeriod;
			
			uint16_t sampleRate = FfbproCalcSampleRate(data->samplePeriod, effect->frequency);

			FfbSetParamMidi_14bit(effect->state, &(midi_data->sampleRate), eid, 
									FFP_MIDI_MODIFY_SAMPLERATE, UsbUint16ToMidiUint14(sampleRate));								
		}
		break;
	
		case USB_EFFECT_SPRING:
		case USB_EFFECT_DAMPER:
		case USB_EFFECT_INERTIA:
		{
			/*
			MIDI effect data:
				uint8_t command;	// always 0x23	-- start counting checksum from here
				uint8_t waveForm;	// 2=sine, 5=Square, 6=RampUp, 7=RampDown, 8=Triange, 0x12=Constant
				uint8_t unknown1;	// ? always 0x7F
				uint16_t duration;	// unit=2ms
				uint16_t triggerButton;	// Bitwise buttons 1 to 9 from LSB
				uint16_t coeffAxis0;
				uint16_t coeffAxis1;
				uint16_t offsetAxis0;
				uint16_t offsetAxis1;
			*/

			volatile FFP_MIDI_Effect_Spring_Inertia_Damper *midi_data =
				(FFP_MIDI_Effect_Spring_Inertia_Damper *)&effect->data;
				
			effect->usb_gain = data->gain;			//Scale coefficients by gain since FFP conditional effects don't have gain parameter
			FfbSetParamMidi_14bit(effect->state, &(midi_data->coeffAxis0), eid, 
						FFP_MIDI_MODIFY_COEFFAXIS0, UsbInt8ToMidiInt14(CalcGainCoeff(effect->usb_coeffAxis0, data->gain)));
			FfbSetParamMidi_14bit(effect->state, &(midi_data->coeffAxis1), eid, 
						FFP_MIDI_MODIFY_COEFFAXIS1, UsbInt8ToMidiInt14(CalcGainCoeff(effect->usb_coeffAxis1, data->gain)));
						
			midi_data_len = sizeof(FFP_MIDI_Effect_Spring_Inertia_Damper);
			
		}
		break;
		
		case USB_EFFECT_FRICTION:
		{
			/*
			MIDI effect data:
				uint8_t command;	// always 0x23	-- start counting checksum from here
				uint8_t waveForm;	// 2=sine, 5=Square, 6=RampUp, 7=RampDown, 8=Triange, 0x12=Constant
				uint8_t unknown1;	// ? always 0x7F
				uint16_t duration;	// unit=2ms
				uint16_t triggerButton;	// Bitwise buttons 1 to 9 from LSB
				uint16_t coeffAxis0;
				uint16_t coeffAxis1;
			*/
			volatile FFP_MIDI_Effect_Friction *midi_data =
					(FFP_MIDI_Effect_Friction *)&effect->data;
					
			effect->usb_gain = data->gain;			//Scale coefficients by gain since FFP conditional effects don't have gain parameter
			FfbSetParamMidi_14bit(effect->state, &(midi_data->coeffAxis0), eid, 
						FFP_MIDI_MODIFY_COEFFAXIS0, UsbInt8ToMidiInt14(CalcGainCoeff(effect->usb_coeffAxis0, data->gain)));
			FfbSetParamMidi_14bit(effect->state, &(midi_data->coeffAxis1), eid, 
						FFP_MIDI_MODIFY_COEFFAXIS1, UsbInt8ToMidiInt14(CalcGainCoeff(effect->usb_coeffAxis1, data->gain)));			
			
			midi_data_len = sizeof(FFP_MIDI_Effect_Friction);

		}
		break;
		
		case USB_EFFECT_CUSTOM:	// custom (midi: ? does FFP support custom forces?)
		{
		}
		break;
		
		default:
		break;
	}
	
	return midi_data_len;
}

void FfbproCreateNewEffect(
	USB_FFBReport_CreateNewEffect_Feature_Data_t* inData,
	volatile TEffectState* effect)
{
	/*
	USB effect data:
		uint8_t		reportId;	// =1
		uint8_t	effectType;	// Enum (1..12): ET 26,27,30,31,32,33,34,40,41,42,43,28
		uint16_t	byteCount;	// 0..511	- only valid with Custom Force
	*/

	// Set defaults to the effect data

	volatile FFP_MIDI_Effect_Basic *midi_data = (volatile FFP_MIDI_Effect_Basic *)&effect->data;

	midi_data->magnitude = 0x7f;
	midi_data->frequency = 0x0001;
	midi_data->attackLevel = 0x00;
	midi_data->attackTime = 0x0000;
	midi_data->fadeLevel = 0x00;
	midi_data->fadeTime = 0x0000;
	midi_data->gain = 0x7F;
	midi_data->triggerButton = 0x0000;
	midi_data->sampleRate = FFP_SAMPLERATE_DEFAULT;	
	
	// Constants
	midi_data->command = 0x23;
	midi_data->unknown1 = 0x7F;
	midi_data->truncate = 0x4E10; // 10000
	if (inData->effectType == 0x01)	// constant
		midi_data->param2 = 0x0000;
	else
		midi_data->param2 = 0x0101;
}