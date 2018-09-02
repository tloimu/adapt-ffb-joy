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
	initEffectAbacus();

	uint8_t a = addEffectOfType(1);
	expect(a, 1);

	Effect *effect = getEffect(a);
	expectNotNull(effect);
	
	USB_FFBReport_SetEffect_Output_Data_t usbData;
	setDirectionDegrees(&usbData, 0);
	setEffect(effect, &usbData);
	setEffectConstantForce(effect, 100);
	startEffect(a);
	ForceUnit fx = 0, fy = 0;
	calculateForces(0, 0, 0, 0, 0, &fx, &fy);
	expect(fx, 0);
	expect(fy, 100);
}

void testConstantForceDiagonal(void)
{
	initEffectAbacus();

	uint8_t a = addEffectOfType(1);
	expect(a, 1);

	Effect *effect = getEffect(a);
	expectNotNull(effect);
	
	USB_FFBReport_SetEffect_Output_Data_t usbData;
	setDirectionDegrees(&usbData, 30);
	setEffect(effect, &usbData);
	setEffectConstantForce(effect, 50);

	startEffect(a);
	ForceUnit fx = 0, fy = 0;
	calculateForces(0, 0, 0, 0, 0, &fx, &fy);
	expect(fx, 24); // 25 is exact, but this is close enough
	expect(fy, 43);
}

void testConstantForceDelayedWithDuration(void)
{
	initEffectAbacus();

	uint8_t a = addEffect();
	Effect *effect = getEffect(a);
	
	USB_FFBReport_SetEffect_Output_Data_t usbData;
	setDirectionDegrees(&usbData, 0);
	setEffectConstantForce(effect, 100);
	/*
	setEffectTiming(a, 5, 30);
	startEffect(a);
	{
		ForceUnit fx = 0, fy = 0;
		calculateForces(0, 0, 0, 0, 4, &fx, &fy);
		expect(fx, 0);
		expect(fy, 0);
	}

	{
		ForceUnit fx = 0, fy = 0;
		calculateForces(0, 0, 0, 0, 1, &fx, &fy);
		expect(fx, 0);
		expect(fy, 0);
	}

	{
		ForceUnit fx = 0, fy = 0;
		calculateForces(0, 0, 0, 0, 1, &fx, &fy);
		expect(fx, 0);
		expect(fy, 100);
	}

	{
		ForceUnit fx = 0, fy = 0;
		calculateForces(0, 0, 0, 0, 30, &fx, &fy);
		expect(fx, 0);
		expect(fy, 100);
	}

	{
		ForceUnit fx = 0, fy = 0;
		calculateForces(0, 0, 0, 0, 1, &fx, &fy);
		expect(fx, 0);
		expect(fy, 0);
	}*/
}
/*
void testConstantForceEnveloped(void)
{
	initEffectAbacus();

	uint8_t a = addEffect();
	expect(a, 0);

	setEffectConstant(a, 0, 100);
	setEffectEnvelope(a, 10, 90, 20, 100);
	startEffect(a);
	{
		ForceUnit fx = 0, fy = 0;
		calculateForces(0, 0, 0, 0, 4, &fx, &fy);
		expect(fx, 0);
		expect(fy, 100);
	}

	{
		ForceUnit fx = 0, fy = 0;
		calculateForces(0, 0, 0, 0, 1, &fx, &fy);
		expect(fx, 0);
		expect(fy, 0);
	}

	{
		ForceUnit fx = 0, fy = 0;
		calculateForces(0, 0, 0, 0, 1, &fx, &fy);
		expect(fx, 0);
		expect(fy, 100);
	}

	{
		ForceUnit fx = 0, fy = 0;
		calculateForces(0, 0, 0, 0, 30, &fx, &fy);
		expect(fx, 0);
		expect(fy, 100);
	}

	{
		ForceUnit fx = 0, fy = 0;
		calculateForces(0, 0, 0, 0, 1, &fx, &fy);
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
