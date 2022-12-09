/**
  ******************************************************************************
  * @file    appSystemStateAPI.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    24-October-2016
  * @brief   This file provides APIs the System State Manager
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

#ifndef _APPSYSTEMSTATEAPI_H_
#define _APPSYSTEMSTATEAPI_H_

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "utilSignalTypes.h"
#include "utilExecutiveAPI.h"
#include "utilParameterTable.h"
#include "utilDataMgrAPI.h"

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
/* Expander Module Flags from HOST */
#define EXPANDER_NONE         0x00      // None Expander Module
#define EXPANDER_RTC          (1 << 0)  // RTC Module

/* size of HOST Serial Number as BYTE (included NULL) */
#define DEVINFO_SERIAL_SIZE   20      

// ------------------------------------------------------------------
// enum for System State machine
// ------------------------------------------------------------------
typedef enum
{
  sysstateIDLE,
/* --------------------- BEGIN CUSTOM AREA --------------------------
 * CONFIGURATION: Add system states here */
  sysstateTHERAPY,
/* ------------------- END CUSTOM AREA --------------------------- */
  sysstateDIAG,
  sysstateSYSLOG_VEIW,
  sysstateINVALID                    // Used for enumerated type range checking (DO NOT REMOVE)
} eSYS_STATE;

// ------------------------------------------------------------------
// enum for elapsed times
// ------------------------------------------------------------------
typedef enum
{
  elapTIME_UNIT,  // include All modes

  elapTIME_INVALID // Sentinel
}GCC_PACKED eELAPSED_TIME;

// ------------------------------------------------------------------
// enum to Photo Id
// ------------------------------------------------------------------
typedef enum
{
  photoID_1,
  photoID_2,
  photoID_3,
  photoID_4,
  photoID_5,

  photoID_INVALID
}GCC_PACKED ePHOTO_ID;

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************
#define SYSLOG_DRIVE              DRIVE_SDCARD
#define SYSLOG_FLUSH_PERIOD_MAX   600     /* It Depend on "HOSTIF_CMD_STATUS" Period 
                                             and "SYSLOG_INDEX_MAX" on UIM Module F/W
                                             ex; HOSTIF_CMD_STATUS = 500ms, SYSLOG_INDEX_MAX=600, 600*0.5s = 300s Maximum. */
#define SYSLOG_SYNC_PERIOD_MAX    600     /* Max Sync from H/W RTC counter. unit is second */
#define SYSLOG_FILESIZE_MAX       10240   /* syslog File Size Max. unit is KBytes. 
                                             if Over this size syslog file will be backed-up */
#define SYSLOG_FREESPACE_TIMES    5       /* If Drive Freespace is low than wLogFileSizeMax*SYSLOG_FREESPACE_TIMES
                                             LOG will not be flushed (Not enough space!) */
// ---------------------------------------------------------------------------
// structure for syslog Properties
// ---------------------------------------------------------------------------
typedef struct tagSYSLOGVARS
{
  BYTE    bLogEnable;             /* TRUE: Log Enable, FALSE: Log Disable */
  BYTE    bLogEnableOnShotOnly;   /* TRUE if enable log during shot only */
  WORD    wFlushPeriod;           /* Syslog Flush Period. unit is second. 
                                     this period started on IDLE(there are no event to syslog). */
  WORD    wTimestampSyncPeriod;   /* Sync from H/W RTC counter. unit is second. Do not use in UIM */
  WORD    wLogFileSizeMax;        /* syslog File Size Max. unit is KBytes */
}GCC_PACKED SYSLOG_VARS, *PSYSLOG_VARS;
typedef CROMDATA SYSLOG_VARS *PCSYSLOG_VARS;

// ---------------------------------------------------------------------------
// structure to System User Settings
// ---------------------------------------------------------------------------
typedef struct tagSTATUSUSER
{
  BYTE              cUserVolume;            /* User Volume Setting. Range:0~4, Step:1, Def:2 */  
  BYTE              cUserOPTime;            /* User Opertion Time Set value. unit is Minute. 
                                               Range:1~60, step:1, Def:15 */  
}GCC_PACKED STATUS_USER, *PSTATUS_USER;
typedef CROMDATA STATUS_USER *PCSTATUS_USER;

// ---------------------------------------------------------------------------
// structure for System runtime status
// ---------------------------------------------------------------------------
typedef struct tagSTATUSRUN
{
  WORD              wErrorFlags;
  WORD              wInfoFlags;
  
  WORD              wTimeRemainingS;      /* Time Remaining. Unit is second */    
}GCC_PACKED STATUS_RUN, *PSTATUS_RUN;
typedef CROMDATA STATUS_RUN *PCSTATUS_RUN;

// ---------------------------------------------------------------------------
// structure for System status
// ---------------------------------------------------------------------------
typedef struct tagSTATUSSYS
{  
  BYTE                    bTherapyStart;  /* Therapy Start Step */
  
  STATUS_USER             sUser;
  STATUS_RUN              sRun;
}GCC_PACKED STATUS_SYS, *PSTATUS_SYS;
typedef CROMDATA STATUS_SYS *PCSTATUS_SYS;

/* ------------------------------ SYSTEM DATA ------------------------------ */
// ---------------------------------------------------------------------------
// structure for System Properties
// ---------------------------------------------------------------------------
typedef struct tagSYSTEMPROP
{
  BYTE            bElapsedTimeEnable;       /* TRUE to enable elapsed time(Unit On) */
  BYTE            cCriticalTimerPeriodMS;   /* Critical Timer Period. Unit is ms. 1~100 */           
}GCC_PACKED SYSTEM_PROP, *PSYSTEM_PROP;
typedef CROMDATA SYSTEM_PROP *PCSYSTEM_PROP;

// ---------------------------------------------------------------------------
// Structure for SystemState NVRAM Data (xxxBytes)
// ---------------------------------------------------------------------------
typedef struct tagSYSTATE_NVRAM_DATA
{
  SYSTEM_PROP           sSysProp;
}GCC_PACKED SYSTATE_NVRAM_DATA, *PSYSTATE_NVRAM_DATA;

/* --------------------------- SYSTEM CONFIGURATION -------------------------- */
// ---------------------------------------------------------------------------
// structure for System Configuration Misc.
// ---------------------------------------------------------------------------
typedef struct tagSYSCONFMISC
{
  BYTE              acHWVersion[2];                         /* 0:Major, 1:Minor */
}GCC_PACKED SYSCONF_MISC, *PSYSCONF_MISC;
typedef CROMDATA SYSCONF_MISC *PCSYSCONF_MISC;  

// ---------------------------------------------------------------------------
// Structure for System Configuration NVRAM Data (xxxBytes)
// This Area can be protected from NVRAM RESET by cProtectFlag
// ---------------------------------------------------------------------------
#define DEVINFO_SERIAL_SIZE 20
typedef struct tagSYSCONFIG_NVRAM_DATA
{
  BYTE            cProtectFlag;  
  DWORD           dwUnitOnTimes;  // Unit On(LCD ON) Times. unit is Minutes. 4Bytes
  SYSCONF_MISC    sSysConfMisc;   // xxBytes
}GCC_PACKED SYSCONFIG_NVRAM_DATA, *PSYSCONFIG_NVRAM_DATA; 


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *  @brief         This API sets up the internal state of this component.
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateInitialize(void);

/**
 *  @brief         This API Release the internal resouces of this component
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateRelease(void);

/**
 *  @brief         This API performs periodic processing for the system state. It updates the
 *  @brief         state of the system appropriately.
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateProcess(void);

/**
 *  @brief      This API performs periodic processing for the system state. 
                Time Critical
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateProcessTimeCritical(void) ;

/**
 *  @brief         This API gets the current system state
 *  @param[in]  None
 *  @param[out]  None
 *  @return     enum for System state
 *  @note       None
 *  @attention  None
 */
eSYS_STATE appSysStateGet(void);

/**
 *  @brief         This API sets the current system state
 *  @param[in]  eState: System state to change
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateSet(eSYS_STATE eState);

/**
 *  @brief         This API Processing Pre-Power Off Steps
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStatePrePowerOff(void);

/**
 *  @brief      This API returns the parameter descriptor in the display ordinate.
 *  @param[in]  eParameter   : Identifier of image path parameter to get the descriptor for
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
PARAM_INFO appSysStateParamInfoGet(eSYS_PARAM eParameter);

/**
 *  @brief      This API increase/decrease the parameter in the display ordinate.
 *  @param[in]  eParameter: Identifier of image path parameter to get the descriptor for
                bInc: TRUE to Increase, FALSE to Decrease
 *  @param[out] None
 *  @return     TRUE if Perform Successfully, FALSE to others
 *  @note       None
 *  @attention  None
 */
BOOL appSysStateParamIncDec(eSYS_PARAM eParameter, BOOL bInc);

/**
 *  @brief         This API Set the parameter in the display ordinate.
 *  @param[in]  eParameter: Identifier of image path parameter to get the descriptor for
                nValue: Value to Set                
 *  @param[out]  None
 *  @return     TRUE if Perform Successfully, FALSE to others
 *  @note       None
 *  @attention  None
 */
BOOL appSysStateParamSet(eSYS_PARAM eParameter, INT nValue);


/**
 *  @brief      This API Set the system status
 *  @param[in]  pcsSysStatusNew : pointer to System status to set
 *  @param[out] None
 *  @return     TRUE if perform successfully, FALSE to others
 *  @note       None
 *  @attention  None
 */
BOOL appSysStateStatusSet(PCSTATUS_SYS pcsSysStatusNew);

/**
 *  @brief      This API Set the system status
 *  @param[in]  None
 *  @param[out] None
 *  @return     System status
 *  @note       None
 *  @attention  None
 */
STATUS_SYS appSysStateStatusGet(void);

/**
 *  @brief      This API Request the system status to Host
 *  @param[in]  None
 *  @param[out] None
 *  @return     None (Noti)
 *  @note       None
 *  @attention  None
 */
void appSysStateStatusRequest(void);

/**
 *  @brief      This API get the pointer of system status structure
 *  @param[in]  None
 *  @param[out] None
 *  @return     pointer of system status structure
 *  @note       None
 *  @attention  None
 */
PSTATUS_SYS appSysStateStatusPtrGet(void);

/**
 *  @brief      This API Init the System log.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateSysLogBufferInit(void);

/**
 *  @brief      This API Append the SYSLOG Info
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Append successfully, FALSE to others
 *  @note       None
 *  @attention  None
 */
BOOL appSysStateSysLogInfoAppend(void);

/**
 *  @brief      This API Append the SYSLOG log Data
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Append successfully, FALSE to others
 *  @note       None
 *  @attention  None
 */
BOOL appSysStateSysLogDataAppend(void);


/**
 *  @brief      This API flush(dump) the SYSLOG to physical storage
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateSysLogFlush(void);

/**
 *  @brief      This API return available Device Diagnotic state
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if available Device Diagnotic state, FASE to else
 *  @note       None
 *  @attention  None
 */
BOOL appSysStateDiagAvailable(void);

/**
 *  @brief      This API perform Unit Test-1 by Key1
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateUnitTestKey_1(void);

/**
 *  @brief      This API perform Unit Test-2 by Key2
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateUnitTestKey_2(void);

/**
 *  @brief      This API perform Unit Test-3 by Key2
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateUnitTestKey_3(void);

/**
 *  @brief        This API Gets the System Properties
 *  @param[in]    None
 *  @param[out]   None
 *  @return       System Properties
 *  @note         None
 *  @attention    None
 */
SYSTEM_PROP appSysStateSystemPropGet(void);

/**
 *  @brief        This API Sets the System Properties
 *  @param[in]    sSysProp: System Properities to set
 *  @param[out]   None
 *  @return       TRUE if value was changed, FALSE for others
 *  @note         None
 *  @attention    None
 */
BOOL appSysStateSystemPropSet(SYSTEM_PROP sSysProp);

/**
 *  @brief         This API Gets the System Timestamp
 *  @param[in]  bReadFromRTC: TRUE to read from RTC
 *  @param[out]  None
 *  @return    Currently System Timestamp
 *  @note       None
 *  @attention  None
 */
DWORD appSysStateTimestampGet(BOOL bReadFromRTC);

/**
 *  @brief         This API Sets the System Timestamp
 *  @param[in]  dwTimestamp: timestamp to set
                          bSaveToRTC: TRUE to save to RTC
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateTimestampSet(DWORD dwTimestamp, BOOL bSaveToRTC);

/**
 *  @brief      This API Sets the System Configuration Protection
 *  @param[in]  bProtect : TRUE to Protect, FALSE for Unlock
 *  @param[out] None
 *  @return     TRUE if Changed, FALSE for others
 *  @note       None
 *  @attention  None
 */
BOOL appSysConfigProtectSet(BOOL bProtect);

/**
 *  @brief      This API Gets the System Configuration Protection
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Protected, FALSE for others
 *  @note       None
 *  @attention  None
 */
BOOL appSysConfigProtectGet(void);

/**
 *  @brief      This API Gets the Device Shot Times
 *  @param[in]  eTimeId: Elapsed Time Id to Get
 *  @param[out] pwDays, pcHour, pcMin : Pointer to return back time format
 *  @return     Elapsed time of total minitues
 *  @note       None
 *  @attention  None
 */
DWORD appSysConfigElapsedTimeGet(eELAPSED_TIME eTimeId
                                              , PWORD pwDays
                                              , PBYTE pcHour
                                              , PBYTE pcMin);

/**
 *  @brief      This API Increase the Device Elapsed Times
 *  @param[in]  eTimeId: Elapsed Time Id to Get                          
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysConfigElapsedTimeIncrease(eELAPSED_TIME eTimeId);

/**
 *  @brief      This API increase the Device Elapsed Times
 *  @param[in]  cFlags: flags that complex bit shift as Elapsed Time Id to set
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysConfigElapsedTimesIncrease(BYTE cFlags);

/**
 *  @brief      This API reset the Device Elapsed Times
 *  @param[in]  cFlags: flags that complex bit shift as Elapsed Time Id to set
 *  @param[out] None
 *  @return     TRUE if perform successfully, FALSE to others
 *  @note       None
 *  @attention  None
 */
BOOL appSysConfigElapsedTimesReset(BYTE cFlags);

/**
 *  @brief         This API Gets the System Configuration Misc.
 *  @param[in]  None
 *  @param[out]  None
 *  @return    System Configuration Misc
 *  @note       None
 *  @attention  None
 */
SYSCONF_MISC appSysConfigMiscGet(void);

/**
 *  @brief      This API Sets the System Configuration Misc
 *  @param[in]  sSysConfMiscNew: System Configuration Misc to set
 *  @param[out] None
 *  @return     TRUE if value was changed, FALSE for others
 *  @note       None
 *  @attention  None
 */
BOOL appSysConfigMiscSet(SYSCONF_MISC sSysConfMiscNew);


/**
 *  @brief      This API is accessed by the Data Manager component to allow initialization
                and resetting of this components persistent data node by handling external
                events triggered by the persistent data storage.
 *  @param[in]  eNodeID : Node identifier that is saved internally to access this node
                eEvent : Identifier of the external event. Event can be  nveRESETALL or nveINITIALIZE
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateNodeCallback(eNODE_NAME eNodeID, eNV_EVENT  eEvent);


#endif /* _APPSYSTEMSTATEAPI_H_ */

