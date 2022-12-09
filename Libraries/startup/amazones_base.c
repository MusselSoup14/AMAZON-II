/*****************************************************************************
*
* Copyright (C) 2014      Adchips Inc. All Rights Reserved.
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

void lcdon()
{
    *R_TPCTRL( 1 ) = 1 << 1;
    *R_TPCTRL( 1 ) &= ~( 1 << 1 ); // Timer counter reset.

    // TMCNT = ( Set_time(s) * APB Clk(Hz) / (1/Pre-scaler Factor) )  // APB clock about 50MHz.

    *R_TMCNT( 1 ) = 120000; // timer pwm 0 peirod 200Hz
    *R_TMDUT( 1 ) = 118800;	// timer pwm 0 duty
    *R_TMCTRL( 1 ) = 1 << 14 | 1 << 6 | 1 << 5 | 1; // timer pwm, Pre-scale 1/8, pwm enable.
	
     // LCD On	
    *R_GPODIR(12) = 1<<3;   
    *R_GPOHIGH(12) = 1<<3;	
}

void lcdoff()
{
	*R_GPOLOW(12) = 1<<3;	
}

void lcd_bright( U32 percent )
{
    if( percent > 99 )
        percent = 99;
    *R_TMDUT( 1 ) = percent * 1200;	// timer pwm 0 duty
}


void port_init()
{
#if USE_AMAZON_STK
    *R_PAF( 0 ) = 0x0 << 14 | 0x0 << 12 | 0x0 << 10 | 0x0 << 8 | 0x0 << 6 | 0x0 << 4 | 0x0 << 2 | 0x0 << 0; // NAND
                          
    *R_PAF( 1 ) = 0x0 << 14 | 0x0 << 12 | 0x0 << 10 | 0x0 << 8 | 0x0 << 6 | 0x0 << 4 | 0x0 << 2 | 0x0 << 0; // NAND , UART0
                          
    *R_PAF( 2 ) = 0x3 << 14 | 0x3 << 12 | 0x0 << 10 | 0x0 << 8 | 0x0 << 6 | 0x0 << 4 | 0x0 << 2 | 0x0 << 0; // PIO 2.7, PIO 2.6, SDHC_CMD, SDHC_CLK, SDHC_D3, SDHC_D2, SDHC_D1, SDHC_D0                        
                          
    *R_PAF( 3 ) = 0x0 << 14 | 0x3 << 12 | 0x3 << 10 | 0x3 << 8 | 0x3 << 6 | 0x3 << 4 | 0x3 << 2 | 0x3 << 0; // ICE_VCLK, PIO 3.6, PIO 3.5, PIO 3.4, PIO 3.3, PIO 3.2, PIO 3.1, PIO 3.0    
                          
    *R_PAF( 4 ) = 0x0 << 14 | 0x0 << 12 | 0x0 << 10 | 0x0 << 8 | 0x0 << 6 | 0x0 << 4 | 0x0 << 2 | 0x0 << 0; // ICE_VDIN0 ~ ICE_VDIN7
				
    *R_PAF( 5 ) = 0x0 << 14 | 0x0 << 12 | 0x0 << 10 | 0x0 << 8 | 0x3 << 6 | 0x3 << 4 | 0x3 << 2 | 0x3 << 0; // SF_CLK, SF_CS#, TWI_SDA, TWI_SCL, PIO 5.3, PIO 5.2, PIO 5.1 PIO 5.0

    *R_PAF( 6 ) = 0x3 << 14 | 0x3 << 12 | 0x3 << 10 | 0x3 << 8 | 0x0 << 6 | 0x0 << 4 | 0x0 << 2 | 0x0 << 0; // PIO 6.7, PIO 6.6, PIO 6.5 PIO 6.4, SF_D3, SD_D2, SF_D1, SF_D0 
	
    *R_PAF( 7 ) = 0x3 << 14 | 0x3 << 12 | 0x3 << 10 | 0x3 << 8 | 0x3 << 6 | 0x3 << 4 | 0x3 << 2 | 0x3 << 0; // PIO 7.7, PIO 7.6, PIO 7.5 PIO 7.4, PIO 7.3, PIO 7.2, PIO 7.1, PIO 7.0 
	
    *R_PAF( 8 ) = 0x3 << 14 | 0x3 << 12 | 0x3 << 10 | 0x3 << 8 | 0x3 << 6 | 0x3 << 4 | 0x3 << 2 | 0x3 << 0; // PIO 8.7, PIO 8.6, PIO 8.5 PIO 8.4, PIO 8.3, PIO 8.2, PIO 8.1, PIO 8.0 
	
    *R_PAF( 9 ) = 0x3 << 14 | 0x3 << 12 | 0x3 << 10 | 0x3 << 8 | 0x3 << 6 | 0x3 << 4 | 0x3 << 2 | 0x3 << 0; // PIO 9.7, PIO 9.6, PIO 9.5 PIO 9.4, PIO 9.3, PIO 9.2, PIO 9.1, PIO 9.0 
	
    *R_PAF( 10 ) = 0x3 << 14 | 0x3 << 12 | 0x3 << 10 | 0x3 << 8 | 0x3 << 6 | 0x3 << 4 | 0x3 << 2 | 0x3 << 0; // PIO 10.7, PIO 10.6, PIO 10.5 PIO 10.4, PIO 10.3, PIO 10.2, PIO 10.1, PIO 10.0 
		
    *R_PAF( 11 ) = 0x1 << 14 | 0x3 << 12 | 0x3 << 10 | 0x1 << 8 | 0x1 << 6 | 0x1 << 4 | 0x1 << 2 | 0x1 << 0; // LCD_VCLKOUT, PIO 11.6, PIO 11.5, BOOTMODE[4], BOOTMODE[3], BOOTMODE[2], BOOTMODE[1], BOOTMODE[0] 
		
    *R_PAF( 12 ) = 0x3 << 6 | 0x0 << 4 |  0x1 << 2  | 0x3 << 0; // PIO 12.3, PWM_OUT1, USB_HOST_CUR, PIO 12.0
		
    *R_PAF( 13 ) = 0x3 << 14 | 0x3 << 12 | 0x3 << 10 | 0x3 << 8 | 0x3 << 6 | 0x3 << 4 | 0x3 << 2 | 0x3 << 0; // PIO 13.7, PIO 13.6, PIO 13.5 PIO 13.4, PIO 13.3, PIO 13.2, PIO 13.1, PIO 13.0 
	
    *R_PAF( 14 ) = 0x3 << 14 | 0x3 << 12 | 0x3 << 10 | 0x3 << 8 | 0x3 << 6 | 0x3 << 4 | 0x3 << 2 | 0x3 << 0; // PIO 14.7, PIO 14.6, PIO 14.5 PIO 14.4, PIO 14.3, PIO 14.2, PIO 14.1, PIO 14.0 	
    
	*R_GPDIR(13) |= (0x7 << 5);  // wm8974 setting pio.

	// for SW 1-4, INPUT Mode
	*R_GPIDIR(14) = 0xf<<4;

	//LED 1-4, OUTPUT Mode
	*R_GPODIR(14) = 0xf;
	*R_GPOLOW(14) = 0xf;// ON
#endif
}
