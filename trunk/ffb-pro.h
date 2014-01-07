
#ifndef _FFB_PRO_
#define _FFB_PRO_

#include <stdint.h>
#include "ffb.h"

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

void FfbproEnableInterrupts(void);
const uint8_t* FfbproGetSysExHeader(uint8_t* hdr_len);
void FfbproSetAutoCenter(uint8_t enable);

void FfbproStartEffect(uint8_t id);
void FfbproStopEffect(uint8_t id);
void FfbproFreeEffect(uint8_t id);

void FfbproModifyDuration(uint8_t effectId, uint16_t duration);

void FfbproSetEnvelope(USB_FFBReport_SetEnvelope_Output_Data_t* data, volatile TEffectState* effect);
void FfbproSetCondition(USB_FFBReport_SetCondition_Output_Data_t* data, volatile TEffectState* effect);
void FfbproSetPeriodic(USB_FFBReport_SetPeriodic_Output_Data_t* data, volatile TEffectState* effect);
void FfbproSetConstantForce(USB_FFBReport_SetConstantForce_Output_Data_t* data, volatile TEffectState* effect);
void FfbproSetRampForce(USB_FFBReport_SetRampForce_Output_Data_t* data, volatile TEffectState* effect);
int  FfbproSetEffect(USB_FFBReport_SetEffect_Output_Data_t *data, volatile TEffectState* effect);
void FfbproCreateNewEffect(USB_FFBReport_CreateNewEffect_Feature_Data_t* inData, volatile TEffectState* effect);

uint8_t FfbproUsbToMidiEffectType(uint8_t usb_effect_type);

#endif // _FFB_PRO_