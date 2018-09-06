#ifndef _FFB_ABACUS_
#define _FFB_ABACUS_

#include <avr/io.h>

#include "ffb.h"

typedef int16_t ForceUnit; // -255..255
typedef int16_t PositionUnit;
typedef uint32_t TimeUnit;

extern const ForceUnit MAX_FORCE;
extern const uint8_t INVALID_EFFECT;
extern const TimeUnit DURATION_INFINITE;

struct FfbEffect_t;

typedef void (*FfbEffectFunc)(
    struct FfbEffect_t*,
    PositionUnit, PositionUnit,
    PositionUnit, PositionUnit,
    TimeUnit,
    ForceUnit*, ForceUnit*);

struct FfbEffect_t
{
    FfbEffectFunc func;

    // Force vector
    float directionX; // -1..1 force coefficient
    float directionY; // -1..1 force coefficient
    ForceUnit magnitude; // 0..MAX_FORCE

    // Timing
    TimeUnit delay; // milliseconds
    TimeUnit duration; // milliseconds

    // Periodic
    float frequency; // Hz
    uint16_t phase; // degrees

    // Envelope
    ForceUnit attackFromForce;
    TimeUnit attackTime;
    ForceUnit decayToForce;
    TimeUnit decayTime; // counted from the end of the duration

    // Sprint center
    PositionUnit x;
    PositionUnit y;
    float coeffX; // spring, friction, dampening
    float coeffY; // spring, friction, dampening

    // Runtime info
    uint8_t enabled;
    TimeUnit localTime;

    void* customDriverData;
};

typedef struct FfbEffect_t FfbEffect;

// Call this once in the app
void FfbAbacus_Init(void);
uint8_t FfbAbacus_GetMaxEffects(void);

// Each return the handle to the effect or INVALID_EFFECT if no free slots are available.
extern const uint8_t FfbEffectType_None;
extern const uint8_t FfbEffectType_Constant;
extern const uint8_t FfbEffectType_Ramp;
extern const uint8_t FfbEffectType_Square;
extern const uint8_t FfbEffectType_Sine;
extern const uint8_t FfbEffectType_Triangle;
extern const uint8_t FfbEffectType_SawtoothUp;
extern const uint8_t FfbEffectType_SawtoothDown;
extern const uint8_t FfbEffectType_Sprint;
extern const uint8_t FfbEffectType_Damper;
extern const uint8_t FfbEffectType_Inertia;
extern const uint8_t FfbEffectType_Friction;
extern const uint8_t FfbEffectType_Custom;

uint8_t FfbAbacus_AddEffect(uint8_t effectType);

void FfbAbacus_RemoveEffect(uint8_t handle);
void FfbAbacus_StartEffect(uint8_t handle);
void FfbAbacus_StopEffect(uint8_t handle);
void FfbAbacus_StopAllEffects(void);
void FfbAbacus_RemoveAllEffects(void);
FfbEffect* FfbAbacus_GetEffect(uint8_t handle);

// Enables (1) or disables (0) the auto center effect
void FfbAbacus_SetAutoCenter(uint8_t enable);

// Methods for setting given effect parameters in the effect stack.
void FfbAbacus_SetEffect(FfbEffect *effect, USB_FFBReport_SetEffect_Output_Data_t *data);
void FfbAbacus_SetEffectEnvelope(FfbEffect *effect, USB_FFBReport_SetEnvelope_Output_Data_t *data);
void FfbAbacus_SetEffectCondition(FfbEffect *effect, USB_FFBReport_SetCondition_Output_Data_t *data);
void FfbAbacus_SetEffectPeriodic(FfbEffect *effect, USB_FFBReport_SetPeriodic_Output_Data_t *data);
void FfbAbacus_SetEffectConstantForce(FfbEffect *effect, ForceUnit magnitude);
void FfbAbacus_SetEffectRampForce(FfbEffect *effect, USB_FFBReport_SetRampForce_Output_Data_t *data);
void FfbAbacus_SetEffectCustomForce(FfbEffect *effect, USB_FFBReport_SetCustomForce_Output_Data_t *data);
void FfbAbacus_SetEffectCustomForceData(FfbEffect *effect, USB_FFBReport_SetCustomForceData_Output_Data_t *data);
void FfbAbacus_SetEffectDownloadForceSample(FfbEffect *effect, USB_FFBReport_SetDownloadForceSample_Output_Data_t *data);

// Set global effects gain for the entire device
void FfbAbacus_SetDeviceGain(float gain);

// Calculate the output forces based on all effects in the effect stack
void FfbAbacus_CalculateForces(PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit dt, ForceUnit* outFx, ForceUnit* outFy);

#endif // _FFB_ABACUS_
