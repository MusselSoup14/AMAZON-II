/**
  ******************************************************************************
  * @file    halRTCCtrlAPI.h
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
#if !defined(_HALRTCCTRLAPI_H_)
#define _HALRTCCTRLAPI_H_

#include "common.h"
#if USE_DS1374_RTC
#include "dvDS1374API.h"
#endif


// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
// ---------------------------------------------------------------------------  
// Structure to HAL RTC Status 
// ---------------------------------------------------------------------------  
typedef struct tagRTCSTATUS
{
  DWORD   dwSysTimestamp;             /* Current system timestamp */
  DWORD   dwSysTimestampSyncNext;     /* Next Sync Counter from H/W RTC */
  WORD    wSysTimeSyncPeriod;         /* Timestamp sync period as second from H/W RTC */
  BYTE    bHWRTCAvailable;            /* TRUE if H/W RTC is available */
}GCC_PACKED HAL_RTC_STATUS, *PHAL_RTC_STATUS;
typedef CROMDATA HAL_RTC_STATUS *PCHAL_RTC_STATUS;

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
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
void halRTCCtrlInitialize(void);

/**
 *  @brief      This API Release the internal state of component.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halRTCCtrlRelease(void);

/**
 *  @brief      This API Process the internal state of component.. 
                Increase timestamp and sync with RTC on due time
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halRTCCtrlProcess(void);

/**
 *  @brief      This API Get the RTC Status.                         
 *  @param[in]  bReadFromRTC: TRUE to read timestamp from RTC
 *  @param[out] None
 *  @return     RTC Status
 *  @note       None
 *  @attention  None
 */
HAL_RTC_STATUS halRTCCtrlStatusGet(BOOL bReadFromRTC);

/**
 *  @brief      This API Get the H/W RTC availance.                         
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if H/W RTC Available, FALSE to others
 *  @note       None
 *  @attention  None
 */
BOOL halRTCCtrlHWAvailableGet(void);

/**
 *  @brief      This API Gets the Timestamp
 *  @param[in]  bReadFromRTC: TRUE to read from RTC
 *  @param[out] None
 *  @return     Currently System Timestamp
 *  @note       None
 *  @attention  None
 */
DWORD halRTCCtrlTimestampGet(BOOL bReadFromRTC);

/**
 *  @brief      This API Sets the Timestamp
 *  @param[in]  dwTimestamp:  timestamp to set
                bSaveToRTC:   TRUE to save to RTC
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halRTCCtrlTimestampSet(DWORD dwTimestamp, BOOL bSaveToRTC);

/**
 *  @brief      This API Gets the Timestamp Sync Period with RTC
 *  @param[in]  None
 *  @param[out] None
 *  @return     Timestamp Sync Period with RTC
 *  @note       None
 *  @attention  None
 */
 WORD halRTCCtrlTimestampSyncPeriodGet(void);

/**
 *  @brief      This API Sets the Timestamp Sync Period with RTC
 *  @param[in]  wSysTimeSyncPeriod: Timestamp Sync Period with RTC. Unit is second
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halRTCCtrlTimestampSyncPeriodSet(WORD wSysTimeSyncPeriod);

#endif //#if !defined(_HALRTCCTRLAPI_H_)

