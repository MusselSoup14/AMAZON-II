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
/**
 * Creates a timer with the specified time-out value.
 *		TICK_PER_SEC  = APB_CLOCK/32786  
 * 		MAX seconds = 0xffffffff /TICK_PER_SEC 
 * 		MAX milliseconds = MAX seconds * 1000 
 * 		
 * \param nCh Number of channel.
 * \param ms  The milliseconds.
 * \return true if it succeeds, false if ms is too big
 * 		   
 * \code
 * void mytimer0_isr()
 * {
 *		....
 * }
 * 
 * void func()
 * {
 *		set_interrupt(INT_TIMER0,mytimer0_isr);
 *		set_timer(0,1000);//1sec
 * }
 * \endcode
 **/
BOOL set_timer( int nCh, U32 ms )
{
    static const int timerintnum[] = {INTNUM_TIMER0, INTNUM_TIMER1, INTNUM_TIMER2, INTNUM_TIMER3};
    if( nCh > MAX_TIMER_CHANNEL )
        return FALSE;
    int i;

    U32 nanosecpertick;
    U32 cnt;
    U32 pres;

    U32 scaler[] = {2, 8, 32, 128, 512, 2048, 8192, 32768, 0};

    U64 usec = ms * 1000;
    i = 0;
    while( 1 )
    {
        pres = scaler[i];
        if( pres == 0 )
            return FALSE;
        nanosecpertick = 1000000000 / ( get_apb_clock() / pres );
        cnt = ( usec * 1000 ) / nanosecpertick;

        if( cnt < 0xffffffff ) // ms is too big to set, adjust PRESACLE.
            break;
        i++;
    }

    //reset
    *R_TPCTRL( nCh ) = 1 << 1;
    *R_TPCTRL( nCh ) = 0;

    *R_TMCNT( nCh ) = cnt;
    *R_TMCTRL( nCh ) = ( 1 << 7 ) | ( i << 1 ) | 1;
    enable_interrupt( timerintnum[nCh], TRUE );
    return TRUE;
}
/**
 * Stops a timer.
 *
 * \param nCh Number of channel.
 *
 * \return true if it succeeds, false if it fails.
 */
BOOL stop_timer( int nCh )
{
    if( nCh > MAX_TIMER_CHANNEL )
        return FALSE;
    if( *R_TMCTRL( nCh ) & 1 ) //check enabled
        *R_TMCTRL( nCh ) = 0 | ( 1 << 7 ); //stop
    return TRUE;
}
