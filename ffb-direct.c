#include "ffb-direct.h"
#include "ffb-abacus.h"

#include "ffb.h"

// --------------------------------------------------
// L298N Driver Controller
// --------------------------------------------------

int16_t gLastFx;
int16_t gLastFy;

void L298N_GetLatestMotors(int16_t *fx, int16_t *fy)
{
    *fx = gLastFx;
    *fy = gLastFy;
}

void L298N_Init(void)
{
    // Setup control ports
    DDRF |= 0b11110000; // Direction control ports

    DDRC |= 0b11000000; // PWM ports
    DDRD |= 0b00100000; // PWM ports
    DDRC = 0xFF;
    DDRD = 0xFF;
    TCCR4A |= (1 << WGM00) | (1 << WGM01) | (1 << COM3A1);
    TCCR4B |= (1 << CS00);
    TCCR4C = (1 << COM4D1) | (1 << PWM4D) | (1 << COM4A1) | (1 << PWM4A);

    // Initial setup
    PORTF &= ~0b11110000; // All off now
    OCR4D = 0;            // X
    OCR4A = 0;            // Y

    gLastFx = 0;
    gLastFy = 0;

    /* Test sequence

    while(1)
    {
        for (int i=0; i<200;i++)
        {
            WaitMs(1);
            L298N_SetMotors(0, i);
        }
        WaitMs(500);
        L298N_SetMotors(200, -200);
        WaitMs(500);
        L298N_SetMotors(-200, -200);
        WaitMs(500);
        L298N_SetMotors(-200, 170);
    }
*/
}

/*
TCCRn
6,3: PWM mode - 11=fast, 00=normal

*/

/*
En=1 Input1=0 Input2=1 ------>Rotate Right
En=1 Input1=1 Input2=0 ------>Rotate Left
En=1 Input1=0 Input2=0 ------>Fast Stop
En=1 Input1=1 Input2=1 ------>Free Running
En=0 Input1=X Input2=X ------>Disable Driver
*/

void L298N_SetMotors(int16_t fx, int16_t fy)
{
    // Send directions for motors
    uint8_t afx = (uint8_t)fx;
    if (fx < 0)
    {
        afx = (uint8_t)-fx;
        PORTF |= 0b10000000;
        PORTF &= ~0b01000000;
    }
    else
    {
        PORTF |= 0b01000000;
        PORTF &= ~0b10000000;
    }

    uint8_t afy = (uint8_t)fy;
    if (fy < 0)
    {
        afy = (uint8_t)-fy;
        PORTF |= 0b00010000;
        PORTF &= ~0b00100000;
    }
    else
    {
        PORTF |= 0b00100000;
        PORTF &= ~0b00010000;
    }

    // Set PWMs for speeds

    // ???? TODO: Put force response curves here to make the effects better
    // ???? TODO: Also, consider using FULL STOP (e.g. IN1=1, IN2=1 for Y) for Square Waves (or other quick changes)

    OCR4D = (2 * afx / 3) + 85;
    OCR4A = (2 * afy / 3) + 85;

    gLastFx = afx;
    gLastFy = afy;
}

// --------------------------------------------------
// FFB Direct Driver
//
// Combines the FFB Acabus and L298N motor drivers
// --------------------------------------------------

volatile uint8_t gAbacusEnabled;
PositionUnit gLastPosX;
PositionUnit gLastPosY;

void FfbDirect_InitializeDriver(void)
{
    FfbAbacus_Init();
    L298N_Init();
    gAbacusEnabled = 1;
    gLastPosX = 0;
    gLastPosY = 0;
}

void FfbDirect_SetAutoCenter(uint8_t enable)
{
    FfbAbacus_SetAutoCenter(enable);
}

void FfbDirect_StartEffect(uint8_t id)
{
    FfbAbacus_StartEffect(id);
}

void FfbDirect_StopEffect(uint8_t id)
{
    FfbAbacus_StopEffect(id);
}

void FfbDirect_StopAllEffects(void)
{
    FfbAbacus_StopAllEffects();
}

void FfbDirect_FreeEffect(uint8_t id)
{
    FfbAbacus_RemoveEffect(id);
}

void FfbDirect_FreeAllEffects(void)
{
    FfbAbacus_RemoveAllEffects();
}

void FfbDirect_SetEnvelope(USB_FFBReport_SetEnvelope_Output_Data_t *data)
{
    uint8_t id = data->effectBlockIndex;
    FfbEffect *effect = FfbAbacus_GetEffect(id);
    if (effect)
        FfbAbacus_SetEffectEnvelope(effect, data);
}

void FfbDirect_SetCondition(USB_FFBReport_SetCondition_Output_Data_t *data)
{
    uint8_t id = data->effectBlockIndex;
    FfbEffect *effect = FfbAbacus_GetEffect(id);
    if (effect)
        FfbAbacus_SetEffectCondition(effect, data);
}

void FfbDirect_SetPeriodic(USB_FFBReport_SetPeriodic_Output_Data_t *data)
{
    uint8_t id = data->effectBlockIndex;
    FfbEffect *effect = FfbAbacus_GetEffect(id);
    if (effect)
        FfbAbacus_SetEffectPeriodic(effect, data);
}

void FfbDirect_SetConstantForce(USB_FFBReport_SetConstantForce_Output_Data_t *data)
{
    uint8_t id = data->effectBlockIndex;
    FfbEffect *effect = FfbAbacus_GetEffect(id);
    if (effect)
        FfbAbacus_SetEffectConstantForce(effect, data->magnitude);
}

void FfbDirect_SetRampForce(USB_FFBReport_SetRampForce_Output_Data_t *data)
{
    uint8_t id = data->effectBlockIndex;
    FfbEffect *effect = FfbAbacus_GetEffect(id);
    if (effect)
        FfbAbacus_SetEffectRampForce(effect, data);
}

int FfbDirect_SetEffect(USB_FFBReport_SetEffect_Output_Data_t *data)
{
    uint8_t id = data->effectBlockIndex;
    FfbEffect *effect = FfbAbacus_GetEffect(id);
    if (effect)
        FfbAbacus_SetEffect(effect, data);

    return 0; // ????
}

uint8_t FfbDirect_CreateNewEffect(uint8_t effectType, uint16_t byteCount)
{
    return FfbAbacus_AddEffect(effectType);
}

uint8_t FfbDirect_GetMaxSimultaneousEffects(void)
{
    return FfbAbacus_GetMaxEffects();
}

void FfbDirect_MaintainEffects(int16_t x, int16_t y, uint16_t dt)
{
    if (gAbacusEnabled)
    {
        // Call the effect calculation and actuate motors
        ForceUnit fx = 0;
        ForceUnit fy = 0;
        FfbAbacus_CalculateForces(x, y, x - gLastPosX, y - gLastPosY, dt, &fx, &fy);
        gLastPosX = x;
        gLastPosY = y;
        L298N_SetMotors(fx, fy);
    }
}

void FfbDirect_SendRawData(uint8_t *data, uint16_t len)
{
    // Allow direct commanding of X and Y motors
    if (len > 3)
    {
        gAbacusEnabled = 0;
        int16_t fx = (int16_t)data[0];
        int16_t fy = (int16_t)data[2];
        L298N_SetMotors(fx, fy);
    }
    else
        gAbacusEnabled = 1;
}
