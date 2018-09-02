/******************************************************************************
 * File Name	: 3DPro.c
 * Project	: 3DP-Vert, Microsoft Sidewinder 3D Pro/PP/FFP to USB converter
 * Date		: 2005/05/31, 2006/12/14, 2008/02/12, 2009/06/26
 * Version	: 4.0 + MODS
 * Target MCU	: AT90USB162/82, AT90USB646/1286, ATMEGA16U4/32U4
 * Tool Chain	: Atmel AVR Studio 4.18 716 / WinAVR 20100110
 * Author	: Detlef "Grendel" Mueller
 *		  detlef@gmail.com
 * References	: Sidewinder.c by Vojtech Pavlik
 *		  Patent #5628686, Apperatus and Method for Bidirectional
 *			Data Communication in a Gameport, by Microsoft
 * Description	: 3D Pro driver and hardware interface
 * Device	: Microsoft SideWinder 3D Pro, Precision Pro, Force Feedback Pro
 *
 * Release Notes:
 *
 *	================
 *	WARNING: This version has been heavily modified to only support
 *           Force Feedback Pro and ATmega32U4.
 *
 *             -- tloimu
 *	================
 *
 *
 *	Pinout AT90USBX2 (out dated!)
 *	================
 *
 *	PB0	--	PC0	(XTAL2)	 PD0	Button1
 *	PB1	(SCLK)	PC1	(/RESET) PD1	Button2
 *	PB2	(MOSI)	PC2	--	 PD2	Button3
 *	PB3	(MISO)			 PD3	Button4
 *	PB4	X1 axis	PC4	--	 PD4	--
 *	PB5	Y2 axis	PC5	--	 PD5	--
 *	PB6	--	PC6	--	 PD6	LED
 *	PB7	--	PC7	--	 PD7	(HWB)
 *
 *	Pinout ATMEGAXU4
 *	================
 *
 *	PB0	Button1		PD0	Button1	(INT)	PF0	--
 *	PB1	Button2		PD1	--				PF1	--
 *	PB2	Button3		PD2	--				PE2	--
 *	PB3	Button4		PD3	MIDI (out)
 *	PB4	X1 axis			PD4	--			PF4	--
 *	PB5	Y2 axis			PD5	--			PF5	--
 *	PB6	--	PC6	--	PD6	LED	PE6	--	PF6	--
 *	PB7	--	PC7	--	PD7	--			PF7	--
 *
 *	Pinout AT90USBX6 (out dated!)
 *	================
 *
 *	PA0	--	PB0	--	PC0	--	PD0	Button1	PE0	--	PF0	--
 *	PA1	--	PB1	--	PC1	--	PD1	Button2	PE1	--	PF1	--
 *	PA2	--	PB2	--	PC2	--	PD2	Button3	PE2	--	PF2	--
 *	PA3	--	PB3	--	PC3	--	PD3	Button4	PE3	--	PF3	--
 *	PA4	--	PB4	X1 axis	PC4	--	PD4	--	PE4	--	PF4	--
 *	PA5	--	PB5	Y2 axis	PC5	--	PD5	--	PE5	--	PF5	--
 *	PA6	--	PB6	--	PC6	--	PD6	LED	PE6	--	PF6	--
 *	PA7	--	PB7	--	PC7	--	PD7	--	PE7	--	PF7	--
 *
 * $Id: 3DPro.c 1.6 2010/04/23 05:27:52 Detlef Exp Detlef $
 ******************************************************************************/

#include "3DPro.h"
#include "ffb.h"
#include <string.h>

//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------

#define	DDO( b )	_B1( b )	/* data direction: out */
#define	DDI( b )	_B0( b )	/* data direction: in  */
#define	PU1( b )	_B1( b )	/* pull up on (if DDI) */
#define	PU0( b )	_B0( b )	/* pull up off */

// Port B

#define PBPU		(PU1( PB7) | PU1( PB6) | PU0( PB5) | PU0( PB4) | \
			 PU1( PB3) | PU1( PB2) | PU1( PB1) | PU1( PB0))
#define DDB		(DDI(DDB7) | DDI(DDB6) | DDI(DDB5) | DDI(DDB4) | \
			 DDI(DDB3) | DDI(DDB2) | DDI(DDB1) | DDI(DDB0))

// Port D

#if defined(__AVR_AT90USBX2__)

#define PDPU		(PU0( PD7) | _B0( PD6) | PU1( PD5) | PU1( PD4) | \
			 PU1( PD3) | PU1( PD2) | PU1( PD1) | PU1( PD0))

#elif defined(__AVR_AT90USB646__)

#define PDPU		(PU1( PD7) | _B0( PD6) | PU1( PD5) | PU1( PD4) | \
			 PU1( PD3) | PU1( PD2) | PU1( PD1) | PU1( PD0))

#elif defined(__AVR_ATmegaXU4__) || defined(__AVR_AT90USB1286__)

#define PDPU		(PU1( PD7) | _B1( PD6) | PU1( PD5) | PU1( PD4) | \
			 PU0( PD3 ) | PU1( PD2) | PU1( PD1) | PU1( PD0))
// ???? removed PU1( PD3) for MIDI out via USART TX

#endif

#define DDD		(DDI(DDD7) | DDO(DDD6) | DDI(DDD5) | DDI(DDD4) | \
			 DDO(DDD3) | DDI(DDD2) | DDI(DDD1) | DDI(DDD0))
// Ports A,C,E,F

#if defined(__AVR_AT90USBX2__)

#define PCPU		(PU1( PC7) | PU1( PC6) | PU1( PC5) | PU1( PC4) | \
			             PU1( PC2) | PU1( PC1) | PU1( PC0))
#define DDC		(DDI(DDC7) | DDI(DDC6) | DDI(DDC5) | DDI(DDC4) | \
			             DDI(DDC2) | DDI(DDC1) | DDI(DDC0))

#elif defined(__AVR_ATmegaXU4__)

#define PCPU		(PU1( PC7) | PU1( PC6))
#define DDC		(DDI(DDC7) | DDI(DDC6))

#define PEPU		(PU1( PE6) | PU0( PE2))
#define DDE		(DDI(DDE6) | DDI(DDE2))

#define PFPU		(PU1( PF7) | PU1( PF6) | PU1( PF5) | PU1( PF4) | \
						 PU1( PF1) | PU1( PF0))
#define DDF		(DDI(DDF7) | DDI(DDF6) | DDI(DDF5) | DDI(DDF4) | \
						 DDI(DDF1) | DDI(DDF0))
#elif defined(__AVR_AT90USBX6__)

#define PAPU		(PU1( PA7) | PU1( PA6) | PU1( PA5) | PU1( PA4) | \
			 PU1( PA3) | PU1( PA2) | PU1( PA1) | PU1( PA0))
#define DDA		(DDI(DDA7) | DDI(DDA6) | DDI(DDA5) | DDI(DDA4) | \
			 DDI(DDA3) | DDI(DDA2) | DDI(DDA1) | DDI(DDA0))
#define PCPU		(PU1( PC7) | PU1( PC6) | PU1( PC5) | PU1( PC4) | \
			 PU1( PC3) | PU1( PC2) | PU1( PC1) | PU1( PC0))
#define DDC		(DDI(DDC7) | DDI(DDC6) | DDI(DDC5) | DDI(DDC4) | \
			 DDI(DDC3) | DDI(DDC2) | DDI(DDC1) | DDI(DDC0))
#define PEPU		(PU1( PE7) | PU1( PE6) | PU1( PE5) | PU1( PE4) | \
			 PU1( PE3) | PU1( PE2) | PU1( PE1) | PU1( PE0))
#define DDE		(DDI(DDE7) | DDI(DDE6) | DDI(DDE5) | DDI(DDE4) | \
			 DDI(DDE3) | DDI(DDE2) | DDI(DDE1) | DDI(DDE0))
#define PFPU		(PU1( PF7) | PU1( PF6) | PU1( PF5) | PU1( PF4) | \
			 PU1( PF3) | PU1( PF2) | PU1( PF1) | PU1( PF0))
#define DDF		(DDI(DDF7) | DDI(DDF6) | DDI(DDF5) | DDI(DDF4) | \
			 DDI(DDF3) | DDI(DDF2) | DDI(DDF1) | DDI(DDF0))
#endif

#define	dis3DP_INT()	clr_bit( EIMSK, INT0 )

#define TRG_pull()	__WRAP__( {				\
				clr_bit( TRGDDR, TRGX1BIT ) ;	\
				clr_bit( TRGDDR, TRGY2BIT ) ;	\
			} )
#define TRG_rel()	__WRAP__( {				\
				set_bit( TRGDDR, TRGX1BIT ) ;	\
				set_bit( TRGDDR, TRGY2BIT ) ;	\
			} )

//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------

uint8_t
    sw_id,					// ID of detected stick
    sw_report[SW_REPSZ_3DP + ADDED_REPORT_DATA_SIZE] ;			// USB report data

static uint8_t
    sw_buttons,					// button buffer
    sw_problem ;				// problem counter

//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------

// Delay using T1 as timing reference, assuming prescaler /1024

static void FA_NOINLINE( Delay_1024 ) ( uint16_t time )
{
    set_bit( GTCCR, PSRSYNC ) ;			// reset prescaler

    ResetTM( 1, time ) ;			// set timer

    for ( ; ! TMexp( 1 ) ; )			// wait until timer expired
	;
}

//------------------------------------------------------------------------------

// Delay using T0 as timing reference, assuming prescaler /64
/*
static void FA_NOINLINE( Delay_64 ) ( uint8_t time )
{
    set_bit( GTCCR, PSRSYNC ) ;			// reset prescaler

    ResetTM( 0, time ) ;			// set timer

    for ( ; ! TMexp( 0 ) ; )			// wait until timer expired
	;
}

//------------------------------------------------------------------------------

// Trigger the stick

static void FA_NOINLINE( Trigger ) ( void )
{
    TRG_pull() ;
    Delay_64( T6DEL48US ) ;			// _delay_us( 47.8125 ) is max @ 16MHz
    TRG_rel() ;
}
*/
//------------------------------------------------------------------------------

// Check if the FFP/PP data packet at *pkt is valid by xoring all bits together.
// Result must be 1 for the packet to have correct parity.

static uint8_t FA_NOINLINE( CheckFFPPkt ) ( uint8_t *pkt, uint8_t pkt_size )
{
    uint8_t
	x ;

    x  = pkt[2] ^ pkt[3] ^ pkt[4] ^ pkt[5] ;
	if ( pkt_size == 16 )
		x ^= pkt[0] ^ pkt[1];

    x ^= x >> 4 ;
    x ^= x >> 2 ;
    x ^= x >> 1 ;

	if ( pkt_size == 11 )
		x ^= (pkt[1]) ;
	
    return ( x & 1 ) ;
}

//------------------------------------------------------------------------------

// Hang out 12ms w/ the LED on for the initial 4ms

static void FA_NOINLINE( Flash_LED_12MS ) ( void )
{
    LED_on() ;
    Delay_1024( T0DEL4MS ) ;
    LED_off() ;
    Delay_1024( T0DEL8MS ) ;
}

//------------------------------------------------------------------------------

// Reboot converter. Kill USB and wait for the watchdog to catch us.
// Note: the watchdog is unleashed in main() after init_hw() returns.

static void FA_NORETURN( reboot ) ( void )
{
    cli() ;					// Disable interrupts

    USBCON = _B0(USBE) | _B1(FRZCLK) ;		// Kill USB

    for ( ;; )					// Wait for watchdog to bite (.5s)
	;
}

//------------------------------------------------------------------------------

// Initialize the hardware
//
// init_hw() is called only once, first thing in main(),
// so it doesn't need a stack frame. Saves lots of push/pops.

void FA_NAKED( init_hw ) ( void )
{
  // Initialize ports (ATmega32U4)
    DDRE  = DDE ;
    PORTE = PEPU ;
    DDRF  = DDF ;
    PORTF = PFPU ;
//  #endif

    // Initialize timers

    SetTMPS( 0, 64 ) ;				// Set T0 prescaler to / 64 for query & us delay
    SetTMPS( 1, 1024 ) ;			// Set T1 prescaler to / 1024 for ms delay

    EICRA  = _B1(ISC01) | _B1(ISC00) ;		// Need INT0 on rising edges

    Delay_1024( T0DEL200MS ) ;			// Allow the stick to boot

    for ( ;; )					// Forever..
	    {
		Flash_LED_12MS() ;			// Flash LED and wait
		Delay_1024( T0DEL200MS ) ;
							// Try to read a data packet,
		QueryFFP( 0, 126 ) ;			// don't know how long - let it time out

		Flash_LED_12MS() ;

		// Analyze clock count

		if ( ! ~sw_clkcnt )
		    sw_clkcnt = 0 ;

		if ( sw_clkcnt == (DATSZFFP+1) &&
			CheckFFPPkt( ffp_packet, DATSZFFP ) )
			{
				sw_id = SW_ID_FFP ;
				// FfbSetDriver(0); // Force Feedback Pro MIDI
				FfbSetDriver(0); // Force Feedback Pro with Direct L298N
				break;
			}

		if ( sw_clkcnt == (DATSZFFPW+1) &&
			CheckFFPPkt( ffp_packet, DATSZFFPW ) )
			{
				sw_id = SW_ID_FFPW ;
				FfbSetDriver(0);
				break;
			}
			
		dis3DP_INT() ;
	    }

    dis3DP_INT() ;				// Disable INT

    cli() ;					// Disable interrupts

    sw_buttons = BUTMSK ;			// All buttons released

    RET() ;					// naked also means no ret..
}

//------------------------------------------------------------------------------

// Read the stick and create a report

void getdata ( void )
	{
    uint8_t
	pkt_size, i ;

	pkt_size = (sw_id == SW_ID_FFPW ? DATSZFFPW : DATSZFFP);
	
	i = QueryFFP( 0, pkt_size ) ;		// Query FFP

    dis3DP_INT() ;

    if ( ! i )					// If query timed out,
	    {
		if ( ++sw_problem > 10 )		// 11th problem in a row
	    	reboot() ;				// We lost the stick, lets start over
    	}
    else
    	{
		sw_problem = 0 ;			// Reset problem counter

		if ( CheckFFPPkt( ffp_packet, pkt_size ) )	// If PP/FFP packet ok
			{
	    	// LED_on() ;			// Signal good packet read

			if ( sw_id == SW_ID_FFPW )
				memcpy(sw_report, ffp_packet, 6);		// Copy data into report
			else
				CopyFFPData( ffp_packet ) ;		// Copy data into report
			}
			
		}
	}

//------------------------------------------------------------------------------
