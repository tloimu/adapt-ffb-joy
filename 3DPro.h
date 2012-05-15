/*******************************************************************************
 * File Name	: 3DPro.h
 * Project	: 3DP-Vert, Microsoft Sidewinder 3D Pro/PP/FFP to USB converter
 * Date		: 2005/05/31, 2006/12/14, 2008/02/12, 2009/06/16
 * Version	: 4.0
 * Target MCU	: AT90USB162/82, AT90USB646/1286, ATMEGA16U4/32U4
 * Tool Chain	: Atmel AVR Studio 4.18 716 / WinAVR 20100110
 * Author	: Detlef "Grendel" Mueller
 *		  detlef@gmail.com
 * Release Notes:
 *
 *	================
 *	WARNING: This version has been heavily modified to only support
 *           Force Feedback Pro and ATmega32U4.
 *           Do not use except for testing purposes!
 *
 *             -- tloimu
 *	================
 *
 * $Id: 3DPro.h 1.6 2010/04/23 05:28:59 Detlef Exp Detlef $
 ******************************************************************************/

#ifndef	_3DPRO_H_
#define	_3DPRO_H_

//-------------------------------------------------------------------------------

#include "includes.h"

//-------------------------------------------------------------------------------

#define	HID_PHYS	1		/* Physical min/max in HID report	*/
					/* (W98 actually needs these...)	*/

//-------------------------------------------------------------------------------
// SideWinder packet sizes

#define	DATSZ3DP	22	/* 3DP data packet size in clock ticks (x3 bit, sent 3x) */
#define	IDSZ3DP		160	/* 3DP ID packet size in clock ticks */
#define	DATSZFFP	16	/* FFP/PP data packet size in clock ticks (x3 bit) */
#define	IDSZPP		40	/* PP ID packet size */
#define	IDSZFFP		14	/* FFP ID packet size */

//-------------------------------------------------------------------------------
// Things not defined in iom32u4.h

#if defined(__AVR_ATmegaXU4__)

#define	PRTIM4		4	/* Power reduction register bit 4 for Timer 4 */

#define	PB7		PORTB7
#define	PB6		PORTB6
#define	PB5		PORTB5
#define	PB4		PORTB4
#define	PB3		PORTB3
#define	PB2		PORTB2
#define	PB1		PORTB1
#define	PB0		PORTB0

#define	PC7		PORTC7
#define	PC6		PORTC6

#define	PD7		PORTD7
#define	PD6		PORTD6
#define	PD5		PORTD5
#define	PD4		PORTD4
#define	PD3		PORTD3
#define	PD2		PORTD2
#define	PD1		PORTD1
#define	PD0		PORTD0

#define	PE6		PORTE6
#define	PE2		PORTE2

#define	PF7		PORTF7
#define	PF6		PORTF6
#define	PF5		PORTF5
#define	PF4		PORTF4
#define	PF1		PORTF1
#define	PF0		PORTF0

#endif

//-------------------------------------------------------------------------------
// Hardware specific definitions

#define	LEDPORT		PORTD
#define	LEDBIT		PD6

#define TRGDDR		DDRB

#define TRGY2BIT	DDB5
#define TRGX1BIT	DDB4

#define	BUTPIN		PINB
#define	BUT1		PB0
#define	BUT2		PB1
#define	BUT3		PB2
#define	BUT4		PB3
#define	BUTMSK		(_B1(BUT4) | _B1(BUT3) | _B1(BUT2) | _B1(BUT1))

#define ClrSerial()	((BUTPIN & BUTMSK) == (_B1(BUT2) | _B1(BUT1)))

//-------------------------------------------------------------------------------
// Timer 0/1 prescaler settings

#define	T0PS_64		(_B0(CS02) | _B1(CS01) | _B1(CS00))
#define	T0PS_1024	(_B1(CS02) | _B0(CS01) | _B1(CS00))

#define	T1PS_64		(_B0(CS12) | _B1(CS11) | _B1(CS10))
#define	T1PS_1024	(_B1(CS12) | _B0(CS11) | _B1(CS10))

//-------------------------------------------------------------------------------

#if ! defined(__ASSEMBLER__)

//-------------------------------------------------------------------------------
// C only definitions
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Macros to deal w/ the timers

#define	ResetTM( n, del )	__WRAP__( {				\
					TCNT ## n = (del) ;		\
					TIFR ## n = _BV( TOV ## n ) ;	\
				} )

#define	TMexp( n )		bit_is_set( TIFR ## n, TOV ## n )

#define	SetTMPS( n, ps )	TCCR ## n ## B = T ## n ## PS_ ## ps

//-------------------------------------------------------------------------------
// Timer Macros, msec or usec to timer ticks. ps = prescaler value

#define MS2TM( ms, ps ) -(int16_t)(((ms) * (F_CPU /    1000.)) / (ps) + .5)
#define US2TM( us, ps ) -(int16_t)(((us) * (F_CPU / 1000000.)) / (ps) + .5)

//-------------------------------------------------------------------------------
// Timer delay values for /1024 prescaler

#define	T0DEL500US	MS2TM(  .5, 1024 )
#define	T0DEL4MS	MS2TM(   4, 1024 )
#define	T0DEL8MS	MS2TM(   8, 1024 )
#define	T0DEL200MS	MS2TM( 200, 1024 )
#define	T0DEL300MS	MS2TM( 300, 1024 )
#define	T0DEL400MS	MS2TM( 400, 1024 )
#define	T0DEL500MS	MS2TM( 500, 1024 )

//-------------------------------------------------------------------------------
// Timer delay values for /64 prescaler

#define	T6DEL48US	US2TM(  48,  64 )
#define	T6DEL140US	US2TM( 140,  64 )
#define	T6DEL440US	US2TM( 440,  64 )
#define	T6DEL865US	US2TM( 865,  64 )

#define	T6DEL4MS	MS2TM(   4,  64 )
#define	T6DEL100MS	MS2TM( 100,  64 )
#define	T6DEL200MS	MS2TM( 200,  64 )
#define	T6DEL300MS	MS2TM( 300,  64 )

//-------------------------------------------------------------------------------

#define	SW_REPSZ_3DP	7			/* report size for 3DP */
#define	SW_REPSZ_FFP	6			/* report size for PP/FFP */

#define ADDED_REPORT_DATA_SIZE 0	/* added report size for additional controls */

#define	SW_ID_3DP	1			/* 3DP connected */
#define	SW_ID_PP	2			/*  PP connected */
#define	SW_ID_FFP	3			/* FFP connected */

//-------------------------------------------------------------------------------
// Inline code

#if defined(__AVR_AT90USBX2__) || defined(__AVR_AT90USB646__)

#define	LED_on()	clr_bit( LEDPORT, LEDBIT )
#define	LED_off()	set_bit( LEDPORT, LEDBIT )

#elif defined(__AVR_ATmegaXU4__) || defined(__AVR_AT90USB1286__)

#define	LED_on()	set_bit( LEDPORT, LEDBIT )
#define	LED_off()	clr_bit( LEDPORT, LEDBIT )

#endif

#define	LED_tog()	tog_bit( LEDPORT, LEDBIT )

//-------------------------------------------------------------------------------
// main.c interface

extern uint8_t
    idle_rate,				// idle rate in 4ms clicks, 0 for indefinite
    idle_cnt ;				// idle counter

//-------------------------------------------------------------------------------
// 3DPro.c interface

extern uint8_t
    sw_id,				// Will be SW_ID_...
    sw_report[SW_REPSZ_3DP + ADDED_REPORT_DATA_SIZE],		// Report buffer
    sw_reportsz ;			// Size of report in bytes

extern void
    init_hw( void ),			// Initialize HW & wait for stick
    getdata( void ) ;			// Read stick and set up sw_report

//-------------------------------------------------------------------------------
// 3DProasm.S interface

extern uint8_t				// Sidewinder 3D Pro packet buffers
    sw_idbuf[60],
    sw_packet3[8],
    sw_packet2[8],
    sw_packet1[8] ;

extern uint8_t				// Sidewinder PP/FFP packet buffer
    ffp_packet[6] ;

extern volatile uint8_t
    sw_pktptr,				// LSB of -> to current byte in buffer
    sw_clkcnt ;				// no. of clock ticks on B1

extern void
    CopyFFPData( uint8_t *packet ),	// Copy PP/FFP data from packet to sw_report
    Copy3DPData( uint8_t *packet ) ;	// Copy 3DP data from packet to sw_report

extern uint8_t
    QueryFFP( uint8_t id, uint8_t sz ),	// Read data from PP/FFP
    Query3DP( uint8_t id, uint8_t sz ) ;// Read data from 3DPro

//-------------------------------------------------------------------------------

#else // __ASSEMBLER__

//-------------------------------------------------------------------------------
// Assembler only definitions

#if F_CPU == 16000000

// Receiver timeout

#define	T6TO48US	244		/* ( 48*16/64) = 12 (0-12 = 244) */
#define	T6TO100US	231		/* (100*16/64) = 25 (0-25 = 231) */

// Initial rec. timeouts

#define	T6TO400US	156		/* (400*16/64) = 100 (0-100 = 156) */
#define	T6TO150US	218		/* (150*16/64) = 37.5 (0-38 = 218) */

#else	// F_CPU undefined or out of range
 #error	"F_CPU must be 16000000"
#endif	// F_CPU

//-------------------------------------------------------------------------------

#endif	// __ASSEMBLER__

//-------------------------------------------------------------------------------

#endif	// _3DPRO_H_
