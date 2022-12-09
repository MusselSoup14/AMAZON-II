/**
  ******************************************************************************
  * @file    utilExecutive.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    24-March-2016
  * @brief   This file provides the utility procedure
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
//#include "common.h"
#include "utilExecutiveAPI.h"
#include "dvCPUAPI.h"
#include "halUserInputCtrlAPI.h"
#include "dvGPIOAPI.h"
#include "utilHostIfAPI.h"
#include "appSystemStateAPI.h"
#include "halUserInputCtrlAPI.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
// ---------------------------------------------------------------------------
// Status information structure for message queues (read and write indices)
// ---------------------------------------------------------------------------
typedef struct
{
    BYTE                cWriteIndex;   // index where last entry was written
                                       // to queue
    BYTE                cReadIndex;    // index where next entry will be read
                                       // from queue
    BYTE                cQueueEmpty;   // flag indicating that queue is empty
} MSG_QUEUE_INFO, *PMSG_QUEUE_INFO;

// ---------------------------------------------------------------------------
// Configuration information for message queues (size and location)
// ---------------------------------------------------------------------------
typedef struct
{
    BYTE                cQLength;      // number of messages in queue
    BYTE                cMsgSize;      // message size in bytes
    WORD                wOffset;       // start of queue in memory
} MSG_QUEUE_CONFIG;
typedef const MSG_QUEUE_CONFIG *PCMSG_QUEUE_CONFIG;

// ---------------------------------------------------------------------------
// Symbols for pulse types.
//
// PULSE_OWNER= Pulse can be set, read or cleared. Whenever the pulse is set
//              all pulse watchers that immediately follow this pulse in
//              m_acPulseType will also be set.
// PULSE_WATCHER= Pulse can only be set when the previous source pulse in
//                m_acPulseType is set. Pulse can be read or cleared.
//
// Note: Pulse watches allow multiple components independent control of
// reading and clearing a pulse that is set from a single pulse owner.
// ---------------------------------------------------------------------------
#define PULSE_OWNER     0
#define PULSE_WATCHER   1

//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************

// --------------------------------CONFIGURATION------------------------------
// Size of messages for each message queue (bytes)
// --------------------------------CONFIGURATION------------------------------
#define USER_INPUT_MSG_SIZE 	sizeof(KEY_EVENT)		
#define USER_TOUCH_MSG_SIZE   sizeof(TOUCH_EVENT)

// --------------------------------CONFIGURATION------------------------------
// Number of messages in each message queue
// --------------------------------CONFIGURATION------------------------------
#define USER_INPUT_QUEUE_MSGS 	100 // was 5, to avoid Queue Full by encoder rotation

/* ---------------------------- END CUSTOM AREA --------------------------- */

// ===========================================================================
// CONFIGURATION: Configuration data for message queues
// ===========================================================================
typedef struct
{
/* --------------------------- BEGIN CUSTOM AREA  --------------------------
 * CONFIGURATION: Structure definition for memory used to store message queues.
 * Each queue should be defined as a separate byte array in the structure. */
  BYTE acUserInput[USER_INPUT_QUEUE_MSGS * USER_INPUT_MSG_SIZE];
  BYTE acUserTouch[USER_INPUT_QUEUE_MSGS * USER_TOUCH_MSG_SIZE];
/* ---------------------------- END CUSTOM AREA --------------------------- */
} MSG_QUEUES;

static CROMDATA MSG_QUEUE_CONFIG m_asQConfig[mqINVALID] = {
  /* --------------------------- BEGIN CUSTOM AREA  --------------------------
 * CONFIGURATION: Size and location configuration for message queues.  The
 * order entries in this array MUST match the order of enumerations in
 * eMESSAGE_QUEUES type */
  {USER_INPUT_QUEUE_MSGS, USER_INPUT_MSG_SIZE, offsetof(MSG_QUEUES, acUserInput)},
  {USER_INPUT_QUEUE_MSGS, USER_TOUCH_MSG_SIZE, offsetof(MSG_QUEUES, acUserTouch)},
/* ---------------------------- END CUSTOM AREA --------------------------- */
};

#define TICK_GET dvCPUTimerGet(cpuTIMER_SYSTEM)

//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************

// ---------------------------------------------------------------------------
// Signal flags for each signal group
// ---------------------------------------------------------------------------
static WORD m_awSignalGroup[sgINVALID] = 
{
  0,
};

// -----------------------------------------------------------------------------
// Variables for each Var group
// -----------------------------------------------------------------------------
static  WORD m_awVarGroup[vgINVALID] =  
{
  0,
};

// ---------------------------------------------------------------------------
// Pulse flags for each pulse group
// ---------------------------------------------------------------------------
static WORD m_awPulseGroup[pgINVALID] = 
{
  0,
};

// ---------------------------------------------------------------------------
// Pulse types for each pulse group
// ---------------------------------------------------------------------------
static const BYTE m_acPulseType[pgINVALID] = {
/* --------------- BEGIN CUSTOM AREA [025] utilExecutive.c -------------------
 * CONFIGURATION: Define pulse types here. Note: The order of the items used
 * in this list must match the order used in ePULSE_GROUPS */
  PULSE_OWNER, //   pgSYS_MGR,
  PULSE_WATCHER, // pgSYS_MGR_UI_WATCHER,

  PULSE_OWNER, //   pgMISC,
  PULSE_WATCHER, // pgMISC_UI_WATCHER,
/* ---------------------------- END CUSTOM AREA --------------------------- */
};

// ---------------------------------------------------------------------------
// Status (busy or not) for each shared resource
// ---------------------------------------------------------------------------
static BYTE m_acRscBusy[reINVALID] = {
    FALSE,
};

// ---------------------------------------------------------------------------
// Status data for each message queue
// ---------------------------------------------------------------------------
static MSG_QUEUE_INFO   m_asQInfo[mqINVALID];

// ---------------------------------------------------------------------------
// Memory buffer for message queues
// ---------------------------------------------------------------------------
static BYTE m_acMsgQueues[sizeof(MSG_QUEUES)];

// ---------------------------------------------------------------------------
// Tick counter data, 1mSec per Tick by gettimeofday()
// ---------------------------------------------------------------------------
//#define USE_DOWN_SYSTEM_TICK (0)  // Upward Tick Counter

static DWORD             m_adwTickCountRef[coINVALID]; 
static DWORD             m_adwTickCountStart[coINVALID];

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************
/**
 *  @brief      Configures Interrupt Vector Table to Executive
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void execNVICInitialize(void)
{

}

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *  @brief      Initializes the internal state of the single-thread executive
                    
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecInitialize(void)
{
  BYTE cIndex;
  
  // initialize counters to empty state
  for (cIndex = 0; cIndex < coINVALID; cIndex++)
  {
    m_adwTickCountRef[cIndex] = 0;
    m_adwTickCountStart[cIndex] = 0;
  }

  /* Initialize the Interrupt */
  execNVICInitialize();
}

/**
 *  @brief      Release the internal state of the single-thread executive
                    
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecRelease(void)
{
}

/**
 *  @brief      This API sets the specified signal or signals in the specified group.
                    
 *  @param[in]  eGroup   : The signal group in which the signal flag(s) will be set
                        wSignals : The signal flag(s) that will be set
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecSignalSet(eSIGNAL_GROUPS eGroup,
                       WORD           wSignals)
{
  // validate parameters in debug mode only
  ASSERT(eGroup < sgINVALID);

  //CPU_INT_DISABLE;
  m_awSignalGroup[eGroup] |= wSignals;
  //CPU_INT_ENABLE;
}

/**
 *  @brief      This API clears the specified signal or signals from the specified group.
                    
 *  @param[in]  eGroup   : The signal group from which the signal flag(s) will be cleared
                        wSignals : The signal flag(s) that will be cleared
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecSignalClear(eSIGNAL_GROUPS eGroup,
                         WORD           wSignals)
{
  // validate parameters in debug mode only
  ASSERT(eGroup < sgINVALID);

  //CPU_INT_DISABLE;
  m_awSignalGroup[eGroup] &= ~(wSignals);
  //CPU_INT_ENABLE;
}

/**
 *  @brief      This API reads the current state of the signal flags in the specified group.
                    
 *  @param[in]  eGroup : The signal group from which the signal flags will be read
                        wMask  : A mask for the signals to be read (only signals that are set in the mask will be read)
 *  @param[out] None
 *  @return     State of the signals selected by the mask
 *  @note       None
 *  @attention  None
 */
WORD utilExecSignalGet(eSIGNAL_GROUPS eGroup,
                       WORD           wMask)
{
  WORD                wRetVal;

  // validate parameters in debug mode only
  ASSERT(eGroup < sgINVALID);

  //CPU_INT_DISABLE;
  wRetVal = (m_awSignalGroup[eGroup] & wMask);
  //CPU_INT_ENABLE;

  return wRetVal;
}

/**
 *  @brief      This API suspends execution of the caller until one or more of the signals
                    in the specified group are set, or until the specified timeout expires. 
                    The caller is suspended in a loop (busy wait).
                    
 *  @param[in]  wMask  : A mask for the signals to be monitored (only signals that are set in the mask will be checked)
                        dwTicks : Number of ticks to wait for a signal
 *  @param[out] None
 *  @return     State of the signals selected by the mask
 *  @note       None
 *  @attention  None
 */
WORD utilExecSignalWaitAny(eSIGNAL_GROUPS eGroup,
                           WORD           wMask,
                           DWORD           dwTicks)
{
  DWORD                dwStartTime, dwEndTime, dwTime;
  // validate parameters in debug mode only
  ASSERT(eGroup < sgINVALID);

  // compute HW timer value after delay
  dwStartTime = TICK_GET;
#if USE_DOWN_SYSTEM_TICK  
  dwEndTime = dwStartTime - (dwTicks + 1);
#else
  if(dwTicks)
  dwEndTime = dwStartTime + (dwTicks - 1);
  else
    dwEndTime = dwStartTime + 1;
#endif

  // loop until any of the signals are set or the time expires.
  while (TRUE)
  {
    if (m_awSignalGroup[eGroup] & wMask)
    {
      break;        
    }
    
    dwTime = TICK_GET;
#if USE_DOWN_SYSTEM_TICK   
    if (dwEndTime < dwStartTime && 
        !(dwTime <= dwStartTime && 
          dwTime > dwEndTime))
    {
      break;
    }

    if (dwEndTime >= dwStartTime && 
        !(dwTime <= dwStartTime ||
           dwTime > dwEndTime))
    {
      break;
    }
#else
    if(dwEndTime < dwStartTime &&
        dwTime < dwStartTime && dwTime >= dwEndTime)
    {
      break;
    }
    
    if(dwEndTime >= dwStartTime &&
        dwTime > dwStartTime && dwTime >= dwEndTime)
    {
      break;
    }
#endif    

  }
    
  return m_awSignalGroup[eGroup] & wMask;
}

/**
 *  @brief      This API suspends execution of the caller until all of the signals in the
                    specified group are set, or until the specified timeout expires.  The
                    caller is suspended in a loop (busy wait).
                    
 *  @param[in]  eGroup : The signal group from which the signal flags will be monitored
                        wMask  : A mask for the signals to be monitored (only signals that are set in the mask will be checked)
                        dwTicks : Number of ticks to wait for the signals
 *  @param[out] None
 *  @return     State of the signals selected by the mask
 *  @note       None
 *  @attention  None
 */
WORD utilExecSignalWaitAll(eSIGNAL_GROUPS eGroup,
                           WORD           wMask,
                           DWORD           dwTicks)
{
  DWORD                dwStartTime, dwEndTime, dwTime;
  // validate parameters in debug mode only
  ASSERT(eGroup < sgINVALID);

  // compute HW timer value after delay
  dwStartTime = TICK_GET;
#if USE_DOWN_SYSTEM_TICK    
  dwEndTime = dwStartTime - (dwTicks + 1);
#else
  if(dwTicks)
  dwEndTime = dwStartTime + (dwTicks - 1);
  else
    dwEndTime = dwStartTime + 1;
#endif

  // loop until all of the signals are set or the time expires.
  while (TRUE)
  {
    if ((m_awSignalGroup[eGroup] & wMask) == wMask)
    {
      break;
    }
    
    dwTime = TICK_GET;
#if USE_DOWN_SYSTEM_TICK    
    if (dwEndTime < dwStartTime && 
        !(dwTime <= dwStartTime && 
          dwTime > dwEndTime))
    {
      break;
    }
    if (dwEndTime >= dwStartTime && 
        !(dwTime <= dwStartTime ||
           dwTime > dwEndTime))
    {
      break;
    }
#else    
    if(dwEndTime < dwStartTime &&
        dwTime < dwStartTime && dwTime >= dwEndTime)
    {
      break;
    }
    
    if(dwEndTime >= dwStartTime &&
        dwTime > dwStartTime && dwTime >= dwEndTime)
    {
      break;
    }
#endif    
  }
  
  return m_awSignalGroup[eGroup] & wMask;
}

/**
 *  @brief      This API sets the specified pulse or pulses in the specified group.
                    
 *  @param[in]  eGroup  : The pulse group in which the pulse flag(s) will be set
                        wPulses : The pulse flag(s) that will be set
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecPulseSet(ePULSE_GROUPS eGroup,
                      WORD          wPulses)
{
  WORD wTmpPulseId = (WORD)eGroup;
  
  // validate parameters in debug mode only
  ASSERT(eGroup < pgINVALID);

  // Validate that pulse being set is in a pulse owner group and not a
  // pulse watcher group. Pulse watchers can only be set by setting a
  // the pulse in the related pulse owner group.
  ASSERT(PULSE_OWNER == m_acPulseType[wTmpPulseId]);

  //CPU_INT_DISABLE;
  
  m_awPulseGroup[wTmpPulseId] |= wPulses;    // set source pulse

  // If pulse owner group has any related pulse watcher groups, set their
  // pulse state too.
  //while (++eGroup < pgINVALID &&
  //       PULSE_WATCHER == m_acPulseType[eGroup])
  while ((ePULSE_GROUPS)(++wTmpPulseId) < pgINVALID &&
         PULSE_WATCHER == m_acPulseType[wTmpPulseId])
  {
    //m_awPulseGroup[eGroup] |= wPulses;
    m_awPulseGroup[wTmpPulseId] |= wPulses;
  }
  //CPU_INT_ENABLE;
}

/**
 *  @brief      This API clears the specified pulse or pulses from the specified group.
                    
 *  @param[in]  eGroup  : The pulse group from which the pulse flag(s) will be cleared
                        wPulses : The pulse flag(s) that will be cleared
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecPulseClear(ePULSE_GROUPS eGroup,
                        WORD          wPulses)
{
  WORD wTmpPulseId = (WORD)eGroup;
  
  // validate parameters in debug mode only
  ASSERT(eGroup < pgINVALID);

  //CPU_INT_DISABLE;
  m_awPulseGroup[wTmpPulseId] &= ~(wPulses);

  if (PULSE_OWNER == m_acPulseType[wTmpPulseId])
  {
    // Pulse being cleared is a pulse owner. If pulse owner group has any
    // related pulse watcher groups, clear their pulse state too.
    while (++wTmpPulseId < pgINVALID &&
           PULSE_WATCHER == m_acPulseType[wTmpPulseId])
    {
      m_awPulseGroup[wTmpPulseId] &= ~(wPulses);
    }
  }
  else
  {
    // Pulse being cleared is a pulse watcher. Check if other pulses in
    // all the other related watchers are also clear. If so, clear the
    // pulses in the pulse owner group too.
    ePULSE_GROUPS   eTmpGroup = eGroup;
    WORD            wPulsesAllGroups = 0;

    // Check pulses in pulse watcher groups that follow this one in the
    // list.
    while (++wTmpPulseId < pgINVALID &&
           PULSE_WATCHER == m_acPulseType[wTmpPulseId])
    {
      // Set flag for any bits in pulse mask that are set.
      wPulsesAllGroups |= (m_awPulseGroup[wTmpPulseId] & wPulses);
    }

    // Check pulses in pulse watcher groups that preceed this one in the
    // list.
    wTmpPulseId = (WORD)eTmpGroup;
    while (PULSE_OWNER != m_acPulseType[--wTmpPulseId])
    {
      // Set flag for any bits in pulse mask that are set.
      wPulsesAllGroups |= (m_awPulseGroup[wTmpPulseId] & wPulses);
    }

    // We want to end up with a mask of bits set to a one for any pulses
    // that were zero in all related watcher groups. We do this by
    /// inverting the gathered bits and then masking them with the pulse
    // mask.
    wPulsesAllGroups = (~wPulsesAllGroups) & wPulses;
    if (wPulsesAllGroups)
    {
      m_awPulseGroup[wTmpPulseId] &= ~(wPulsesAllGroups);
    }
  }

  //CPU_INT_ENABLE;
}

/**
 *  @brief      This API reads the current state of the pulse flags in the specified group.
                    
 *  @param[in]  eGroup : The pulse group from which the pulse flags will be read
                        wMask  : A mask for the pulses to be read (only pulses that are set in the mask will be read)
 *  @param[out] None
 *  @return     State of the pulses selected by the mask
 *  @note       None
 *  @attention  None
 */
WORD utilExecPulseGet(ePULSE_GROUPS eGroup,
                      WORD          wMask)
{
  WORD                wRetVal;

  // validate parameters in debug mode only
  ASSERT(eGroup < pgINVALID);

  //CPU_INT_DISABLE;
  wRetVal = (m_awPulseGroup[eGroup] & wMask);
  //CPU_INT_ENABLE;

  return wRetVal;
}

/**
 *  @brief      This API suspends execution of the caller until one or more of the pulses
                    in the specified group are set, or until the specified timeout expires. 
                    The caller is suspended in a loop (busy wait).
                    
 *  @param[in]  eGroup : The signal group in which the signal flags will be monitored
                        wMask  : A mask for the pulses to be monitored (only pulses that are set in the mask will be checked)
                        dwTicks : Number of ticks to wait for a pulse
 *  @param[out] None
 *  @return     State of the pulses selected by the mask
 *  @note       None
 *  @attention  None
 */
WORD utilExecPulseWaitAny(ePULSE_GROUPS eGroup,
                          WORD          wMask,
                          DWORD          dwTicks)
{
  DWORD                dwStartTime, dwEndTime, dwTime;
  // validate parameters in debug mode only
  ASSERT(eGroup < pgINVALID);

  // compute HW timer value after delay
  dwStartTime = TICK_GET;
#if USE_DOWN_SYSTEM_TICK  
  dwEndTime = dwStartTime - (dwTicks + 1);
#else
  if(dwTicks)    
  dwEndTime = dwStartTime + (dwTicks -1);
  else
    dwEndTime = dwStartTime + 1;
#endif

  // loop until any of the pulses are set or the time expires.
  while (TRUE)
  {
    if (m_awPulseGroup[eGroup] & wMask)
    {
      break;
    }
      
    dwTime = TICK_GET;

#if USE_DOWN_SYSTEM_TICK
    if (dwEndTime < dwStartTime && 
        !(dwTime <= dwStartTime && 
          dwTime > dwEndTime))
    {
      break;
    }
    if (dwEndTime >= dwStartTime && 
        !(dwTime <= dwStartTime ||
           dwTime > dwEndTime))
    {
      break;
    }
#else
    if(dwEndTime < dwStartTime &&
        dwTime < dwStartTime && dwTime >= dwEndTime)
    {
      break;
    }
    
    if(dwEndTime >= dwStartTime &&
        dwTime > dwStartTime && dwTime >= dwEndTime)
    {
      break;
    }
#endif
  }
  
  return m_awPulseGroup[eGroup] & wMask;
}

/**
 *  @brief      This API suspends execution of the caller until all of the pulses in the
                    specified group are set, or until the specified timeout expires.  The
                    caller is suspended in a loop (busy wait).
                    
 *  @param[in]  eGroup : The pulse group from which the pulse flags will be monitored
                        wMask  : A mask for the pulses to be monitored (only pulses that are set
                                     in the mask will be checked)
                        dwTicks : Number of ticks to wait for the signals
 *  @param[out] None
 *  @return     State of the pulses selected by the mask
 *  @note       None
 *  @attention  None
 */
WORD utilExecPulseWaitAll(ePULSE_GROUPS eGroup,
                          WORD          wMask,
                          DWORD          dwTicks)
{
  DWORD                dwStartTime, dwEndTime, dwTime;
  // validate parameters in debug mode only
  ASSERT(eGroup < pgINVALID);

  // compute HW timer value after delay
  dwStartTime = TICK_GET;
#if USE_DOWN_SYSTEM_TICK  
  dwEndTime = dwStartTime - (dwTicks + 1);
#else
  if(dwTicks)
  dwEndTime = dwStartTime + (dwTicks -1);
  else
    dwEndTime = dwStartTime + 1;
#endif

  // loop until all of the pulses are set or the time expires.
  while (TRUE)
  {
    if ((m_awPulseGroup[eGroup] & wMask) == wMask)
    {
      break;
    }
      
    dwTime = TICK_GET;
#if USE_DOWN_SYSTEM_TICK    
    if (dwEndTime < dwStartTime && 
        !(dwTime <= dwStartTime && 
          dwTime > dwEndTime))
    {
      break;
    }
      
    if (dwEndTime >= dwStartTime && 
        !(dwTime <= dwStartTime ||
           dwTime > dwEndTime))
    {
      break;
    }
#else
    if(dwEndTime < dwStartTime &&
        dwTime < dwStartTime && dwTime >= dwEndTime)
    {
      break;
    }
    
    if(dwEndTime >= dwStartTime &&
        dwTime > dwStartTime && dwTime >= dwEndTime)
    {
      break;
    }
#endif
    
  }
  
  return m_awPulseGroup[eGroup] & wMask;
}

/**
 *  @brief      This API sets the value of the specified tick counter.
                    
 *  @param[in]  eCounter : Identifier of counter to set
                dwTicks   : Value to set timer to, in executive ticks.
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecCounterSet(eCOUNTERS eCounter,
                              DWORD      dwTicks)
{
    // validate arguments in debug mode only
    ASSERT(eCounter < coINVALID);

    // store timer start value and
    m_adwTickCountStart[eCounter] = dwTicks;

    // get current value of timer 0 and store it, reset rollover flag
    m_adwTickCountRef[eCounter] = TICK_GET;
}

/**
 *  @brief      This API reads the current value of the specified tick counter.
                    
 *  @param[in]  eCounter : Identifier of counter to read
 *  @param[out] None
 *  @return     Current value of timer, in executive ticks
 *  @note       None
 *  @attention  None
 */
DWORD utilExecCounterGet(eCOUNTERS eCounter)
{
  DWORD                dwTime = 0;

  // validate arguments in debug mode only
  ASSERT(eCounter < coINVALID);

  // if the count timeout is zero, just return remaining count of zero
  if (m_adwTickCountStart[eCounter] > 0)
  {
    // compute the timer ticks that have elapsed so far
    CPU_INT_DISABLE;
#if USE_DOWN_SYSTEM_TICK        
    dwTime = (DWORD)(m_adwTickCountRef[eCounter] - TICK_GET); 
#else        
    dwTime =  (DWORD)(TICK_GET - m_adwTickCountRef[eCounter]); 
#endif
    CPU_INT_ENABLE;

    // compute the counter ticks remaining until timeout
    if (m_adwTickCountStart[eCounter] > dwTime)
    {
      // this subtraction requires unsigned, same size start and timer values 
      dwTime = m_adwTickCountStart[eCounter] - dwTime;
    }
    else
    {
      // the counter has finished so disable it and return zero
      m_adwTickCountStart[eCounter] = 0;
      dwTime = 0;
    }

  }

  return dwTime;
}

/**
 *  @brief      This API sends a message to the specified queue.
                    
 *  @param[in]  eQueue  : Queue to send message to
                        pacData : Pointer to message data
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecMsgSend(eMESSAGE_QUEUES eQueue,
                     PBYTE           pacData)
{
  PMSG_QUEUE_INFO     psQInfo = &m_asQInfo[eQueue];
  PCMSG_QUEUE_CONFIG  psQCfg = &m_asQConfig[eQueue];

  // validate parameters in debug mode only
  ASSERT(eQueue < mqINVALID);
  ASSERT(pacData != NULL);

  if (psQInfo->cQueueEmpty)
  {
    psQInfo->cQueueEmpty = FALSE;
  }
  else
  {
    psQInfo->cWriteIndex =
        (psQInfo->cWriteIndex == psQCfg->cQLength - 1) ?
        0 : psQInfo->cWriteIndex + 1;
    ASSERT(psQInfo->cWriteIndex != psQInfo->cReadIndex);
  }

  memcpy(&m_acMsgQueues[psQCfg->wOffset + psQCfg->cMsgSize * psQInfo->cWriteIndex],
                 pacData,
                 psQCfg->cMsgSize);
  
}

/**
 *  @brief      This API reads a message from the specified queue.
                    
 *  @param[in]  eQueue  : Queue to read message from                        
 *  @param[out] pacData : Pointer where message data will be stored
 *  @return     rcSUCCESS if a message was read, rcERROR if queue was empty
 *  @note       None
 *  @attention  None
 */
eRESULT utilExecMsgRead(eMESSAGE_QUEUES eQueue,
                        PBYTE           pacData)
{
  BYTE                cDepth;
  PMSG_QUEUE_INFO     psQInfo = &m_asQInfo[eQueue];
  PCMSG_QUEUE_CONFIG  psQCfg = &m_asQConfig[eQueue];

  // validate parameters in debug mode only
  ASSERT(eQueue < mqINVALID);
  ASSERT(pacData != NULL);

  if (psQInfo->cWriteIndex == psQInfo->cReadIndex)
  {
    cDepth = psQInfo->cQueueEmpty ? 0 : 1;
  }
  else
  {
    cDepth = (psQInfo->cWriteIndex > psQInfo->cReadIndex) ?
        psQInfo->cWriteIndex - psQInfo->cReadIndex :
        psQInfo->cWriteIndex - psQInfo->cReadIndex + psQCfg->cQLength + 1;
  }

  if (cDepth)
  {
    memcpy(pacData,
                   &m_acMsgQueues[psQCfg->wOffset + psQCfg->cMsgSize * psQInfo->cReadIndex],
                   psQCfg->cMsgSize);

    if (psQInfo->cReadIndex == psQInfo->cWriteIndex)
    {
      psQInfo->cQueueEmpty = TRUE;
    }
    else
    {
      psQInfo->cReadIndex = (psQInfo->cReadIndex == psQCfg->cQLength - 1) 
                                                         ? 0 : psQInfo->cReadIndex + 1;
    }
  }

  return cDepth ? rcSUCCESS : rcERROR;
}

/**
 *  @brief      This API clears all messages from the specified queue.
                    
 *  @param[in]  eQueue : Queue to clear
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecMsgClear(eMESSAGE_QUEUES eQueue)
{
    PMSG_QUEUE_INFO     psQInfo = &m_asQInfo[eQueue];

    // validate parameters in debug mode only
    ASSERT(eQueue < mqINVALID);

    psQInfo->cReadIndex = 0;
    psQInfo->cWriteIndex = 0;
    psQInfo->cQueueEmpty = TRUE;
}

/**
 *  @brief      This API checks if a message is present in the specified queue.
                    
 *  @param[in]  eQueue : Queue to check for messages
 *  @param[out] None
 *  @return     TRUE if there is a message in the queue, FALSE otherwise
 *  @note       None
 *  @attention  None
 */
BOOL utilExecMsgPresent(eMESSAGE_QUEUES eQueue)
{
    // validate parameters in debug mode only
    ASSERT(eQueue < mqINVALID);

    return !m_asQInfo[eQueue].cQueueEmpty;
}

/**
 *  @brief      This API suspends execution of the caller until a message is received in
                    the specified queue.  The caller is suspended in a loop (busy wait).
                    
 *  @param[in]  eQueue  : Queue to send message to
                        wTicks  : Time to wait for message before returning                        
 *  @param[out] pacData : Pointer to message data
 *  @return     rcSUCCESS if a message was read, rcERROR if queue was empty
 *  @note       None
 *  @attention  None
 */
eRESULT utilExecMsgWait(eMESSAGE_QUEUES eQueue,
                        WORD            wTicks,
                        PBYTE           pacData)
{
  eRESULT             eResult = rcERROR;
  PMSG_QUEUE_INFO     psQInfo = &m_asQInfo[eQueue];
  PCMSG_QUEUE_CONFIG  psQCfg = &m_asQConfig[eQueue];
  DWORD                dwStartTime, dwEndTime, dwTime;

  // validate parameters in debug mode only
  ASSERT(eQueue < mqINVALID);
  ASSERT(pacData != NULL);

  // compute HW timer value after delay
  dwStartTime = TICK_GET;
#if USE_DOWN_SYSTEM_TICK    
  dwEndTime = dwStartTime - (wTicks + 1);
#else
  if(wTicks)
  dwEndTime = dwStartTime + (wTicks - 1);
  else
    dwEndTime = dwStartTime  + 1;
#endif

  // loop until the que is not empty or the time expires.
  while (TRUE)
  {
    if (!(psQInfo->cQueueEmpty))
    {
        break;
    }
      
    dwTime = TICK_GET;
      
#if USE_DOWN_SYSTEM_TICK        
    if (dwEndTime < dwStartTime && 
        !(dwTime <= dwStartTime && 
          dwTime > dwEndTime))
    {
      break;
    }
    if (dwEndTime >= dwStartTime && 
        !(dwTime <= dwStartTime ||
           dwTime > dwEndTime))
    {
      break;
    }
#else
    if(dwEndTime < dwStartTime &&
        dwTime < dwStartTime && dwTime >= dwEndTime)
    {
      break;
    }

    if(dwEndTime >= dwStartTime &&
        dwTime > dwStartTime && dwTime >= dwEndTime)
    {
      break;
    }
#endif
  }
    
  if (!psQInfo->cQueueEmpty)
  {
    memcpy(pacData, &m_acMsgQueues[psQCfg->wOffset + psQCfg->cMsgSize * psQInfo->cReadIndex],
                   psQCfg->cMsgSize);
    eResult = rcSUCCESS;
  }

    return eResult;
}

/**
 *  @brief      This API sets the specified variable.
                    
 *  @param[in]  eVar: the variable flag
                        wValue: the WORD value for variable
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void  utilExecVarSet (eVAR_GROUPS eVar, WORD wValue)
{
  ASSERT(eVar < vgINVALID);
  
  m_awVarGroup[eVar] = wValue;
}

/**
 *  @brief      This API gets the specified variable.
                    
 *  @param[in]  eVar: the variable flag
                        wValue: the WORD value for variable
 *  @param[out] None
 *  @return     Return: Value the WORD value of variable
 *  @note       None
 *  @attention  None
 */
WORD  utilExecVarGet (eVAR_GROUPS eVar)
{	
  WORD wValue;
  
  ASSERT(eVar < vgINVALID);

  wValue = m_awVarGroup[eVar];
  
  return wValue;
}

/**
 *  @brief      This API marks the specified shared resource as busy.
                    
 *  @param[in]  eResource : The resource to be marked busy (reserved)
 *  @param[out] None
 *  @return     rcSUCCESS if resource not busy, rcERROR if resource was already marked busy
 *  @note       None
 *  @attention  None
 */
eRESULT utilExecRscBusySet(eRESOURCES eResource)
{
    eRESULT             eResult;

    //CPU_INT_DISABLE;

    // validate arguments in debug mode only
    ASSERT(eResource < reINVALID);

    // mark resource as reserved unless it is already reserved
    if (!m_acRscBusy[eResource])
    {
        m_acRscBusy[eResource] = TRUE;
        eResult = rcSUCCESS;
    }
    else
    {
        eResult = rcERROR;
    }

    //CPU_INT_ENABLE;

    return eResult;
}

/**
 *  @brief      This API reads the current state of the specified shared resource.
                    
 *  @param[in]  eResource : The resource to be read (reserved)
 *  @param[out] None
 *  @return     TRUE  if resource is busy, FALSE if resource is not
 *  @note       None
 *  @attention  None
 */
BOOL utilExecRscBusyGet(eRESOURCES eResource)
{
    BOOL             bResult;

    //CPU_INT_DISABLE;

    // validate arguments in debug mode only
    ASSERT(eResource < reINVALID);

    bResult = m_acRscBusy[eResource];
    
    //CPU_INT_ENABLE;

    return bResult;
}

/**
 *  @brief      This API clears the busy status for the specified shared resource.
                    
 *  @param[in]  eResource : The resource to be cleared (released)
 *  @param[out] pacData : Pointer to message data
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecRscBusyClear(eRESOURCES eResource)
{
    //CPU_INT_DISABLE;

    // validate arguments in debug mode only
    ASSERT(eResource < reINVALID);

    // mark the resource as unused
    m_acRscBusy[eResource] = FALSE;

    //CPU_INT_ENABLE;
}


