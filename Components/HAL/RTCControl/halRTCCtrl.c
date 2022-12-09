/**
  ******************************************************************************
  * @file    halRTCCtrl.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    15-05-2019
  * @brief   This file provides APIs the Real Time Clock
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
#include "halRTCCtrlAPI.h"
#include "dvDeviceBusAPI.h"
#if USE_HW_RTC
#include "dvDS1374API.h"
#endif

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
/* Sync from H/W RTC counter. unit is second. 10minutes */
#define HAL_TIMESTAMP_SYNC_PERIOD_DEFAULT 600 

//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************


//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************
/* Internal RTC Status */
static HAL_RTC_STATUS m_sHalRTCStatus; 

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************
/**
 *  @brief      This API sets up the internal state of component.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
void halRTCCtrlInitialize(void)
{
  ZERO_STRUCTURE(m_sHalRTCStatus);
  
  m_sHalRTCStatus.wSysTimeSyncPeriod      = HAL_TIMESTAMP_SYNC_PERIOD_DEFAULT;
#if USE_DS1374_RTC
  m_sHalRTCStatus.bHWRTCAvailable         = dvDS1374PresenceGet(0);
#endif
}

/**
 *  @brief      This API Release the internal state of component.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halRTCCtrlRelease(void)
{
  /* NONE */
}

/**
 *  @brief      This API Process the internal state of component..
                Increase timestamp and sync with RTC on due time
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halRTCCtrlProcess(void)
{
  m_sHalRTCStatus.dwSysTimestamp++;
  //msgDbg("Timestamp: %d", m_dwSysTimestamp);  
  
#if USE_HW_RTC  
  /* Sync from H/W RTC */
  if(m_sHalRTCStatus.bHWRTCAvailable
    && m_sHalRTCStatus.dwSysTimestamp >= m_sHalRTCStatus.dwSysTimestampSyncNext)
  {
    DWORD dwTimestamp = 0;
    
    dwTimestamp = halRTCCtrlTimestampGet(TRUE);

    if(dwTimestamp < DS1374_TIMESTAMP_ERROR)
    {
      m_sHalRTCStatus.dwSysTimestamp = dwTimestamp;
      // Set Next Sync Counter
      m_sHalRTCStatus.dwSysTimestampSyncNext 
              = m_sHalRTCStatus.dwSysTimestamp + m_sHalRTCStatus.wSysTimeSyncPeriod;
    }
  }
#endif  
}

/**
 *  @brief      This API Get the RTC Status.                         
 *  @param[in]  bReadFromRTC: TRUE to read timestamp from RTC
 *  @param[out] None
 *  @return     RTC Status
 *  @note       None
 *  @attention  None
 */
HAL_RTC_STATUS halRTCCtrlStatusGet(BOOL bReadFromRTC)
{
  HAL_RTC_STATUS sStatus = m_sHalRTCStatus;

  if(bReadFromRTC)
    sStatus.dwSysTimestamp = halRTCCtrlTimestampGet(bReadFromRTC);
  
  return sStatus;
}

/**
 *  @brief      This API Get the H/W RTC availance.                         
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if H/W RTC Available, FALSE to others
 *  @note       None
 *  @attention  None
 */
BOOL halRTCCtrlHWAvailableGet(void)
{
  return m_sHalRTCStatus.bHWRTCAvailable;
}

/**
 *  @brief      This API Gets the Timestamp
 *  @param[in]  bReadFromRTC: TRUE to read from RTC
 *  @param[out] None
 *  @return     Currently Timestamp
 *  @note       None
 *  @attention  None
 */
DWORD halRTCCtrlTimestampGet(BOOL bReadFromRTC)
{
  DWORD dwSysTimestamp = 0;
  BOOL bHWTimestampIsValid = FALSE;

#if USE_HW_RTC
  if(bReadFromRTC
    && halRTCCtrlHWAvailableGet())
  {
  #if USE_DS1374_RTC      
    if(rcSUCCESS == dvDS1374TimestampGet(0, &dwSysTimestamp))
    {
      bHWTimestampIsValid = TRUE;
    }
  #endif
  }
#endif    

  // if invalid H/W timestamp then get it from S/W counter based timestamp
  if(!bHWTimestampIsValid)
  {
    dwSysTimestamp = m_sHalRTCStatus.dwSysTimestamp;
  }
  
  return dwSysTimestamp;
}

/**
 *  @brief      This API Sets the Timestamp
 *  @param[in]  dwTimestamp:  timestamp to set
                bSaveToRTC:   TRUE to save to RTC
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halRTCCtrlTimestampSet(DWORD dwTimestamp, BOOL bSaveToRTC)
{
  m_sHalRTCStatus.dwSysTimestamp = dwTimestamp;
  m_sHalRTCStatus.dwSysTimestampSyncNext = dwTimestamp + m_sHalRTCStatus.wSysTimeSyncPeriod;

#if USE_HW_RTC
  if(bSaveToRTC && halRTCCtrlHWAvailableGet())
  {
  #if USE_DS1374_RTC
    dvDS1374TimestampSet(0, dwTimestamp);
  #endif  
  }
#endif  
}

/**
 *  @brief      This API Gets the Timestamp Sync Period with RTC
 *  @param[in]  None
 *  @param[out] None
 *  @return     Timestamp Sync Period with RTC
 *  @note       None
 *  @attention  None
 */
 WORD halRTCCtrlTimestampSyncPeriodGet(void)
{
  return m_sHalRTCStatus.wSysTimeSyncPeriod;
}

/**
 *  @brief      This API Sets the Timestamp Sync Period with RTC
 *  @param[in]  wSysTimeSyncPeriod: Timestamp Sync Period with RTC. Unit is second
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halRTCCtrlTimestampSyncPeriodSet(WORD wSysTimeSyncPeriod)
{
  // Set Sync period
  m_sHalRTCStatus.wSysTimeSyncPeriod = wSysTimeSyncPeriod;
  
  // Set Next Sync time
  m_sHalRTCStatus.dwSysTimestampSyncNext 
    = m_sHalRTCStatus.dwSysTimestamp + wSysTimeSyncPeriod;  
}

