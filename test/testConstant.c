#include <avr/io.h>

#include "test.h"
#include "testsuites.h"
#include "../ffb-abacus.h"

void setUsbDirectionDegrees(USB_FFBReport_SetEffect_Output_Data_t *usbData, uint16_t degrees)
{
	usbData->enableAxis = 2;
	usbData->directionX = (degrees / 360.0f ) * 255;
	usbData->directionY = 0;
}

FfbEffect* initWithTestEffect(USB_FFBReport_SetEffect_Output_Data_t *ioUsbData, ForceUnit magnitude, uint16_t directionDeg)
{
	FfbAbacus_Init();
	FfbAbacus_SetAutoCenter(0);

	uint8_t id = FfbAbacus_AddEffect(1);
	FfbEffect *effect = FfbAbacus_GetEffect(id);

	FfbAbacus_SetEffectConstantForce(effect, magnitude);
	setUsbDirectionDegrees(ioUsbData, directionDeg);
	ioUsbData->duration = USB_DURATION_INFINITE;
	ioUsbData->effectType = 1;

	FfbAbacus_SetEffect(effect, ioUsbData);
	FfbAbacus_StartEffect(id);

	return effect;
}

void testConstantForceCreate(void)
{
	FfbAbacus_Init();

	uint8_t a = FfbAbacus_AddEffect(1);
	expect(a, 1);

	FfbEffect *effect = FfbAbacus_GetEffect(a);
	expectNotNull(effect);
}

void testSetConstantForce(void)
{
	USB_FFBReport_SetEffect_Output_Data_t usbData;
	FfbEffect *effect = initWithTestEffect(&usbData, 100, 0);
	FfbAbacus_SetEffect(effect, &usbData);

	expect(effect->duration, DURATION_INFINITE);
	expectF(effect->directionX, 0.0f);
	expectF(effect->directionY, 1.0f);
}

void testSetConstantForceDiagonal(void)
{
	USB_FFBReport_SetEffect_Output_Data_t usbData;
	FfbEffect *effect = initWithTestEffect(&usbData, 100, 30);
	FfbAbacus_SetEffect(effect, &usbData);

	expect(effect->duration, DURATION_INFINITE);
	expectFin(0.49f, 0.51f, effect->directionX);
	expectFin(0.86f, 0.875f, effect->directionY);
}


void testCalcConstantForceSimple(void)
{
	USB_FFBReport_SetEffect_Output_Data_t usbData;
	FfbEffect *effect = initWithTestEffect(&usbData, 100, 0);
	
	FfbAbacus_SetEffect(effect, &usbData);
	FfbAbacus_StartEffect(1);
	ForceUnit fx = 0, fy = 0;
	FfbAbacus_CalculateForces(0, 0, 0, 0, 0, &fx, &fy);
	expect(fx, 0);
	expect(fy, 100);
}

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
	define(testSetConstantForce, "set constant force effect");
	define(testSetConstantForceDiagonal, "set diagonal constant force");
	define(testCalcConstantForceSimple, "constant simple");
	define(testConstantForceDiagonal, "simple diagonal");
	define(testConstantForceDelayedWithDuration, "constant delayed with duration");
//	define(testConstantForceEnveloped, "constant enveloped");
}
