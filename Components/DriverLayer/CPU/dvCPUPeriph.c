/**
  ******************************************************************************
  * @file    dvCPUPeriph.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    02-August-2014
  * @brief   This file provides basic the CPU firmware functions.
  ******************************************************************************
  * @attention
  *
  * MadeFactory owns the sole copyright to this software. Under international
  * copyright laws you (1) may not make a copy of this software except for
  * the purposes of maintaining a single archive copy, (2) may not derive
  * works herefrom, (3) may not distribute this work to others. These rights
  * are provided for information clarification, other restrictions of rights
  * may apply as well.
  *
  * <h2><center>&copy; Copyright 1997-2016 (c) MadeFactory Inc.</center></h2>
  ******************************************************************************
  */  
/* Includes ------------------------------------------------------------------*/
#include "dvCPUPeriphAPI.h"
#include "dvGPIO.h"
#include "dvGPIOAPI.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
void display_clock_init(); // amzon2kInit.c

//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************

//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************

// ***************************************************************************
// **************** START OF ISR PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************
/**
 *  @brief      Configures the different system clocks.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void CPUPeriphClockIntialize(void)
{
  /* Display Clock Init on "halLCDCtrlInitialize()" */

  /* I2S Audio Clock Init */
  sound_init();
#if USE_I2S_EXTERNAL_CLOCK
  // Re-Init the Mixer Out Register (MIXER_OUT) to 12.288MHz from External Audio Clock
  *R_SNDOUT(0) = 0x00008003; // prescale_on, div1, 256fs, 64fs, i2s, 48Khz
#endif
}


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *  @brief      This API must be called once during system startup, before calls are made
                to any other APIs in the Chip Driver component.  
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvCPUPeriphInitialize(void)
{

#if (USE_AMAZON_STK)
  /* AF settings. Do not allocate AF as GPIO here. AF as GPIO set in dvGPIO.c  */ 
  {
    // PIO 2.7, PIO 2.6, SDHC_CMD, SDHC_CLK, SDHC_D3, SDHC_D2, SDHC_D1, SDHC_D0                                                
    //*R_PAF( 2 ) = 0x3 << 14 | 0x3 << 12 | 0x0 << 10 | 0x0 << 8 | 0x0 << 6 | 0x0 << 4 | 0x0 << 2 | 0x0 << 0; 
    *R_PAF(2) = 0x3 << 14 | 0x3 << 12 | 0x0 << 10 | 0x0 << 8 | 0x0 << 6 | 0x0 << 4 | 0x0 << 2 | 0x0 << 0; 
    // ICE_VCLK, PIO 3.6, PIO 3.5, PIO 3.4, PIO 3.3, PIO 3.2, PIO 3.1, PIO 3.0                          
    *R_PAF( 3 ) = 0x0 << 14 | 0x3 << 12 | 0x3 << 10 | 0x3 << 8 | 0x3 << 6 | 0x3 << 4 | 0x3 << 2 | 0x3 << 0; 
    // ICE_VDIN0 ~ ICE_VDIN7                        
    *R_PAF( 4 ) = 0x0 << 14 | 0x0 << 12 | 0x0 << 10 | 0x0 << 8 | 0x0 << 6 | 0x0 << 4 | 0x0 << 2 | 0x0 << 0; 
    // SF_CLK, SF_CS#, TWI_SDA, TWI_SCL, PIO 5.3, PIO 5.2, PIO 5.1 PIO 5.0    
    //*R_PAF( 5 ) = 0x0 << 14 | 0x0 << 12 | 0x0 << 10 | 0x0 << 8 | 0x3 << 6 | 0x3 << 4 | 0x3 << 2 | 0x3 << 0; 
    *R_PAF( 5 ) = 0x0 << 14 | 0x0 << 12 | 0x0 << 10 | 0x0 << 8 | 0x3 << 6 | 0x3 << 4 | AF5_1_UART_RX3 | AF5_0_UART_TX3; 
    // PIO 6.7, PIO 6.6, PIO 6.5 PIO 6.4, SF_D3, SD_D2, SF_D1, SF_D0 
    *R_PAF( 6 ) = 0x3 << 14 | 0x3 << 12 | 0x3 << 10 | 0x3 << 8 | 0x0 << 6 | 0x0 << 4 | 0x0 << 2 | 0x0 << 0; 
    // PIO 7.7, PIO 7.6, PIO 7.5 PIO 7.4, PIO 7.3, PIO 7.2, PIO 7.1, PIO 7.0 
    *R_PAF( 7 ) = 0x3 << 14 | 0x3 << 12 | 0x3 << 10 | 0x3 << 8 | 0x3 << 6 | 0x3 << 4 | 0x3 << 2 | 0x3 << 0; 
    // PIO 8.7, PIO 8.6, PIO 8.5 PIO 8.4, PIO 8.3, PIO 8.2, PIO 8.1, PIO 8.0 
    *R_PAF( 8 ) = 0x3 << 14 | 0x3 << 12 | 0x3 << 10 | 0x3 << 8 | 0x3 << 6 | 0x3 << 4 | 0x3 << 2 | 0x3 << 0; 
    // PIO 9.7, PIO 9.6, PIO 9.5 PIO 9.4, PIO 9.3, PIO 9.2, PIO 9.1, PIO 9.0 
    *R_PAF( 9 ) = 0x3 << 14 | 0x3 << 12 | 0x3 << 10 | 0x3 << 8 | 0x3 << 6 | 0x3 << 4 | 0x3 << 2 | 0x3 << 0; 
    // PIO 10.7, PIO 10.6, PIO 10.5 PIO 10.4, PIO 10.3, PIO 10.2, PIO 10.1, PIO 10.0 
    ///*R_PAF( 10 ) = 0x3 << 14 | 0x3 << 12 | 0x3 << 10 | 0x3 << 8 | 0x3 << 6 | 0x3 << 4 | 0x3 << 2 | 0x3 << 0; 
    *R_PAF( 10 ) = 0x3 << 14 | 0x3 << 12 | 0x3 << 10 | 0x3 << 8 | 0x3 << 6 | 0x3 << 4 | AF10_1_UART_RX2 | AF10_0_UART_TX2; 
    // LCD_VCLKOUT, PIO 11.6, PIO 11.5, BOOTMODE[4], BOOTMODE[3], BOOTMODE[2], BOOTMODE[1], BOOTMODE[0]  
    *R_PAF( 11 ) = 0x1 << 14 | 0x3 << 12 | 0x3 << 10 | 0x1 << 8 | 0x1 << 6 | 0x1 << 4 | 0x1 << 2 | 0x1 << 0; 
    // PIO 12.3, BOOTCODE + (USB_HOST_CUR, PIO 12.0)
    ///*R_PAF( 12 ) |= 0x1 << 2  | 0x3 << 0; 
    *R_PAF( 12 ) = 0x3 << 6 | 0x0 << 4 |  0x1 << 2  | 0x3 << 0; // PIO 12.3, PWM_OUT1, USB_HOST_CUR, PIO 12.0
    // PIO 13.7, PIO 13.6, PIO 13.5 PIO 13.4, PIO 13.3, PIO 13.2, PIO 13.1, PIO 13.0   
    *R_PAF( 13 ) = 0x3 << 14 | 0x3 << 12 | 0x3 << 10 | 0x3 << 8 | 0x3 << 6 | 0x3 << 4 | 0x3 << 2 | 0x3 << 0; 
    // PIO 14.7, PIO 14.6, PIO 14.5 PIO 14.4, PIO 14.3, PIO 14.2, PIO 14.1, PIO 14.0   
    *R_PAF( 14 ) = 0x3 << 14 | 0x3 << 12 | 0x3 << 10 | 0x3 << 8 | 0x3 << 6 | 0x3 << 4 | 0x3 << 2 | 0x3 << 0; 
  }

  /* ETC Settings */
  // WM8974 setting pio.
  *R_GPDIR(13) |= (0x7 << 5);    

  ///msgDbg("%d", (volatile unsigned int)*R_PAF2);
  ///msgDbg("%d", (volatile unsigned int)*R_PAF(2));
  
#else // (USE_LCD_1024x600 || USE_LCD_1280x800)  
  /* SDHC Initialized in Bootloader bootDeviceInit() */
  /**R_PAF(2) = AF2_0_SDHC_D0 | AF2_1_SDHC_D1 | AF2_2_SDHC_D2 | AF2_3_SDHC_D3 
              | AF2_4_SDHC_CLK | AF2_5_SDHC_CMD | AF2_6_GP6 | AF2_7_GP7;*/
  *R_PAF(3) = AF3_0_GP0 | AF3_1_GP1 | AF3_2_GP2 | AF3_3_GP3 | AF3_4_GP4 | AF3_5_GP5 | AF3_6_GP6 | AF3_7_GP7;
  *R_PAF(4) = AF4_0_GP0 | AF4_1_GP1 | AF4_2_GP2 | AF4_3_GP3 | AF4_4_GP4 | AF4_5_GP5 | AF4_6_GP6 | AF4_7_GP7;
  /* UART3, I2C, CLOCKIN_I2S_MCLK */
  *R_PAF(5) = AF5_0_UART_TX3 | AF5_1_UART_RX3 | AF5_2_GP2 | AF5_3_CAP_IN3 
              | AF5_4_TWI_SCL | AF5_5_TWI_SDA | AF5_6_GP6 | AF5_7_GP7;  
  *R_PAF(6) = AF6_0_GP0 | AF6_1_GP1 | AF6_2_GP2 | AF6_3_GP3 | AF6_4_GP4 | AF6_5_GP5 | AF6_6_GP6 | AF6_7_GP7;
  *R_PAF(7) = AF7_0_GP0 | AF7_1_GP1 | AF7_2_GP2 | AF7_3_GP3 | AF7_4_GP4 | AF7_5_GP5 | AF7_6_GP6 | AF7_7_GP7;
  *R_PAF(8) = AF8_0_GP0 | AF8_1_GP1 | AF8_2_GP2 | AF8_3_GP3 | AF8_4_GP4 | AF8_5_GP5 | AF8_6_GP6 | AF8_7_GP7;
  *R_PAF(9) = AF9_0_GP0 | AF9_1_GP1 | AF9_2_GP2 | AF9_3_GP3 | AF9_4_GP4 | AF9_5_GP5 | AF9_6_GP6 | AF9_7_GP7;
  /* UART2, SPI1 Initialized in HardwareMaster3WireInit() */
  *R_PAF(10) = AF10_0_UART_TX2 | AF10_1_UART_RX2 | AF10_2_GP2 | AF10_3_GP3 
              | AF10_4_GP4 | AF10_5_GP5 | AF10_6_GP6 | AF10_7_GP7;
  /* BOOTMODE, EIRQ1 Initialized in AK4183DeviceInitialize(), PCLKOUT */
  *R_PAF(11) = AF11_0_BOOTMODE_0 | AF11_1_BOOTMODE_1 | AF11_2_BOOTMODE_2 | AF11_3_BOOTMODE_3 
              | AF11_4_BOOTMODE_4 | AF11_5_GP5 | AF11_6_GP6 | AF11_7_LCD_VCLKOUT;
  /* BACKLIGHT PWM */
  *R_PAF(12) = AF12_0_GP0 | AF12_1_GP1 | AF12_2_PWM_OUT1 | AF12_3_GP3;
  
  /* ETC Settings */
  // NTD
#endif

  CPUPeriphClockIntialize();
}

/**
 *  @brief      This API must be called once during system Release
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvCPUPeriphRelease(void)
{
}


