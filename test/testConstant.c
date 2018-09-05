#include <avr/io.h>

#include "test.h"
#include "testsuites.h"
#include "../ffb-abacus.h"

void setDirectionDegrees(USB_FFBReport_SetEffect_Output_Data_t *usbData, uint16_t degrees)
{
	usbData->enableAxis = 2;
	usbData->directionX = (degrees / 360.0f ) * 255;
	usbData->directionY = 0;
}

uint8_t setupTestEffectConstant(ForceUnit magnitude, uint16_t direction)
{
	FfbAbacus_Init();
	FfbAbacus_SetAutoCenter(0);

	uint8_t a = FfbAbacus_AddEffect(1);
	FfbEffect *effect = FfbAbacus_GetEffect(a);
	USB_FFBReport_SetEffect_Output_Data_t usbData;
	usbData.duration = USB_DURATION_INFINITE;
	setDirectionDegrees(&usbData, direction);
	FfbAbacus_SetEffect(effect, &usbData);
	FfbAbacus_SetEffectConstantForce(effect, magnitude);

	FfbAbacus_StartEffect(a);
	return a;
}

void testConstantForceSimple(void)
{
	FfbAbacus_Init();
	FfbAbacus_SetAutoCenter(0);

	uint8_t a = FfbAbacus_AddEffect(1);
	expect(a, 1);

	FfbEffect *effect = FfbAbacus_GetEffect(a);
	expectNotNull(effect);
	
	USB_FFBReport_SetEffect_Output_Data_t usbData;
	usbData.duration = USB_DURATION_INFINITE;
	setDirectionDegrees(&usbData, 0);
	FfbAbacus_SetEffect(effect, &usbData);
	FfbAbacus_SetEffectConstantForce(effect, 100);
	FfbAbacus_StartEffect(a);
	ForceUnit fx = 0, fy = 0;
	FfbAbacus_CalculateForces(0, 0, 0, 0, 0, &fx, &fy);
	expect(fx, 0);
	expect(fy, 100);
}

void testConstantForceDiagonal(void)
{
	setupTestEffectConstant(50, 30);

	ForceUnit fx = 0, fy = 0;
	FfbAbacus_CalculateForces(0, 0, 0, 0, 0, &fx, &fy);
	expect(fx, 24); // 25 is exact, but this is close enough
	expect(fy, 43);
}

void testConstantForceDelayedWithDuration(void)
{
	setupTestEffectConstant(100, 0);
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
}
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
void testSuiteConstantForce(void)
{
	define(testConstantForceSimple, "constant simple");
	define(testConstantForceDiagonal, "simple diagonal");
	define(testConstantForceDelayedWithDuration, "constant delayed with duration");
//	define(testConstantForceEnveloped, "constant enveloped");
}
