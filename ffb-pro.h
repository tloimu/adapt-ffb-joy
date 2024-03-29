
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
	uint8_t unknown1;	// Overwrite an allocated effect - leave as unknown1 because we don't need it and don't know what it does for wheel
	uint16_t duration;	// unit=2ms
	uint16_t triggerButton;	// Bitwise buttons 1 to 9 from LSB
	uint16_t direction;
	uint8_t gain;
	uint16_t sampleRate;	//default 0x64 0x00 = 100Hz
	uint16_t truncate;		//default 0x10 0x4e = 10000 for full waveform
	uint8_t attackLevel;
	uint16_t	attackTime;
	uint8_t		magnitude; //i.e. envelope sustain
	uint16_t	fadeTime;
	uint8_t	fadeLevel;
	uint16_t frequency;	// unit=Hz; 1 for constant and ramps
	uint16_t param1;	// Varies by effect type; Constant: positive=7f 00, negative=01 01, Other effects: 01 01
	uint16_t param2;	// Varies by effect type; Constant: 00 00, Other effects 01 01
	} FFP_MIDI_Effect_Basic;

typedef struct
	{
	uint8_t command;	// always 0x23	-- start counting checksum from here
	uint8_t waveForm;	// 0xd=Spring, 0x0e=Damper, 0xf=Inertia
	uint8_t unknown1;	// Overwrite an allocated effect - leave as unknown1 because we don't need it and don't know what it does for wheel
	uint16_t duration;	// unit=2ms
	uint16_t triggerButton;	// Bitwise buttons 1 to 9 from LSB
	uint16_t coeffAxis0;
	uint16_t coeffAxis1;
	uint16_t offsetAxis0;
	uint16_t offsetAxis1;
	} FFP_MIDI_Effect_Spring_Inertia_Damper;

typedef struct
	{
	uint8_t command;	// always 0x23	-- start counting checksum from here
	uint8_t waveForm;	// 0x10=Friction
	uint8_t unknown1;	// Overwrite an allocated effect - leave as unknown1 because we don't need it and don't know what it does for wheel
	uint16_t duration;	// unit=2ms
	uint16_t triggerButton;	// Bitwise buttons 1 to 9 from LSB
	uint16_t coeffAxis0;
	uint16_t coeffAxis1;
	} FFP_MIDI_Effect_Friction;

// Data structures: to be shared between Output reports for calculating MIDI parameters coupled to multiple USB parameters
// Set [MAX_SHARE_DATA] bytes to largest of
typedef struct
	{
	uint16_t usb_duration;
	uint16_t usb_fadeTime;
	uint8_t usb_attackLevel;
	uint8_t usb_fadeLevel;
	uint8_t range;	
	uint16_t usb_samplePeriod;
	uint16_t frequency;
	uint8_t invert;
	} FFP_Share_Periodic_Ramp;
	
typedef struct
	{
	uint16_t usb_duration;
	uint16_t usb_fadeTime;
	uint8_t usb_attackLevel;
	uint8_t usb_fadeLevel;
	uint8_t range;			//Not varied for constant but simplifies use of common structure
	uint8_t usb_magnitude;
	uint8_t usb_direction;
	} FFP_Share_Constant;
	
typedef struct
	{
	uint16_t usb_duration;
	uint16_t usb_fadeTime;
	uint8_t usb_attackLevel;
	uint8_t usb_fadeLevel;
	uint8_t range;		
	} FFP_Share_Basic_common_t;	

typedef struct	
	{
	uint8_t usb_coeffAxis0;
	uint8_t usb_coeffAxis1;
	uint8_t usb_gain;
	} FFP_Share_Condition;

void FfbproEnableInterrupts(void);
uint8_t FfbproDeviceControl(uint8_t usb_control);
const uint8_t* FfbproGetSysExHeader(uint8_t* hdr_len);

void FfbproStartEffect(uint8_t id);
void FfbproStopEffect(uint8_t id);
void FfbproFreeEffect(uint8_t id);

void FfbproSendModify(uint8_t effectId, uint8_t address, uint16_t value);

void FfbproModifyDuration(uint8_t effectState, uint16_t* midi_data_param, uint8_t effectId, uint16_t duration);
void FfbproModifyDeviceGain(uint8_t gain);

void FfbproSetEnvelope(USB_FFBReport_SetEnvelope_Output_Data_t* data, volatile TEffectState* effect);
void FfbproSetCondition(USB_FFBReport_SetCondition_Output_Data_t* data, volatile TEffectState* effect);
void FfbproSetPeriodic(USB_FFBReport_SetPeriodic_Output_Data_t* data, volatile TEffectState* effect);
void FfbproSetConstantForce(USB_FFBReport_SetConstantForce_Output_Data_t* data, volatile TEffectState* effect);
void FfbproSetRampForce(USB_FFBReport_SetRampForce_Output_Data_t* data, volatile TEffectState* effect);
int  FfbproSetEffect(USB_FFBReport_SetEffect_Output_Data_t *data, volatile TEffectState* effect);
void FfbproCreateNewEffect(USB_FFBReport_CreateNewEffect_Feature_Data_t* inData, volatile TEffectState* effect);

uint8_t FfbproUsbToMidiEffectType(uint8_t usb_effect_type);
uint8_t FfbproEffectMemFull(uint8_t new_midi_type);

#define FFP_MIDI_MODIFY_DURATION		0x40
#define FFP_MIDI_MODIFY_TRIGGERBUTTON	0x44
#define FFP_MIDI_MODIFY_DIRECTION		0x48
#define FFP_MIDI_MODIFY_GAIN			0x4C
#define FFP_MIDI_MODIFY_SAMPLERATE		0x50
#define FFP_MIDI_MODIFY_ATTACK			0x64
#define FFP_MIDI_MODIFY_ATTACKTIME		0x5C
#define FFP_MIDI_MODIFY_MAGNITUDE		0x68 //byte 22 i.e. sustain
#define FFP_MIDI_MODIFY_FADETIME		0x60
#define FFP_MIDI_MODIFY_FADE			0x6C
#define FFP_MIDI_MODIFY_FREQUENCY		0x70
#define FFP_MIDI_MODIFY_PARAM1			0x74
#define FFP_MIDI_MODIFY_PARAM2			0x78
#define FFP_MIDI_MODIFY_COEFFAXIS0		0x48 //roll
#define FFP_MIDI_MODIFY_COEFFAXIS1		0x4C //pitch
#define FFP_MIDI_MODIFY_OFFSETAXIS0		0x50 //roll
#define FFP_MIDI_MODIFY_OFFSETAXIS1		0x54 //pitch

#define FFP_MIDI_MODIFY_DEVICEGAIN		0x7C

#define FFP_SAMPLERATE_DEFAULT 			0x0064 //100Hz

#endif // _FFB_PRO_