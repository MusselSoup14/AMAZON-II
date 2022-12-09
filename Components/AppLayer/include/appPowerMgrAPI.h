/**
  ******************************************************************************
  * @file    appPowerMgrAPI.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    13-August-2014
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

#ifndef _APPPOWERMGRAPI_H_
#define _APPPOWERMGRAPI_H_

#include "common.h"
#include "utilPowerTypes.h"

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************


// ***************************************************************************
// ******************** END OF PUBLIC CONFIGURATION SECTION ******************
// ***************************************************************************

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *  @brief      This API sets the internal state of the component. The local state
                       transition timers are initialized and the first power transition from
                       pwrOFF or pwrSTANDBY to pwrON is started. Custom areas are provided to
                       allow the OEM to automatically enter any of the defined power states if the
                       system is first powering up.
 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appPowerMgrInitialize(void);

/**
 *  @brief         This API Release the internal resouces of this component
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appPowerMgrRelease(void);

/**
 *  @brief      This API forces a change from the current power state to the new state
                provided by the caller. The Route Id is included to provide a mechanism to
                power down all components for just the given route.
 
 *  @param[in]  eNewState : The desired new power state
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appPowerMgrPowerStateSet(ePOWERSTATE eNewState);

/**
 *  @brief         This API returns the current system power state to the caller.
 
 *  @param[in]  eNewState : The desired new power state
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
ePOWERSTATE appPowerMgrPowerStateGet(void);

/**
 *  @brief      This API performs the cyclic processing for the power manager component.
                       Local timing is controlled using a counter created in the initialization
                       procedure. When a power state transition event is created automatically or
                       by a call to appPowerMgrPowerStateSet this API will call the transition
                       handler for the currently selected power state until the transition is
                       marked as complete by the handler. For example, a forced transition from
                       pwrON to pwrSTANDBY would call the local procedure
                       appProcessPowerOnTransition until the current and next power states are
                       both equal to pwrSTANDBY. The transition handler is where any delays or
                       state machines for a particular transition should be implemented.
 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appPowerMgrProcess(void);

/**
 *  @brief      This API Processing Pre-Power Off Steps
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appPowerMgrPrePowerOff(void);


#endif // _APPPOWERMGRAPI_H_
