#ifdef _TEST_UTILS_
#define _TEST_UTILS_

#include <avr/io.h>
#include "../ffb.h"

void setUsbDirectionDegrees(USB_FFBReport_SetEffect_Output_Data_t *usbData, uint16_t degrees);

#endif // _TEST_UTILS_