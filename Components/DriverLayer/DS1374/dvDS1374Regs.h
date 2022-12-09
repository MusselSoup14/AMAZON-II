/**
  ******************************************************************************
  * @file    dvDS1374Regs.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    09-05-2019
  * @brief   This file contains Register Map for the Audio Amp Driver
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
  * <h2><center>&copy; Copyright 1997-2019 (c) MadeFactory Inc.</center></h2>
  ******************************************************************************
  */  
  
/* Define to prevent recursive inclusion -------------------------------------*/
#if !defined(_INCLUDED_DVDS1374REGS_H_)
#define _INCLUDED_DVDS1374REGS_H_

 /* Includes ------------------------------------------------------------------*/
#include "common.h"

#if (USE_LCD_1024x600 ||  USE_LCD_1280x800)
/** @addtogroup CWM-T10
  * @{
  */


/** @defgroup dvDS1374 
  * @brief DS1374 Driver
  * @{
  */

/** @defgroup dvDS1374_Exported_Types
  * @{
  */
//----------------------------------------------------------------------------
// DS1374 Register Map
//----------------------------------------------------------------------------
// 0x00~0x03[?]: Time of day counter - a 32-bit up counter that increments once per second 
// when the oscillator is running
// First Power on value is not 0
#define REG_TODC REG_ADDR(dwTODCounter)

// 0x04~0x06[b'0000_0000]: The contents of the watchdog/alarm counter, 
// which is a separate 24-bit down counter
#define REG_WDnALMC0 REG_ADDR(cWD_nALMCounter0)
#define REG_WDnALMC1 REG_ADDR(cWD_nALMCounter1)
#define REG_WDnALMC2 REG_ADDR(cWD_nALMCounter2)

// 0x07[b'0000_0110]: Control Register
typedef struct tagcCONTROL
{
  BYTE AIE: 1, // Alarm Interrupt Enable
    // When set to logic 1, this bit permits the alarm flag (AF) bit 
    // in the status register to assert INT (when WDSTR = 1).
    RS: 2, // Rate Select. These bits control the frequency of the square-wave output 
           // when the square wave has been enabled
    // RS2  RS1   Square-Wave Output Frequency
    //  b'0   b'0    1Hz
    //  b'0   b'1    4.096KHz
    //  b'1   b'0    8.192KHz
    //  b'1   b'1    32.768KHz
    WDSTR: 1, // Watchdog Reset Steering Bit
    BBSQW: 1, // Battery-Backed Square-Wave Enable
    WD_nALM: 1, // WD/nALM Counter Select
    WACE: 1, // WD/nALM Counter Enable
    nEOSC: 1; // Enable Oscillator
}cCONTROL;
#define REG_CONTROL REG_ADDR(cCONTROL)

// 0x08[b'x000_000x]: Control Register
typedef struct tagSTATUS
{
  BYTE AF: 1, // Alarm Flag
    // AF is cleared when written to logic 0.
    RSV: 6, // Reserved
    OSF: 1;  // Oscillator Stop Flag
    // The following are examples of conditions that can cause the OSF bit to be set:
    // 1) The first time power is applied.
    // 2) The voltage present on both VCC and VBACKUP are insufficient to support oscillation.
    // 3) The EOSC bit is turned off.
    // 4) External influences on the crystal (i.e., noise, leakage, etc.).
    // This bit remains at logic 1 until written to logic 0.
}cSTATUS;
#define REG_STATUS REG_ADDR(cSTATUS)

// 0x09[b'0000_0000] Trickle-Charge Register. Refer datasheet page13 table5. Trickle Charge Register
typedef struct tagTRICKLECHARGER
{
  BYTE ROUT: 2, // Register out
    DS: 2, // Diode select
    TCS: 4;    // Trickle-charge select
}cTRICHARGER;
#define REG_TRICHARGER REG_ADDR(cTRICHARGER)

typedef struct tagDS1374_REGMAP
{
  DWORD       dwTODCounter;     // 0x00~0x03, 32bits Time-of-Day Counter
  BYTE        cWD_nALMCounter0; // 0x04, Watchdog/Alarm Counter Byte 0
  BYTE        cWD_nALMCounter1; // 0x05, Watchdog/Alarm Counter Byte 1
  BYTE        cWD_nALMCounter2; // 0x06, Watchdog/Alarm Counter Byte 2
  cCONTROL    cCONTROL;         // 0x07, Control
  cSTATUS     cSTATUS;          // 0x08, Status
  cTRICHARGER cTRICHARGER;      // 0x09, Trickle Charger
}GCC_PACKED DS1374_REGMAP;
typedef DS1374_REGMAP *PDS1374_REGMAP;

#define REG_ADDR(p) (BYTE)offsetof(DS1374_REGMAP, p)
/**
  * @}
  */


/** @defgroup dvDS1374_Exported_constants 
  * @{
  */


/**
  * @}
  */

/** @defgroup dvDS1374_Interrupt_Service_Routine
  * @{
  */

/**
  * @}
  */


/** @defgroup dvDS1374_Exported_Functions
  * @{
  */


/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

#endif // #if (USE_LCD_1024x600 ||  USE_LCD_1280x800)

#endif // #if !defined(_INCLUDED_DVDS1374REGS_H_)  

/******************* (C) COPYRIGHT 1997-2019 (c) MadeFactory Inc. *****END OF FILE****/  
