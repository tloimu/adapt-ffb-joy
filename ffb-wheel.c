/*
	Copyright 2013  Saku Kekkonen

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "ffb-wheel.h"

#include <LUFA/Drivers/Board/LEDs.h>
#include <util/delay.h>

uint8_t FfbwheelUsbToMidiEffectType(uint8_t usb_effect_type)
{
	const uint8_t usbToMidiEffectType[] = {
		0x06,	// Constant, 
		0x05, 	// Ramp
		0x03, 	// Square
		0x02, 	// Sine
		0x04,	// Triangle
		0x05,	// SawtoothDown
		0x05,	// SawtoothUp
		0x08,	// Spring
		0x09,	// Damper
		0x0a,	// Inertia
		0x0b,	// Friction
		0x00 	// Custom ?
	};
	
	if (usb_effect_type >= sizeof(usbToMidiEffectType))
		return 0;
		
	return usbToMidiEffectType[usb_effect_type];
}

uint8_t FfbwheelEffectMemFull(uint8_t new_midi_type)
{
	return 0; //Supported quantities of each effect not yet known
}

/**
 * Initialize wheel for FF. Releases spring effect.
 *
 * Force Editor with Windows XP and gameport sends 
 * X1 pulse groups during initialization, but
 * those are not needed for enabling FF.
 */
void FfbwheelEnableInterrupts(void)
	{
	
	const uint8_t startupFfbWheelData_0[] = {
		0xf3, 0x1d
	};
	
	const uint8_t startupFfbWheelData_1[] = {
		0xf1 ,0x0e ,0x43 ,0x01 ,0x00 ,0x7d,
		0xf1 ,0x7e ,0x04 ,0x01 ,0x3e ,0x4e,
		0xf1 ,0x1c ,0x45 ,0x01 ,0x3e ,0x2f,
		0xf1 ,0x0b ,0x46 ,0x01 ,0x7d ,0x00,
	};
	
    WaitMs(100);

	FfbSendData(startupFfbWheelData_0, sizeof(startupFfbWheelData_0));
	FfbSendData(startupFfbWheelData_1, sizeof(startupFfbWheelData_1));
	FfbwheelDeviceControl(USB_DCTRL_RESET); // Leave auto centre on
	
	WaitMs(100);
	}

uint8_t FfbwheelDeviceControl(uint8_t usb_control)
{ // CHANGED FOR COMPATIBILITY - NOT TESTED FOR WHEEL
	/*
	USB_DCTRL_ACTUATORS_ENABLE	0x01
	USB_DCTRL_ACTUATORS_DISABLE	0x02 
	USB_DCTRL_STOPALL			0x03 
	USB_DCTRL_RESET				0x04
	USB_DCTRL_PAUSE				0x05
	USB_DCTRL_CONTINUE			0x06
	*/
	uint8_t command[2] = {0xf3};
	
	if (usb_control == USB_DCTRL_RESET) {
		command[1] = 0x1d;
	} else if (usb_control == USB_DCTRL_STOPALL) {
		command[1] = 0x6a;
	} else {
		return 0; //not supported
	}

	FfbSendData(command, sizeof(command));
	//Is a wait needed?
	return 1; //supported command
}

const uint8_t* FfbwheelGetSysExHeader(uint8_t* hdr_len)
{
	static const uint8_t header[] = {0xf0, 0x00, 0x01, 0x0a, 0x15};
	*hdr_len = sizeof(header);
	return header;
}

// effect operations ---------------------------------------------------------

static void FfbwheelSendEffectOper(uint8_t effectId, uint8_t operation)
{
	cmd_f2_t op;
	
	op.cmd = 0xf2;
	op.effect_id = effectId;
	op.operation_and_checksum = operation << 4;
	
	uint8_t sum = 0xf ^ 0x2 ^ (op.operation_and_checksum >> 4)
			^ (op.effect_id >> 4) ^ (op.effect_id & 0x0f);
			
	op.operation_and_checksum &= 0xf0;
	op.operation_and_checksum |= sum;
	
	FfbSendData((const uint8_t*)&op, sizeof(op));
}

void FfbwheelStartEffect(uint8_t effectId)
{
	FfbwheelSendEffectOper(effectId, 2);
}

void FfbwheelStopEffect(uint8_t effectId)
{
	FfbwheelSendEffectOper(effectId, 3);
}

void FfbwheelFreeEffect(uint8_t effectId)
{
	FfbwheelSendEffectOper(effectId, 1);
}

// modify operations ---------------------------------------------------------

void FfbwheelSendModify(uint8_t effectId, uint8_t address, uint16_t value)
{
	cmd_f1_t op;

	op.cmd = 0xf1;
	op.def_and_address = 0x40 | address;	// could set 0x00 for values that are defaults
	op.effect_id = effectId;
	op.value = value;
	
	uint8_t* d = (uint8_t*)&op;
	uint8_t sum = d[0] + (d[2] & ~0x40) + d[3] + d[4] + d[5];
	op.checksum = (0x80 - sum) & 0x7f;
	
	FfbSendData(d, sizeof(op));
}

void FfbwheelModifyDuration(uint8_t effectState, uint16_t* midi_data_param, uint8_t effectId, uint16_t duration)
{
	//FfbwheelSendModify(effectId, 0x00, duration);
	FfbSetParamMidi_14bit(effectState, midi_data_param, effectId, 0x00, duration); // CHANGED FOR COMPATIBILITY - NOT TESTED FOR WHEEL
}

void FfbwheelModifyDeviceGain(uint8_t gain)
{ // TO IMPLEMENT: CHANGED FOR COMPATIBILITY - NOT TESTED FOR WHEEL
	static const uint8_t gainCommand[] = {0xf1, 0x10, 0x40, 0x00, 0x7f, 0x00}; // only sends max gain for now
	FfbSendData(gainCommand, sizeof(gainCommand));

}


void FfbwheelSetEnvelope(
	USB_FFBReport_SetEnvelope_Output_Data_t* data,
	volatile TEffectState* effect)
{
}

void FfbwheelSetCondition(
	USB_FFBReport_SetCondition_Output_Data_t* data,
	volatile TEffectState* effect)
{
}

void FfbwheelSetPeriodic(
	USB_FFBReport_SetPeriodic_Output_Data_t* data,
	volatile TEffectState* effect)
{
}

void FfbwheelSetConstantForce(
	USB_FFBReport_SetConstantForce_Output_Data_t* data,
	volatile TEffectState* effect)
{
}

void FfbwheelSetRampForce(
	USB_FFBReport_SetRampForce_Output_Data_t* data,
	volatile TEffectState* e)
{
}

int FfbwheelSetEffect(
	USB_FFBReport_SetEffect_Output_Data_t *data,
	volatile TEffectState* e)
{
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
   
	uint8_t midi_data_len = 0;
   
	switch (data->effectType)
	{
	case USB_EFFECT_SQUARE:
	case USB_EFFECT_SINE:
	case USB_EFFECT_TRIANGLE:
	case USB_EFFECT_SAWTOOTHDOWN:
	case USB_EFFECT_SAWTOOTHUP:
	case USB_EFFECT_RAMP:
	{
		midi_data_len = sizeof(cmd_f0_wave_t);
		cmd_f0_wave_t* midi_data = (cmd_f0_wave_t*)e->data;
	}
	break;
	
	case USB_EFFECT_CONSTANT:
	{
		midi_data_len = sizeof(cmd_f0_constant_force_t);
		cmd_f0_constant_force_t* midi_data = (cmd_f0_constant_force_t*)e->data;
	}
	break;

	case USB_EFFECT_SPRING:
	case USB_EFFECT_DAMPER:
	case USB_EFFECT_INERTIA:
	
	case USB_EFFECT_FRICTION:
	{
		midi_data_len = sizeof(cmd_f0_friction_t);
		cmd_f0_friction_t* midi_data = (cmd_f0_friction_t*)e->data;
	}
	break;
	
	case USB_EFFECT_CUSTOM:
	default:
	{
	}
	break;
	}

	return midi_data_len;
}

void FfbwheelCreateNewEffect(
	USB_FFBReport_CreateNewEffect_Feature_Data_t* data,
	volatile TEffectState* effect)
{
	cmd_f0_common_t* c = (cmd_f0_common_t*)&effect->data;
	c->command = 0x20; // always 0x20
	c->unknown = 0x7f; // always 0x7f
	c->direction = 0x00; // 0 for effect not using direction

		/* ramp   f0 00 01 0a 15 20 05 7f 6e 1e 40 7f 00 00 7f 00 00 7f 6e 1e 7f 6e 1e 3e  3e f7 */
	/* damper f0 00 01 0a 15 20 09 7f 6e 1e 00 00 7d 3e 3f 3e 3f 7d 00  58 f7 */
	/* spring f0 00 01 0a 15 20 08 7f 6e 1e 00 00 7d 3e 4e 3e 2f 7d 00  5a f7 */
	
	/*
	 
	effect command for waveforms

   xx 			effect type (sine=2, square=3, triangle=4, sawtooth=5)
   7f			always 7f
   xx xx		duration
   xx 		        direction (angle*128/360)
   ??			precise direction indicator ? 7f=precise, 7d=not
   00 40		periodic x-offset, default 00 40
   7f			envelope y1, default 7f
   00 00		envelope x1, default 00 00
   7f			periodic amplitude
   65 12		envelope x2
   7f			envelope y2
   74 03		periodic x-hz
   3e			periodic y-offset
   */
	

	
	switch (data->effectType)
	{
	case USB_EFFECT_SQUARE:
	case USB_EFFECT_SINE:
	case USB_EFFECT_TRIANGLE:
	case USB_EFFECT_SAWTOOTHDOWN:
	case USB_EFFECT_SAWTOOTHUP:
	case USB_EFFECT_RAMP:
	{
		cmd_f0_wave_t* midi_data = (cmd_f0_wave_t*)effect->data;
		midi_data->common.direction = 0x40;
		
		// TODO: convert from 16/8-bit presentation

		midi_data->precise_dir = 0x7f;
		midi_data->e_y1 = 0x7f;
		midi_data->e_x1 = 0x0000;
		midi_data->p_amplitude = 0x07f;
		midi_data->e_y2 = 0x7f;
		midi_data->p_y_offset = 0x3e;
		midi_data->e_y2 = 0x7f;
		midi_data->p_y_offset = 0x3e;
		
		if (data->effectType == USB_EFFECT_RAMP) {
			midi_data->p_x_offset = 0x0000;
			midi_data->e_x2 = 0x1e6e;
			midi_data->p_t = 0x1e6e;
		} else {
			midi_data->p_x_offset = 0x4000; // 0x00 0x40
			midi_data->e_x2 = 0x1265;
			midi_data->p_t = 0x0374;
		}
	}
	break;
	
	case USB_EFFECT_CONSTANT:
	{
		cmd_f0_constant_force_t* midi_data = (cmd_f0_constant_force_t*)effect->data;
	}
	break;

	case USB_EFFECT_SPRING:
	case USB_EFFECT_DAMPER:
	case USB_EFFECT_INERTIA:
	
	case USB_EFFECT_FRICTION:
	{
		cmd_f0_friction_t* midi_data = (cmd_f0_friction_t*)effect->data;
		midi_data->positive_coefficient = 0x7e;
	}
	break;
	
	case USB_EFFECT_CUSTOM:
	default:
	{
	}
	break;
	}
}
