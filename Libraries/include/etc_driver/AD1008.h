/******************************************************************************
 Copyright (C) 2012      Advanced Digital Chips Inc. 
						http://www.adc.co.kr
 Author : Software Team.

******************************************************************************/

#pragma once


// AD1008.h

#define AD1008_DEVICE_ADDR	0x9C
#define AD1008_DEVICE_ADDR2	0xBC

#define AD1008_SINGLE		(0<<3)
#define AD1008_MULTI		(1<<3)
//////////////////////////////////////////////////////////////////

#define BF_Mode 			0
#define FS_Mode				1
#define SL_Mode				2
#define BF_Mode_always		4
#define BS_Mode_always		5
#define Sleep_Mode_always	6
//////////////////////////////////////////////////////////////////

#define LED_Select_ON			(0<<3)
#define LED_Select_OFF			(1<<3)
#define LED_Select_ON_Delay		(2<<3)
#define LED_Select_OFF_DelayOFF	(3<<3)
//////////////////////////////////////////////////////////////////

#define LED_Dimming_0		0
#define LED_Dimming_1		1
#define LED_Dimming_2		3
#define LED_Dimming_3		7
//////////////////////////////////////////////////////////////////

#define Detect_Mode_F3		(0<<2)
#define Detect_Mode_F4		(1<<2)
#define Detect_Mode_F3F4	(2<<2)
#define Detect_Mode_Full	(3<<2)
//////////////////////////////////////////////////////////////////

#define LED_Disp_1Sec		0
#define LED_Disp_2Sec		1
#define LED_Disp_3Sec		2
#define LED_Disp_4Sec		3
//////////////////////////////////////////////////////////////////

#define Sensitivity_Lev1	0			// High Sensitivity
#define Sensitivity_Lev2	1
#define Sensitivity_Lev3	2
#define Sensitivity_Lev4	3			// Detectable 10x10mm PAD in 5mm acrylic isolation
#define Sensitivity_Lev5	4
#define Sensitivity_Lev6	5
#define Sensitivity_Lev7	6
#define Sensitivity_Lev8	7
//////////////////////////////////////////////////////////////////

#define AD1008_CH1_Mask	0x38f			// 3 - register addr, 8f - mask
#define AD1008_CH2_Mask	0x3f8
#define AD1008_CH3_Mask	0x48f
#define AD1008_CH4_Mask	0x4f8
#define AD1008_CH5_Mask	0x58f
#define AD1008_CH6_Mask	0x5f8
#define AD1008_CH7_Mask	0x68f
#define AD1008_CH8_Mask	0x6f8
//////////////////////////////////////////////////////////////////

#define AD1008_REG00		0x00
#define AD1008_REG01		0x01
#define AD1008_REG02		0x02
#define AD1008_REG03		0x03
#define AD1008_REG04		0x04
#define AD1008_REG05		0x05
#define AD1008_REG06		0x06
#define AD1008_REG08		0x08

#define AD1008_SENSITIVITY	Sensitivity_Lev1

BOOL AD1008_Write(U8 addr,U8 data);
BOOL AD1008_Read(U8 addr,U8 *data);
BOOL AD1008_DetectRead(U8 *data);
BOOL AD1008_SingleMulti(U8 Mode); 
BOOL AD1008_OperatingMode(U8 Mode);
BOOL AD1008_LedDisplayType(U8 Mode);
BOOL AD1008_LedDimming(U8 Value);
BOOL AD1008_DetectMode(U8 Value);
BOOL AD1008_LedDisplayDelay(U8 Value);
BOOL AD1008_SensitivitySet(U16 channel, U8 Value);
BOOL AD1008_Init(void);
