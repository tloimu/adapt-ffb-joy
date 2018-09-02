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

#ifndef _FFB_WHEEL_
#define _FFB_WHEEL_

#define CMD_PLAY		0x02
#define CMD_STOP		0x03
#define CMD_DELETE		0x01

#define EFFECT_SINE		0x02
#define EFFECT_SQUARE	0x03
#define EFFECT_TRIANGLE	0x04
#define EFFECT_SAWTOOTH	0x05

#include <stdint.h>
#include "ffb.h"

typedef struct
{
	uint8_t		cmd;		// f2
	uint8_t		operation_and_checksum;
	uint8_t		effect_id;
} cmd_f2_t;

typedef struct
{
	uint8_t		cmd;		// f1
	uint8_t		checksum;
	uint8_t		def_and_address;
	uint8_t		effect_id;
	uint16_t	value;
} cmd_f1_t;

typedef struct
{
	uint8_t		command; 	// always 0x20
	uint8_t 	effect_type;
	uint8_t 	unknown;	// always 0x7f
	
	uint16_t	duration;
	uint8_t		direction;
} cmd_f0_common_t;

/* type for sine, square, triangle, sawtooth and ramp */
typedef struct
{
	cmd_f0_common_t	common;
	
	uint8_t		precise_dir;
	uint16_t	p_x_offset;
	uint8_t		e_y1;
	uint16_t	e_x1;
	uint8_t		p_amplitude;
	uint16_t	e_x2;
	uint8_t		e_y2;
	uint16_t	p_t;
	uint8_t		p_y_offset;
} cmd_f0_wave_t;

typedef struct
{
	cmd_f0_common_t	common;
	uint8_t	positive_coefficient;
} cmd_f0_friction_t;

typedef struct
{
	cmd_f0_common_t	common;
	
	uint8_t		unknown;
	uint8_t		e_y1;
	uint16_t 	e_x1;
	uint8_t 	force;
	uint16_t 	e_x2;
	uint8_t 	e_y2;
	uint8_t 	force_direction;
} cmd_f0_constant_force_t;

void FfbwheelEnableInterrupts(void);
const uint8_t* FfbwheelGetSysExHeader(uint8_t* hdr_len);
void FfbwheelSetAutoCenter(uint8_t enable);

void FfbwheelStartEffect(uint8_t effectId);
void FfbwheelStopEffect(uint8_t effectId);
void FfbwheelFreeEffect(uint8_t effectId);

void FfbwheelModifyDuration(uint8_t effectId, uint16_t duration);

void FfbwheelSetEnvelope(USB_FFBReport_SetEnvelope_Output_Data_t* data, volatile TEffectState* e);
void FfbwheelSetCondition(USB_FFBReport_SetCondition_Output_Data_t* data, volatile TEffectState* e);
void FfbwheelSetPeriodic(USB_FFBReport_SetPeriodic_Output_Data_t* data, volatile TEffectState* e);
void FfbwheelSetConstantForce(USB_FFBReport_SetConstantForce_Output_Data_t* data, volatile TEffectState* e);
void FfbwheelSetRampForce(USB_FFBReport_SetRampForce_Output_Data_t* data, volatile TEffectState* e);
int  FfbwheelSetEffect(USB_FFBReport_SetEffect_Output_Data_t *data, volatile TEffectState* effect);
void FfbwheelCreateNewEffect(USB_FFBReport_CreateNewEffect_Feature_Data_t* inData, volatile TEffectState* effect);

uint8_t FfbwheelUsbToMidiEffectType(uint8_t usb_effect_type);

void FfbWheelMaintainEffects(void);

#endif // _FFB_WHEEL_