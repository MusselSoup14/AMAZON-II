
/******************************************************************************
 Copyright (C) 2013      Advanced Digital Chips Inc.
						http://www.adc.co.kr
 Author : Software Team.
******************************************************************************/

#include "sdk.h"


void lcdon()
{
    *R_TPCTRL( 1 ) = 1 << 1;
    *R_TPCTRL( 1 ) &= ~( 1 << 1 ); // Timer counter reset.

    // TMCNT = ( Set_time(s) * APB Clk(Hz) / (1/Pre-scaler Factor) )  // APB clock about 50MHz.

    *R_TMCNT( 1 ) = 120000; // timer pwm 0 peirod 200Hz
    *R_TMDUT( 1 ) = 118800;	// timer pwm 0 duty
    *R_TMCTRL( 1 ) = 1 << 14 | 1 << 6 | 1 << 5 | 1; // timer pwm, Pre-scale 1/8, pwm enable.
}

void lcd_bright( U32 percent )
{
    if( percent > 99 )
        percent = 99;
    *R_TMDUT( 1 ) = percent * 1200;	// timer pwm 0 duty
}


void boardinit()
{
    *R_PAF( 0 ) = 0x0;             //NAND
    *R_PAF( 1 ) = 0x0;             //NAND , UART0
    *R_PAF( 2 ) = 0x3 << 14 | 0x1 << 12; // SDHC, PIO2.6, SRAM_CS1#
    *R_PAF( 3 ) = 0x3fff;          // PIO3.0 ~ PIO 3.6, ICE_VCLK
    *R_PAF( 4 ) = 0x0;             //ICE_VDIN0 ~ ICE_VDIN7
    *R_PAF( 5 ) = 0x0;             // SPI_CS0#, SPI_SCK0, SPI_MISO0, SPI_MOSI0, TWI_SCL, TWI_SDA, SF_CS#, SF_CLK
    *R_PAF( 6 ) = 0x3 << 14 | 0x3 << 12; // SF_D0 ~ SF_D3, SRAM_WE#, SRAM_RE#, PIO6.6, PIO6.7
    *R_PAF( 7 ) = 0x0;             // SRAM_A0 ~ SRAM_A7
    *R_PAF( 8 ) = 0x3 << 14;       // SRAM_A8 ~ SRAM_A14, PIO8.7(ETHERNET IRQ)
    *R_PAF( 9 ) = 0x0;             // SRAM_D0 ~ SRAMD7
    *R_PAF( 10 ) = 0x0;            // SRAM_D8 ~ SRAMD15
    *R_PAF( 11 ) = 0x1 << 14 | 0x1 << 12 | 0x1 << 10 | 0x2 << 8 | 0x2 << 6 | 0x2 << 4 | 0x2 << 2 | 0x2 << 0; // BOOTMODE[0] ~ BOOTMODE[4], LCD_VCLKIN, EIRQ1, LCD_VCLKOUT
    *R_PAF( 12 ) = 0x1 << 6 | ( 0x1 << 2 ) | 0x3 << 0; // PIO12.0 (USB_HOST_POW), USB_HOST_CUR, PWM_OUT1, SRAM_CS3
    *R_PAF( 13 ) = 0xffff;         // PIO13.0 ~ PIO13.7
    *R_PAF( 14 ) = 0xffff;         // PIO14.0 ~ PIO14.7

}
