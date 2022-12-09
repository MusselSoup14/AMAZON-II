/*****************************************************************************
*
* Copyright (C) 2014      Advanced Digital Chips, Inc. All Rights Reserved.
*						http://www.adc.co.kr
*
* THIS SOFTWARE IS PROVIDED BY ADCHIPS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ADCHIPS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE
*
*****************************************************************************/
#include "sdk.h"

/*
valid prescale value
2,8,32,128,512,2048,8192,23768
Tick/sec  = (APB clock)/prescale
if APB clock is 48Mhz and prescale is 2048.
	1 tick duration = 1000/(APB clock/prescale) = 0.042ms

if you want to more accuracy, PRESCALE value is should be down
*/
static volatile int delaytimerisrtick = 0;
static void delaytimerisr()
{
    delaytimerisrtick++;
}

BOOL delayus( int us )
{
    int nch;
    void* orgisr;
    //check watchdog timer

	if( ( ( *R_WDTCTRL ) & 1 ) == 0 )
    {
		delaytimerisrtick = 0;
        int tickperms = get_apb_clock() / 1000000;
        long long cnt = ( long long )us * tickperms;
        int loopcnt = cnt >> 32;   
		
		orgisr  = get_interrupt(INTNUM_WATCHDOG);
		set_interrupt(INTNUM_WATCHDOG,delaytimerisr);
        enable_interrupt(INTNUM_WATCHDOG, TRUE);
        
		if(loopcnt)
		{			
			*R_WDTCNT = 0xffffffff;
			*R_WDTCTRL = ( 1 << 1 ) | 1; //interrupt mode, enable					
			
			while(1)
			{
				if(loopcnt <= delaytimerisrtick)
				{					
					*R_WDTCTRL = 0;
					delaytimerisrtick = 0;		
					break;
				}
			}			
		}		
		
		*R_WDTCNT = cnt & 0xffffffff;
		*R_WDTCTRL = ( 1 << 1 ) | 1; //interrupt mode, enable					
			
		while(1)
		{
			if(delaytimerisrtick)
			{
				*R_WDTCTRL = 0;
				break;
			}
		}							
        enable_interrupt(INTNUM_WATCHDOG, FALSE);
		set_interrupt(INTNUM_WATCHDOG, orgisr );
        return TRUE;
    }	

    U32 conval;
    for( nch = 0; nch < MAX_TIMER_CHANNEL + 1; nch++ )
    {
        if( ( ( *R_TMCTRL( nch ) ) & 1 ) == 0 )
            break;
    }
    if( nch == MAX_TIMER_CHANNEL )
    {
        return FALSE;
    }

    int prescale = 2;
    conval = 0; //prescale 0

    INTERRUPT_TYPE intnum ;
    switch( nch )
    {
    case 0:
        intnum = INTNUM_TIMER0;
        break;
    case 1:
        intnum = INTNUM_TIMER1;
        break;
    case 2:
        intnum = INTNUM_TIMER2;
        break;
    case 3:
        intnum = INTNUM_TIMER3;
        break;
    default:
        return FALSE;
    }
    delaytimerisrtick = 0;
    orgisr  = get_interrupt( intnum );
    set_interrupt( intnum, delaytimerisr );
    enable_interrupt( intnum, TRUE );

    U32 waitdelaytimerisrtick = 0;
    U32 tickperus = get_apb_clock() / ( prescale * 1000000 );
    long long waittick = ( long long )tickperus * us;
    waitdelaytimerisrtick = ( U32 )( waittick >> 16 );
    if( waitdelaytimerisrtick )
    {
        //reset
        *R_TPCTRL( nch ) = 1 << 1;
        *R_TPCTRL( nch ) = 0;

        *R_TMCNT( nch ) = 0xffff; //max
        *R_TMCTRL( nch ) = ( 1 << 7 ) | conval | 1;

        while( 1 )
        {
            if( waitdelaytimerisrtick <= delaytimerisrtick )
            {
                stop_timer( nch );
                break;
            }
        }
    }
    delaytimerisrtick = 0;
    //reset
    *R_TPCTRL( nch ) = 1 << 1;
    *R_TPCTRL( nch ) = 0;

    *R_TMCNT( nch ) = waittick & 0xffff;
    *R_TMCTRL( nch ) = ( 1 << 7 ) | conval | 1;

    while( 1 )
    {
        if( delaytimerisrtick )
        {
            stop_timer( nch );
            break;
        }
    }

    enable_interrupt( intnum, FALSE );
    set_interrupt( intnum, orgisr );
    return TRUE;
}

BOOL delayms( U32 ms )
{
    int nch;
    void* orgisr;
    //check watchdog timer	
    if( ( ( *R_WDTCTRL ) & 1 ) == 0 )
    {		
		delaytimerisrtick = 0;
        int tickperms = get_apb_clock() / 1000;
        long long cnt = ( long long )ms * tickperms;
        int loopcnt = cnt >> 32;
		orgisr  = get_interrupt(INTNUM_WATCHDOG);
		set_interrupt(INTNUM_WATCHDOG,delaytimerisr);
        enable_interrupt(INTNUM_WATCHDOG, TRUE);
        
		if(loopcnt)
		{			
			*R_WDTCNT = 0xffffffff;
			*R_WDTCTRL = ( 1 << 1 ) | 1; //interrupt mode, enable					
			
			while(1)
			{
				if(loopcnt <= delaytimerisrtick)
				{
					*R_WDTCTRL = 0;
					delaytimerisrtick = 0;		
					break;
				}
			}			
		}		
		
		*R_WDTCNT = cnt & 0xffffffff;
		*R_WDTCTRL = ( 1 << 1 ) | 1; //interrupt mode, enable					
			
		while(1)
		{
			if(delaytimerisrtick)
			{
				*R_WDTCTRL = 0;
				break;
			}
		}					
        enable_interrupt(INTNUM_WATCHDOG, FALSE);
		set_interrupt(INTNUM_WATCHDOG, orgisr );
        return TRUE;
    }

    U32 conval;
    for( nch = 0; nch < MAX_TIMER_CHANNEL + 1; nch++ )
    {
        if( ( ( *R_TMCTRL( nch ) ) & 1 ) == 0 )
            break;
    }
    if( nch == MAX_TIMER_CHANNEL )
    {
        return FALSE;
    }

    int prescale = 128;
    conval = 3 << 1;

    INTERRUPT_TYPE intnum ;
    switch( nch )
    {
    case 0:
        intnum = INTNUM_TIMER0;
        break;
    case 1:
        intnum = INTNUM_TIMER1;
        break;
    case 2:
        intnum = INTNUM_TIMER2;
        break;
    case 3:
        intnum = INTNUM_TIMER3;
        break;
    default:
        return FALSE;
    }
    delaytimerisrtick = 0;
    orgisr  = get_interrupt( intnum );
    set_interrupt( intnum, delaytimerisr );
    enable_interrupt( intnum, TRUE );

    U32 waitdelaytimerisrtick = 0;
    U32 tickperms = get_apb_clock() / ( prescale  * 1000 );
    long long waittick = ( long long )tickperms * ms;
    waitdelaytimerisrtick = ( U32 )( waittick >> 16 );
    if( waitdelaytimerisrtick )
    {
        //reset
        *R_TPCTRL( nch ) = 1 << 1;
        *R_TPCTRL( nch ) = 0;

        *R_TMCNT( nch ) = 0xffff; //max
        *R_TMCTRL( nch ) = conval | ( 1 << 7 ) | 1;

        while( 1 )
        {
            if( waitdelaytimerisrtick <= delaytimerisrtick )
            {
                stop_timer( nch );
                break;
            }
        }
    }
    delaytimerisrtick = 0;
    //reset
    *R_TPCTRL( nch ) = 1 << 1;
    *R_TPCTRL( nch ) = 0;

    *R_TMCNT( nch ) = waittick & 0xffff;
    *R_TMCTRL( nch ) = conval | ( 1 << 7 ) | 1;

    while( 1 )
    {
        if( delaytimerisrtick )
        {
            stop_timer( nch );
            break;
        }
    }

    enable_interrupt( intnum, FALSE );
    set_interrupt( intnum, orgisr );
    return TRUE;
}


