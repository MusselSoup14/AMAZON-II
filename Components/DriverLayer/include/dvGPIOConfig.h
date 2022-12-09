/**
  ******************************************************************************
  * @file    dvGPIOConfig.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    02-August-2014
  * @brief   This file provides basic the GPIO Configuration Header.
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
#include "dvGPIOType.h"

#if !defined(_INCLUDED_DVGPIOCONFIG_H_)
#define _INCLUDED_DVGPIOCONFIG_H_

extern CROMDATA GPIO_FIELD g_asGPIOFields[];

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

// ------------------------------------------------------------------
// GPIO Field Enumeration. 
// ------------------------------------------------------------------
typedef enum
{
  gfGPO_LED_MSG,          /* P14.0. Active Low */
  gfGPO_LED_BLUE,         /* P14.1. Active Low */
  gfGPO_LED_GREEN,        /* P14.2. Active Low */  
  gfGPO_LED_RED,          /* P14.3. Active Low */

  gfGPI_KEY_1,            /* P14.5. Trigger High */
  gfGPI_KEY_2,            /* P14.6. Trigger High */
  gfGPI_KEY_3,            /* P14.7. Trigger High */ 

  gfGPO_SYS_STDBY,        /* P8.0. Active High (L: Ready, H: Standby) */

  gfGPI_uSDHC_DETECT,     /* P13.3. Trigger High */
  gfGPO_AMP_nRESET,       /* P13.1. Active Low */
  gfGPO_CAN_STBY,         /* P7.0. Normal Low */
  gfGPI_CAN_RX0BF_IRQ,    /* P7.1. Trigger Low */
  gfGPI_CAN_RX1BF_IRQ,    /* P7.2. Trigger Low */
  gfGPO_CAN_nRESET,       /* P7.6. Active Low */
  gfGPO_CAN_nCS,          /* P10.4. Active Low */

  gfGPO_LCD_nRESET,       /* P12.3[STK], P9.0. Active Low [STK, 600] */
  gfGPO_LCD_nSTDBY,       /* P9.1. Active Low [600] */
  gfGPO_LCD_LR_nFLIP,     /* P9.2. Active Low [600] */
  gfGPO_LCD_UD_FLIP,      /* P9.3. Active High [600] */
  gfGPO_LCD_nPOWER,       /* P9.4. Active Low [600] */
  gfGPO_LCD_BLU_ON,       /* P9.5. Active High [800, 600] */
  gfGPO_LCD_LVDS_ON,      /* P3.6. Active High [800, 600] */
    
  gfINVALID,              // Sentinel
} eGPIO_FIELD;


// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************




#endif //#if !defined(_INCLUDED_DVGPIOCONFIG_H_)


