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

struct Effect_t;

typedef void (*EffectFunc)(
    struct Effect_t*,
    PositionUnit, PositionUnit,
    PositionUnit, PositionUnit,
    TimeUnit,
    ForceUnit*, ForceUnit*);

struct Effect_t
{
    EffectFunc func;

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

typedef struct Effect_t Effect;

// Call this once in the app
void initEffectAbacus(void);
uint8_t getMaxEffects(void);

// Each return the handle to the effect or INVALID_EFFECT if no free slots are available.
/* Effect Type:
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

uint8_t addEffect(void);
uint8_t addEffectOfType(uint8_t effectType);

void removeEffect(uint8_t handle);
void startEffect(uint8_t handle);
void stopEffect(uint8_t handle);
void stopAllEffects(void);
void removeAllEffects(void);
Effect* getEffect(uint8_t handle);

// Methods for setting given effect parameters in the effect stack.
void setEffect(Effect *effect, USB_FFBReport_SetEffect_Output_Data_t *data);
void setEffectEnvelope(Effect *effect, USB_FFBReport_SetEnvelope_Output_Data_t *data);
void setEffectCondition(Effect *effect, USB_FFBReport_SetCondition_Output_Data_t *data);
void setEffectPeriodic(Effect *effect, USB_FFBReport_SetPeriodic_Output_Data_t *data);
void setEffectConstantForce(Effect *effect, ForceUnit magnitude);
void setEffectRampForce(Effect *effect, USB_FFBReport_SetRampForce_Output_Data_t *data);
void setEffectCustomForce(Effect *effect, USB_FFBReport_SetCustomForce_Output_Data_t *data);
void setEffectCustomForceData(Effect *effect, USB_FFBReport_SetCustomForceData_Output_Data_t *data);
void setEffectDownloadForceSample(Effect *effect, USB_FFBReport_SetDownloadForceSample_Output_Data_t *data);

// Set global effects gain
void setEffectsGain(float gain);

// Calculate the output forces based on all effects in the effect stack
void calculateForces(PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit dt, ForceUnit* outFx, ForceUnit* outFy);

#endif // _FFB_ABACUS_
