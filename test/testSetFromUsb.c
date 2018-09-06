#include <avr/io.h>

#include "test.h"
#include "testUtils.h"
#include "testsuites.h"
#include "../ffb-abacus.h"
#include "../ffb-abacus-calc.h"

FfbEffect* initAndSetEffect(USB_FFBReport_SetEffect_Output_Data_t *ioUsbData)
{
	FfbAbacus_Init();
	FfbAbacus_SetAutoCenter(0);

	uint8_t id = FfbAbacus_AddEffect(ioUsbData->effectType);
	FfbEffect *effect = FfbAbacus_GetEffect(id);
	FfbAbacus_SetEffect(effect, ioUsbData);

	return effect;
}

FfbEffect* makeTestEffect(uint8_t effectType, uint8_t usbDirection, uint16_t usbDuration)
{
	FfbAbacus_Init();
	FfbAbacus_SetAutoCenter(0);

	USB_FFBReport_SetEffect_Output_Data_t usbData;
	usbData.duration = usbDuration;
	usbData.enableAxis = 2;
	usbData.directionX = usbDirection;
	usbData.effectType = effectType;

	uint8_t id = FfbAbacus_AddEffect(effectType);
	FfbEffect *effect = FfbAbacus_GetEffect(id);
	FfbAbacus_SetEffect(effect, &usbData);

	FfbAbacus_StartEffect(id);

	return effect;
}


void testCreateEffect(void)
{
	FfbAbacus_Init();

	uint8_t a = FfbAbacus_AddEffect(FfbEffectType_Constant);
	expect(a, 1);

	FfbEffect *effect = FfbAbacus_GetEffect(a);
	expectNotNull(effect);
}

void testSetEffect(void)
{
	USB_FFBReport_SetEffect_Output_Data_t usbData;
	usbData.effectType = FfbEffectType_Constant;

	// Straight north limited time
	usbData.duration = 32000;
	setUsbDirectionDegrees(&usbData, 0);
	FfbEffect *effect = initAndSetEffect(&usbData);

	expectPointer(effect->func, calcEffectConstant);
	expect(effect->duration, 32000);
	expectF(effect->directionX, 0.0f);
	expectF(effect->directionY, -1.0f);

	// Infinite duration
	usbData.duration = USB_DURATION_INFINITE;
	FfbAbacus_SetEffect(effect, &usbData);

	expect(effect->duration, DURATION_INFINITE);

	// Diagonal
	setUsbDirectionDegrees(&usbData, 30);
	FfbAbacus_SetEffect(effect, &usbData);

	expectFin(0.49f, 0.51f, effect->directionX);
	expectFin(-0.875f, -0.86f, effect->directionY);

	// Diagonal over 180 deg
	setUsbDirectionDegrees(&usbData, 270);
	FfbAbacus_SetEffect(effect, &usbData);
	expectFin(-1.05f, -0.95f, effect->directionX);
	expectFin(-0.1f, 0.1f, effect->directionY);
}

void testSetConstantForce(void)
{
	FfbEffect* effect = makeTestEffect(FfbEffectType_Constant, 0, 1000);

	FfbAbacus_SetEffectConstantForce(effect, 0);
	expect(effect->magnitude, 0);

	FfbAbacus_SetEffectConstantForce(effect, 100);
	expect(effect->magnitude, 100);

	FfbAbacus_SetEffectConstantForce(effect, 255);
	expect(effect->magnitude, MAX_FORCE);
}

void testSetPeriodicForce(void)
{
	FfbEffect* effect = makeTestEffect(FfbEffectType_Sine, 0, 1000);
	USB_FFBReport_SetPeriodic_Output_Data_t usbForceData;

	usbForceData.magnitude = 200;
	usbForceData.offset = 0;
	usbForceData.period = 100;
	usbForceData.phase = 30;

	FfbAbacus_SetEffectPeriodic(effect, &usbForceData);
	expect(effect->magnitude, 200);
	expect(effect->x, 0);
	expect(effect->y, 0);
	expect(effect->phase, 42);
	expect(effect->frequency, 10);
}

void testSetEnvelope(void)
{
	// ???? TODO:
}

void testSetCondition(void)
{
	// ???? TODO:
}


void testSuiteSetFromUsb(void)
{
	define(testCreateEffect, "set effect");
	define(testSetEffect, "set effect");
	define(testSetConstantForce, "set constant force effect");
	define(testSetPeriodicForce, "set periodic force effect");
	define(testSetEnvelope, "set envelope");
	define(testSetCondition, "set condition");
	// ???? TODO: rest of the Set-from-USB functions
}
