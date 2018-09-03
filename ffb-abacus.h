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
void FfbAcabus_Init(void);
uint8_t FfbAcabus_GetMaxEffects(void);

// Each return the handle to the effect or INVALID_EFFECT if no free slots are available.
/* Effect Type:
    null=0, // Not known (yet)
    calcEffectConstant=1,	// Constant, 
    calcEffectRamp, 	// Ramp
    calcEffectSquare, 	// Square
    calcEffectSine, 	// Sine
    calcEffectTriangle,	// Triangle
    CalcEffectSawtoothDown,	// SawtoothDown
    CalcEffectSawtoothUp,	// SawtoothUp
    CalcEffectSpring,	// Spring
    CalcEffectDamper,	// Damper
    CalcEffectInertia,	// Inertia
    CalcEffectFriction,	// Friction
    CalcEffectCustom 	// Custom ?
*/

uint8_t FfbAcabus_AddEffect(uint8_t effectType);

void FfbAcabus_RemoveEffect(uint8_t handle);
void FfbAcabus_StartEffect(uint8_t handle);
void FfbAcabus_StopEffect(uint8_t handle);
void FfbAcabus_StopAllEffects(void);
void FfbAcabus_RemoveAllEffects(void);
FfbEffect* FfbAcabus_GetEffect(uint8_t handle);

// Enables (1) or disables (0) the auto center effect
void FfbAcabus_SetAutoCenter(uint8_t enable);

// Methods for setting given effect parameters in the effect stack.
void FfbAcabus_SetEffect(FfbEffect *effect, USB_FFBReport_SetEffect_Output_Data_t *data);
void FfbAcabus_SetEffectEnvelope(FfbEffect *effect, USB_FFBReport_SetEnvelope_Output_Data_t *data);
void FfbAcabus_SetEffectCondition(FfbEffect *effect, USB_FFBReport_SetCondition_Output_Data_t *data);
void FfbAcabus_SetEffectPeriodic(FfbEffect *effect, USB_FFBReport_SetPeriodic_Output_Data_t *data);
void FfbAcabus_SetEffectConstantForce(FfbEffect *effect, ForceUnit magnitude);
void FfbAcabus_SetEffectRampForce(FfbEffect *effect, USB_FFBReport_SetRampForce_Output_Data_t *data);
void FfbAcabus_SetEffectCustomForce(FfbEffect *effect, USB_FFBReport_SetCustomForce_Output_Data_t *data);
void FfbAcabus_SetEffectCustomForceData(FfbEffect *effect, USB_FFBReport_SetCustomForceData_Output_Data_t *data);
void FfbAcabus_SetEffectDownloadForceSample(FfbEffect *effect, USB_FFBReport_SetDownloadForceSample_Output_Data_t *data);

// Set global effects gain for the entire device
void FfbAcabus_SetDeviceGain(float gain);

// Calculate the output forces based on all effects in the effect stack
void FfbAcabus_CalculateForces(PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit dt, ForceUnit* outFx, ForceUnit* outFy);

#endif // _FFB_ABACUS_
