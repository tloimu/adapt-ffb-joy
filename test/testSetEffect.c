#include <avr/io.h>

#include "test.h"
#include "testsuites.h"
#include "../ffb-abacus.h"

FfbEffect* makeTestEffect(void)
{
	FfbAcabus_Init();
	FfbAcabus_SetAutoCenter(0);
	uint8_t a = FfbAcabus_AddEffect(1);
	FfbEffect *effect = FfbAcabus_GetEffect(a);
	return effect;
}

void testSetEffect(void)
{
	FfbEffect *effect = makeTestEffect();

	USB_FFBReport_SetEffect_Output_Data_t usbData;
	usbData.duration = 500;
	usbData.enableAxis = 2; // X = direction
	usbData.directionX = 21; // almost 30 deg
	usbData.directionY = 0;
	usbData.gain = 100;
	usbData.samplePeriod = 555;
	FfbAcabus_SetEffect(effect, &usbData);

	expect(effect->duration, 500);
	expect(effect->directionX, 49); // close enough to 30 deg sin
	expect(effect->directionY, 87); // close enough to 30 deg cos

	// Check infinite duration
	usbData.duration = USB_DURATION_INFINITE;
	FfbAcabus_SetEffect(effect, &usbData);
	expect(effect->duration == DURATION_INFINITE, 1);
}

void testSetCondition(void)
{
	// ???? TODO:
}

void testSetEnvelope(void)
{
	// ???? TODO:
}

void testSuiteSetEffect(void)
{
	testSetEffect();
	testSetCondition();
	testSetEnvelope();
}
