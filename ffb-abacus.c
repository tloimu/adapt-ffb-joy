#include "ffb-abacus.h"
#include <math.h>
#include <string.h>

#define PI 3.1415f

// -----------------------------------
// Overall Settings
// -----------------------------------

const ForceUnit MAX_FORCE = 255;
const uint8_t INVALID_EFFECT = 0;
const TimeUnit DURATION_INFINITE = -1;

// -----------------------------------
// Internal utilities and globals
// -----------------------------------

#define MAX_EFFECTS 20

Effect gEffects[MAX_EFFECTS];
float gEffectGain;

typedef struct
{
    ForceUnit x;
    ForceUnit y;
} ForceVector;

uint8_t findFreeEffectSlot(void)
{
    for (uint8_t i = 1; i < MAX_EFFECTS; i++)
    {
        if (gEffects[i].func == 0)
            return i;
    }
    return INVALID_EFFECT;
}

void calcRemainingPhaseTime(Effect *effect, TimeUnit timeDelta)
{
    // ???? TODO:
}

void calcEnvelope(Effect *effect, ForceVector *ioForce)
{
    // ???? TODO:
}

void calcEffectLocalTime(Effect *effect, TimeUnit dt)
{
    // Advance the effect's local time with the given dt and stop it if duration has passed
    effect->localTime += dt;

    if (effect->localTime > (effect->delay + effect->duration))
        effect->enabled = 0;
}

// -------------------------------------------
// Implement the force calculation functions
// -------------------------------------------

void calcEffectConstant(Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy);
void calcEffectConstant(
    Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy)
    {
        ForceVector force;
        force.x = 0;
        force.y = 0;

        force.x = effect->magnitude * effect->directionX;
        force.y = effect->magnitude * effect->directionY;

        calcEnvelope(effect, &force);
        *outFx = force.x;
        *outFy = force.y;
    }

void calcEffectSine(Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy);
void calcEffectSine(
    Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy)
    {
        // ???? TODO:
        calcRemainingPhaseTime(effect, timeDelta);
    }

void calcEffectSquare(Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy);
void calcEffectSquare(
    Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy)
    {
        // ???? TODO:
        calcRemainingPhaseTime(effect, timeDelta);
    }

void calcEffectTriangle(Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy);
void calcEffectTriangle(
    Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy)
    {
        // ???? TODO:
        calcRemainingPhaseTime(effect, timeDelta);
    }

void calcEffectFriction(Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy);
void calcEffectFriction(
    Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy)
    {
        // ???? TODO: use axis accelection as metric
    }

void calcEffectInertia(Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy);
void calcEffectInertia(
    Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy)
    {
        // ???? TODO:
    }

void trimForces(ForceUnit *fx, ForceUnit *fy)
{
    if (*fx > 255)
        *fx = 255;
    if (*fy > 255)
        *fy = 255;
    if (*fx < -255)
        *fx = -255;
    if (*fy < -255)
        *fy = -255;
}

void calcEffectSpring(Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy);
void calcEffectSpring(
    Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy)
    {
        ForceVector force;
        force.x = 0;
        force.y = 0;

        // force = Negative Coefficient * (x - (CP OffsetX – Dead BandX))
        // force = Positive Coefficient * (q - (CP OffsetX + Dead BandX))
        //
        // offset is -10, dead band = 5
        //
        // ---------------B----C----B----0----------
        // ----neg coeff--|         |--pos coeff----

        force.x = -(x/2) * ( effect->coeffX / MAX_FORCE);
        force.y = -(y/2) * ( effect->coeffY / MAX_FORCE);
        trimForces(&force.x, &force.y);

        calcEnvelope(effect, &force);
        *outFx = force.x;
        *outFy = force.y;
    }

void calcEffectSawtoothDown(Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy);
void calcEffectSawtoothDown(
    Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy)
    {
        // ???? TODO:
    }

void calcEffectSawtoothUp(Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy);
void calcEffectSawtoothUp(
    Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy)
    {
        // ???? TODO:
    }

void calcEffectRamp(Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy);
void calcEffectRamp(
    Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy)
    {
        // ???? TODO:
    }

void calcEffectDamper(Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy);
void calcEffectDamper(
    Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy)
    {
        // ???? TODO: use axis velocity as metric
    }

void calcEffectCustom(Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy);
void calcEffectCustom(
    Effect* effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit* outFx, ForceUnit* outFy)
    {
        // ???? TODO:
    }

// -----------------------------------
// Implement the public interface
// -----------------------------------

void initEffectAbacus(void)
{
    gEffectGain = 1.0f;

    removeAllEffects();
}

uint8_t getMaxEffects(void)
{
    return MAX_EFFECTS;
}

uint8_t addEffect(void)
{
    uint8_t id = findFreeEffectSlot();
    if (id != INVALID_EFFECT)
        {
            Effect* effect = &gEffects[id];
            memset(effect, 0, sizeof(Effect));
            effect->duration = DURATION_INFINITE;
        }
    return id;
}

const EffectFunc UsbEffectTypeCalculatorFunc[] = {
    0, // Invalid
    calcEffectConstant,	// Constant, 
    calcEffectRamp, 	// Ramp
    calcEffectSquare, 	// Square
    calcEffectSine, 	// Sine
    calcEffectTriangle,	// Triangle
    calcEffectSawtoothDown,	// SawtoothDown
    calcEffectSawtoothUp,	// SawtoothUp
    calcEffectSpring,	// Spring
    calcEffectDamper,	// Damper
    calcEffectInertia,	// Inertia
    calcEffectFriction,	// Friction
    calcEffectCustom 	// Custom ?
};

uint8_t addEffectOfType(uint8_t effectType)
{
    if (effectType > 12)
        return INVALID_EFFECT;

    uint8_t id = addEffect();
    if (id != INVALID_EFFECT)
        {
            gEffects[id].func = UsbEffectTypeCalculatorFunc[effectType];
        }
    return id;
}

void removeEffect(uint8_t handle)
{
    if (handle < MAX_EFFECTS)
        gEffects[handle].func = 0;
}

void removeAllEffects(void)
{
    for (int i = 0; i < MAX_EFFECTS; i++)
    {
        memset(&gEffects[i], 0, sizeof(Effect));
    }
}

void startEffect(uint8_t handle)
{
    if (handle < MAX_EFFECTS)
    {
        gEffects[handle].enabled = 1;
        gEffects[handle].localTime = 0;
    }
}

void stopEffect(uint8_t handle)
{
    if (handle < MAX_EFFECTS)
        gEffects[handle].enabled = 0;
}

void stopAllEffects(void)
{
    for (int i = 0; i < MAX_EFFECTS; i++)
    {
        gEffects[i].enabled = 0;
    }
}

Effect* getEffect(uint8_t handle)
{
    return &gEffects[handle];
}


void setEffect(Effect *effect, USB_FFBReport_SetEffect_Output_Data_t *data)
{
    /*
	uint8_t	effectType;	// 1..12 (effect usages: 26,27,30,31,32,33,34,40,41,42,43,28)
	uint16_t	duration; // 0..32767 ms
	uint16_t	triggerRepeatInterval; // 0..32767 ms
	uint16_t	samplePeriod;	// 0..32767 ms
	uint8_t	gain;	// 0..255	 (physical 0..10000)
	uint8_t	triggerButton;	// button ID (0..8)
	uint8_t	enableAxis; // bits: 0=X, 1=Y, 2=DirectionEnable
	uint8_t	directionX;	// angle (0=0 .. 255=360deg)
	uint8_t	directionY;	// angle (0=0 .. 255=360deg)
//	uint16_t	startDelay;	// 0..32767 ms
    */

    effect->duration = data->duration;
//    effect->delay = data->startDelay;
	if (data->enableAxis == 2)
	{
		float dirRad = PI * (data->directionX / 128.0f);
		effect->directionX = sin(dirRad);
		effect->directionY = cos(dirRad);
	}
	else if (data->enableAxis == 1)
	{
		effect->directionX = data->directionX / 255;
		effect->directionY = 0;
	}
	else
	{
		effect->directionX = 0;
		effect->directionY = data->directionY / 255;
	}
}

void setEffectEnvelope(Effect *effect, USB_FFBReport_SetEnvelope_Output_Data_t *data)
{
    // ????? TODO:
}

void setEffectCondition(Effect *effect, USB_FFBReport_SetCondition_Output_Data_t *data)
{
    if (data->parameterBlockOffset == 0)
    {
        effect->x = data->cpOffset;
        effect->coeffX = data->positiveCoefficient;
        //effect->deadBandX = data->deadBand;
    }
    else
    {
        effect->y = data->cpOffset;
        effect->coeffY = data->positiveCoefficient;
        //effect->deadBandY = data->deadBand;
    }

    // ???? TODO: If only one condition report is given, the Direction must be applied instead of axes
}

void setEffectPeriodic(Effect *effect, USB_FFBReport_SetPeriodic_Output_Data_t *data)
{
    // ???? TODO:
}

void setEffectConstantForce(Effect *effect, ForceUnit magnitude)
{
    effect->magnitude = magnitude;
}

void setEffectRampForce(Effect *effect, USB_FFBReport_SetRampForce_Output_Data_t *data)
{
    // ???? TODO:
}

void setEffectCustomForce(Effect *effect, USB_FFBReport_SetCustomForce_Output_Data_t *data)
{
    // ????? TODO:
}

void setEffectCustomForceData(Effect *effect, USB_FFBReport_SetCustomForceData_Output_Data_t *data)
{
    // ????? TODO: Interleaved samples for axis
    // E.g. 6 samples:
    // 1,2,3,4,5,6 => sample 1: x=1,y=2, sample 2: x=3,y=4...    
}

void setEffectDownloadForceSample(Effect *effect, USB_FFBReport_SetDownloadForceSample_Output_Data_t *data)
{
    // Never actually downloaded - The report definition just describes the format of the sample data
}


void setEffectsGain(float gain)
{
    gEffectGain = gain;
}


// Calculate the output forces based on all effects in the effect stack
void calculateForces(PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit dt, ForceUnit* outFx, ForceUnit* outFy)
{
    for (uint8_t i = 0; i < MAX_EFFECTS; i++)
    {
        Effect *e = &gEffects[i];
        if (e->func && e->enabled)
        {
			if (e->localTime >= e->delay)
				{
				ForceUnit afx = 0;
				ForceUnit afy = 0;
				gEffects[i].func(e, x, y, dx, dy, dt, &afx, &afy);
				(*outFx) += afx; // gEffectGain * afx;
				(*outFy) += afy; // gEffectGain * afy;
				}
            calcEffectLocalTime(e, dt);
        }
    }
}
