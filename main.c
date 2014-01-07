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

#include "Descriptors.h"

void CDC1_Task(void);

/** Contains the current baud rate and other settings of the first virtual serial port. While this demo does not use
 *  the physical USART and thus does not use these settings, they must still be retained and returned to the host
 *  upon request or the host will assume the device is non-functional.
 *
 *  These values are set by the host via a class-specific request, however they are not required to be used accurately.
 *  It is possible to completely ignore these value or use other settings as the host is completely unaware of the physical
 *  serial link characteristics and instead sends and receives data in endpoint streams.
 */



static CDC_LineEncoding_t LineEncoding1 = { .BaudRateBPS = 0,
                                            .CharFormat  = CDC_LINEENCODING_OneStopBit,
                                            .ParityType  = CDC_PARITY_None,
                                            .DataBits    = 8                            };


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
		FlushDebugBuffer();

		CDC1_Task();
		FlushDebugBuffer();

		USB_USBTask();
		FlushDebugBuffer();
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
#ifdef ENABLE_JOYSTICK_SERIAL
	/* Setup first CDC Interface's Endpoints */
	ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC1_TX_EPNUM, EP_TYPE_BULK, ENDPOINT_DIR_IN,
	                                            CDC_TXRX_EPSIZE, ENDPOINT_BANK_SINGLE);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC1_RX_EPNUM, EP_TYPE_BULK, ENDPOINT_DIR_OUT,
	                                            CDC_TXRX_EPSIZE, ENDPOINT_BANK_SINGLE);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC1_NOTIFICATION_EPNUM, EP_TYPE_INTERRUPT, ENDPOINT_DIR_IN,
	                                            CDC_NOTIFICATION_EPSIZE, ENDPOINT_BANK_SINGLE);
#endif // ENABLE_JOYSTICK_SERIAL

	/* Reset line encoding baud rates so that the host knows to send new values */
	LineEncoding1.BaudRateBPS = 0;

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

#ifdef ENABLE_JOYSTICK_SERIAL
	/* Determine which interface's Line Coding data is being set from the wIndex parameter */
	void* LineEncodingData = (USB_ControlRequest.wIndex == 1) ? &LineEncoding1 : NULL;
#endif // ENABLE_JOYSTICK_SERIAL

	if (DoDebug(DEBUG_DETAIL))
		{
		LogTextP(PSTR("CtrlReq(val,idx,req):"));
		LogBinary(&USB_ControlRequest.wValue, 2);
		LogBinary(&USB_ControlRequest.wIndex, 2);
		LogBinaryLf(&USB_ControlRequest.bRequest, 1);
		//LogBinary(&USB_ControlRequest, sizeof(USB_ControlRequest));
		}

	switch (USB_ControlRequest.bRequest)
		{
		// Joystick stuff

		case HID_REQ_GetReport:
			if (DoDebug(DEBUG_DETAIL))
				LogTextLf("GetReport");
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
				{
				LEDs_SetAllLEDs(LEDS_ALL_LEDS);

				/*if (USB_ControlRequest.wValue == 0x0306)
					{	// Feature 2: PID Block Load Feature Report
					LogTextP(PSTR("GetReport PID Block Load Feature"));
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
			if (DoDebug(DEBUG_DETAIL))
				LogTextLf("SetReport");

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

#ifdef ENABLE_JOYSTICK_SERIAL

			// Serial stuff
		case CDC_REQ_GetLineEncoding:
			if (DoDebug(DEBUG_DETAIL))
				LogTextP(PSTR("  = GetLineEncoding"));

			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				// Write the line coding data to the control endpoint
				Endpoint_Write_Control_Stream_LE(LineEncodingData, sizeof(CDC_LineEncoding_t));
				Endpoint_ClearOUT();
			}

			break;
		case CDC_REQ_SetLineEncoding:
			if (DoDebug(DEBUG_DETAIL))
				LogTextP(PSTR("  = SetLineEncoding"));

			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				// Read the line coding data in from the host into the global struct
				Endpoint_Read_Control_Stream_LE(LineEncodingData, sizeof(CDC_LineEncoding_t));
				Endpoint_ClearIN();
			}

			break;
		case CDC_REQ_SetControlLineState:
			if (DoDebug(DEBUG_DETAIL))
				LogTextP(PSTR("  = SetControlLineState"));

			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				Endpoint_ClearStatusStage();
			}

			break;
#endif // ENABLE_JOYSTICK_SERIAL
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
//			LogTextP(PSTR("Starting to read new report (reportId, bytesToWait)"));
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



// -------------------------------
// Serial stuff

/*	Function to manage CDC data transmission and reception to and from the host for the.
	It is used for debugging and allows controlling the force feedback effects via
	serial line too.
	
	The protocol is ASCII so that it can be used e.g. with regular terminal program.
	The protocol towards the device is based on "messages". Each message start with
	a letter (single byte) that designates the command. The following bytes depend on
	command. Once a complete message is received, it is immediately executed. Some
	messages contain only the command letter while others may have variable sized
	parameter data following it. Spaces, tabs, enters and other non-alphanumeric input
	is ignored.
	
	Commands:
		"l"
			List all effect info from the adapter/joystick. Sends info about each
			effect index in the device (loaded or free).
			
		"d" 01 SETTING
			Disable the given debug setting. Settings are cumulative:
				01 = Send debug data to UART
				02 = Send debug data to USB
				04 = Include additional details to debug data
				
		"D" 01 SETTING
			Enable the given debug setting.
			
		"t" 01 EFFECTTYPE
			Disable all effects with type EFFECTTYPE. Currently supported effect types here are:
				1 = Constant effects
				2 = Sine effects
				5 = Triangle effects
				8 = Spring effects
				
		"T" 01 EFFECTTYPE
			Enable all effects with type EFFECTTYPE. See Above.
			
		"e" 01 EFFECTID
			Disable effect with ID EFFECTID. See also command "l".
		
		"E" 01 EFFECTID
			Enable effect with ID EFFECTID. See also command "l".
			
		"m" LENGTH ...data...
			Send given data directly to joystick's MIDI channel. LENGTH is the number of
			bytes in the data.
			
			e.g. "m 03 A5 7F 00" will send bytes "A5 7F 00" as binary to joystick.
			
		"u" LENGTH ...data...
			Process the given data as if it came as a force feedback report via USB.
			LENGTH is the number of bytes in the data.
			
			e.g. "u 02 0D FF" will send bytes "0D FF" as binary to adapter's FFB data
			processing. This example would trigger DeviceGain-report (id=0x0D) with one
			byte parameter 0xFF.
*/


#if !defined ENABLE_JOYSTICK_SERIAL
void CDC1_Task(void)  {}
#else

void ProcessDataFromCOMSerial(char data);

void CDC1_Task(void)
	{
	/* Device must be connected and configured for the task to run */
	if (USB_DeviceState != DEVICE_STATE_Configured)
		return;

	FlushDebugBuffer();

	/* Select the Serial Rx Endpoint */
	Endpoint_SelectEndpoint(CDC1_RX_EPNUM);

	char data[128];
	uint16_t len = 0;

	/* Throw away any received data from the host */
	if (Endpoint_IsOUTReceived())
		{
		LEDs_SetAllLEDs(LEDS_ALL_LEDS);

		while (Endpoint_BytesInEndpoint() && len < 128)
			{
			// Read the reportID from the package to determine amount of data to expect next
			while (Endpoint_Read_Stream_LE(&data[len++], 1, NULL)
					 == ENDPOINT_RWSTREAM_IncompleteTransfer)
				{	// busy loop until the first byte is read out
				}
			}

		Endpoint_ClearOUT();

		for (uint16_t i = 0; i < len; i++)
			{
			ProcessDataFromCOMSerial(data[i]);
			}

		LEDs_SetAllLEDs(LEDS_NO_LEDS);
		}
	}

uint8_t ParseHexNibble(char data)
	{
	if (data >= '0' && data <= '9')
		return data - '0';
	else if (data >= 'a' && data <= 'f')
		return (data - 'a') + 10;
	else if (data >= 'A' && data <= 'F')
		return (data- 'A') + 10;
	else
		return 0xFF; // not a hex nibble
	}


void DoCommandListEffects(void);
void DoCommandSetDebug(char command, char value);
void DoCommandSetEffectType(char effectType, char value);
void DoCommandSetEffectAtIndex(uint8_t effectIndex, char value);
void DoCommandSendMidi(uint8_t *data, uint16_t len);
void DoCommandSimulateUsbReceive(uint8_t *data, uint16_t len);

void ProcessCommandDataFromCOMSerial(char command, char data);

volatile static uint8_t gOngoingSerialCommandDataLen = 0; // expected length of actual command data
volatile static char gOngoingSerialCommand = '\0';

void ProcessDataFromCOMSerial(char data)
	{
	volatile static uint8_t gOngoingSerialCommandParameterPos = 0; // how many parameter nibbles have been read
	volatile static uint8_t gDataByte = 0; // currently parsed data byte cache

	// Check for start of a new command
	if (gOngoingSerialCommand == 0)
		{
		if (DoDebug(DEBUG_DETAIL))
			{
			LogBinaryLf(&data, 1);
			}

		if (data <= 32 || data > 'z') return; // some sanity checking - skip possible garbage or formatting

		// Commands with no parameters can be handled directly here
		if (data == 'l')
			{
			DoCommandListEffects();
			return;
			}

		// The command has parameter data - need to parse and collect them nibble by nibble
		gOngoingSerialCommand = data;
		gOngoingSerialCommandParameterPos = 0;
		gOngoingSerialCommandDataLen = 0;
		return;
		}

	if (DoDebug(DEBUG_DETAIL))
		{
		LogBinary(&data, 1);
		}

	// Check for data length parameter (2 hexadecimal nibbles)
	if (gOngoingSerialCommandParameterPos == 0)
		{
		uint8_t value = ParseHexNibble(data);
		if (value == 0xFF)
			return;
		gOngoingSerialCommandParameterPos = 1;
		gOngoingSerialCommandDataLen = value << 4;
		return;
		}

	if (gOngoingSerialCommandParameterPos == 1)
		{
		uint8_t value = ParseHexNibble(data);
		if (value == 0xFF)
			return;
		gOngoingSerialCommandParameterPos = 2;
		gOngoingSerialCommandDataLen += value;

		if (DoDebug(DEBUG_DETAIL))
			{
			LogTextP(PSTR("Expecting data len="));
			LogBinaryLf((uint8_t*) &gOngoingSerialCommandDataLen, 1);
			}

		return;
		}

	// Parse actual data
	uint8_t value = ParseHexNibble(data);
	if (value == 0xFF)
		return;
	gOngoingSerialCommandParameterPos++;
	if (gOngoingSerialCommandParameterPos % 2)
		{
		gDataByte = value << 4;
		}
	else
		{
		gDataByte = gDataByte + value;
		ProcessCommandDataFromCOMSerial(gOngoingSerialCommand, gDataByte);
		}
	}

void CompletedCommandDataFromCOMSerial(char command, char *data, uint16_t len);

void ProcessCommandDataFromCOMSerial(char command, char data)
	{
	if (DoDebug(DEBUG_DETAIL))
		{
		LogTextP(PSTR("Received data="));
		LogBinaryLf((uint8_t*) &data, 1);
		}

	// Reserve a buffer for sending raw-data from COM serial to USB/MIDI handling
#define SERIAL_COMMAND_BUFFER_SIZE 40
	static char SERIAL_COMMAND_BUFFER[SERIAL_COMMAND_BUFFER_SIZE];
	volatile static uint8_t gOngoingSerialCommandDataPos = 0; // writer offset of command data in SERIAL_COMMAND_BUFFER

	SERIAL_COMMAND_BUFFER[gOngoingSerialCommandDataPos] = data;
	gOngoingSerialCommandDataPos++;

	if (gOngoingSerialCommandDataPos >= gOngoingSerialCommandDataLen)
		{ // all data received - command completely received
		CompletedCommandDataFromCOMSerial(command, SERIAL_COMMAND_BUFFER, gOngoingSerialCommandDataLen);
		gOngoingSerialCommand = 0; // wait for the next command
		gOngoingSerialCommandDataPos = 0;
		}
	}

void CompletedCommandDataFromCOMSerial(char command, char *data, uint16_t len)
	{
	LogTextP(PSTR("Com  => Command="));
	LogBinary(&command, 1);
	LogTextP(PSTR(", data="));
	LogBinaryLf(data, len);

	if (command == 'd' || command == 'D')
		DoCommandSetDebug(command, data[0]);
	else if (command == 'm')
		DoCommandSendMidi((uint8_t*) data, len);
	else if (command == 'u')
		DoCommandSimulateUsbReceive((uint8_t*) data, len);
	else if (command == 't') // disable effect type
		DoCommandSetEffectType(data[0], 0);
	else if (command == 'T') // enable effect type
		DoCommandSetEffectType(data[0], 1);
	else if (command == 'e') // disable effect at index
		DoCommandSetEffectAtIndex(data[0], 0);
	else if (command == 'E') // enable effect at index
		DoCommandSetEffectAtIndex(data[0], 1);
	else
		{
		LogTextLfP(PSTR("Error: unknown command"));
		}
	}

void DoCommandListEffects()
	{
	LogTextP(PSTR("Effects:\n"));
	uint8_t i = 0;
	while (FfbDebugListEffects(&i))
		FlushDebugBuffer();

	if (gDisabledEffects.springs)
		LogTextP(PSTR(" Springs disabled\n"));
	if (gDisabledEffects.constants)
		LogTextP(PSTR(" Constants disabled\n"));
	if (gDisabledEffects.triangles)
		LogTextP(PSTR(" Triangles disabled\n"));
	if (gDisabledEffects.sines)
		LogTextP(PSTR(" Sines disabled\n"));
	}

void DoCommandSetDebug(char command, char value)
	{
	if (command == 'd')
		gDebugMode = gDebugMode & (~value);
	else
		gDebugMode |= value;
	}

void DoCommandSetEffectType(char effectType, char value)
	{
	if (effectType == 8)
		FfbEnableSprings(value);
	else if (effectType == 1)
		FfbEnableConstants(value);
	else if (effectType == 5)
		FfbEnableTriangles(value);
	else if (effectType == 2)
		FfbEnableSines(value);
	else
		{
		LogTextLfP(PSTR("Error: unknown effect type to enable/disable"));
		}
	}

void DoCommandSetEffectAtIndex(uint8_t effectIndex, char value)
	{
	FfbEnableEffectId(effectIndex, value);
	}

void DoCommandSendMidi(uint8_t *data, uint16_t len)
	{
	FfbSendData(data, len);
	}

void DoCommandSimulateUsbReceive(uint8_t *data, uint16_t len)
	{
	FfbOnUsbData(data, len);
	}

#endif //ENABLE_JOYSTICK_SERIAL
