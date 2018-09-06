#include <avr/io.h>

#include "test.h"
#include "testUtils.h"
#include "testsuites.h"
#include "../ffb-abacus.h"
#include "../ffb-abacus-calc.h"
#include <string.h>

FfbEffect* makeEffect(uint8_t effectType);
FfbEffect* makeEffect(uint8_t effectType)
{
	FfbAbacus_Init();
	FfbAbacus_SetAutoCenter(0);

	uint8_t id = FfbAbacus_AddEffect(effectType);
	FfbEffect *effect = FfbAbacus_GetEffect(id);
	USB_FFBReport_SetEffect_Output_Data_t usbData;
	memset(&usbData, sizeof(USB_FFBReport_SetEffect_Output_Data_t), 0);
	usbData.duration = USB_DURATION_INFINITE;
	usbData.enableAxis = 2;
	usbData.directionX = 0;
	usbData.effectType = effectType;
	FfbAbacus_SetEffect(effect, &usbData);

	FfbAbacus_StartEffect(id);

	return effect;
}

void testCalcForces(void)
{
	FfbEffect *effect = makeEffect(FfbEffectType_Constant);
	effect->directionX = 0.0f;
	effect->directionY = 1.0f;
	effect->magnitude = 100;

	ForceUnit fx = 0, fy = 0;
	FfbAbacus_CalculateForces(0, 0, 0, 0, 1000, &fx, &fy);
	expect(fx, 0);
	expect(fy, 100);

	fx = 0, fy = 0;
	FfbAbacus_CalculateForces(0, 0, 0, 0, 1000, &fx, &fy);
	expect(fx, 0);
	expect(fy, 100);
}

void testCalcConstantForce(void)
{
	FfbEffect *effect = makeEffect(FfbEffectType_Constant);
	effect->directionX = 0.3f;
	effect->directionY = 0.7f;
	effect->magnitude = 100;

	ForceUnit fx = 0, fy = 0;
	calcEffectConstant(effect, 20, 20, 20, 20, 20, &fx, &fy);
	expect(fx, 30);
	expect(fy, 70);
}

void testCalcSineForceSimple(void)
{
	FfbEffect *effect = makeEffect(FfbEffectType_Sine);
	effect->directionX = 0.0f;
	effect->directionY = 1.0f;
	effect->magnitude = 100;
	effect->frequency = 10;

	ForceUnit fx = 0, fy = 0;
	effect->localTime = 0;
	calcEffectSine(effect, 20, 20, 20, 20, 20, &fx, &fy);
	expect(fx, 0);
	expect(fy, 0);

	effect->localTime = 25;
	calcEffectSine(effect, 20, 20, 20, 20, 20, &fx, &fy);
	expect(fx, 0);
	expect(fy, 99); // close enough

	effect->localTime = 50;
	calcEffectSine(effect, 20, 20, 20, 20, 20, &fx, &fy);
	expect(fx, 0);
	expect(fy, 0);

	effect->localTime = 75;
	calcEffectSine(effect, 20, 20, 20, 20, 20, &fx, &fy);
	expect(fx, 0);
	expect(fy, -100);

	effect->localTime = 125;
	calcEffectSine(effect, 20, 20, 20, 20, 20, &fx, &fy);
	expect(fx, 0);
	expect(fy, 99);
}

void testCalcSineForceDiagonal(void)
{
	FfbEffect *effect = makeEffect(FfbEffectType_Sine);
	effect->directionX = 1.0f;
	effect->directionY = 1.0f;
	effect->magnitude = 100;
	effect->frequency = 10;

	ForceUnit fx = 0, fy = 0;
	effect->localTime = 0;
	calcEffectSine(effect, 20, 20, 20, 20, 20, &fx, &fy);
	expect(fx, 0);
	expect(fy, 0);

	effect->localTime = 25;
	calcEffectSine(effect, 20, 20, 20, 20, 20, &fx, &fy);
	expect(fx, 99); // close enough
	expect(fy, 99); // close enough

	effect->localTime = 50;
	calcEffectSine(effect, 20, 20, 20, 20, 20, &fx, &fy);
	expect(fx, 0);
	expect(fy, 0);

	effect->localTime = 75;
	calcEffectSine(effect, 20, 20, 20, 20, 20, &fx, &fy);
	expect(fx, -100);
	expect(fy, -100);
}

/*
void testConstantForceDiagonal(void)
{
	USB_FFBReport_SetEffect_Output_Data_t usbData;
	FfbEffect *effect = initWithTestEffect(&usbData, 50, 30);

	FfbAbacus_SetEffect(effect, &usbData);
	FfbAbacus_StartEffect(1);
	ForceUnit fx = 0, fy = 0;
	FfbAbacus_CalculateForces(0, 0, 0, 0, 0, &fx, &fy);
	expect(fx, 24); // 25 is exact, but this is close enough
	expect(fy, 43);
}

void testConstantForceDelayedWithDuration(void)
{
	USB_FFBReport_SetEffect_Output_Data_t usbData;
	FfbEffect *effect = initWithTestEffect(&usbData, 100, 0);	
	/*
	FfbAbacus_SetEffectTiming(a, 5, 30);
	FfbAbacus_StartEffect(a);
	{
		ForceUnit fx = 0, fy = 0;
		FfbAbacus_CalculateForces(0, 0, 0, 0, 4, &fx, &fy);
		expect(fx, 0);
		expect(fy, 0);
	}

	{
		ForceUnit fx = 0, fy = 0;
		FfbAbacus_CalculateForces(0, 0, 0, 0, 1, &fx, &fy);
		expect(fx, 0);
		expect(fy, 0);
	}

	{
		ForceUnit fx = 0, fy = 0;
		FfbAbacus_CalculateForces(0, 0, 0, 0, 1, &fx, &fy);
		expect(fx, 0);
		expect(fy, 100);
	}

	{
		ForceUnit fx = 0, fy = 0;
		FfbAbacus_CalculateForces(0, 0, 0, 0, 30, &fx, &fy);
		expect(fx, 0);
		expect(fy, 100);
	}

	{
		ForceUnit fx = 0, fy = 0;
		FfbAbacus_CalculateForces(0, 0, 0, 0, 1, &fx, &fy);
		expect(fx, 0);
		expect(fy, 0);
	}*/
//}
/*
void testConstantForceEnveloped(void)
{
	FfbAbacus_Init();

	uint8_t a = FfbAbacus_AdEffect();
	expect(a, 0);

	FfbAbacus_SetEffectConstant(a, 0, 100);
	FfbAbacus_SetEffectEnvelope(a, 10, 90, 20, 100);
	FfbAbacus_StartEffect(a);
	{
		ForceUnit fx = 0, fy = 0;
		FfbAbacus_CalculateForces(0, 0, 0, 0, 4, &fx, &fy);
		expect(fx, 0);
		expect(fy, 100);
	}

	{
		ForceUnit fx = 0, fy = 0;
		FfbAbacus_CalculateForces(0, 0, 0, 0, 1, &fx, &fy);
		expect(fx, 0);
		expect(fy, 0);
	}

	{
		ForceUnit fx = 0, fy = 0;
		FfbAbacus_CalculateForces(0, 0, 0, 0, 1, &fx, &fy);
		expect(fx, 0);
		expect(fy, 100);
	}

	{
		ForceUnit fx = 0, fy = 0;
		FfbAbacus_CalculateForces(0, 0, 0, 0, 30, &fx, &fy);
		expect(fx, 0);
		expect(fy, 100);
	}

	{
		ForceUnit fx = 0, fy = 0;
		FfbAbacus_CalculateForces(0, 0, 0, 0, 1, &fx, &fy);
		expect(fx, 0);
		expect(fy, 0);
	}
}
*/

void testSuiteCalcForces(void)
{
	define(testCalcForces, "FfbAbacus_CalculateForces");
	define(testCalcConstantForce, "constant force simple");
	define(testCalcSineForceSimple, "sine force simple");
	define(testCalcSineForceDiagonal, "sine force diagonal");
/*
	define(testConstantForceDiagonal, "simple diagonal");
	define(testConstantForceDelayedWithDuration, "constant delayed with duration");
	//	define(testConstantForceEnveloped, "constant enveloped");
*/
}
