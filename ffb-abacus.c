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
const int32_t COEFFICIENT_DIVIDER = 100;
const Frequency FREQUENCY_DIVIDER = 100;

// -----------------------------------
// Internal utilities and globals
// -----------------------------------

#define MAX_EFFECTS 20

FfbEffect gEffects[MAX_EFFECTS];
Coefficient gDeviceEffectGain;

volatile uint8_t gAutoCenterEnabled;

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

void calcRemainingPhaseTime(FfbEffect *effect, TimeUnit timeDelta)
{
    // ???? TODO:
}

void calcEnvelope(FfbEffect *effect, ForceVector *ioForce)
{
    // ???? TODO:
}

void calcEffectLocalTime(FfbEffect *effect, TimeUnit dt)
{
    // Advance the effect's local time with the given dt and stop it if duration has passed
    effect->localTime += dt;

    if (effect->duration != DURATION_INFINITE && effect->localTime > (effect->delay + effect->duration))
        effect->enabled = 0;
}

// -------------------------------------------
// Implement the force calculation functions
// -------------------------------------------

void calcEffectConstant(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectConstant(
    FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy)
{
    ForceVector force;
    force.x = 0;
    force.y = 0;

    force.x = effect->magnitude * effect->directionX / COEFFICIENT_DIVIDER;
    force.y = effect->magnitude * effect->directionY / COEFFICIENT_DIVIDER;

    calcEnvelope(effect, &force);
    *outFx = force.x;
    *outFy = force.y;
}

void calcEffectSine(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectSine(
    FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy)
{
    // ???? TODO:
    calcRemainingPhaseTime(effect, timeDelta);
}

void calcEffectSquare(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectSquare(
    FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy)
{
    // ???? TODO:
    calcRemainingPhaseTime(effect, timeDelta);
}

void calcEffectTriangle(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectTriangle(
    FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy)
{
    // ???? TODO:
    calcRemainingPhaseTime(effect, timeDelta);
}

void calcEffectFriction(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectFriction(
    FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy)
{
    // ???? TODO: use axis acceleration as metric
}

void calcEffectInertia(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectInertia(
    FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy)
{
    // ???? TODO:
}

void trimForces(ForceUnit *fx, ForceUnit *fy)
{
    if (*fx > MAX_FORCE)
        *fx = MAX_FORCE;
    else if (*fx < -MAX_FORCE)
        *fx = -MAX_FORCE;
    if (*fy > MAX_FORCE)
        *fy = MAX_FORCE;
    else if (*fy < -MAX_FORCE)
        *fy = -MAX_FORCE;
}

void calcEffectSpring(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectSpring(
    FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy)
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

    force.x = -(x / 2) * (effect->coeffX / COEFFICIENT_DIVIDER / MAX_FORCE);
    force.y = -(y / 2) * (effect->coeffY / COEFFICIENT_DIVIDER / MAX_FORCE);
    trimForces(&force.x, &force.y);

    calcEnvelope(effect, &force);
    *outFx = force.x;
    *outFy = force.y;
}

void calcEffectSawtoothDown(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectSawtoothDown(
    FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy)
{
    // ???? TODO:
}

void calcEffectSawtoothUp(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectSawtoothUp(
    FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy)
{
    // ???? TODO:
}

void calcEffectRamp(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectRamp(
    FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy)
{
    // ???? TODO:
}

void calcEffectDamper(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectDamper(
    FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy)
{
    // ???? TODO: use axis velocity as metric
}

void calcEffectCustom(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectCustom(
    FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy)
{
    // ???? TODO:
}

void calcEffectAutoCenter(PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectAutoCenter(PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, ForceUnit *outFx, ForceUnit *outFy)
{
    ForceUnit fx = -(x / 2);
    ForceUnit fy = -(y / 2);
	trimForces(&fx, fy);
    (*outFx) += fx;
    (*outFy) += fy;
}

// -----------------------------------
// Implement the public interface
// -----------------------------------

void FfbAcabus_Init(void)
{
    gDeviceEffectGain = COEFFICIENT_DIVIDER; // = 1.0
    gAutoCenterEnabled = 1;

    FfbAcabus_RemoveAllEffects();
}

uint8_t FfbAcabus_GetMaxEffects(void)
{
    return MAX_EFFECTS;
}

const FfbEffectFunc UsbEffectTypeCalculatorFunc[] = {
    0,                      // Invalid
    calcEffectConstant,     // Constant,
    calcEffectRamp,         // Ramp
    calcEffectSquare,       // Square
    calcEffectSine,         // Sine
    calcEffectTriangle,     // Triangle
    calcEffectSawtoothDown, // SawtoothDown
    calcEffectSawtoothUp,   // SawtoothUp
    calcEffectSpring,       // Spring
    calcEffectDamper,       // Damper
    calcEffectInertia,      // Inertia
    calcEffectFriction,     // Friction
    calcEffectCustom        // Custom
};

uint8_t FfbAcabus_AddEffect(uint8_t effectType)
{
    if (effectType > 12)
        return INVALID_EFFECT;

    uint8_t id = findFreeEffectSlot();
    if (id != INVALID_EFFECT)
    {
        FfbEffect *effect = &gEffects[id];
        memset(effect, 0, sizeof(FfbEffect));
        effect->duration = DURATION_INFINITE;
        if (effectType > 0)
            gEffects[id].func = UsbEffectTypeCalculatorFunc[effectType];
    }
    return id;
}

void FfbAcabus_RemoveEffect(uint8_t handle)
{
    if (handle < MAX_EFFECTS)
        gEffects[handle].func = 0;
}

void FfbAcabus_RemoveAllEffects(void)
{
    for (int i = 0; i < MAX_EFFECTS; i++)
    {
        memset(&gEffects[i], 0, sizeof(FfbEffect));
    }
}

void FfbAcabus_StartEffect(uint8_t handle)
{
    if (handle < MAX_EFFECTS)
    {
        gEffects[handle].enabled = 1;
        gEffects[handle].localTime = 0;
    }
}

void FfbAcabus_StopEffect(uint8_t handle)
{
    if (handle < MAX_EFFECTS)
        gEffects[handle].enabled = 0;
}

void FfbAcabus_StopAllEffects(void)
{
    for (int i = 0; i < MAX_EFFECTS; i++)
    {
        gEffects[i].enabled = 0;
    }
}

FfbEffect *FfbAcabus_GetEffect(uint8_t handle)
{
    return &gEffects[handle];
}

void FfbAcabus_SetAutoCenter(uint8_t enable)
{
    gAutoCenterEnabled = enable;
}

void FfbAcabus_SetEffect(FfbEffect *effect, USB_FFBReport_SetEffect_Output_Data_t *data)
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

	if (data->duration == USB_DURATION_INFINITE)
	    effect->duration = DURATION_INFINITE;
	else
	    effect->duration = data->duration;
    //    effect->delay = data->startDelay;
    if (data->enableAxis == 2)
    {
        float dirRad = PI * (data->directionX / 128.0f);
        effect->directionX = ((float) COEFFICIENT_DIVIDER * sin(dirRad));
        effect->directionY = ((float) COEFFICIENT_DIVIDER * cos(dirRad));
    }
    else if (data->enableAxis == 1)
    {
        effect->directionX = data->directionX * COEFFICIENT_DIVIDER;
        effect->directionY = 0;
    }
    else
    {
        effect->directionX = 0;
        effect->directionY = data->directionY * COEFFICIENT_DIVIDER;
    }
}

void FfbAcabus_SetEffectEnvelope(FfbEffect *effect, USB_FFBReport_SetEnvelope_Output_Data_t *data)
{
    // ????? TODO:
}

void FfbAcabus_SetEffectCondition(FfbEffect *effect, USB_FFBReport_SetCondition_Output_Data_t *data)
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

void FfbAcabus_SetEffectPeriodic(FfbEffect *effect, USB_FFBReport_SetPeriodic_Output_Data_t *data)
{
    // ???? TODO:
}

void FfbAcabus_SetEffectConstantForce(FfbEffect *effect, ForceUnit magnitude)
{
    effect->magnitude = magnitude;
}

void FfbAcabus_SetEffectRampForce(FfbEffect *effect, USB_FFBReport_SetRampForce_Output_Data_t *data)
{
    // ???? TODO:
}

void FfbAcabus_SetEffectCustomForce(FfbEffect *effect, USB_FFBReport_SetCustomForce_Output_Data_t *data)
{
    // ????? TODO:
}

void FfbAcabus_SetEffectCustomForceData(FfbEffect *effect, USB_FFBReport_SetCustomForceData_Output_Data_t *data)
{
    // ????? TODO: Interleaved samples for axis
    // E.g. 6 samples:
    // 1,2,3,4,5,6 => sample 1: x=1,y=2, sample 2: x=3,y=4...
}

void FfbAcabus_SetEffectDownloadForceSample(FfbEffect *effect, USB_FFBReport_SetDownloadForceSample_Output_Data_t *data)
{
    // Never actually downloaded - The report definition just describes the format of the sample data
}

void FfbAcabus_SetEffectsGain(float gain)
{
    gDeviceEffectGain = gain * COEFFICIENT_DIVIDER;
}

// Calculate the output forces based on all effects in the effect stack
void FfbAcabus_CalculateForces(PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit dt, ForceUnit *outFx, ForceUnit *outFy)
{
    ForceUnit fx = 0;
    ForceUnit fy = 0;

    if (gAutoCenterEnabled)
    {
        calcEffectAutoCenter(x, y, dx, dy, outFx, outFy);
    }
    else
    {
        for (uint8_t i = 0; i < MAX_EFFECTS; i++)
        {
            FfbEffect *e = &gEffects[i];
            if (e->func && e->enabled)
            {
                if (e->localTime >= e->delay)
                {
	                gEffects[i].func(e, x, y, dx, dy, dt, &fx, &fy);
					(*outFx) += fx;
					(*outFy) += fy;
                }
                calcEffectLocalTime(e, dt);
            }
        }
    }
	(*outFx) = (*outFx) * gDeviceEffectGain / COEFFICIENT_DIVIDER;
	(*outFy) = (*outFy) * gDeviceEffectGain / COEFFICIENT_DIVIDER;
}
