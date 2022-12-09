/**
  ******************************************************************************
  * @file    halLEDCtrlAPI.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    24-March-2016
  * @brief   This file provides the main procedure
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
#ifndef _HALLCDCTRLAPI_H_
#define _HALLCDCTRLAPI_H_

#include "common.h"

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
// ---------------------------------------------------------------------------
// structure to LCD Status
// ---------------------------------------------------------------------------
typedef struct tagHALLCDSTATUS
{
  BYTE    bPowerOn;     /* [CD]       LCD Power On/Off */
  BYTE    bReset;       /* [STK,CD]   LCD Reset. TRUE to Reset hold */
  BYTE    bDisplayOn;   /* [CD]       LCD Display On/Off. TRUE to Display */
  BYTE    bHFlip;       /* [CD]       LCD H Flip */  
  BYTE    bVFlip;       /* [CD]       LCD V Flip */  
  BYTE    bBLUOn;       /* [VA,CD]    LCD Backlight On/Off */
  BYTE    cBrightness;  /* [ALL]      LCD Brightness. Unit is % */
  BYTE    bTouchReset;  /* [VA]       LCD Touch IC Reset */
  BYTE    bLVDSOn;      /* [VA,CD]    LCD LVDS Transmitter On/Off */
}GCC_PACKED HAL_LCD_STATUS, *PHAL_LCD_STATUS;
typedef CROMDATA HAL_LCD_STATUS *PCHAL_LCD_STATUS;

// ***************************************************************************
// ******************** END OF PUBLIC CONFIGURATION SECTION ******************
// ***************************************************************************

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *  @brief      This API sets up the internal state of component..
 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlInitialize(void);

/**
 *  @brief      This API Release the internal state of component..
 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlRelease(void);

/**
 *  @brief      This API Sets the LCD Status
 *  @param[in]  sStatusNew: LCD status to Set
                bForceUpdate: TRUE to Force Update
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlStatusSet(HAL_LCD_STATUS sStatusNew, BOOL bForceUpdate);

/**
 *  @brief      This API Gets the status of LCD
 *  @param[in]  None
 *  @param[out] None
 *  @return     status of LCD
 *  @note       None
 *  @attention  None
 */
HAL_LCD_STATUS halLCDCtrlStatusGet(void);

/**
 *  @brief      This API Reset the LCD
 *  @param[in]  bReset: TRUE to Reset, FALSE to Reset Release
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlHWResetSet(BOOL bReset);

/**
 *  @brief      This API Set the LCD Power On (PMIC)
 *  @param[in]  bOn : TRUE for Power On, FALSE for others
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlPowerSet(BOOL bOn);

/**
 *  @brief      This API Enable/Disable the LCD Display (TCON)
 *  @param[in]  bEnable : TRUE for LCD Display Enable, FALSE for LCD Display Disable
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlDisplayOnSet(BOOL bOn);

/**
 *  @brief      This API Set the LCD Horizontal Flip
 *  @param[in]  bFlipEnable : TRUE for LCD Flip Enable, FALSE for others
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlHFlipSet(BOOL bFlipEnable);

/**
 *  @brief      This API Set the LCD Vertical Flip
 *  @param[in]  bFlipEnable : TRUE for LCD Flip Enable, FALSE for others
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlVFlipSet(BOOL bFlipEnable);

/**
 *  @brief      This API Set the LCD Backlight On
 *  @param[in]  bOn : TRUE for BLU On, FALSE for others
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlBluOnSet(BOOL bOn);

/**
 *  @brief      This API Set the LCD Brightness
 *  @param[in]  cBrightness : Brightness value as %. 
 *  @param[out] None
 *  @return     None
 *  @note       0% to PWM Disable. other value to PWM Enable.
 *  @attention  None
 */
void halLCDCtrlBrightnessSet(BYTE cBrightness);

/**
 *  @brief      This API Set the Touch IC Reset
 *  @param[in]  bReset : TRUE to Reset, FALSE for others
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlTouchReset(BOOL bReset);

/**
 *  @brief      This API Set the LVDS Transmitter On
 *  @param[in]  bOn : TRUE for On, FALSE for others
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlLVDSOnSet(BOOL bOn);

#endif // _HALLCDCTRLAPI_H_

