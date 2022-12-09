/**
  ******************************************************************************
  * @file    appSystemState.c
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

/* Includes ------------------------------------------------------------------*/
#include "appSystemStateAPI.h"
#include "utilHostIfAPI.h"
#include "appUserInterfaceAPI.h"
#include "appPowerMgrAPI.h"
#include "utilMiscAPI.h"
#include "halPowerCtrlAPI.h"
#include "halLEDCtrlAPI.h"
#include "utilCRC.h"
#include "halCANCtrlAPI.h"
#include "halRTCCtrlAPI.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
// ------------------------------------------------------------------
// NVRAM Protect Flags
// ------------------------------------------------------------------
#define SYSCONFIG_PROTECT_FLAG      0xAC   // Protected from NVRAM Reset
#define SYSCONFIG_UNPROTECT_FLAG    0xFF

// ------------------------------------------------------------------
// System Counter Period 
// ------------------------------------------------------------------
#define PERIOD_MONITOR_10MS         (10000)     // per 10ms
#define PERIOD_MONITOR_100MS        (100000)    // per 100ms
#define PERIOD_MONITOR_500MS        (500000)    // per 500ms
#define PERIOD_MONITOR_1S           (1000000)   // per 1s

// ------------------------------------------------------------------
// SYSLOG structure symbols
// ------------------------------------------------------------------
#define SYSLOG_ID_START       0xF1FE
#define SYSLOG_ID_END         0xFEF1
#define SYSLOG_ID_LEN         2
#define SYSLOG_HEADER_LEN     sizeof(SYSLOG_HEADER)
#define SYSLOG_TAIL_LEN       sizeof(SYSLOG_TAIL)

#define SYSLOG_BUFFER_SIZE    600 //was 1200 //was 600 // syslog Index count, 0.5s * 1200 = 600s = 10minutes

#define SYSLOG_DIR            "../log"
#define SYSLOG_FILENAME       "CWMT10_SYSLOG"
#define SYSLOG_PATH           SYSLOG_DIR"/"SYSLOG_FILENAME
#define SYSLOG_PATH_MAX_LEN   30 // syslog file path max (for buffer temporary)

// ------------------------------------------------------------------
// enum for SYSLOG payload type
// ------------------------------------------------------------------
typedef enum
{
  sysptINFO,
  sysptLOG,
  
  sysptINVALID
}GCC_PACKED  eSYSLOG_PAYLOAD_TYPE;

// ------------------------------------------------------------------
// SYSLOG Header
// ------------------------------------------------------------------
typedef struct tagSYSLOGHEADER
{
  WORD                  wIDStart;
  eSYSLOG_PAYLOAD_TYPE  ePayloadType;
  WORD                  wPayloadSize;
}GCC_PACKED SYSLOG_HEADER, *PSYSLOG_HEADER;

// ------------------------------------------------------------------
// SYSLOG payload info
// ------------------------------------------------------------------
typedef struct tagSYSLOGPAYLOADINFO
{
  BYTE      cSyslogVerMajor;                      /* syslog Versiion Major */
  BYTE      cSyslogVerMinor;                      /* Syslog Version Minor */
  CHAR      acSerialNo[DEVINFO_SERIAL_SIZE];  
  BYTE      acSWVer[6];                           /* Host S/W Version
                                                      0:Major, 1:Minor, 2:Custom, 3:Candidate, 
                                                      4:NVRAM Major, 5:NVRAM Minor */
  BYTE      cExpanderFlags;                       /* Expander Module Board Flags */
}GCC_PACKED SYSLOG_PAYLOADINFO, *PSYSLOG_PAYLOADINFO;
typedef CROMDATA SYSLOG_PAYLOADINFO *PCSYSLOG_PAYLOADINFO;

// ------------------------------------------------------------------
// SYSLOG payload
// ------------------------------------------------------------------
typedef struct tagSYSLOGPAYLOAD
{
  DWORD       dwTimestamp;
/* --------------------- BEGIN CUSTOM AREA --------------------------
 * CONFIGURATION: Add SYSLOG payload item here */  

/* ------------------- END CUSTOM AREA --------------------------- */  
}GCC_PACKED SYSLOG_PAYLOADLOG, *PSYSLOG_PAYLOADLOG;
typedef CROMDATA SYSLOG_PAYLOADLOG *PCSYSLOG_PAYLOADLOG;

// ------------------------------------------------------------------
// SYSLOG tail
// ------------------------------------------------------------------
typedef struct tagSYSLOGTAIL
{
  BYTE    cChecksum;
  WORD    wIDEnd;  
}GCC_PACKED SYSLOG_TAIL;

// ------------------------------------------------------------------
// SYSLOG structure
// ------------------------------------------------------------------
typedef struct tagSYSLOG
{
  SYSLOG_HEADER sHeader;
  
  union 
  {
    SYSLOG_PAYLOADINFO sPayloadInfo;
    SYSLOG_PAYLOADLOG sPayloadLog;
  } uPayload;
  
  SYSLOG_TAIL sTail;
}GCC_PACKED SYSLOG, *PSYSLOG;
typedef CROMDATA SYSLOG *PCSYSLOG;

// ------------------------------------------------------------------
// structure to syslog status
// ------------------------------------------------------------------
typedef struct tagSYSLOG_STATUS
{
  BOOL      bLogFileAvailable;                /* TRUE if SYSLOG file available */
  DWORD     dwTimestampFlushNext;             /* Next SYSLOG flush timestamp */
  WORD      wLogBufferIndex;                  /* SYSLOG buffer index */
  SYSLOG    asLogBuffer[SYSLOG_BUFFER_SIZE];  /* SYSLOG buffer */
}SYSLOG_STATUS, *PSYSLOG_STATUS;
typedef CROMDATA SYSLOG_STATUS *PCSYSLOG_STATUS;


//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************
// ---------------------------------------------------------------------------
// System Configraution MISC Default
// ---------------------------------------------------------------------------
static CROMDATA SYSCONF_MISC m_sSystemConfMiscDefault = 
{
  .acHWVersion          = {0, 1},
};

// ---------------------------------------------------------------------------
// System Properties Default
// ---------------------------------------------------------------------------
static CROMDATA SYSTEM_PROP m_sSysPropDefault = 
{
  .bElapsedTimeEnable         = TRUE,
  .cCriticalTimerPeriodMS     = 2
};


//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************
/* System State machine */
static eSYS_STATE      m_eStateSys;

/* System Configuration Protect flag */
static BYTE            m_cProtectFlag; 

/* System Configuration MISC */
static PCSYSCONF_MISC  m_pcsSysConfMisc  = NULL; // Pointer to System Configuration RAM Mirror

/* System Properties */
static PCSYSTEM_PROP   m_pcsSysProp      = NULL; // Pointer to System Properties RAM Mirror

/* parameter storage and range configuration data */
static PPARAM_INFO     m_pasSysParamInfo;

/* System Status */
static STATUS_SYS       m_sStatusSys;
static PSTATUS_RUN      m_psStatusRun  = NULL;
static PSTATUS_USER     m_psStatusUser = NULL;

/* Pointer to SYSLOG Var */
static PCSYSLOG_VARS    m_pcsSysLogVars = NULL;

/* SYSLOG status */
static SYSLOG_STATUS    m_sSysLogStatus; 

/* SYSLOG file info. */
static FIL m_sSyslogFile; 

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************
/**
 *  @brief      Init the SYSLOG. set the next flush timestamp and init index and buffer
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysLogBufferInit(void)
{  
  m_sSysLogStatus.dwTimestampFlushNext 
    = appSysStateTimestampGet(FALSE) + m_pcsSysLogVars->wFlushPeriod;
  m_sSysLogStatus.wLogBufferIndex = 0;
  
  memset(m_sSysLogStatus.asLogBuffer, 0, sizeof(SYSLOG)*SYSLOG_BUFFER_SIZE);
}

/**
 *  @brief      Append the Sys log to Memory
 *  @param[in]  ePayloadType: payload type id
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysLogAppend(eSYSLOG_PAYLOAD_TYPE ePayloadType)
{
  PSYSLOG     psSyslog = NULL;  

  if(m_sSysLogStatus.bLogFileAvailable
    && m_sSysLogStatus.wLogBufferIndex < SYSLOG_BUFFER_SIZE
    && ePayloadType < sysptINVALID)
  {
    psSyslog = &m_sSysLogStatus.asLogBuffer[m_sSysLogStatus.wLogBufferIndex];
    
    psSyslog->sHeader.wIDStart      = SYSLOG_ID_START;
    psSyslog->sHeader.ePayloadType  = ePayloadType;
    psSyslog->sHeader.wPayloadSize  = sizeof(psSyslog->uPayload);

    switch(ePayloadType)
    {
      case sysptINFO:
      {
        PSYSLOG_PAYLOADINFO psPayloadInfo
          = (PSYSLOG_PAYLOADINFO)&m_sSysLogStatus.asLogBuffer[m_sSysLogStatus.wLogBufferIndex].uPayload;
        PCHOST_DEV_INFO     pcsHostInfo = utilHostIfHostInfoPtrGet();

        psPayloadInfo->cSyslogVerMajor  = _SYSLOG_PROTOCOL_VERSION_MAJOR_;
        psPayloadInfo->cSyslogVerMinor  = _SYSLOG_PROTOCOL_VERSION_MINOR_;
        memcpy(psPayloadInfo->acSerialNo, 
                pcsHostInfo->acSerialNo, 
                sizeof(DEVINFO_SERIAL_SIZE));
        memcpy(psPayloadInfo->acSWVer, 
                pcsHostInfo->acSWVer, 
                sizeof(psPayloadInfo->acSWVer));
        psPayloadInfo->cExpanderFlags   = pcsHostInfo->cExpanderFlags;
      }
      break;

      case sysptLOG:
      {
        PSYSLOG_PAYLOADLOG psPayloadLog 
          = (PSYSLOG_PAYLOADLOG)&m_sSysLogStatus.asLogBuffer[m_sSysLogStatus.wLogBufferIndex].uPayload;
        
        //psSyslog->sHeader.wPayloadSize = sizeof(SYSLOG_PAYLOADLOG);
        
        psPayloadLog->dwTimestamp = appSysStateTimestampGet(FALSE);
      }
      break;

      default: break;
    }

    psSyslog->sTail.cChecksum 
      = utilCRCCheckSumBuild((PCBYTE)&m_sSysLogStatus.asLogBuffer[m_sSysLogStatus.wLogBufferIndex].uPayload
                             , psSyslog->sHeader.wPayloadSize);
    psSyslog->sTail.wIDEnd = SYSLOG_ID_END;
      
    m_sSysLogStatus.wLogBufferIndex++;
  }
}

/**
 *  @brief      Flush the Sys log to Physical storage
 *  @param[in]  None
 *  @param[out] None
 *  @return     Written Bytes
 *  @note       None
 *  @attention  None
 */
static UINT sysLogFlush(void)
{
  BOOL      bFlushed      = FALSE;
  UINT      unWrittenByte = 0;  
  FRESULT   fResult; 
  BOOL      bDriveChanged = FALSE;

  if(m_sSysLogStatus.bLogFileAvailable
    && m_sSysLogStatus.wLogBufferIndex)
  {

    /* Buffer Protection */
    if(m_sSysLogStatus.wLogBufferIndex >= (SYSLOG_BUFFER_SIZE-1))
      m_sSysLogStatus.wLogBufferIndex = (SYSLOG_BUFFER_SIZE-1);

    do
    {  
      /* check free space */
      {
        FATFS *fs;
        DWORD dwFreeCluster, dwFreeSector, dwFreeSpaceKBytes;
        CHAR  acLogicalDriveName[5] = {0,};            
        
        sprintf(acLogicalDriveName, "%d:", SYSLOG_DRIVE);
        fResult = f_getfree(acLogicalDriveName, &dwFreeCluster, &fs);
        dwFreeSector = dwFreeCluster * fs->csize;
        dwFreeSpaceKBytes = dwFreeSector/(1024/sdhc_getsectorsize()); // asuuming 512Bytes per sector
        #if USE_DEBUG_SYSSTATE        
        msgDbg("Free Space = %lu KBytes, Limit Space = %d KBytes"
                                        , dwFreeSpaceKBytes
                                        , m_pcsSysLogVars->wLogFileSizeMax*SYSLOG_FREESPACE_TIMES);
        #endif

        
        #if 1   
        /* If not enough space for 5 times of max log file size then */
        if(dwFreeSpaceKBytes <= m_pcsSysLogVars->wLogFileSizeMax*SYSLOG_FREESPACE_TIMES)
        #else
        if(dwFreeSpaceKBytes <= 126275) // Test Purpose Only
        #endif
        {
          DIR       sDirectory;
          FILINFO   sFileInfo;
          DWORD     dwFileNameToNumber;
          DWORD     dwFileNameToNumberPrev = 0xFFFFFFFF;;
          WORD      wCandidateFileCount = 0;
          CHAR      acPath[SYSLOG_PATH_MAX_LEN] = {0};

          f_chdrive(SYSLOG_DRIVE);
          bDriveChanged = TRUE;

          fResult = f_opendir(&sDirectory, SYSLOG_DIR); // Open the directory
          if(fResult == FR_OK)
          {
            do
            {              
              fResult = f_readdir(&sDirectory, &sFileInfo); // read a directory item
              
              if(fResult != FR_OK || sFileInfo.fname[0] == 0) break; // Break on error or end of dir
              if(sFileInfo.fattrib & AM_DIR) continue; // Skip directory
              if(strncmp(sFileInfo.fname, SYSLOG_FILENAME, 6) == 0) continue; // Skip current log file              

              // Hex string format filename to digit
              dwFileNameToNumber = strtoul(sFileInfo.fname, NULL, 16);
              #if USE_DEBUG_SYSSTATE              
              msgDbg("> %s:%ld", sFileInfo.fname, dwFileNameToNumber);
              #endif

              if(dwFileNameToNumber < dwFileNameToNumberPrev)
              {                
                memset(acPath, 0, SYSLOG_PATH_MAX_LEN);
                sprintf(acPath, "%s/%s", SYSLOG_DIR, sFileInfo.fname); // copy filename with path
                dwFileNameToNumberPrev = dwFileNameToNumber;
                wCandidateFileCount++; // Increase file foundation
              }
            }while(1);

            // If found removable file then remove it
            if(wCandidateFileCount)
            {
              fResult = f_unlink((const TCHAR *)acPath);
              #if USE_DEBUG_SYSSTATE              
              if(fResult == FR_OK){ msgDbg("\"%s\" Removed !", acPath);}
              #endif              
            }
              
          }
        }
      }

      fResult = f_open(&m_sSyslogFile, SYSLOG_PATH, (FA_WRITE| FA_READ | FA_OPEN_ALWAYS));      
      if(!(fResult == FR_OK || fResult == FR_EXIST))
        break;

      // check file size and rename
      #if USE_DEBUG_SYSSTATE      
      msgDbg("SYSLOG file size = %ld Bytes", m_sSyslogFile.fsize);
      #endif
      if(m_sSyslogFile.fsize >= m_pcsSysLogVars->wLogFileSizeMax*1024)
      {
        CHAR acNewName[SYSLOG_PATH_MAX_LEN] = {0};
        
        f_close(&m_sSyslogFile);        
        //sprintf_s(acNewName, 50, "%s.%ld", SYSLOG_PATH, m_pcsConnStatus->dwSysTimestamp);
        //sprintf(acNewName, "%s.%ld", SYSLOG_PATH, m_pcsConnStatus->dwSysTimestamp);
        //sprintf(acNewName, "%ld", m_pcsConnStatus->dwSysTimestamp);
        sprintf(acNewName, "%s/%.8lX", SYSLOG_DIR, appSysStateTimestampGet(FALSE)); // Save to Hex format
        fResult = f_rename(SYSLOG_PATH, (const TCHAR *)acNewName);    
        #if USE_DEBUG_SYSSTATE        
        msgDbg("SYSLOG file backed-up (result = %d)", fResult);        
        #endif

        fResult = f_open(&m_sSyslogFile, SYSLOG_PATH, (FA_WRITE| FA_READ | FA_OPEN_ALWAYS));
        if(!(fResult == FR_OK || fResult == FR_EXIST))
          break;
      }
      
      // move file pointer to end of file
      f_lseek(&m_sSyslogFile, m_sSyslogFile.fsize);
      // write syslog buffer to file
      f_write(&m_sSyslogFile, m_sSysLogStatus.asLogBuffer
                                  , sizeof(SYSLOG)*m_sSysLogStatus.wLogBufferIndex
                                  , &unWrittenByte);      
      #if USE_DEBUG_SYSSTATE      
      msgDbg("Flushed Bytes:%d(%d*%d)", unWrittenByte, sizeof(SYSLOG), m_sSysLogStatus.wLogBufferIndex);
      #endif
      #if 0      
      f_sync(&m_sSyslogFile);
      #endif
      f_close(&m_sSyslogFile);

      bFlushed = TRUE;
      
    }while(0);

    /* If Drive was changed than roll-back to NAND */
    if(bDriveChanged)
      f_chdrive(DRIVE_NAND);

    if(bFlushed)    
      sysLogBufferInit();
  }

  return unWrittenByte;
}

/**
 *  @brief      Start Appending the SYSLOG
 *  @param[in]  ePayloadType: payload type id
 *  @param[out] None
 *  @return     TRUE if Append successfully, FALSE to others
 *  @note       None
 *  @attention  None
 */
BOOL sysLogAppendStart(eSYSLOG_PAYLOAD_TYPE ePayloadType)
{
  BOOL bAppend = FALSE;  
  
  do
  {
    // Do not append in Log file is not available
    if(!m_sSysLogStatus.bLogFileAvailable)
      break; //------------>

    // Do not append in Log Disabled
    if(!m_pcsSysLogVars->bLogEnable)  
      break; //------------>

    // Do not append the log during IDLE (not in shot state)
    // but passed if ERROR or SYSLOG Payload type is INFO
    if(m_pcsSysLogVars->bLogEnableOnShotOnly && !m_sStatusSys.bTherapyStart
      && ePayloadType != sysptINFO
      && m_psStatusRun->wErrorFlags == ERR_NONE)
      break; //------------>

    if(m_sSysLogStatus.wLogBufferIndex >= (SYSLOG_BUFFER_SIZE-1) // No more space to append
        || sizeof(SYSLOG)*m_sSysLogStatus.wLogBufferIndex >= m_pcsSysLogVars->wLogFileSizeMax*1024 // Buffer is bigger than max log file size
    )
    {
      // Flush immediately
      sysLogFlush();
      //break;
    }    

    bAppend = TRUE;
  }while(0);    

  if(bAppend)
  {
    /* Dump to Memory Buffer */
    sysLogAppend(ePayloadType);
  }

  return bAppend;
}


/**
 *  @brief         process Sys log. if log enabled, store them to physical storage. must be called by appSysStateProcess()
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void ProcessSysLog(void)
{
  if(m_pcsSysLogVars->bLogEnable
    && (m_sSysLogStatus.wLogBufferIndex >= (SYSLOG_BUFFER_SIZE-1)
        || appSysStateTimestampGet(FALSE) >= m_sSysLogStatus.dwTimestampFlushNext))
  {    
    appSysStateSysLogFlush();
  }  
}


/**
 *  @brief      Init Monitor Vars.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void MonitorInit(void)
{ 

  utilExecPulseClear(pgSYS_MGR, 0xFFFF);
}

/**
 *  @brief      Process LED
 *  @param[in]  ePowerState : power state
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void ProcessLED(ePOWERSTATE ePowerState)
{
  if(m_eStateSys != sysstateDIAG)
  {
    
  }
}

/**
 *  @brief      Process timeout transition
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void ProcessTimeoutTransition(void)
{

}

/**
 *  @brief      Process the System Timestamp
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void ProcessSysTimestamp(void)
{
  if(!utilExecCounterGet(coTIMESTAMP))
  {
    halRTCCtrlProcess();
    utilExecCounterSet(coTIMESTAMP, uS_TO_TICKS(1000000L));
  }
}

/**
 *  @brief      monitors a pending channel change operation that requires multiple
                calls to complete.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void ProcessMonitor(void)
{
}

/**
 *  @brief      Enter the IDLE state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysStateIdleEnter(void)
{
}

/**
 *  @brief      Release the System IDLE state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysStateIdleExit(void)
{
  /* NTD */
}

/**
 *  @brief      Process the System IDLE state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysStateIdleProcess(void)
{
  ePOWERSTATE ePowerState = appPowerMgrPowerStateGet();

  if(pwrON == ePowerState)
  {
#if USE_REQUEST_STATUS  
    /* Request System Status to Host */
    if(utilHostIfGUIReadyGet()
      && !utilExecCounterGet(coHOST_STATUS_REQ))
    {
      appSysStateStatusRequest();
    }
#endif      
  }
}

/**
 *  @brief      Init Therapy
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysStateTherapyInit(void)
{

}

/**
 *  @brief      Enter the System Therapy state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysStateTherapyEnter(void)
{
  /* Use Time base set Zero */
  ///m_cElapTimerShot1Min = 0;

  sysStateTherapyInit();  
}

/**
 *  @brief      Release the System Therapy state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysStateTherapyExit(void)
{  
  sysStateTherapyInit();
}

/**
 *  @brief         Process the System Therapy state
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysStateTherapyProcess(void)
{
  ePOWERSTATE ePowerState = appPowerMgrPowerStateGet();

  if(pwrON == ePowerState)
  {
#if USE_REQUEST_STATUS  
    /* Request System Status to Host */
    if(utilHostIfGUIReadyGet()
      && !utilExecCounterGet(coHOST_STATUS_REQ))
    {
      appSysStateStatusRequest();
    }
#endif      
  }
}

/**
 *  @brief      Init Device Diagnostic state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysStateDiagInit(void)
{

}

/**
 *  @brief      Enter the Device Diagnostic state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysStateDiagEnter(void)
{
  sysStateDiagInit();
}

/**
 *  @brief      Release the Device Diagnostic state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysStateDiagExit(void)
{  
  sysStateDiagInit();
}

/**
 *  @brief      Process the Device Diagnostic state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysStateDiagProcess(void)
{

}

/**
 *  @brief      Init Device SYSLOG View state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysStateSysLogViewInit(void)
{

}

/**
 *  @brief      Enter the SYSLOG View state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysStateSysLogViewEnter(void)
{
  sysStateSysLogViewInit();  
}

/**
 *  @brief      Release the SYSLOG View state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysStateSysLogViewExit(void)
{  
  sysStateSysLogViewInit();
}

/**
 *  @brief      Process the SYSLOG View state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysStateSysLogViewProcess(void)
{
  ePOWERSTATE ePowerState = appPowerMgrPowerStateGet();

  if(pwrON == ePowerState)
  {
   
  }
}

/**
 *  @brief         sets the system state. All flags are cleared
 *  @param[in]  eState  : New system state to set
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void sysStateChange(eSYS_STATE eState)
{
  /* Exit Current State */
  switch(m_eStateSys)
  {
    case sysstateIDLE:
      sysStateIdleExit();
    break; 

    case sysstateTHERAPY:
      sysStateTherapyExit();
    break;

    case sysstateDIAG:
      sysStateDiagExit();
    break;    

    case sysstateSYSLOG_VEIW:
      sysStateSysLogViewExit();
    break;

    default: ASSERT_ALWAYS();
  }

  /* Enter New State */
  switch (eState)
  {
    case sysstateIDLE:
      sysStateIdleEnter();
    break;    

    case sysstateTHERAPY:
      sysStateTherapyEnter();
    break;

    case sysstateDIAG:
      sysStateDiagEnter();
    break;  

    case sysstateSYSLOG_VEIW:
      sysStateSysLogViewEnter();
    break;
  
    default:
      ASSERT_ALWAYS();
  }  

  /* set new state */
  m_eStateSys = eState;  
}

/**
 *  @brief         Process the System State Transition
 *  @param[in]  ePowerState : current power state
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void ProcessSysStateTransition(ePOWERSTATE ePowerState)
{
  ///PCSTATUS_SYS pcsSysStatus = &m_sStatusSys;

  if(sysstateINVALID != m_eStateSys
      && sysstateDIAG != m_eStateSys
      && sysstateSYSLOG_VEIW != m_eStateSys)
  {
    if(hifCONNMODE_GUI_DIAGDEV == utilHostIfGUIConnModeGet())
      sysStateChange(sysstateDIAG);  
  }

  switch(m_eStateSys)
  {
    /* IDLE State */
    case sysstateIDLE:
    if(pwrON == ePowerState)
    {
      sysStateChange(sysstateTHERAPY);
    }
    break;

    /* Therapy State */
    case sysstateTHERAPY:
    if(pwrON == ePowerState)
    {
    }        
    break;

    case sysstateDIAG:
      if(hifCONNMODE_GUI_DIAGDEV != utilHostIfGUIConnModeGet())
        sysStateChange(sysstateIDLE);
    break;  

    case sysstateSYSLOG_VEIW:
    break;
 
    default:
      ASSERT_ALWAYS();
  }

}


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *  @brief         This API sets up the internal state of this component.
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       Must call after appTVChMgrInitialize()
 *  @attention  None
 */
void appSysStateInitialize(void)
{ 
  PSYSTATE_NVRAM_DATA   psNVRAMData       = NULL;
  PSYSCONFIG_NVRAM_DATA psNVRAMSysConfig  = NULL;

  m_eStateSys = sysstateIDLE;

  /* System status */
  memset(&m_sStatusSys, 0, sizeof(STATUS_SYS));

  m_psStatusRun = &m_sStatusSys.sRun;
  m_psStatusUser = &m_sStatusSys.sUser;

  msgDbg("size of SYSCONFIG_NVRAM_DATA %d", sizeof(SYSCONFIG_NVRAM_DATA));
  msgDbg("size of SYSTATE_NVRAM_DATA %d", sizeof(SYSTATE_NVRAM_DATA)); 

  /* Load from System Config NVRAM to internal Variables */
  psNVRAMSysConfig = utilDataMgrReadLockData(nnSYSCONFIG_NVRAM_DATA);
  utilDataMgrUnlock(nnSYSCONFIG_NVRAM_DATA, FALSE); 
  m_cProtectFlag = psNVRAMSysConfig->cProtectFlag;  
  m_pcsSysConfMisc = &psNVRAMSysConfig->sSysConfMisc;

  /* Load from System state NVRAM to internal Variables */
  psNVRAMData = utilDataMgrReadLockData(nnSYSSTATE_NVRAM_DATA);
  utilDataMgrUnlock(nnSYSSTATE_NVRAM_DATA, FALSE);  

  /* System Properties */
  m_pcsSysProp = &psNVRAMData->sSysProp;

  m_pasSysParamInfo = (PPARAM_INFO)utilHostIfHostInfoPtrGet()->asSysParamInfo;
  m_pcsSysLogVars   = (PCSYSLOG_VARS)&utilHostIfHostInfoPtrGet()->sSyslogVars;

  /* Init Minitor Vars */
  MonitorInit(); // Signal, Pulse Clear  

  /* Set the Critical Timer Period */
  ///sysCriticalTimerPeriodSet(m_pcsSysProp->cCriticalTimerPeriodMS);
}

/**
 *  @brief         This API Release the internal resouces of this component
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateRelease(void)
{
  /* Release Code Here */
}

/**
 *  @brief         This API performs periodic processing for the system state. It updates the
 *  @brief         state of the system appropriately.
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateProcess(void)
{
  ePOWERSTATE ePowerState = appPowerMgrPowerStateGet();

  /* Transition system state by external event */
  ProcessSysStateTransition(ePowerState);  

  /* Process timeout transition */
  ProcessTimeoutTransition();

  /* Process LEDs */
  ProcessLED(ePowerState); 

  /* Process system timestamp */
  ProcessSysTimestamp();

  /* Process common monitoring */
  ProcessMonitor();

  if(pwrON == ePowerState
    || sysstateDIAG == m_eStateSys)
  {    
    switch(m_eStateSys)
    {
      case sysstateIDLE:
        sysStateIdleProcess();
      break; 

      case sysstateTHERAPY:
        ProcessSysLog();        
        sysStateTherapyProcess();
      break;

      case sysstateDIAG:
        sysStateDiagProcess();
      break;      

      case sysstateSYSLOG_VEIW:
        sysStateSysLogViewProcess();
      break;

      default:
        ASSERT_ALWAYS();
    }
  }
}

/**
 *  @brief      This API performs periodic processing for the system state. 
                Time Critical
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateProcessTimeCritical(void)
{
  /* NTD */
}

/**
 *  @brief        This API gets the current system state
 *  @param[in]    None
 *  @param[out]   None
 *  @return       enum for System state
 *  @note         None
 *  @attention    None
 */
eSYS_STATE appSysStateGet(void)
{
  return m_eStateSys;
}

/**
 *  @brief        This API sets the current system state
 *  @param[in]    eState: System state to change
 *  @param[out]   None
 *  @return       None
 *  @note         None
 *  @attention    None
 */
void appSysStateSet(eSYS_STATE eState)
{
  sysStateChange(eState);
}

/**
 *  @brief         This API Processing Pre-Power Off Steps
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStatePrePowerOff(void)
{
  appSysStateSet(sysstateIDLE);
}

/**
 *  @brief      This API returns the parameter descriptor in the display ordinate.
 *  @param[in]  eParameter   : Identifier of image path parameter to get the descriptor for
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
PARAM_INFO appSysStateParamInfoGet(eSYS_PARAM eParameter)
{
  ASSERT(eParameter < sysPAR_INVALID);  

  return m_pasSysParamInfo[eParameter];
}

/**
 *  @brief      This API increase/decrease the parameter in the display ordinate.
 *  @param[in]  eParameter: Identifier of image path parameter to get the descriptor for
                bInc: TRUE to Increase, FALSE to Decrease
 *  @param[out] None
 *  @return     TRUE if Perform Successfully, FALSE to others
 *  @note       None
 *  @attention  None
 */
BOOL appSysStateParamIncDec(eSYS_PARAM eParameter, BOOL bInc)
{
  BOOL            bChanged      = FALSE;  

  ASSERT(eParameter < sysPAR_INVALID);  

  return bChanged;
}

/**
 *  @brief         This API Set the parameter in the display ordinate.
 *  @param[in]  eParameter: Identifier of image path parameter to get the descriptor for
                nValue: Value to Set                
 *  @param[out]  None
 *  @return     TRUE if Perform Successfully, FALSE to others
 *  @note       None
 *  @attention  None
 */
BOOL appSysStateParamSet(eSYS_PARAM eParameter, INT nValue)
{
  BOOL            bChanged      = FALSE;

  ASSERT(eParameter < sysPAR_INVALID);  

  return bChanged;
}

/**
 *  @brief      This API Set the system status
 *  @param[in]  pcsSysStatusNew : pointer to System status to set
 *  @param[out] None
 *  @return     TRUE if perform successfully, FALSE to others
 *  @note       None
 *  @attention  None
 */
BOOL appSysStateStatusSet(PCSTATUS_SYS pcsSysStatusNew)
{
  BOOL bReturn = TRUE;  

  PSTATUS_SYS psSysStatusCur = &m_sStatusSys; 
  
  ASSERT(pcsSysStatusNew != NULL);

  if(pwrON == appPowerMgrPowerStateGet())
  {  
    memcpy(psSysStatusCur
          , pcsSysStatusNew
          , sizeof(STATUS_SYS));
    
    bReturn = TRUE;
  }

  return bReturn;
}

/**
 *  @brief      This API Set the system status
 *  @param[in]  None
 *  @param[out] None
 *  @return     System status
 *  @note       None
 *  @attention  None
 */
STATUS_SYS appSysStateStatusGet(void)
{
  return m_sStatusSys;
}

/**
 *  @brief      This API Request the system status to Host
 *  @param[in]  None
 *  @param[out] None
 *  @return     None (Noti)
 *  @note       None
 *  @attention  None
 */
void appSysStateStatusRequest(void)
{
  utilHostIfGUISend(HOST_INSTANCE_AUTO, hifGUI_CTH_SysStatus_Get, NULL, 0);
}


/**
 *  @brief      This API get the pointer of system status structure
 *  @param[in]  None
 *  @param[out] None
 *  @return     pointer of system status structure
 *  @note       None
 *  @attention  None
 */
PSTATUS_SYS appSysStateStatusPtrGet(void)
{
  return &m_sStatusSys;
}

/**
 *  @brief      This API Init the System log.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateSysLogBufferInit(void)
{
  sysLogBufferInit(); 
}

/**
 *  @brief      This API Append the SYSLOG Info
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Append successfully, FALSE to others
 *  @note       None
 *  @attention  None
 */
BOOL appSysStateSysLogInfoAppend(void)
{
  return sysLogAppendStart(sysptINFO);
}

/**
 *  @brief      This API Append the SYSLOG log Data
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Append successfully, FALSE to others
 *  @note       None
 *  @attention  None
 */
BOOL appSysStateSysLogDataAppend(void)
{
  return sysLogAppendStart(sysptLOG);
}

/**
 *  @brief         This API flush(dump) the system log to physical storage
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateSysLogFlush(void)
{
  /* This API must be processed in 500ms(status sending period) */  
  sysLogFlush();
}

/**
 *  @brief      This API return available Device Diagnotic state
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if available Device Diagnotic state, FASE to else
 *  @note       None
 *  @attention  None
 */
BOOL appSysStateDiagAvailable(void)
{
  return (sysstateINVALID != m_eStateSys
          && sysstateDIAG != m_eStateSys
          && sysstateSYSLOG_VEIW != m_eStateSys);
}

/**
 *  @brief      This API perform Unit Test-1 by Key1
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateUnitTestKey_1(void)
{
  if(utilExecSignalGet(sgUI, SIGNAL_UI_FACTORY_ACTIVATED))
    utilExecSignalClear(sgUI, SIGNAL_UI_FACTORY_ACTIVATED);
  else
    utilExecSignalSet(sgUI, SIGNAL_UI_FACTORY_ACTIVATED);
  
#if USE_DEBUG_SYSSTATE
  msgDbg("Unit-Test 1 Performed");
#endif
}

/**
 *  @brief      This API perform Unit Test-2 by Key2
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateUnitTestKey_2(void)
{
#if 0 // Test USB Mass-storage 
    if(utilExecSignalGet(sgMISC, SIGNAL_MISC_MASS_ACTIVATED))
    {
      /* Send Signal to MASS EXIT */
      utilExecSignalClear(sgMISC, SIGNAL_MISC_MASS_LOGVIEW_REQUEST);
      utilExecSignalClear(sgMISC, SIGNAL_MISC_MASS_DFU_REQUEST);
    }
    else
    {
      if(utilExecSignalGet(sgMISC, SIGNAL_MISC_MASS_DFU_REQUEST))
        utilExecSignalClear(sgMISC, SIGNAL_MISC_MASS_DFU_REQUEST);
      else
      {
        utilExecSignalClear(sgMISC, SIGNAL_MISC_MASS_LOGVIEW_REQUEST);
        utilExecSignalSet(sgMISC, SIGNAL_MISC_MASS_DFU_REQUEST);
        utilMiscMassStorageSourceSet(FALSE);
      }
    }
#endif  

#if 0
  {
    BYTE acBuffer[8] = {0,1,2,3,4,5,6,7};

    msgDbg("%d", halCANCtrlWrite(0, sizeof(acBuffer), acBuffer));
  }
#endif

#if 0
  {
  	register INT addr;
  	addr = 0xf4001024;
    UINT nValue;
    static UINT nEnable = 0;

    nValue = *(VPUINT)(addr);
    msgDbg("0x%X", nValue);
    nValue &= ~(1 << 24);
    nValue |= (nEnable << 24);
    *(VPUINT)(addr) = nValue;
    nEnable = (nEnable) ? 0 : 1;
    nValue = *(VPUINT)(addr);
    msgDbg("0x%X", nValue);
  }
#endif

#if USE_DEBUG_SYSSTATE
  msgDbg("Unit-Test 2 Performed");
#endif
}

/**
 *  @brief      This API perform Unit Test-3 by Key2
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateUnitTestKey_3(void)
{
#if 0 // Test USB Mass-storage 
  if(utilExecSignalGet(sgMISC, SIGNAL_MISC_MASS_ACTIVATED))
  {
    /* Send Signal to MASS EXIT */
    utilExecSignalClear(sgMISC, SIGNAL_MISC_MASS_LOGVIEW_REQUEST);
    utilExecSignalClear(sgMISC, SIGNAL_MISC_MASS_DFU_REQUEST);
  }
  else
  {
    if(utilExecSignalGet(sgMISC, SIGNAL_MISC_MASS_LOGVIEW_REQUEST))
      utilExecSignalClear(sgMISC, SIGNAL_MISC_MASS_LOGVIEW_REQUEST);
    else
    {
      utilExecSignalClear(sgMISC, SIGNAL_MISC_MASS_DFU_REQUEST);
      utilExecSignalSet(sgMISC, SIGNAL_MISC_MASS_LOGVIEW_REQUEST);
      utilMiscMassStorageSourceSet(TRUE);
    }
  }
#endif  

#if USE_DEBUG_SYSSTATE
    msgDbg("Unit-Test 3 Performed");
#endif
}

/**
 *  @brief        This API Gets the System Properties
 *  @param[in]    None
 *  @param[out]   None
 *  @return       System Properties
 *  @note         None
 *  @attention    None
 */
SYSTEM_PROP appSysStateSystemPropGet(void)
{
  return *m_pcsSysProp;
}

/**
 *  @brief        This API Sets the System Properties
 *  @param[in]    sSysProp: System Properities to set
 *  @param[out]   None
 *  @return       TRUE if value was changed, FALSE for others
 *  @note         None
 *  @attention    None
 */
BOOL appSysStateSystemPropSet(SYSTEM_PROP sSysProp)
{
  BOOL bChanged = FALSE;
  eNODE_NAME eNodeID = nnSYSSTATE_NVRAM_DATA;
  PSYSTATE_NVRAM_DATA psNVRAMSys = NULL;
  ///PARAM_INFO sParamInfo;

  if(0 != memcmp(&sSysProp , m_pcsSysProp, sizeof(SYSTEM_PROP)))
  {
    bChanged = TRUE;
    
    psNVRAMSys = utilDataMgrWriteLockData(eNodeID);

    /* Elapsed Time use */    
    psNVRAMSys->sSysProp.bElapsedTimeEnable = sSysProp.bElapsedTimeEnable;

    /* System Critical Timer Period */
    psNVRAMSys->sSysProp.cCriticalTimerPeriodMS = sSysProp.cCriticalTimerPeriodMS;
    
    /* Unlock Data manager */
    utilDataMgrUnlock(eNodeID, TRUE);    
  }

  return bChanged;
}

/**
 *  @brief         This API Gets the System Timestamp
 *  @param[in]  bReadFromRTC: TRUE to read from RTC
 *  @param[out]  None
 *  @return    Currently System Timestamp
 *  @note       None
 *  @attention  None
 */
DWORD appSysStateTimestampGet(BOOL bReadFromRTC)
{
  return halRTCCtrlTimestampGet(bReadFromRTC);
}

/**
 *  @brief         This API Sets the System Timestamp
 *  @param[in]  dwTimestamp: timestamp to set
                          bSaveToRTC: TRUE to save to RTC
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysStateTimestampSet(DWORD dwTimestamp, BOOL bSaveToRTC)
{
  halRTCCtrlTimestampSet(dwTimestamp, bSaveToRTC);
}

/**
 *  @brief      This API Sets the System Configuration Protection
 *  @param[in]  bProtect : TRUE to Protect, FALSE for Unlock
 *  @param[out] None
 *  @return     TRUE if Changed, FALSE for others
 *  @note       None
 *  @attention  None
 */
BOOL appSysConfigProtectSet(BOOL bProtect)
{
  BOOL bChanged = FALSE;
  eNODE_NAME eNodeID = nnSYSCONFIG_NVRAM_DATA;
  PSYSCONFIG_NVRAM_DATA psNVRAMSysConfig = NULL;
  BYTE cProtectFlag = (bProtect) ? SYSCONFIG_PROTECT_FLAG : SYSCONFIG_UNPROTECT_FLAG;

  if(m_cProtectFlag != cProtectFlag)
  {
    psNVRAMSysConfig = utilDataMgrWriteLockData(eNodeID);    
    psNVRAMSysConfig->cProtectFlag = cProtectFlag;
    m_cProtectFlag = cProtectFlag;
    utilDataMgrUnlock(eNodeID, TRUE);  
    bChanged = TRUE;
  }

  return bChanged;
}

/**
 *  @brief      This API Gets the System Configuration Protection
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Protected, FALSE for others
 *  @note       None
 *  @attention  None
 */
BOOL appSysConfigProtectGet(void)
{
  return (SYSCONFIG_PROTECT_FLAG == m_cProtectFlag) ? TRUE : FALSE;
}

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
                                              , PBYTE pcMin)
{
  eNODE_NAME eNodeID = nnSYSCONFIG_NVRAM_DATA;
  PSYSCONFIG_NVRAM_DATA psNVRAMSysConfig = NULL;
  DWORD dwElapsedTimes = 0;

  ASSERT(eTimeId < elapTIME_INVALID);

  psNVRAMSysConfig = utilDataMgrReadLockData(eNodeID);      
  utilDataMgrUnlock(eNodeID, FALSE);

  switch(eTimeId)
  {
    case elapTIME_UNIT:
      dwElapsedTimes = psNVRAMSysConfig->dwUnitOnTimes;
    break;  
  
    default: break;
  }

  if(pwDays != NULL
    && pcHour != NULL
    && pcMin != NULL)
  {
    if(dwElapsedTimes)
    {
      DWORD dwShotTimesTemp;
      
      *pcMin = (BYTE)(dwElapsedTimes % 60);
      dwShotTimesTemp = dwElapsedTimes / 60;    
      *pcHour = (BYTE)(dwShotTimesTemp % 24);
      dwShotTimesTemp /= 24;    
      *pwDays = (WORD)dwShotTimesTemp;    
    }
    else
    {
      *pcMin = 0;
      *pcHour = 0;
      *pwDays = 0;
    }
  }
  
  return dwElapsedTimes;

}

/**
 *  @brief      This API Increase the Device Elapsed Times
 *  @param[in]  eTimeId: Elapsed Time Id to Get                          
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysConfigElapsedTimeIncrease(eELAPSED_TIME eTimeId)
{
  eNODE_NAME eNodeID = nnSYSCONFIG_NVRAM_DATA;
  PSYSCONFIG_NVRAM_DATA psNVRAMSysConfig = NULL; 

  ASSERT(eTimeId < elapTIME_INVALID);
 
  psNVRAMSysConfig = utilDataMgrWriteLockData(eNodeID);   

  switch(eTimeId)
  {
    case elapTIME_UNIT:
      if(psNVRAMSysConfig->dwUnitOnTimes < 0xFFFFFFFF)
        psNVRAMSysConfig->dwUnitOnTimes++;
    break;    

    default: break;
  }  
  
  utilDataMgrUnlock(eNodeID, TRUE);
}

/**
 *  @brief      This API increase the Device Elapsed Times
 *  @param[in]  cFlags: flags that complex bit shift as Elapsed Time Id to set
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appSysConfigElapsedTimesIncrease(BYTE cFlags)
{
  eNODE_NAME eNodeID = nnSYSCONFIG_NVRAM_DATA;
  PSYSCONFIG_NVRAM_DATA psNVRAMSysConfig = NULL; 

  if(cFlags)
  {
    psNVRAMSysConfig = utilDataMgrWriteLockData(eNodeID);   

    if((cFlags & (1 << elapTIME_UNIT))
      && psNVRAMSysConfig->dwUnitOnTimes < 0xFFFFFFFF) psNVRAMSysConfig->dwUnitOnTimes++;

    utilDataMgrUnlock(eNodeID, TRUE);  
  }
}

/**
 *  @brief      This API reset the Device Elapsed Times
 *  @param[in]  cFlags: flags that complex bit shift as Elapsed Time Id to set
 *  @param[out] None
 *  @return     TRUE if perform successfully, FALSE to others
 *  @note       None
 *  @attention  None
 */
BOOL appSysConfigElapsedTimesReset(BYTE cFlags)
{
  BOOL bSuccess = FALSE;
  eNODE_NAME eNodeID = nnSYSCONFIG_NVRAM_DATA;
  PSYSCONFIG_NVRAM_DATA psNVRAMSysConfig = NULL; 

  if(cFlags)
  {
    psNVRAMSysConfig = utilDataMgrWriteLockData(eNodeID);   

    if(cFlags & (1 << elapTIME_UNIT))
    {
      psNVRAMSysConfig->dwUnitOnTimes = 0;
    }
    else
    {
      // TODO: appSysConfigElapsedTimesReset()
    }

    utilDataMgrUnlock(eNodeID, TRUE);  
    
    bSuccess = TRUE;    
  }

  return bSuccess;
}

/**
 *  @brief         This API Gets the System Configuration Misc.
 *  @param[in]  None
 *  @param[out]  None
 *  @return    System Configuration Misc
 *  @note       None
 *  @attention  None
 */
SYSCONF_MISC appSysConfigMiscGet(void)
{
  return *m_pcsSysConfMisc;
}

/**
 *  @brief      This API Sets the System Configuration Misc
 *  @param[in]  sSysConfMiscNew: System Configuration Misc to set
 *  @param[out] None
 *  @return     TRUE if value was changed, FALSE for others
 *  @note       None
 *  @attention  None
 */
BOOL appSysConfigMiscSet(SYSCONF_MISC sSysConfMiscNew)
{
  BOOL                  bChanged          = FALSE;
  eNODE_NAME            eNodeID           = nnSYSCONFIG_NVRAM_DATA;
  PSYSCONFIG_NVRAM_DATA psNVRAMSysConfig  = NULL;

  if(0 != memcmp(&sSysConfMiscNew , m_pcsSysConfMisc, sizeof(SYSCONF_MISC)))
  {
    bChanged = TRUE;
    
    psNVRAMSysConfig = utilDataMgrWriteLockData(eNodeID);

    memcpy(&psNVRAMSysConfig->sSysConfMisc, &sSysConfMiscNew, sizeof(SYSCONF_MISC));
    
    utilDataMgrUnlock(eNodeID, TRUE);    
  }

  return bChanged;

}

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
void appSysStateNodeCallback(eNODE_NAME eNodeID, eNV_EVENT  eEvent)
{
  switch(eNodeID)
  {
    case nnSYSCONFIG_NVRAM_DATA:
      switch(eEvent)
      {
        case nveRESETALL:
#if USE_NVRAM_PROTECT_AREA          
        case nveINITIALIZE:
#endif        
        {
          PSYSCONFIG_NVRAM_DATA psNVRAMSysConfig = NULL;
          
          psNVRAMSysConfig = utilDataMgrWriteLockData(eNodeID);

#if USE_NVRAM_PROTECT_AREA          
          if(SYSCONFIG_PROTECT_FLAG != psNVRAMSysConfig->cProtectFlag)
#endif            
          {
            psNVRAMSysConfig->dwUnitOnTimes = 0;
            /* SYS CONFIG MISC */
            psNVRAMSysConfig->sSysConfMisc = m_sSystemConfMiscDefault;
          }

          psNVRAMSysConfig->cProtectFlag = SYSCONFIG_PROTECT_FLAG;
          
          utilDataMgrUnlock(eNodeID, TRUE); 
        }
        break;

#if !USE_NVRAM_PROTECT_AREA          
        case nveINITIALIZE:
          /* None Process in here */
        break;
#endif               

        default: ASSERT_ALWAYS(); break;        
      }
    break;

    case nnSYSSTATE_NVRAM_DATA:
      switch (eEvent)
      {
        case nveRESETALL:
        case nveINITIALIZE:        
        {
          PSYSTATE_NVRAM_DATA psNVRAMSys = NULL;
          
          psNVRAMSys = utilDataMgrWriteLockData(eNodeID);
          
          /* System Properties */
          psNVRAMSys->sSysProp = m_sSysPropDefault;

          /* User Value */
          // NTD
          
          utilDataMgrUnlock(eNodeID, TRUE); 
        }
          break;          

        default:
          ASSERT_ALWAYS();
          break;
      }
      
      break;

    default: ASSERT_ALWAYS(); break;

  }
}


