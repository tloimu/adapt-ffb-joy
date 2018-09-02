
#ifndef _FFB_FfbDirect__
#define _FFB_FfbDirect__

#include <stdint.h>
#include "ffb.h"

void L298N_Init(void);
void L298N_SetMotors(int16_t fx, int16_t fy);
void L298N_GetLatestMotors(int16_t *fx, int16_t *fy);


void FfbDirect_InitializeDriver(void);

void FfbDirect_SetAutoCenter(uint8_t enable);

uint8_t FfbDirect_CreateNewEffect(uint8_t effectType, uint16_t byteCount);
uint8_t FfbDirect_GetMaxSimultaneousEffects(void);

void FfbDirect_StartEffect(uint8_t id);
void FfbDirect_StopEffect(uint8_t id);
void FfbDirect_StopAllEffects(void);
void FfbDirect_FreeEffect(uint8_t id);
void FfbDirect_FreeAllEffects(void);

void FfbDirect_ModifyDuration(uint8_t effectId, uint16_t duration);

void FfbDirect_SetEnvelope(USB_FFBReport_SetEnvelope_Output_Data_t* data);
void FfbDirect_SetCondition(USB_FFBReport_SetCondition_Output_Data_t* data);
void FfbDirect_SetPeriodic(USB_FFBReport_SetPeriodic_Output_Data_t* data);
void FfbDirect_SetConstantForce(USB_FFBReport_SetConstantForce_Output_Data_t* data);
void FfbDirect_SetRampForce(USB_FFBReport_SetRampForce_Output_Data_t* data);
int  FfbDirect_SetEffect(USB_FFBReport_SetEffect_Output_Data_t *data);

void FfbDirect_MaintainEffects(int16_t x, int16_t y, uint16_t dt);
void FfbDirect_SendRawData(uint8_t *data, uint16_t len);

#endif // _FFB_FfbDirect__