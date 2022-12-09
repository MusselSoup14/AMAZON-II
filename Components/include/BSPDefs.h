/**
  ******************************************************************************
  * @file    BSPDefs.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    30-04-2019
  * @brief   This file provides Custom Configuration
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
/* Includes ------------------------------------------------------------------*/  
#include "sdk.h"

// ---------------------------------------------------------------------------
// BSP Properties
// ---------------------------------------------------------------------------

/* LCD */
#define _DEF_LCD_050_800x480_     0 // AMAZON-SDK. HD50WV-PF1 800x480@60Hz
#define _DEF_LCD_101_1024x600_    1 // Clear Display 10.1
#define _DEF_LCD_101_1280x800_    2 // INNOLUX 10.1

#define LCD_MODULE_TYPE   (_DEF_LCD_050_800x480_)   //(_DEF_LCD_101_1280x800_)

#define USE_LCD_800x480   (_DEF_LCD_050_800x480_  == LCD_MODULE_TYPE)
#define USE_LCD_1024x600  (_DEF_LCD_101_1024x600_ == LCD_MODULE_TYPE)
#define USE_LCD_1280x800  (_DEF_LCD_101_1280x800_ == LCD_MODULE_TYPE)
#define USE_AMAZON_STK    USE_LCD_800x480     /* STK Definition */

/* Touch IC */
#define _DEF_TCIC_AK4183          0
#define _DEF_TCIC_ILI2511         1
#define _DEF_TCIC_ST1912          2
#define _DEF_TCIC_AR1010          3

#if USE_AMAZON_STK
#define PANE_WIDTH_MAX    800
#define PANE_HEIGHT_MAX   480
#define TOUCH_IC_TYPE     _DEF_TCIC_AK4183
#elif USE_LCD_1024x600
#define PANE_WIDTH_MAX    1024
#define PANE_HEIGHT_MAX   600
#define TOUCH_IC_TYPE     _DEF_TCIC_AK4183
#elif USE_LCD_1280x800
#define PANE_WIDTH_MAX    1280
#define PANE_HEIGHT_MAX   800
#define TOUCH_IC_TYPE     _DEF_TCIC_ILI2511
#else
#error "BSP Version Need to Define"
#endif

#define USE_TOUCH_AK4183        (_DEF_TCIC_AK4183   == TOUCH_IC_TYPE)
#define USE_TOUCH_ILI2511       (_DEF_TCIC_ILI2511  == TOUCH_IC_TYPE)
#define USE_TOUCH_ST1912        (_DEF_TCIC_ST1912   == TOUCH_IC_TYPE)
#define USE_TOUCH_AR1010        (_DEF_TCIC_AR1010   == TOUCH_IC_TYPE)

/* Filesystem Section */
#define FAT_APP_TYPE            DRIVE_NAND // DRIVE_NAND or DRIVE_SDCARD
//#define FAT_APP_TYPE DRIVE_SDCARD
#define APPLICATOPN_FILE_NAME   "CWM-T10_Application.bin"
#define USB_DETECT_PORT         14
#define USB_DETECT_PIN          4
#define USB_DETECT_MASK         (0x1 << USB_DETECT_PIN)
#define USB_DETECT_FLAG         0

/* Appliacation Section */
#define USE_UIM_PWM_BLU       (1) // 1 to BLU PWM from Amazon-II


