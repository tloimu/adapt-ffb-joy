/*
  Force Feedback Joystick
  Generic main file for force feedback joysticks.

  Copyright 2012  Tero Loimuneva (tloimu [at] gmail [dot] com)
  MIT License

  Some code is based on LUFA Library, for which the license:

  Copyright 2012  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/



#include "main.h"
#include "3DPro.h"
#include "Joystick.h"
#include "ffb.h"
#include "usb_hid.h"
#include "debug.h"


/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
	{
	SetupHardware();

	LEDs_SetAllLEDs(LEDS_NO_LEDS);
	sei();

	for (;;)
		{
		while (!Joystick_Connect())
			{
			LEDs_SetAllLEDs(LEDS_ALL_LEDS);
			WaitMs(300);
			LEDs_SetAllLEDs(LEDS_NO_LEDS);
			WaitMs(300);
			}

		HID_Task();
		USB_USBTask();
		}
	}

/** Configures the board hardware and chip peripherals for the functionality. */
void SetupHardware(void)
	{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Hardware Initialization */
	LEDs_Init();

	// Call the joystick's init and connection methods
	Joystick_Init();

	USB_Init();
	}

/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Device_Connect(void)
{
	/* Indicate USB enumerating */
	LEDs_SetAllLEDs(LEDS_ALL_LEDS);
}

/** Event handler for the USB_Disconnect event. This indicates that the device is no longer connected to a host via
 *  the status LEDs and stops the USB management and joystick reporting tasks.
 */
void EVENT_USB_Device_Disconnect(void)
{
	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_NO_LEDS);
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host set the current configuration
 *  of the USB device after enumeration - the device endpoints are configured and the joystick reporting task started.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
	{
	bool ConfigSuccess = true;

	/* Setup HID Report Endpoint */
	ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_EPNUM, EP_TYPE_INTERRUPT, ENDPOINT_DIR_IN,
	                                            JOYSTICK_EPSIZE, ENDPOINT_BANK_SINGLE);

	ConfigSuccess &= Endpoint_ConfigureEndpoint(FFB_EPNUM, EP_TYPE_INTERRUPT, ENDPOINT_DIR_OUT,
	                                            FFB_EPSIZE, ENDPOINT_BANK_SINGLE);

	/* Indicate endpoint configuration success or failure */
	LEDs_SetAllLEDs(ConfigSuccess ? LEDS_NO_LEDS : LEDS_ALL_LEDS);
	}


volatile bool sPIDStatusPending = false;
volatile USB_FFBReport_PIDStatus_Input_Data_t sPIDStatus;


/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
	{
	/* Handle HID Class specific requests */

	/*
	USB_ControlRequest :=
		uint8_t 	bmRequestType
		uint8_t 	bRequest
		uint16_t 	wValue
		uint16_t 	wIndex
		uint16_t 	wLength
	*/

	LogText("Control Request (value, index, all data): ");
	LogBinary(&USB_ControlRequest.wValue, 2);
	LogBinary(&USB_ControlRequest.wIndex, 2);
	LogBinary(&USB_ControlRequest, sizeof(USB_ControlRequest));

	switch (USB_ControlRequest.bRequest)
		{
		case HID_REQ_GetReport:
			LogText("  = GetReport");
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
				{
				LEDs_SetAllLEDs(LEDS_ALL_LEDS);

				/*if (USB_ControlRequest.wValue == 0x0306)
					{	// Feature 2: PID Block Load Feature Report
					LogText("GetReport PID Block Load Feature");
					_delay_us(500);	// Windows needs this delay to register the below feature report correctly
					USB_FFBReport_PIDBlockLoad_Feature_Data_t featureData;
					FfbOnPIDBlockLoad(&featureData);
					Endpoint_ClearSETUP();

					// Write the report data to the control endpoint
					Endpoint_Write_Control_Stream_LE(&featureData, sizeof(USB_FFBReport_PIDBlockLoad_Feature_Data_t));
					Endpoint_ClearOUT();
					}
				else */
				if (USB_ControlRequest.wValue == 0x0307)
					{	// Feature 3: PID Pool Feature Report
					LogText("GetReport PID Pool Feature");
					USB_FFBReport_PIDPool_Feature_Data_t featureData;
					FfbOnPIDPool(&featureData);

					Endpoint_ClearSETUP();

					// Write the report data to the control endpoint
					Endpoint_Write_Control_Stream_LE(&featureData, sizeof(USB_FFBReport_PIDPool_Feature_Data_t));
					Endpoint_ClearOUT();
					}
				else
					{
					USB_JoystickReport_Data_t JoystickReportData;

					/* Create the next HID report to send to the host */
					Joystick_CreateInputReport(USB_ControlRequest.wValue & 0xFF, &JoystickReportData);

					Endpoint_ClearSETUP();

					/* Write the report data to the control endpoint */
					Endpoint_Write_Control_Stream_LE(&JoystickReportData, sizeof(USB_JoystickReport_Data_t));
					Endpoint_ClearOUT();
					}

				//LogData("  -> GetReport:", 0, &USB_ControlRequest, sizeof(USB_ControlRequest));

				LEDs_SetAllLEDs(LEDS_NO_LEDS);
				}


			break;
		case HID_REQ_SetReport:
			LogText("  = SetReport");

			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
				{
				LEDs_SetAllLEDs(LEDS_ALL_LEDS);

				Endpoint_ClearSETUP();

				uint8_t data[10];	// This is enough room for all reports
				uint16_t len = 0;	// again, enough for all

				len = USB_ControlRequest.wLength;
				
				// Read in the report data from host

				// Read the report data from the control endpoint
				Endpoint_Read_Control_Stream_LE(&data, len);
				Endpoint_ClearStatusStage();

				// Process the incoming report
				if (USB_ControlRequest.wValue == 0x0305)
					{	// Feature 1
					_delay_us(500);	// Windows does not like to be answered too quickly
//					LogData("    => SetReport CreateNewEffect:", USB_ControlRequest.wValue & 0xFF, data, len);

					USB_FFBReport_PIDBlockLoad_Feature_Data_t pidBlockLoadData;
					FfbOnCreateNewEffect((USB_FFBReport_CreateNewEffect_Feature_Data_t*) data, &pidBlockLoadData);

					Endpoint_ClearSETUP();

					// Write the report data to the control endpoint
					Endpoint_Write_Control_Stream_LE(&pidBlockLoadData, sizeof(USB_FFBReport_PIDBlockLoad_Feature_Data_t));
					Endpoint_ClearOUT();
					}
				else if (USB_ControlRequest.wValue == 0x0306)
					{	// Feature 1
//					LogData("    => SetReport PIDBlockLoad:", USB_ControlRequest.wValue & 0xFF, data, len);
					// ???? What should be returned here?
					}
				else if (USB_ControlRequest.wValue == 0x0307)
					{	// Feature 1
//					LogData("    => SetReport PIDPool:", USB_ControlRequest.wValue & 0xFF, data, len);
					// ???? What should be returned here?
					}
/*				else
					LogData("    => SetReport data: ", USB_ControlRequest.wValue & 0xFF, data, len);
*/

				LEDs_SetAllLEDs(LEDS_NO_LEDS);
				}
			break;
		}
	}


/** Function to manage HID report generation and transmission to the host. */
void HID_Task(void)
	{
	/* Device must be connected and configured for the task to run */
	if (USB_DeviceState != DEVICE_STATE_Configured)
	  return;

	/* Select the Joystick Report Endpoint */
	Endpoint_SelectEndpoint(JOYSTICK_EPNUM);

	/* Check to see if the host is ready for another packet */
	if (Endpoint_IsINReady())
		{
		USB_JoystickReport_Data_t JoystickReportData;

		/* Create the next HID report to send to the host */
		Joystick_CreateInputReport(INPUT_REPORTID_ALL, &JoystickReportData);

		/* Write Joystick Report Data */
		Endpoint_Write_Stream_LE(&JoystickReportData, sizeof(USB_JoystickReport_Data_t), NULL);

		/* Finalize the stream transfer to send the last packet */
		Endpoint_ClearIN();
		}

	// Receive FFB data
	Endpoint_SelectEndpoint(FFB_EPNUM);

	if (Endpoint_IsOUTReceived())
		{
		LEDs_SetAllLEDs(LEDS_ALL_LEDS);

		uint8_t out_ffbdata[64];	// enough for any single OUT-report
		uint8_t total_bytes_read = 0;
		while (Endpoint_BytesInEndpoint() && total_bytes_read < 64)
			{
			uint16_t out_wait_report_bytes = 0, out_report_data_read = 0;

			// Read the reportID from the package to determine amount of data to expect next
			while (Endpoint_Read_Stream_LE(&out_ffbdata, 1, NULL)
					 == ENDPOINT_RWSTREAM_IncompleteTransfer)
				{	// busy loop until the first byte is read out
				}

			total_bytes_read += 1;

			out_wait_report_bytes = OutReportSize[out_ffbdata[0]-1] - 1;
			if (out_wait_report_bytes + total_bytes_read >= 64)
				{
				while (1)  {
				LEDs_SetAllLEDs(LEDS_NO_LEDS);
				_delay_ms(100);
				LEDs_SetAllLEDs(LEDS_ALL_LEDS);
				_delay_ms(100); }
				}
//			LogText("Starting to read new report (reportId, bytesToWait)");
//			LogBinary(out_ffbdata, 1);
//			LogBinary((uint8_t*) &out_wait_report_bytes, 2);

			out_report_data_read = 0;
			while (Endpoint_Read_Stream_LE(&out_ffbdata[1], out_wait_report_bytes, NULL)
					== ENDPOINT_RWSTREAM_IncompleteTransfer)
				{	// busy loop until the rest of the report data is read out
				}

//			LogData("Read OUT data:", out_ffbdata[0], &out_ffbdata[1], out_wait_report_bytes);
			total_bytes_read += out_wait_report_bytes;

			FfbOnUsbData(out_ffbdata, out_wait_report_bytes + 1);
			_delay_ms(1);
			}

		// Clear the endpoint ready for new packet
		Endpoint_ClearOUT();
		}
	}

