/**
  ******************************************************************************
  * @file    halPowerCtrl.c
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
  * <h2><center>&copy; Copyright 1997-2014 (c) MadeFactory Inc.</center></h2>
  ******************************************************************************
  */  
/* Includes ------------------------------------------------------------------*/
#include "halPowerCtrlAPI.h"
#include "dvCPUAPI.h"
#include "dvGPIOAPI.h"
#include "halLEDCtrlAPI.h"
#include "utilPowerTypes.h"
#include "utilExecutiveAPI.h"
#include "halLCDCtrlAPI.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS **************
// ***************************************************************************

// ***************************************************************************
// ****************** START OF PRIVATE CONFIGURATION SECTION *****************
// ***************************************************************************

/* ---------------- BEGIN CUSTOM AREA [010] halPowerCtrl.c -------------------- */

/* ---------------------------- END CUSTOM AREA --------------------------- */

// ***************************************************************************
// ******************* END OF PRIVATE CONFIGURATION SECTION ******************
// ***************************************************************************

// ***************************************************************************
// ******************** START OF PRIVATE DATA DECLARATIONS *******************
// ***************************************************************************

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
void halPowerCtrlInitialize(void)
{
}

/**
 *  @brief      This API is used to prepare the driver layer for a power state change.
                Initially this API has just a single call to the chip driver when preparing
                to enter the ultra low power mode. Upon returning from that call
                dvPowerctrl will be called and it will not return to this API. Only a valid
                wake event will bring the system back to the pwrON power state through the
                boot code.
 
 *  @param[in]  eNewPowerState : The power state to change to..
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halPowerCtrlPowerSet(ePOWERSTATE eNewPowerState)
{

  CPU_INT_DISABLE; // to avoid user msg que fulled by key event

#if USE_WATCHDOG
  dvCPUWatchDogEnable(FALSE);
#endif   
  
  switch (eNewPowerState)
  {
/* ---------------- BEGIN CUSTOM AREA [020] halPowerCtrl.c ---------------- */
// Avoid defining local variable here.   (Especically, no assignment command follows!!! )
    case pwrON:
      // We are transitioning from a low power state where we had turned
      // off the external power. Turn it back on here so the rest of the
      // initialization can complete.
    break;

    case pwrSTANDBY:

    break;

    case pwrSOFTRESET:
    break;

    default:  break;
/* ---------------------------- END CUSTOM AREA --------------------------- */
  }

#if USE_WATCHDOG
  dvCPUWatchDogEnable(TRUE);
#endif    
  
  CPU_INT_ENABLE;  
}


