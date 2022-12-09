/**
  ******************************************************************************
  * @file    dvGPIOType.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    02-August-2014
  * @brief   This file provides basic the GPIO Type.
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
#include "common.h"

#if !defined(_INCLUDED_DVGPIOTYPE_H_)
#define _INCLUDED_DVGPIOTYPE_H_

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
// ---------------------------------------------------------------------------
// Port Sources
// ---------------------------------------------------------------------------
#define GPIO_PinSource0  (0x00)
#define GPIO_PinSource1  (0x01)
#define GPIO_PinSource2  (0x02)
#define GPIO_PinSource3  (0x03)
#define GPIO_PinSource4  (0x04)
#define GPIO_PinSource5  (0x05)
#define GPIO_PinSource6  (0x06)
#define GPIO_PinSource7  (0x07)

// ---------------------------------------------------------------------------
// Port Pins
// ---------------------------------------------------------------------------
#define GPIO_Pin_0  0x01
#define GPIO_Pin_1  0x02
#define GPIO_Pin_2  0x04
#define GPIO_Pin_3  0x08
#define GPIO_Pin_4  0x10
#define GPIO_Pin_5  0x20
#define GPIO_Pin_6  0x40
#define GPIO_Pin_7  0x80

// ---------------------------------------------------------------------------
// enum to Port Channel
// ---------------------------------------------------------------------------
typedef enum
{
  GPIO_0,
  GPIO_1,
  GPIO_2,  
  GPIO_3,  
  GPIO_4,  
  GPIO_5,  
  GPIO_6,  
  GPIO_7,  
  GPIO_8,
  GPIO_9,
  GPIO_10,
  GPIO_11,
  GPIO_12, /* 0 ~ 3 */
  GPIO_13, /* No AF */
  GPIO_14, /* NO AF */
  
  GPIO_INVALID // Sentinel
}eGPIO_CHANNEL;

// ---------------------------------------------------------------------------
// In/out Mode to Port
// ---------------------------------------------------------------------------
typedef enum
{
  GPIO_MODE_IN,
  GPIO_MODE_OUT,
  
  GPIO_MODE_INVALID
}eGPIO_MODE;


// ---------------------------------------------------------------------------
// In/out Mode to Port
// ---------------------------------------------------------------------------
typedef enum
{
  GPIO_PUPD_PD,     /* Pull-Down */
  GPIO_PUPD_PU,     /* Pull-Up */
  GPIO_PUPD_OD,     /* Open Drain */  

  GPIO_PUPD_CMO,    /* CMOS output */
  GPIO_PUPD_CMI,    /* CMOS input */
  GPIO_PUPD_SCHI,   /* Schmitt input */
  
  GPIO_PUPD_INVALID
}eGPIO_PUPD;


// ---------------------------------------------------------------------------
// Configuration data for a GPIO field
// ---------------------------------------------------------------------------
typedef struct tagGPIOFIELD
{
  eGPIO_CHANNEL     eChannel;   /* GPIO Channel */
  DWORD             dwMask;     /* Mask Value */
  BYTE              cShift;     /* Left Shift Value */
  eGPIO_MODE        eMode;      /* GPIO In/Out Mode */
  eGPIO_PUPD        ePuPd;      /* Pull-Up/down/Open drain ... */
  DWORD             dwInit;     /* Initial Value */    
}GPIO_FIELD, *PGPIO_FIELD;
typedef CROMDATA GPIO_FIELD *PCGPIO_FIELD;

///#define R_PAF(N) ((volatile unsigned int*)(0xF4000400+(N*4)))

// ---------------------------------------------------------------------------
// Port Alternate Function Register 0 (PAF0)
// ---------------------------------------------------------------------------
#define R_PAF0 ((volatile unsigned int*)0xF4000400)

#define AF0_0_NFD0	          (0 << 0)
#define AF0_0_UART_TX1			  (1 << 0)
#define AF0_0_GP0			        (3 << 0)

#define AF0_1_NFD1	          (0 << 2)
#define AF0_1_UART_RX1			  (1 << 2)
#define AF0_1_GP1			        (3 << 2)

#define AF0_2_NFD2	          (0 << 4)
#define AF0_2_PWM_OUT1			  (1 << 4)
#define AF0_2_GP2			        (3 << 4)

#define AF0_3_NFD3	          (0 << 6)
#define AF0_3_CAP_IN1			    (1 << 6)
#define AF0_3_GP3			        (3 << 6)

#define AF0_4_NFD4	          (0 << 8)
#define AF0_4_UART_TX2			  (1 << 8)
#define AF0_4_GP4			        (3 << 8)

#define AF0_5_NFD5	          (0 << 10)
#define AF0_5_UART_RX2			  (1 << 10)
#define AF0_5_GP5			        (3 << 10)

#define AF0_6_NFD6	          (0 << 12)
#define AF0_6_PWM_OUT2			  (1 << 12)
#define AF0_6_GP6			        (3 << 12)

#define AF0_7_NFD7	          (0 << 14)
#define AF0_7_CAP_IN2			    (1 << 14)
#define AF0_7_GP7			        (3 << 14)

// ---------------------------------------------------------------------------
// Port Alternate Function Register 1 (PAF1)
// ---------------------------------------------------------------------------
#define R_PAF1 ((volatile unsigned int*)0xF4000404)

#define AF1_0_NF_NCS	        (0 << 0)
#define AF1_0_GP0	            (3 << 0)

#define AF1_1_NF_ALE	        (0 << 2)
#define AF1_1_GP1	            (3 << 2)

#define AF1_2_NF_CLE	        (0 << 4)
#define AF1_2_GP2	            (3 << 4)

#define AF1_3_NF_NWE	        (0 << 6)
#define AF1_3_GP3	            (3 << 6)

#define AF1_4_NF_NRE	        (0 << 8)
#define AF1_4_GP4	            (3 << 8)

#define AF1_5_NF_NBUSY	      (0 << 10)
#define AF1_5_GP5	            (3 << 10)

#define AF1_6_UART_TX0	      (0 << 12)
#define AF1_6_GP6	            (3 << 12)

#define AF1_7_UART_RX0	      (0 << 14)
#define AF1_7_GP7	            (3 << 14)

// ---------------------------------------------------------------------------
// Port Alternate Function Register 2 (PAF2)
// ---------------------------------------------------------------------------
#define R_PAF2 ((volatile unsigned int*)0xF4000408)

#define AF2_0_SDHC_D0	        (0 << 0)
#define AF2_0_UART_TX3	      (1 << 0)
#define AF2_0_GP0	            (3 << 0)

#define AF2_1_SDHC_D1	        (0 << 2)
#define AF2_1_UART_RX3	      (1 << 2)
#define AF2_1_GP1	            (3 << 2)

#define AF2_2_SDHC_D2	        (0 << 4)
#define AF2_2_PWM_OUT3	      (1 << 4)
#define AF2_2_GP2	            (3 << 4)

#define AF2_3_SDHC_D3	        (0 << 6)
#define AF2_3_CAP_IN3	        (1 << 6)
#define AF2_3_GP3	            (3 << 6)

#define AF2_4_SDHC_CLK	      (0 << 8)
#define AF2_4_GP4	            (3 << 8)

#define AF2_5_SDHC_CMD	      (0 << 10)
#define AF2_5_GP5	            (3 << 10)

#define AF2_6_PWM_OUT0	      (0 << 12)
#define AF2_6_GP6	            (3 << 12)

#define AF2_7_CAP_IN0	        (0 << 14)
#define AF2_7_SRAM_NCS1	      (1 << 14)
#define AF2_7_GP7	            (3 << 14)

// ---------------------------------------------------------------------------
// Port Alternate Function Register 3 (PAF3)
// ---------------------------------------------------------------------------
#define R_PAF3 ((volatile unsigned int*)0xF400040C)

#define AF3_0_I2S_CDCLK0	    (0 << 0)
#define AF3_0_GP0	            (3 << 0)

#define AF3_1_I2S_TX_SCLK0	  (0 << 2)
#define AF3_1_GP1	            (3 << 2)

#define AF3_2_I2S_TX_WS0	    (0 << 4)
#define AF3_2_GP2	            (3 << 4)

#define AF3_3_I2S_SDO0	      (0 << 6)
#define AF3_3_GP3	            (3 << 6)

#define AF3_4_I2S_SDI0	      (0 << 8)
#define AF3_4_GP4	            (3 << 8)

#define AF3_5_I2S_RX_SCLK0	  (0 << 10)
#define AF3_5_GP5	            (3 << 10)

#define AF3_6_I2S_RX_WS0	    (0 << 12)
#define AF3_6_GP6	            (3 << 12)

#define AF3_7_ICE_VCLK	      (0 << 14)
#define AF3_7_GP7	            (3 << 14)

// ---------------------------------------------------------------------------
// Port Alternate Function Register 4 (PAF4)
// ---------------------------------------------------------------------------
#define R_PAF4 ((volatile unsigned int*)0xF4000410)

#define AF4_0_ICE_VDIN0	      (0 << 0)
#define AF4_0_I2S_CDCLK1	    (1 << 0)
#define AF4_0_GP0	            (3 << 0)

#define AF4_1_ICE_VDIN1	      (0 << 2)
#define AF4_1_I2S_TX_SCLK1	  (1 << 2)
#define AF4_1_GP1	            (3 << 2)

#define AF4_2_ICE_VDIN2	      (0 << 4)
#define AF4_2_I2S_TX_WS1	    (1 << 4)
#define AF4_2_GP2	            (3 << 4)

#define AF4_3_ICE_VDIN3	      (0 << 6)
#define AF4_3_I2S_SDO1	      (1 << 6)
#define AF4_3_GP3	            (3 << 6)

#define AF4_4_ICE_VDIN4	      (0 << 8)
#define AF4_4_I2S_SDI1	      (1 << 8)
#define AF4_4_GP4	            (3 << 8)

#define AF4_5_ICE_VDIN5	      (0 << 10)
#define AF4_5_I2S_RX_SCLK1	  (1 << 10)
#define AF4_5_GP5	            (3 << 10)

#define AF4_6_ICE_VDIN6	      (0 << 12)
#define AF4_6_I2S_RX_WS1	    (1 << 12)
#define AF4_6_GP6	            (3 << 12)

#define AF4_7_ICE_VDIN7	      (0 << 14)
#define AF4_7_EIRQ0	          (1 << 14)
#define AF4_7_GP7	            (3 << 14)

// ---------------------------------------------------------------------------
// Port Alternate Function Register 5 (PAF5)
// ---------------------------------------------------------------------------
#define R_PAF5 ((volatile unsigned int*)0xF4000414)

#define AF5_0_SPI_NCS0	      (0 << 0)
#define AF5_0_UART_TX3	      (1 << 0)
#define AF5_0_GP0	            (3 << 0)

#define AF5_1_SPI_SCK0	      (0 << 2)
#define AF5_1_UART_RX3	      (1 << 2)
#define AF5_1_GP1	            (3 << 2)

#define AF5_2_SPI_MISO0	      (0 << 4)
#define AF5_2_PWM_OUT3	      (1 << 4)
#define AF5_2_GP2	            (3 << 4)

#define AF5_3_SPI_MOSI0	      (0 << 6)
#define AF5_3_CAP_IN3	        (1 << 6)
#define AF5_3_GP3	            (3 << 6)

#define AF5_4_TWI_SCL	        (0 << 8)
#define AF5_4_GP4	            (3 << 8)

#define AF5_5_TWI_SDA	        (0 << 10)
#define AF5_5_GP5	            (3 << 10)

#define AF5_6_SF_NCS	        (0 << 12)
#define AF5_6_GP6	            (3 << 12)

#define AF5_7_SF_CLK	        (0 << 14)
#define AF5_7_GP7	            (3 << 14)


// ---------------------------------------------------------------------------
// Port Alternate Function Register 6 (PAF6)
// ---------------------------------------------------------------------------
#define R_PAF6 ((volatile unsigned int*)0xF4000418)

#define AF6_0_SF_D0	          (0 << 0)
#define AF6_0_GP0	            (3 << 0)

#define AF6_1_SF_D1	          (0 << 2)
#define AF6_1_GP1	            (3 << 2)

#define AF6_2_SF_D2	          (0 << 4)
#define AF6_2_GP2	            (3 << 4)

#define AF6_3_SF_D3	          (0 << 6)
#define AF6_3_GP3	            (3 << 6)

#define AF6_4_SRAM_NWE	      (0 << 8)
#define AF6_4_GP4	            (3 << 8)

#define AF6_5_SRAM_NRE	      (0 << 10)
#define AF6_5_GP5	            (3 << 10)

#define AF6_6_SRAM_NBE1	      (0 << 12)
#define AF6_6_GP6	            (3 << 12)

#define AF6_7_SRAM_NCS0	      (0 << 14)
#define AF6_7_GP7	            (3 << 14)

// ---------------------------------------------------------------------------
// Port Alternate Function Register 7 (PAF7)
// ---------------------------------------------------------------------------
#define R_PAF7 ((volatile unsigned int*)0xF400041C)

#define AF7_0_SRAM_A0	        (0 << 0)
#define AF7_0_GP0	            (3 << 0)

#define AF7_1_SRAM_A1	        (0 << 2)
#define AF7_1_GP1	            (3 << 2)

#define AF7_2_SRAM_A2	        (0 << 4)
#define AF7_2_GP2	            (3 << 4)

#define AF7_3_SRAM_A3	        (0 << 6)
#define AF7_3_GP3	            (3 << 6)

#define AF7_4_SRAM_A4	        (0 << 8)
#define AF7_4_GP4	            (3 << 8)

#define AF7_5_SRAM_A5	        (0 << 10)
#define AF7_5_GP5	            (3 << 10)

#define AF7_6_SRAM_A6	        (0 << 12)
#define AF7_6_GP6	            (3 << 12)

#define AF7_7_SRAM_A7	        (0 << 14)
#define AF7_7_GP7	            (3 << 14)

// ---------------------------------------------------------------------------
// Port Alternate Function Register 8 (PAF8)
// ---------------------------------------------------------------------------
#define R_PAF8 ((volatile unsigned int*)0xF4000420)

#define AF8_0_SRAM_A8	        (0 << 0)
#define AF8_0_I2S_CDCLK1	    (1 << 0)
#define AF8_0_GP0	            (3 << 0)

#define AF8_1_SRAM_A9	        (0 << 2)
#define AF8_1_I2S_TX_SCLK1	  (1 << 2)
#define AF8_1_GP1	            (3 << 2)

#define AF8_2_SRAM_A10	      (0 << 4)
#define AF8_2_I2S_TX_WS1	    (1 << 4)
#define AF8_2_GP2	            (3 << 4)

#define AF8_3_SRAM_A11	      (0 << 6)
#define AF8_3_I2S_SDO1	      (1 << 6)
#define AF8_3_GP3	            (3 << 6)

#define AF8_4_SRAM_A12	      (0 << 8)
#define AF8_4_I2S_SDI1	      (1 << 8)
#define AF8_4_GP4	            (3 << 8)

#define AF8_5_SRAM_A13	      (0 << 10)
#define AF8_5_I2S_RX_SCLK1	  (1 << 10)
#define AF8_5_GP5	            (3 << 10)

#define AF8_6_SRAM_A14	      (0 << 12)
#define AF8_6_I2S_RX_WS1	    (1 << 12)
#define AF8_6_GP6	            (3 << 12)

#define AF8_7_SRAM_A15	      (0 << 14)
#define AF8_7_EIRQ2           (1 << 14)
#define AF8_7_GP7	            (3 << 14)

// ---------------------------------------------------------------------------
// Port Alternate Function Register 9 (PAF9)
// ---------------------------------------------------------------------------
#define R_PAF9 ((volatile unsigned int*)0xF4000424)

#define AF9_0_SRAM_D0	        (0 << 0)
#define AF9_0_GP0	            (3 << 0)

#define AF9_1_SRAM_D1	        (0 << 2)
#define AF9_1_GP1	            (3 << 2)

#define AF9_2_SRAM_D2	        (0 << 4)
#define AF9_2_GP2	            (3 << 4)

#define AF9_3_SRAM_D3	        (0 << 6)
#define AF9_3_GP3	            (3 << 6)

#define AF9_4_SRAM_D4	        (0 << 8)
#define AF9_4_GP4	            (3 << 8)

#define AF9_5_SRAM_D5	        (0 << 10)
#define AF9_5_GP5	            (3 << 10)

#define AF9_6_SRAM_D6	        (0 << 12)
#define AF9_6_GP6	            (3 << 12)

#define AF9_7_SRAM_D7	        (0 << 14)
#define AF9_7_GP7	            (3 << 14)

// ---------------------------------------------------------------------------
// Port Alternate Function Register 10 (PAF10)
// ---------------------------------------------------------------------------
#define R_PAF10 ((volatile unsigned int*)0xF4000428)

#define AF10_0_SRAM_D8	      (0 << 0)
#define AF10_0_UART_TX2	      (1 << 0)
#define AF10_0_GP0	          (3 << 0)

#define AF10_1_SRAM_D9	      (0 << 2)
#define AF10_1_UART_RX2	      (1 << 2)
#define AF10_1_GP1	          (3 << 2)

#define AF10_2_SRAM_D10	      (0 << 4)
#define AF10_2_PWMOUT2	      (1 << 4)
#define AF10_2_GP2	          (3 << 4)

#define AF10_3_SRAM_D11	      (0 << 6)
#define AF10_3_CAPIN2	        (1 << 6)
#define AF10_3_GP3	          (3 << 6)

#define AF10_4_SRAM_D12	      (0 << 8)
#define AF10_4_SPI_NCS1	      (1 << 8)
#define AF10_4_GP4	          (3 << 8)

#define AF10_5_SRAM_D13	      (0 << 10)
#define AF10_5_SPI_SCK1	      (1 << 10)
#define AF10_5_GP5	          (3 << 10)

#define AF10_6_SRAM_D14	      (0 << 12)
#define AF10_6_SPI_MISO1	    (1 << 12)
#define AF10_6_GP6	          (3 << 12)

#define AF10_7_SRAM_D15	      (0 << 14)
#define AF10_7_SPI_MOSI1	    (1 << 14)
#define AF10_7_GP7	          (3 << 14)

// ---------------------------------------------------------------------------
// Port Alternate Function Register 11 (PAF11)
// ---------------------------------------------------------------------------
#define R_PAF11 ((volatile unsigned int*)0xF400042C)

#define AF11_0_SRAM_A16	      (0 << 0)
#define AF11_0_BOOTMODE_0	    (1 << 0)
#define AF11_0_GP0	          (3 << 0)

#define AF11_1_SRAM_A17	      (0 << 2)
#define AF11_1_BOOTMODE_1	    (1 << 2)
#define AF11_1_GP1	          (3 << 2)

#define AF11_2_SRAM_A18	      (0 << 4)
#define AF11_2_BOOTMODE_2	    (1 << 4)
#define AF11_2_GP2	          (3 << 4)

#define AF11_3_SRAM_A19	      (0 << 6)
#define AF11_3_BOOTMODE_3	    (1 << 6)
#define AF11_3_GP3	          (3 << 6)

#define AF11_4_SRAM_A20	      (0 << 8)
#define AF11_4_BOOTMODE_4	    (1 << 8)
#define AF11_4_GP4	          (3 << 8)

#define AF11_5_SRAM_A21	      (0 << 10)
#define AF11_5_LCD_VCLKIN	    (1 << 10)
#define AF11_5_GP5	          (3 << 10)

#define AF11_6_SRAM_NWAIT	    (0 << 12)
#define AF11_6_EIRQ1	        (1 << 12)
#define AF11_6_GP6	          (3 << 12)

#define AF11_7_EIRQ0	        (0 << 14)
#define AF11_7_LCD_VCLKOUT	  (1 << 14)
#define AF11_7_GP7	          (3 << 14)

// ---------------------------------------------------------------------------
// Port Alternate Function Register 12 (PAF12)
// ---------------------------------------------------------------------------
#define R_PAF12 ((volatile unsigned int*)0xF4000430)

#define AF12_0_UART_TX1	      (0 << 0)
#define AF12_0_USB_HOST_POW	  (1 << 0)
#define AF12_0_GP0	          (3 << 0)

#define AF12_1_UART_RX1	      (0 << 2)
#define AF12_1_USB_HOST_CUR	  (1 << 2)
#define AF12_1_GP1	          (3 << 2)

#define AF12_2_PWM_OUT1	      (0 << 4)
#define AF12_2_SRAM_CS2	      (1 << 4)
#define AF12_2_GP2	          (3 << 4)

#define AF12_3_CAP_IN1	      (0 << 6)
#define AF12_3_SRAM_CS3	      (1 << 6)
#define AF12_3_GP3	          (3 << 6)

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************


#endif //#if !defined(_INCLUDED_DVGPIOTYPE_H_)


