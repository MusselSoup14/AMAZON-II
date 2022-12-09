/**
  ******************************************************************************
  * @file    appPowerMgr.c
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
#include "appPowerMgrAPI.h"
#include "utilExecutiveAPI.h"
#include "halLEDCtrlAPI.h"
#include "halPowerCtrlAPI.h"
#include "dvCPUAPI.h"
#include "appSystemStateAPI.h"
#include "utilHostIfAPI.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS **************
// ***************************************************************************
// There are currently no private types defined locally.

// ***************************************************************************
// ****************** START OF PRIVATE CONFIGURATION SECTION *****************
// ***************************************************************************

// ***************************************************************************
// ******************* END OF PRIVATE CONFIGURATION SECTION ******************
// ***************************************************************************

// ***************************************************************************
// ******************** START OF PRIVATE DATA DECLARATIONS *******************
// ***************************************************************************
// Local power state the system is currently running in.
static ePOWERSTATE      m_ePowerState;

// Local power state the system will be in after completing the state
// transition
static ePOWERSTATE      m_eNextPowerState;

// ***************************************************************************
// *************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ****************
// ***************************************************************************

/**
 *  @brief      This API starts a power transition by setting up the next power state. The
                next time through the main loop process routine we will start any delays or
                special processing associated with this state transition.
 
 *  @param[in]  eNewPowerState : The power state we want to transition to
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void PowerMgrStartStateTransition(ePOWERSTATE eNewPowerState)
{
    // Record the next power state
    m_eNextPowerState = eNewPowerState;

    // Add any special processing such as messages signals here.
    switch (eNewPowerState)
    {
/* ---------------- BEGIN CUSTOM AREA [050] appPowerMgr.c ----------------- */
// Avoid defining local variable here.   (Especically, no assignment command follows!!! )
        case pwrON:
        break;

        case pwrSTANDBY:          
        break;

        case pwrSOFTRESET:          
        break;

        default:
        break;
/* ---------------------------- END CUSTOM AREA --------------------------- */
    }
}

/**
 *  @brief      This API manages transitions from pwrON to pwrSTANDBY, pwrSUSPEND,
                pwrSOFTOFF Any delays or special message processing should be done here in
                advance of calling the hal routins to actually change the power state.
 *  @param[in]  eNextPowerState : The power state to transition to.
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void ProcessPowerOnTransition(ePOWERSTATE eNextPowerState)
{
  // Valid transitions from pwrON include pwrSTANDBY, pwrSUSPEND, pwrSOFTOFF

  switch (eNextPowerState)
  {
/* ---------------- BEGIN CUSTOM AREA [060] appPowerMgr.c ----------------- */
// Avoid defining local variable here.   (Especically, no assignment command follows!!! )
    case pwrSTANDBY:
      // The user just hit the power button so we need to force a system
      // shutdown and go into the low power monitor loop in
      // dvPowerCtrl.c - Wait a while for the buttons to settle.      
      {        
        halPowerCtrlPowerSet(pwrSTANDBY);
        m_ePowerState = eNextPowerState;
      }
    break;

    case pwrSOFTRESET:
      halPowerCtrlPowerSet(pwrSOFTRESET);
    break;

    default:
    break;
/* ---------------------------- END CUSTOM AREA --------------------------- */
  }
}

/**
 *  @brief      This API manages transitions from pwrSTANDBY to pwrON
                Any delays or special message processing should be done here in advance of
                calling the hal routinEs to actually change the power state.
 *  @param[in]  eNextPowerState - The power state to transition to.
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void ProcessStandbyTransition(ePOWERSTATE eNextPowerState)
{
  switch (eNextPowerState)
  {
/* ---------------- BEGIN CUSTOM AREA [090] appPowerMgr.c ----------------- */
// Avoid defining local variable here.   (Especically, no assignment command follows!!! )
    // Valid transition from pwrSOFTOFF is only pwrON
    // For now we are not doing anything special just set the state to on.
    case pwrON:
      halPowerCtrlPowerSet(pwrON);
      m_ePowerState = eNextPowerState;
    break;

    case pwrSOFTRESET:          
      halPowerCtrlPowerSet(pwrSOFTRESET);
    break;
    
    default:      
    break;
/* ---------------------------- END CUSTOM AREA --------------------------- */
  }
}

// ***************************************************************************
// **************** START OF PUBLIC PROCEDURE IMPLEMENTATIONS ****************
// ***************************************************************************

/**
 *  @brief      This API sets the internal state of the component. The local state
                transition timers are initialized and the first power transition from
                pwrOFF or pwrSOFTOFF to pwrON is started. Custom areas are provided to
                allow the OEM to automatically enter any of the defined power states if the
                system is first powering up.
 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appPowerMgrInitialize(void)
{
  ePOWERSTATE     eLastBootPowerState;

  // Before we let the system wake up see how we got here.
  // Before we let the system wake up see how we got here.
#if USE_POWER_STARTFROMON  
  eLastBootPowerState = pwrON;//halPowerCtrlGetLastBootPowerState();
#else
  eLastBootPowerState = pwrSTANDBY;//halPowerCtrlGetLastBootPowerState();
#endif  

  switch(eLastBootPowerState)
  {
    case pwrON:
      m_ePowerState = pwrSTANDBY;
      m_eNextPowerState = pwrON;
      break;
      
    case pwrSTANDBY:
      m_ePowerState = pwrON; // To execute hal Power standby set
      m_eNextPowerState = pwrSTANDBY;
      break;

    default: break;
  }

  if(m_eNextPowerState == pwrON)
  {
  }  
  
  utilExecSignalClear(sgPWR_MGR, 0xFFFF);
}

/**
 *  @brief      This API Release the internal resouces of this component
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appPowerMgrRelease(void)
{
  /* Release Code Here */
}

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
void appPowerMgrPowerStateSet(ePOWERSTATE eNewState)
{

  // Kick off the state transition through the common local routine used by
  // the automatic state monitoring logic.
  // Don't allow a transition to the same state while a transition is in
  // progress. This can happen in the case of some slow devices like IR
  // where we are waking up and there are extra ON button key presses
  // present even though we have not officially transitioned to pwrON yet.
    if (m_eNextPowerState != eNewState && m_ePowerState != eNewState)
    {
        PowerMgrStartStateTransition(eNewState);
    }
}

/**
 *  @brief      This API returns the current system power state to the caller.
 
 *  @param[in]  eNewState : The desired new power state
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
ePOWERSTATE appPowerMgrPowerStateGet(void)
{
    ePOWERSTATE ePowerStateRet;

    if (m_ePowerState != m_eNextPowerState)
        ePowerStateRet = m_eNextPowerState;
    else
        ePowerStateRet = m_ePowerState;

    return (ePowerStateRet);
}

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
void appPowerMgrProcess(void)
{
  if(m_ePowerState != m_eNextPowerState)
  {
    switch(m_ePowerState)
    {
/* ---------------- BEGIN CUSTOM AREA [170] appPowerMgr.c ----------------- */
// Avoid defining local variable here.   (Especically, no assignment command follows!!! )
      case pwrON:
          ProcessPowerOnTransition(m_eNextPowerState);
      break;

      case pwrSTANDBY:
          ProcessStandbyTransition(m_eNextPowerState);
      break;

      default:
      break;
/* ---------------------------- END CUSTOM AREA --------------------------- */
      
    }
  }
    
  if(!utilExecSignalGet(sgPWR_MGR, SIGNAL_PM_RESET))
  {
#if USE_WATCHDOG      
    dvCPUWatchDogKick();
#endif
  }
}

/**
 *  @brief         This API Processing Pre-Power Off Steps
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appPowerMgrPrePowerOff(void)
{
  /* System State Pre Power Off */
  appSysStatePrePowerOff();
}


