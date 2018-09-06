#include "testUtils.h"
#include "../ffb.h"

void setUsbDirectionDegrees(USB_FFBReport_SetEffect_Output_Data_t *usbData, uint16_t degrees)
{
	usbData->enableAxis = 2;
	usbData->directionX = (degrees / 360.0f ) * 192;
	usbData->directionY = 0;
}
