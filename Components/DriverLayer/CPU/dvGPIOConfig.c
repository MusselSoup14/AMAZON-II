/**
  ******************************************************************************
  * @file    dvGPIOConfig.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    02-August-2014
  * @brief   This file provides basic the GPIO Configuration.
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
#include "dvGPIOConfig.h"

// ---------------------------------------------------------------------------
// Configuration data for a GPIO field
// ---------------------------------------------------------------------------
CROMDATA GPIO_FIELD g_asGPIOFields[gfINVALID] =
{
  // gfGPO_LED_MSG,          /* P14.0. Active Low */
  {
    .eChannel = GPIO_14,
    .dwMask   = GPIO_Pin_0,
    .cShift   = GPIO_PinSource0,    
    .eMode    = GPIO_MODE_OUT,
    .ePuPd    = GPIO_PUPD_OD,
    .dwInit   = HIGH,  
  },
  // gfGPO_LED_BLUE,          /* P14.1. Active Low */
  {
    .eChannel = GPIO_14,
    .dwMask   = GPIO_Pin_1,
    .cShift   = GPIO_PinSource1,
    .eMode    = GPIO_MODE_OUT,
    .ePuPd    = GPIO_PUPD_OD,
    .dwInit   = HIGH,  
  },
  // gfGPO_LED_GREEN,          /* P14.2. Active Low */
  {
    .eChannel = GPIO_14,
    .dwMask   = GPIO_Pin_2,
    .cShift   = GPIO_PinSource2,
    .eMode    = GPIO_MODE_OUT,
    .ePuPd    = GPIO_PUPD_OD,
    .dwInit   = HIGH,  
  },
  // gfGPO_LED_RED,          /* P14.3. Active Low */
  {
    .eChannel = GPIO_14,
    .dwMask   = GPIO_Pin_3,
    .cShift   = GPIO_PinSource3,
    .eMode    = GPIO_MODE_OUT,
    .ePuPd    = GPIO_PUPD_OD,
    .dwInit   = HIGH,  
  },    

  // gfGPI_KEY_1,            /* P14.5. Trigger High */
  {
    .eChannel = GPIO_14,
    .dwMask   = GPIO_Pin_5,
    .cShift   = GPIO_PinSource5,    
    .eMode    = GPIO_MODE_IN,
    .ePuPd    = GPIO_PUPD_OD,
    .dwInit   = HIGH,  
  },  
  // gfGPI_KEY_2,            /* P14.6. Trigger High */
  {
    .eChannel = GPIO_14,
    .dwMask   = GPIO_Pin_6,
    .cShift   = GPIO_PinSource6,    
    .eMode    = GPIO_MODE_IN,
    .ePuPd    = GPIO_PUPD_OD,
    .dwInit   = HIGH,  
  },  
  // gfGPI_KEY_3,            /* P14.7. Trigger High */    
  {
    .eChannel = GPIO_14,
    .dwMask   = GPIO_Pin_7,
    .cShift   = GPIO_PinSource7,    
    .eMode    = GPIO_MODE_IN,
    .ePuPd    = GPIO_PUPD_OD,
    .dwInit   = HIGH,  
  },      

  // gfGPO_SYS_STDBY,        /* P8.0. Active High (L: Ready, H: Standby) */
  {
    .eChannel = GPIO_8,
    .dwMask   = GPIO_Pin_0,
    .cShift   = GPIO_PinSource0,
    .eMode    = GPIO_MODE_OUT,
    .ePuPd    = GPIO_PUPD_PU,
    .dwInit   = HIGH,
  },        

  // gfGPI_uSDHC_DETECT,     /* P13.3. Trigger High */
  {
    .eChannel = GPIO_13,
    .dwMask   = GPIO_Pin_3,
    .cShift   = GPIO_PinSource3,    
    .eMode    = GPIO_MODE_IN,
    .ePuPd    = GPIO_PUPD_OD,
    .dwInit   = HIGH,  
  },     
  // gfGPO_AMP_nRESET,       /* P13.1. Active Low */
  {
    .eChannel = GPIO_13,
    .dwMask   = GPIO_Pin_1,
    .cShift   = GPIO_PinSource1,    
    .eMode    = GPIO_MODE_OUT,
    .ePuPd    = GPIO_PUPD_PU,
    .dwInit   = LOW,
  },  
  // gfGPO_CAN_STBY,         /* P7.0. Normal Low */
  {
    .eChannel = GPIO_7,
    .dwMask   = GPIO_Pin_0,
    .cShift   = GPIO_PinSource0,    
    .eMode    = GPIO_MODE_OUT,
    .ePuPd    = GPIO_PUPD_PU,
    .dwInit   = HIGH,
  },    
  // gfGPI_CAN_RX0BF_IRQ,    /* P7.1. Trigger Low */
  {
    .eChannel = GPIO_7,
    .dwMask   = GPIO_Pin_1,
    .cShift   = GPIO_PinSource1,
    .eMode    = GPIO_MODE_IN,
    .ePuPd    = GPIO_PUPD_OD,
    .dwInit   = HIGH,  
  },     
  // gfGPI_CAN_RX1BF_IRQ,    /* P7.2. Trigger Low */
  {
    .eChannel = GPIO_7,
    .dwMask   = GPIO_Pin_2,
    .cShift   = GPIO_PinSource2,    
    .eMode    = GPIO_MODE_IN,
    .ePuPd    = GPIO_PUPD_OD,
    .dwInit   = HIGH,  
  },     
  // gfGPO_CAN_nRESET,       /* P7.6. Active Low */    
  {
    .eChannel = GPIO_7,
    .dwMask   = GPIO_Pin_6,
    .cShift   = GPIO_PinSource6,
    .eMode    = GPIO_MODE_OUT,
    .ePuPd    = GPIO_PUPD_PU,
    .dwInit   = LOW,
  },   
  // gfGPO_CAN_nCS,         /* P10.4. Active Low */
  {
    .eChannel = GPIO_10,
    .dwMask   = GPIO_Pin_4,
    .cShift   = GPIO_PinSource4,
    .eMode    = GPIO_MODE_OUT,
    .ePuPd    = GPIO_PUPD_PU,
    .dwInit   = HIGH,
  },   
#if (USE_AMAZON_STK)  
  // gfGPO_LCD_nRESET,       /* P12.3. Active Low */
  {
    .eChannel = GPIO_12,
    .dwMask   = GPIO_Pin_3,
    .cShift   = GPIO_PinSource3,    
    .eMode    = GPIO_MODE_OUT,
    .ePuPd    = GPIO_PUPD_PU,
    .dwInit   = LOW,
  },
#else
  // gfGPO_LCD_nRESET,       /* P9.0. Active Low */
  {
    .eChannel = GPIO_9,
    .dwMask   = GPIO_Pin_0,
    .cShift   = GPIO_PinSource0,    
    .eMode    = GPIO_MODE_OUT,
    .ePuPd    = GPIO_PUPD_PU,
    .dwInit   = LOW,
  },
#endif
  // gfGPO_LCD_nSTDBY,       /* P9.1. Active Low [600] */
  {
    .eChannel = GPIO_9,
    .dwMask   = GPIO_Pin_1,
    .cShift   = GPIO_PinSource1,    
    .eMode    = GPIO_MODE_OUT,
    .ePuPd    = GPIO_PUPD_PU,
    .dwInit   = LOW,
  },  
  // gfGPO_LCD_LR_nFLIP,     /* P9.2. Active Low [600] */
  {
    .eChannel = GPIO_9,
    .dwMask   = GPIO_Pin_2,
    .cShift   = GPIO_PinSource2,    
    .eMode    = GPIO_MODE_OUT,
    .ePuPd    = GPIO_PUPD_PU,
    .dwInit   = HIGH,
  },   
  // gfGPO_LCD_UD_FLIP,      /* P9.3. Active High [600] */
  {
    .eChannel = GPIO_9,
    .dwMask   = GPIO_Pin_3,
    .cShift   = GPIO_PinSource3,    
    .eMode    = GPIO_MODE_OUT,
    .ePuPd    = GPIO_PUPD_PU,
    .dwInit   = LOW,
  },     
  // gfGPO_LCD_nPOWER,       /* P9.4. Active Low [600] */
  {
    .eChannel = GPIO_9,
    .dwMask   = GPIO_Pin_4,
    .cShift   = GPIO_PinSource4,    
    .eMode    = GPIO_MODE_OUT,
    .ePuPd    = GPIO_PUPD_PU,
    .dwInit   = HIGH,
  },       
  // gfGPO_LCD_BLU_ON,       /* P9.5. Active High [800, 600] */
  {
    .eChannel = GPIO_9,
    .dwMask   = GPIO_Pin_5,
    .cShift   = GPIO_PinSource5,    
    .eMode    = GPIO_MODE_OUT,
    .ePuPd    = GPIO_PUPD_PU,
    .dwInit   = LOW,
  },
  // gfGPO_LCD_LVDS_ON,      /* P3.6. Active High [800, 600] */
  {
    .eChannel = GPIO_3,
    .dwMask   = GPIO_Pin_6,
    .cShift   = GPIO_PinSource6,    
    .eMode    = GPIO_MODE_OUT,
    .ePuPd    = GPIO_PUPD_PU,
    .dwInit   = LOW,
  }  
};

