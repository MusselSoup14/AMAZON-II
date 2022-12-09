/**
  ******************************************************************************
  * @file    halPowerCtrlAPI.h
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
#ifndef _HALPOWERCTRLAPI_H_
#define _HALPOWERCTRLAPI_H_

#include "common.h"
#include "utilPowerTypes.h"

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

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
void halPowerCtrlInitialize(void);

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
void halPowerCtrlPowerSet(ePOWERSTATE eNewPowerState);

#endif // _HALPOWERCTRLAPI_H_
