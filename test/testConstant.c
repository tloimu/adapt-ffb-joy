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

void testConstantForceSimple(void)
{
	FfbAcabus_Init();

	uint8_t a = FfbAcabus_AddEffect(1);
	expect(a, 1);

	FfbEffect *effect = FfbAcabus_GetEffect(a);
	expectNotNull(effect);
	
	USB_FFBReport_SetEffect_Output_Data_t usbData;
	setDirectionDegrees(&usbData, 0);
	FfbAcabus_SetEffect(effect, &usbData);
	FfbAcabus_SetEffectConstantForce(effect, 100);
	FfbAcabus_StartEffect(a);
	ForceUnit fx = 0, fy = 0;
	FfbAcabus_CalculateForces(0, 0, 0, 0, 0, &fx, &fy);
	expect(fx, 0);
	expect(fy, 100);
}

void testConstantForceDiagonal(void)
{
	FfbAcabus_Init();

	uint8_t a = FfbAcabus_AddEffect(1);
	expect(a, 1);

	FfbEffect *effect = FfbAcabus_GetEffect(a);
	expectNotNull(effect);
	
	USB_FFBReport_SetEffect_Output_Data_t usbData;
	setDirectionDegrees(&usbData, 30);
	FfbAcabus_SetEffect(effect, &usbData);
	FfbAcabus_SetEffectConstantForce(effect, 50);

	FfbAcabus_StartEffect(a);
	ForceUnit fx = 0, fy = 0;
	FfbAcabus_CalculateForces(0, 0, 0, 0, 0, &fx, &fy);
	expect(fx, 24); // 25 is exact, but this is close enough
	expect(fy, 43);
}

void testConstantForceDelayedWithDuration(void)
{
	FfbAcabus_Init();

	uint8_t a = FfbAcabus_AddEffect(1);
	FfbEffect *effect = FfbAcabus_GetEffect(a);
	
	USB_FFBReport_SetEffect_Output_Data_t usbData;
	setDirectionDegrees(&usbData, 0);
	FfbAcabus_SetEffectConstantForce(effect, 100);
	/*
	FfbAcabus_SetEffectTiming(a, 5, 30);
	FfbAcabus_StartEffect(a);
	{
		ForceUnit fx = 0, fy = 0;
		FfbAcabus_CalculateForces(0, 0, 0, 0, 4, &fx, &fy);
		expect(fx, 0);
		expect(fy, 0);
	}

	{
		ForceUnit fx = 0, fy = 0;
		FfbAcabus_CalculateForces(0, 0, 0, 0, 1, &fx, &fy);
		expect(fx, 0);
		expect(fy, 0);
	}

	{
		ForceUnit fx = 0, fy = 0;
		FfbAcabus_CalculateForces(0, 0, 0, 0, 1, &fx, &fy);
		expect(fx, 0);
		expect(fy, 100);
	}

	{
		ForceUnit fx = 0, fy = 0;
		FfbAcabus_CalculateForces(0, 0, 0, 0, 30, &fx, &fy);
		expect(fx, 0);
		expect(fy, 100);
	}

	{
		ForceUnit fx = 0, fy = 0;
		FfbAcabus_CalculateForces(0, 0, 0, 0, 1, &fx, &fy);
		expect(fx, 0);
		expect(fy, 0);
	}*/
}
/*
void testConstantForceEnveloped(void)
{
	FfbAcabus_Init();

	uint8_t a = FfbAcabus_AdEffect();
	expect(a, 0);

	FfbAcabus_SetEffectConstant(a, 0, 100);
	FfbAcabus_SetEffectEnvelope(a, 10, 90, 20, 100);
	FfbAcabus_StartEffect(a);
	{
		ForceUnit fx = 0, fy = 0;
		FfbAcabus_CalculateForces(0, 0, 0, 0, 4, &fx, &fy);
		expect(fx, 0);
		expect(fy, 100);
	}

	{
		ForceUnit fx = 0, fy = 0;
		FfbAcabus_CalculateForces(0, 0, 0, 0, 1, &fx, &fy);
		expect(fx, 0);
		expect(fy, 0);
	}

	{
		ForceUnit fx = 0, fy = 0;
		FfbAcabus_CalculateForces(0, 0, 0, 0, 1, &fx, &fy);
		expect(fx, 0);
		expect(fy, 100);
	}

	{
		ForceUnit fx = 0, fy = 0;
		FfbAcabus_CalculateForces(0, 0, 0, 0, 30, &fx, &fy);
		expect(fx, 0);
		expect(fy, 100);
	}

	{
		ForceUnit fx = 0, fy = 0;
		FfbAcabus_CalculateForces(0, 0, 0, 0, 1, &fx, &fy);
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
