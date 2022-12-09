/**
  ******************************************************************************
  * @file    halLEDCtrl.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    02-August-2014
  * @brief   This file provides APIs the Power Manager
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
#include "halLCDCtrlAPI.h"
#include "dvCPUAPI.h"
#include "dvGPIOAPI.h"
#include "utilExecutiveAPI.h"
#include "dvCPUPeriphAPI.h"
#include "dvILI2511API.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS **************
// ***************************************************************************
void display_clock_init(); // amzon2kInit.c

// ***************************************************************************
// ****************** START OF PRIVATE CONFIGURATION SECTION *****************
// ***************************************************************************

// ***************************************************************************
// ******************* END OF PRIVATE CONFIGURATION SECTION ******************
// ***************************************************************************

// ***************************************************************************
// ******************** START OF PRIVATE DATA DECLARATIONS *******************
// ***************************************************************************
static HAL_LCD_STATUS m_sHalLCDStatus;
// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************

// ***************************************************************************
// **************** START OF PUBLIC PROCEDURE IMPLEMENTATIONS ****************
// ***************************************************************************

/**
 *  @brief      This API sets up the internal state of component..
 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlInitialize(void)
{
  memset(&m_sHalLCDStatus, 0, sizeof(HAL_LCD_STATUS));  

  display_clock_init();    

  /* Sync with Driver Layer */
  halLCDCtrlStatusSet(m_sHalLCDStatus, TRUE); 

#if USE_AMAZON_STK
  halLCDCtrlHWResetSet(TRUE);
  commDelayMS(1000);
  halLCDCtrlHWResetSet(FALSE);
  commDelayMS(100);

  dvAK4183Initialize();  
#elif USE_LCD_1024x600
  halLCDCtrlHWResetSet(TRUE);
  commDelayMS(1000);
  halLCDCtrlHWResetSet(FALSE);
  commDelayMS(100);
  
  halLCDCtrlDisplayOnSet(TRUE);
  commDelayMS(100);  
#elif USE_LCD_1280x800
  #if USE_TOUCH_ILI2511
  dvILI2511Initialize();
  #endif
#endif
}

/**
 *  @brief      This API Release the internal state of component..
 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlRelease(void)
{

}

/**
 *  @brief      This API Sets the LCD Status
 *  @param[in]  sStatusNew: LCD status to Set
                bForceUpdate: TRUE to Force Update
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlStatusSet(HAL_LCD_STATUS sStatusNew, BOOL bForceUpdate)
{
  PCHAL_LCD_STATUS pcsLCDStatusCur = (PCHAL_LCD_STATUS)&m_sHalLCDStatus;
    
  if(pcsLCDStatusCur->bPowerOn != sStatusNew.bPowerOn || bForceUpdate)
    halLCDCtrlPowerSet(sStatusNew.bPowerOn); //LCD Power 3.3V

  if(pcsLCDStatusCur->bReset != sStatusNew.bReset || bForceUpdate)
    halLCDCtrlHWResetSet(sStatusNew.bReset);

  if(pcsLCDStatusCur->bDisplayOn != sStatusNew.bDisplayOn || bForceUpdate)
    halLCDCtrlDisplayOnSet(sStatusNew.bDisplayOn);

  if(pcsLCDStatusCur->bHFlip != sStatusNew.bHFlip || bForceUpdate)
    halLCDCtrlHFlipSet(sStatusNew.bHFlip);

  if(pcsLCDStatusCur->bVFlip != sStatusNew.bVFlip || bForceUpdate)
    halLCDCtrlVFlipSet(sStatusNew.bVFlip);

  if(pcsLCDStatusCur->bBLUOn != sStatusNew.bBLUOn || bForceUpdate)
    halLCDCtrlBluOnSet(sStatusNew.bBLUOn); //

  if(pcsLCDStatusCur->cBrightness != sStatusNew.cBrightness || bForceUpdate)
    halLCDCtrlBrightnessSet(sStatusNew.cBrightness); //LCD BLU 

  if(pcsLCDStatusCur->bTouchReset != sStatusNew.bTouchReset || bForceUpdate)
    halLCDCtrlTouchReset(sStatusNew.bTouchReset); // 

  if(pcsLCDStatusCur->bLVDSOn != sStatusNew.bLVDSOn || bForceUpdate)
    halLCDCtrlLVDSOnSet(sStatusNew.bLVDSOn); // LVDS IC Power 
  
}

/**
 *  @brief      This API Gets the status of LCD
 *  @param[in]  None
 *  @param[out] None
 *  @return     status of LCD
 *  @note       None
 *  @attention  None
 */
HAL_LCD_STATUS halLCDCtrlStatusGet(void)
{
  return m_sHalLCDStatus;
}

/**
 *  @brief      This API Reset the LCD
 *  @param[in]  bReset: TRUE to Reset, FALSE to Reset Release
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlHWResetSet(BOOL bReset)
{
#if (USE_AMAZON_STK || USE_LCD_1024x600)
  dvGPIOFieldSet(gfGPO_LCD_nRESET, !bReset);
#endif
  m_sHalLCDStatus.bReset = bReset;  
}

/**
 *  @brief      This API Set the LCD Power On (PMIC)
 *  @param[in]  bOn : TRUE for Power On, FALSE for others
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlPowerSet(BOOL bOn)
{
  dvGPIOFieldSet(gfGPO_LCD_nPOWER, (BOOL)!bOn);
  m_sHalLCDStatus.bPowerOn = bOn;
}

/**
 *  @brief      This API Enable/Disable the LCD Display (TCON)
 *  @param[in]  bEnable : TRUE for LCD Display Enable, FALSE for LCD Display Disable
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlDisplayOnSet(BOOL bOn)
{  
  dvGPIOFieldSet(gfGPO_LCD_nSTDBY, !bOn);
  m_sHalLCDStatus.bDisplayOn = bOn;  
}

/**
 *  @brief      This API Set the LCD Horizontal Flip
 *  @param[in]  bFlipEnable : TRUE for LCD Flip Enable, FALSE for others
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlHFlipSet(BOOL bFlipEnable)
{
  dvGPIOFieldSet(gfGPO_LCD_LR_nFLIP, !bFlipEnable);
  m_sHalLCDStatus.bHFlip = bFlipEnable;
}

/**
 *  @brief      This API Set the LCD Vertical Flip
 *  @param[in]  bFlipEnable : TRUE for LCD Flip Enable, FALSE for others
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlVFlipSet(BOOL bFlipEnable)
{
  dvGPIOFieldSet(gfGPO_LCD_UD_FLIP, bFlipEnable);
  m_sHalLCDStatus.bVFlip = bFlipEnable;
}

/**
 *  @brief      This API Set the LCD Backlight On
 *  @param[in]  bOn : TRUE for BLU On, FALSE for others
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlBluOnSet(BOOL bOn)
{
  dvGPIOFieldSet(gfGPO_LCD_BLU_ON, bOn);
  m_sHalLCDStatus.bBLUOn = bOn;
}

/**
 *  @brief      This API Set the LCD Brightness
 *  @param[in]  cBrightness : Brightness value as %. 
 *  @param[out] None
 *  @return     None
 *  @note       0% to PWM Disable. other value to PWM Enable.
 *  @attention  None
 */
void halLCDCtrlBrightnessSet(BYTE cBrightness)
{  
  cBrightness = MIN(cBrightness, 100);

  dvCPUPWMDutySet(cpuPWM_LCD_DIMMING, cBrightness, FALSE);
  dvCPUPWMEnableSet(cpuPWM_LCD_DIMMING, (cBrightness) ? TRUE : FALSE);

  m_sHalLCDStatus.cBrightness = cBrightness;
}

/**
 *  @brief      This API Set the Touch IC Reset
 *  @param[in]  bReset : TRUE to Reset, FALSE for others
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlTouchReset(BOOL bReset)
{
#if USE_TOUCH_ILI2511
  dvILI2511HWReset();
#endif
  
  m_sHalLCDStatus.bTouchReset = FALSE; //was bReset but make it always FALSE
}

/**
 *  @brief      This API Set the LVDS Transmitter On
 *  @param[in]  bOn : TRUE for On, FALSE for others
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLCDCtrlLVDSOnSet(BOOL bOn)
{
  dvGPIOFieldSet(gfGPO_LCD_LVDS_ON, bOn);
  m_sHalLCDStatus.bLVDSOn = bOn;
}


