/*
  Force Feedback Joystick
  Joystick model specific code for handling joystick input data.
  This code is for Microsoft Sidewinder Force Feedback Pro joystick.

  Copyright 2012  Tero Loimuneva (tloimu [at] gmail [dot] com)
  Copyright 2013  Saku Kekkonen - Modifications for FF Wheel support
  MIT License.

  Code marked with "3DPVert" is from Detlef "Grendel" Mueller's
  3DPVert-project and is distributed with artistic/GPL license.

  Some code is based on LUFA Library, for which uses MIT license:

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


#include "3DPro.h"
#include "Joystick.h"
#include "ffb.h"
#include "usb_hid.h"
#include "debug.h"

//#define USE_FAKE_JOYSTICK

const uint8_t INPUT_REPORTID_ALL = 0xFF;

// Code from 3DPVert begins-->
//------------------------------------------------------------------------------
// Delay until reading stick after last report was sent

 #define READ_DEL	MS2TM( 4.5, 1024 )
 #define IDLE_DEL	MS2TM( 4  , 64 )


//------------------------------------------------------------------------------

uint8_t
    sw_reportsz ;				// Size of SW report in bytes

uint8_t
    idle_rate,					// idle rate in 4ms clicks, 0 for indefinite
    idle_cnt,					// idle timer (count down)
    sw_repsav[SW_REPSZ_3DP + ADDED_REPORT_DATA_SIZE] ;			// USB report data saved

//------------------------------------------------------------------------------
// Check if current report differs from the last saved one and save it.

static uint8_t sw_repchg ( void )
{
    uint8_t
	r = FALSE,
	i = sw_reportsz ;

    for ( ; i-- ; )
	if ( sw_repsav[i] != sw_report[i] )
	{
	    sw_repsav[i] = sw_report[i] ;
	    r = TRUE ;
	}

    return ( r ) ;
}
// <--- Code from 3DPVert ends


/** Configures the board hardware and chip peripherals for the joystick's functionality. */
void Joystick_Init(void)
	{
    uint8_t sw_sendrep ;

    // Initialize..

    init_hw() ;					// hardware. Note: defined as naked !
	sw_reportsz = SW_REPSZ_FFP + ADDED_REPORT_DATA_SIZE;

    SetTMPS( 1, 64 ) ;				// Set T1 prescaler to /64
    SetTMPS( 0, 1024 ) ;			// Set T0 prescaler to / 1024 for delay

    sw_sendrep = sw_repchg() ;			// Init send report flag, saved report

	WaitMs(1000);

	// Force feedback
	FfbInitMidi();

	// ADC for extra controls
	DDRF = 0; // all inputs
//	PORTF |= 0xff; // all pullups enabled

	// Init and enable ADC
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADMUX |= (1 << REFS0);		// ADC reference := Vcc
//	ADCSRA |= (1 << ADFR); 		// ADATE
//	ADCSRA |= (1 << ADATE); 	// free/continous mode
	ADMUX |= (1 << ADLAR); 		// 8-bit resolution to HIGH
	ADCSRA |= (1 << ADEN); 		// Enable ADC
	ADCSRA |= (1 << ADIE);  	// Enable ADC Interrupt 

	ADCSRA |= (1 << ADSC); 		// Go ADC
	}

/** Configures the board hardware and chip peripherals for the joystick's functionality. */
int Joystick_Connect(void)
	{
	return 1;
	}

/** Fills the given HID report data structure with the next HID report to send to the host.
 *
 *  \param[out] outReportData  Pointer to a HID report data structure to be filled
 *
 *  \return Boolean true if the new report differs from the last report, false otherwise
 */
typedef struct
	{
	int16_t 	position;
	uint16_t 	buttons;
	uint8_t 	others;
	uint16_t 	scaler;
	} JoystickData;

typedef struct
	{
	uint8_t sampledChannel;
	uint8_t	trim1, trim2, pedal1, pedal2;
	} AddedControls_ADC_t;

static volatile AddedControls_ADC_t added_controls_adc;

static volatile JoystickData prev_joystick_data;
static volatile uint8_t ADC_is_ready = 0;



int Joystick_CreateInputReport(uint8_t inReportId, USB_JoystickReport_Data_t* const outReportData)
	{
	// Read the data from the FFP-joystick

	int InputChanged = 1;	// ???? TODO: check for actual changes to avoid unnecessary input reports

#ifndef USE_FAKE_JOYSTICK
	// Code from 3DPVert begins-->
	SetTMPS( 0, 64 ) ;		// Set T0 prescaler to / 64 for query
	getdata();

	// -------------------------------------------------------------------------------
	// *******************************************************************************
	// 	Decode the raw FFP/PP data and store it into sw_report
	// 
	//  Input:
	// 	Pointer to start of packet to copy
	// 
	// 	FFP/PP data packet structure
	// 	============================
	// 
	// 	44444444 33333333 33222222 22221111 11111100 00000000
	// 	76543210 98765432 10987654 32109876 54321098 76543210
	// 	-------0 -------1 -------2 -------3 -------4 -------5
	// 	ppHHHHRR RRRRTTTT TTTYYYYY YYYYYXXX XXXXXXXB BBBBBBBB
	// 	  321054 32106543 21098765 43210987 65432109 87654321
	// 
	// 	USB report data structure
	// 	=========================
	// 
	// 	-------0 -------1 -------2 -------3 -------4 -------5
	// 	XXXXXXXX YYYYYYXX HHHHYYYY BBRRRRRR TBBBBBBB 00TTTTTT
	// 	76543210 54321098 32109876 21543210 09876543   654321
	// 
	// -------------------------------------------------------------------------------

	// Copy the data to the USB report
	// <--- Code from 3DPVert ends
#else
	memset(sw_report, 0, sizeof(sw_report));
#endif

	// ???? This could be done more directly by modifying the 3DPVert code
	// ???? that generates its own USB report to the abovementioned format.
	// Here we read the additional analog controls in
	// rotation for each AD-channel at a time.
	//
	// Here,
	//	ADC0 - Trim 1
	//	ADC1 - Trim 2
	//	ADC4 - Left Pedal
	//	ADC5 - Right Pedal

	if (ADC_is_ready)
		{
		ADC_is_ready = 0;

		switch (added_controls_adc.sampledChannel)
			{
			case 0:
				added_controls_adc.trim1 = ADCH;
				added_controls_adc.sampledChannel = 1;
				break;
			case 1:
				added_controls_adc.trim2 = ADCH;
				added_controls_adc.sampledChannel = 4;
				break;
			case 4:
				added_controls_adc.pedal1 = ADCH;
				added_controls_adc.sampledChannel = 5;
				break;
			case 5:
				added_controls_adc.pedal2 = ADCH;
				added_controls_adc.sampledChannel = 0;
				break;
			default:
				added_controls_adc.sampledChannel = 0;	// just in case
				break;
			}

		// Start AD-conversion for the next channel in the rotation
		ADMUX &= 0b11111000;
		ADMUX |= (0b111 & added_controls_adc.sampledChannel);
		ADCSRA|=(1<<ADSC);
		}

/*
5 wwwwwwww
4 aaaaaaww 
3 BAbbbbbb
2 1FLZYXRC
1 p-------
0 --------
*/
		
	// Convert the raw input data to USB report

	outReportData->reportId = 1;	// Input report ID 1

	if (sw_id == SW_ID_FFPW)
		{
		outReportData->Button = ((sw_report[2] << 2) | (sw_report[3] >> 6)) ^ 0x00ff;
		outReportData->X = ((sw_report[4] & 0x03) << 8) + sw_report[5];
		outReportData->Y = (sw_report[4] & 0xfc) << 2;
		/* actually break for wheel */
		outReportData->Throttle = 63-(sw_report[3] & 0x3f);
		}
	else
		{
		// Convert data from Sidewinder Force Feedback Pro
		outReportData->X = sw_report[0] + ((sw_report[1] & 0x03) << 8);
		if (sw_report[1] & 0x02)
			outReportData->X |= (0b11111100 << 8);
		
		
		outReportData->Y = (sw_report[1] >> 2) + ((sw_report[2] & 0x0F) << 6);
		if (sw_report[2] & 0x08)
			outReportData->Y |= (0b11111100 << 8);
		outReportData->Button = ((sw_report[4] & 0x7F) << 2) + ((sw_report[3] & 0xC0) >> 6);
		outReportData->Hat = sw_report[2] >> 4;
		outReportData->Rz = (sw_report[3] & 0x3f) - 32;
		outReportData->Throttle = ((sw_report[5] & 0x3f) << 1) + (sw_report[4] >> 7);
		if (sw_report[5] & 0x20)
			outReportData->Throttle |= 0b11000000;

		outReportData->Z = 0;	// not used at the moment

		// Get data from additional controls
		outReportData->Rudder = (added_controls_adc.pedal2 - added_controls_adc.pedal1) / 2 - 128;	// Combine two pedals into a single rudder
		outReportData->Rx = added_controls_adc.trim2;	// rudder trim
		outReportData->Ry = added_controls_adc.trim1;	// elevator trim
		}

/*
	// This test code generates ever changing position and button values
	// to make it easy to see whether the position reports are working
	// or not even if you don't have an actual joystick or other control
	// connected.

	prev_joystick_data.scaler++;
	if (prev_joystick_data.scaler < 200)
		return false;
	prev_joystick_data.scaler = 0;

	// "Read" the joystick
	if (prev_joystick_data.buttons == 0)
		prev_joystick_data.buttons = 1;
	else
		prev_joystick_data.buttons = prev_joystick_data.buttons << 1;

	if (prev_joystick_data.position > 500)
		prev_joystick_data.position = 0;

	if (prev_joystick_data.position < 500)
		prev_joystick_data.position++;
	else
		prev_joystick_data.position = -500;


	LogData("Joy: ", 1, outReportData, sizeof(USB_JoystickReport_Data_t));

	// Clear the report contents
	memset(outReportData, 0, sizeof(USB_JoystickReport_Data_t));

	outReportData->reportId = 1;
	outReportData->Button = prev_joystick_data.buttons;
	outReportData->Y = prev_joystick_data.position;
	outReportData->X = prev_joystick_data.position;
	outReportData->Z = prev_joystick_data.position;
	outReportData->Rz = prev_joystick_data.position & 0x7F;
	outReportData->Rx = prev_joystick_data.position & 0xFF;
	outReportData->Ry = prev_joystick_data.position & 0xFF;
	outReportData->Throttle = prev_joystick_data.position & 0xFF;
	outReportData->Slider = prev_joystick_data.position & 0xFF;
	outReportData->Hat = prev_joystick_data.position % 8;
*/
	return InputChanged;
	}


// AD-conversion-completed-interrupt handler.
// We only set a "ADC ready"-flag here so that
// the actual data can be read later when there
// is more time. Doing any heavier stuff here
// seems to cause problems reading data from FFP joystick.
ISR(ADC_vect) 
	{
	ADC_is_ready = 1;
	}
