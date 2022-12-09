/**
  ******************************************************************************
  * @file    utilDataMgr.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    04-August-2014
  * @brief   This file provides APIs the Data Manager
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
#include "utilDataMgrAPI.h"
#include "dvNVRAMDriverAPI.h"
#include "utilCRC.h"
#include "appSystemStateAPI.h"
#include "appAudioMgrAPI.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

// ---------------------------------------------------------------------------
// sentinel values for node state
// ---------------------------------------------------------------------------
#define DIRTY           0x80
#define WRITECOUNTER    0xff
#define INITMASK        0xffff

// ---------------------------------------------------------------------------
// node status information structure
// ---------------------------------------------------------------------------
typedef struct
{
    BYTE                cLockCounter;
    BYTE                cFlag;
} NODE_CONTROL_INFO;

// ---------------------------------------------------------------------------
// node configuration information structure
// ---------------------------------------------------------------------------
typedef struct
{
    void (*fpCallBack)  (eNODE_NAME eNodeID, eNV_EVENT nveEvent);
    WORD                wNodeSize;
    WORD                wNodeOffset;
} NODE_INFO;

// ---------------------------------------------------------------------------
// Version information structure
// ---------------------------------------------------------------------------
typedef struct
{
    WORD    wSysVer;          // store the system data version.
    WORD    wStructVer;    // store the calibration data version.
} DATAMGR_VERSION_DATA, *PDATAMGR_VERSION_DATA;

// ---------------------------------------------------------------------------
// MACROs defined based on _DEF_NVRAM_CRC_CHECK - DO NOT MODIFY
// ---------------------------------------------------------------------------
#if defined(_DEF_NVRAM_CRC_CHECK)
#define NODECRC(x) WORD x
#define CRCLEN    2
#else
#define NODECRC(x)
#define CRCLEN    0
#endif

//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************
typedef struct tagNVRAMDATA
{
/* ---------------- BEGIN CUSTOM AREA [020] utilDataMgr.c --------------------
 * CONFIGURATION: Add structure definitions for all data stored by Data
 * Manager */
    // Important NVRAM data should be place first
  /* nnSYSCONFIG_NVRAM_DATA */
  SYSCONFIG_NVRAM_DATA sSysConfig;
#if defined(_DEF_NVRAM_CRC_CHECK)
  NODECRC( crc_SysConfig );        	// + node crc field.  
#endif 
  /* nnSYSSTATE_NVRAM_DATA */
  SYSTATE_NVRAM_DATA sSysState;
#if defined(_DEF_NVRAM_CRC_CHECK)
  NODECRC( crc_SysState );        	// + node crc field.  
#endif 
  /* nnAUDIOMGR_NVRAM_DATA */
  AUDIOMGR_NVRAM_DATA sAudioMgrNode;
#if defined(_DEF_NVRAM_CRC_CHECK)
  NODECRC( crc_AudioMgr );        	// + node crc field.  
#endif  
/* ---------------------------- END CUSTOM AREA --------------------------- */
}GCC_PACKED NVRAM_DATA;

static CROMDATA NODE_INFO m_asNodeInfo[nnINVALID_NODE] = 
{
  /* ---------------- BEGIN CUSTOM AREA [030] utilDataMgr.c --------------------
   * CONFIGURATION: Add configuration data for each data node (callback
   * routines, sizes and location) The order of this data must match the
   * corresponding names in eNODE_NAME. */
  {/* nnSYSCONFIG_NVRAM_DATA */
    appSysStateNodeCallback,
    sizeof(SYSCONFIG_NVRAM_DATA) + CRCLEN,
    offsetof(NVRAM_DATA, sSysConfig)
  },
  {/* nnSYSSTATE_NVRAM_DATA */
    appSysStateNodeCallback,
    sizeof(SYSTATE_NVRAM_DATA) + CRCLEN,
    offsetof(NVRAM_DATA, sSysState)
  },
  {/* nnAUDIOMGR_NVRAM_DATA */
    appAudioMgrNodeCallback,
    sizeof(AUDIOMGR_NVRAM_DATA) + CRCLEN,
    offsetof(NVRAM_DATA, sAudioMgrNode)
  },      
  /* ---------------------------- END CUSTOM AREA --------------------------- */      
};


//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************

//static pthread_mutex_t  m_mutexDataMgr = PTHREAD_MUTEX_INITIALIZER;

// ---------------------------------------------------------------------------
// Node status information array
// ---------------------------------------------------------------------------
static NODE_CONTROL_INFO m_asNodeControlInfo[nnINVALID_NODE];

// ---------------------------------------------------------------------------
// RAM copy ('mirror') of NVRAM data
// ---------------------------------------------------------------------------
static NVRAM_DATA       m_sRamMirror;

// ---------------------------------------------------------------------------
// Don't update when m_bUpdateInhibit is TRUE
// ---------------------------------------------------------------------------
static BOOL             m_bUpdateInhibit = FALSE;

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************

#if USE_NVRAM_TYPE
// ---------------------------------------------------------------------------
// Load NVRam data into RAM mirror.
//
// Params:
//  eNodeID : node ID
//
// Return: none
// ---------------------------------------------------------------------------
static void LoadNvramToRamMirror(eNODE_NAME eNodeID)
{
  PBYTE               pcRamMirror;
  WORD                wCopySize, wStartByte;

  pcRamMirror = (PBYTE)&m_sRamMirror +
                m_asNodeInfo[eNodeID].wNodeOffset;

  wStartByte = m_asNodeInfo[eNodeID].wNodeOffset + sizeof(DATAMGR_VERSION_DATA);
  wCopySize = m_asNodeInfo[eNodeID].wNodeSize;

  if (rcSUCCESS != dvNVRAMBytesRead(wStartByte,
                                    wCopySize,
                                    pcRamMirror))
  {
    ASSERT_ALWAYS();
  }
}

// ---------------------------------------------------------------------------
// Write RAM mirror data to NVRAM.
//
// Params:
//  eNodeID : node ID
//
// Return: none
// ---------------------------------------------------------------------------
static void WriteRamMirrorToNvram(eNODE_NAME eNodeID)
{
  PBYTE               pcRamMirror;
  WORD                wNodeSize, wNodeOffset;    

  pcRamMirror = (PBYTE)&m_sRamMirror +
                m_asNodeInfo[eNodeID].wNodeOffset;
  wNodeSize = m_asNodeInfo[eNodeID].wNodeSize;
  wNodeOffset = m_asNodeInfo[eNodeID].wNodeOffset;
    
  if (rcSUCCESS != dvNVRAMBytesWrite(wNodeOffset + sizeof(DATAMGR_VERSION_DATA),
                                   wNodeSize,
                                   pcRamMirror))
  {
    ASSERT_ALWAYS();
  }  
  
  //commDelayMS(5); // Add?
}
#endif  // #if USE_NVRAM_TYPE

#if USE_NVRAM_TYPE
// ---------------------------------------------------------------------------
// calculate CRC for the nodes in RAM mirror.
//
// Params:
//  eNodeID : node ID
//
// Return: none
//
// Notes:
//  CRC is calculated and recorded before the RAM data is written to NVRAM
// ---------------------------------------------------------------------------
static void setNodeCRC(eNODE_NAME eNodeID)
{
  #if defined(_DEF_NVRAM_CRC_CHECK)
  PWORD               pCRC;
  PBYTE               pData;

  pData = (PBYTE)&m_sRamMirror +
          m_asNodeInfo[eNodeID].wNodeOffset;
  pCRC = (PWORD)((PBYTE)&m_sRamMirror +
                 (m_asNodeInfo[eNodeID].wNodeOffset +
                  m_asNodeInfo[eNodeID].wNodeSize - CRCLEN));

  *pCRC = utilCRC16Calc(pData,
                        m_asNodeInfo[eNodeID].wNodeSize - CRCLEN);
  #endif
}
#endif


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *  @brief      This API initializes static data structures that are used subsequently at
 *                 runtime and Initializes the device by setting registers that do not change
 *                 frequently at runtime.
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilDataMgrInitialize(void)
{
#if USE_NVRAM_TYPE
  #if defined(_DEF_NVRAM_CRC_CHECK)
  PBYTE                 pData;
  PWORD                 pCRC;
  #endif
  int                   i;
  eNV_EVENT             eNvEvent = nveINVALID;
  DATAMGR_VERSION_DATA  sVersion = {0xFFFF,0xFFFF};
  eRESULT               eResult;
#endif

  //dataMgrTest();
  
  // Initialize Ram mirror - m_sRamMirror to 0
  memset(&m_sRamMirror, 0,  sizeof(m_sRamMirror));
  
  // initialize node control information
  memset(&m_asNodeControlInfo, 0, sizeof(m_asNodeControlInfo));

#if USE_NVRAM_TYPE
  dvNVRAMInitialize();
 
  // read data version from NVRAM if available (version
  // numbers are always at start of NVRAM)
  eResult = dvNVRAMBytesRead(0,
                   sizeof(DATAMGR_VERSION_DATA),
                   (PBYTE)&sVersion);
  
  msgDbg("NVRAM Version [%d.%d]", sVersion.wStructVer & 0xFF, sVersion.wSysVer);
  ///BREAK_PT
  
  // if structure data version is different than
  // the stored data, then "RESETALL"
  if (_DATAMGR_VERSION_STRUCTURE_ != sVersion.wStructVer)
  {
    eNvEvent = nveRESETALL;
    msgInfo("NVRAM STRUCTURE RESET INVOKED [%d.%d] -> [%d.%d]"
                              ,  sVersion.wStructVer & 0xFF
                              , sVersion.wSysVer
                              , _DATAMGR_VERSION_STRUCTURE_ & 0xFF
                              , _DATAMGR_VERSION_SYSTEM_);
  }
  else if (_DATAMGR_VERSION_SYSTEM_ != sVersion.wSysVer)
  {
    // if system data version is different than the stored
    // data, then "INITIALIZE except calibration data"
    eNvEvent = nveINITIALIZE;
    msgInfo("NVRAM DATA INITIALIZATION INVOKED [%d.%d] -> [%d.%d]"
                              ,  sVersion.wStructVer & 0xFF
                              , sVersion.wSysVer
                              , _DATAMGR_VERSION_STRUCTURE_ & 0xFF
                              , _DATAMGR_VERSION_SYSTEM_);
  }
  
#if !USE_NVRAM_LOADFIRST
  if(nveINVALID != eNvEvent)
  {
    // send NVRAM event to all nodes with valid callback routines
    utilDataMgrReset(eNvEvent);
    
    // set version numbers to current values and store
    sVersion.wStructVer = _DATAMGR_VERSION_STRUCTURE_;
    sVersion.wSysVer = _DATAMGR_VERSION_SYSTEM_;

    eResult = dvNVRAMBytesWrite(0,
                      sizeof(DATAMGR_VERSION_DATA),
                      (PCBYTE)&sVersion);
    
    //commDelayMS(5); // Add?

    ASSERT (eResult == rcSUCCESS);
  }
#endif  

#if !USE_NVRAM_LOADFIRST
  if(nveINVALID == eNvEvent)
#endif    
  {
    for (i = 0; i < nnINVALID_NODE; i++)
    {
#if defined(_DEF_NVRAM_CRC_CHECK)
      // load NVRam data to Ram mirror when CRC is correct
      utilDataMgrWriteLockData((eNODE_NAME)i);
      LoadNvramToRamMirror((eNODE_NAME)i);
      m_asNodeControlInfo[(eNODE_NAME)i].cLockCounter = 0;

      pData = (PBYTE)&m_sRamMirror + m_asNodeInfo[(eNODE_NAME)i].wNodeOffset;
      pCRC = (PWORD)((PBYTE)&m_sRamMirror +
                     m_asNodeInfo[(eNODE_NAME)i].wNodeOffset +
                     m_asNodeInfo[(eNODE_NAME)i].wNodeSize - CRCLEN);

      // if CRC is wrong, call client to initialize RAM mirror to default
      if (*pCRC != utilCRC16Calc(
                      pData,
                      m_asNodeInfo[(eNODE_NAME)i].wNodeSize - CRCLEN))
      {
          // Ensure that callback is not NULL.
          if (NULL != m_asNodeInfo[i].fpCallBack)
          {
              m_asNodeInfo[i].fpCallBack((eNODE_NAME)i,
                                         nveRESETALL);
          }
      }

#else // no CRC check

      // load NVRam data to Ram mirror without checking CRC
      utilDataMgrWriteLockData((eNODE_NAME)i);
      LoadNvramToRamMirror((eNODE_NAME)i);
      m_asNodeControlInfo[(eNODE_NAME)i].cLockCounter = 0;
#endif // NVRAM_CRC_CHECK
    }
  }

#if USE_NVRAM_LOADFIRST
  if(nveINVALID != eNvEvent)
  {
    // send NVRAM event to all nodes with valid callback routines
    utilDataMgrReset(eNvEvent);
    
    // set version numbers to current values and store
    sVersion.wStructVer = _DATAMGR_VERSION_STRUCTURE_;
    sVersion.wSysVer = _DATAMGR_VERSION_SYSTEM_;

    eResult = dvNVRAMBytesWrite(0,
                      sizeof(DATAMGR_VERSION_DATA),
                      (PCBYTE)&sVersion);
    
    //commDelayMS(5); // Add?

    ASSERT (eResult == rcSUCCESS);
  }
#endif  

#else // #if USE_NVRAM_TYPE
  // send RESETALL event to all nodes with valid callback routines
  utilDataMgrReset(nveRESETALL);  
#endif  
  
}

/**
 *  @brief      This API Release the components
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilDataMgrRelease(void)
{
#if USE_NVRAM_TYPE
  dvNVRAMRelease();
#endif
}

// ---------------------------------------------------------------------------
// This API calls all client callback routines with a reset event, so the
// client components can reset the data they have stored in non-volatile
// memory.
//
// Params:
//  eResetType : Reset type, sent to node callback functions
//
// Return: none
// ---------------------------------------------------------------------------
void utilDataMgrReset(eNV_EVENT eResetType)
{
    int                 i;

    ASSERT(eResetType < nveINVALID);

    // send NVRAM event to all nodes with valid callback routines
    for (i = 0; i < nnINVALID_NODE; i++)
    {
        if (NULL != m_asNodeInfo[i].fpCallBack)
        {
            m_asNodeInfo[i].fpCallBack((eNODE_NAME)i,
                                       eResetType);
        }
    }
}

#if USE_FACTORY_RESET
// ---------------------------------------------------------------------------
// This API calls all client callback routines with a reset event, so the
// client components can reset the data they have stored in non-volatile
// memory.
//
// Params:
//  eResetType : Reset type, sent to node callback functions
//
// Return: none
// ---------------------------------------------------------------------------
void utilDataMgrResetFactory(eNV_EVENT eResetType)
{
    int                 i;

    ASSERT(eResetType < nveINVALID);

    // send NVRAM event to all nodes with valid callback routines
    for (i = 1; i < nnINVALID_NODE; i++)
    {
        if (NULL != m_asNodeInfo[i].fpCallBack)
        {
            m_asNodeInfo[i].fpCallBack((eNODE_NAME)i,
                                       eResetType);
        }
    }
}
#endif

// TODO: utilDataMgr LOCK COUNTER
// ---------------------------------------------------------------------------
// This API locks the specified node for reading and returns a pointer to the
// node data in the RAM mirror.
//
// Params:
//  eNodeID : The node identifier
//
// Return:
//  Address of the node data in RAM
// ---------------------------------------------------------------------------
PVOID utilDataMgrReadLockData(eNODE_NAME eNodeID)
{
    BYTE                *pacRamNodeAddress;

    // validate arguments in debug mode only
    ASSERT(eNodeID < nnINVALID_NODE);

    pacRamNodeAddress =
        (PBYTE)&m_sRamMirror + m_asNodeInfo[eNodeID].wNodeOffset;
    
    m_asNodeControlInfo[eNodeID].cLockCounter++;
    
    //ASSERT(WRITECOUNTER > m_asNodeControlInfo[eNodeID].cLockCounter);   

    return (PVOID)pacRamNodeAddress;
}

// ---------------------------------------------------------------------------
// This API locks the specified node for writing and returns a pointer to the
// node data in the RAM mirror.
//
// Params:
//  eNodeID : The node identifier
//
// Return:
//  Address of the node data in RAM.
// ---------------------------------------------------------------------------
PVOID utilDataMgrWriteLockData(eNODE_NAME eNodeID)
{
    BYTE                *pacRamNodeAddress;
    //WORD                wMask;
    //BOOL                bCorrectNode = FALSE;

    // validate arguments in debug mode only
    ASSERT(eNodeID < nnINVALID_NODE);
  
    pacRamNodeAddress =
        (PBYTE)&m_sRamMirror + m_asNodeInfo[eNodeID].wNodeOffset;

    m_asNodeControlInfo[eNodeID].cLockCounter = WRITECOUNTER;

    return (PVOID)pacRamNodeAddress;
}


// ---------------------------------------------------------------------------
// This API unlocks the specified node.
//
// Params:
//  eNodeID : The node identifier
//  bWrite  : TRUE releases the write lock for this node and task
//            (multi-threaded systems). The node will be marked dirty to force
//            an update to NVRAM.   FALSE releases read lock.
//
// Return: none
// ---------------------------------------------------------------------------
void utilDataMgrUnlock(eNODE_NAME eNodeID, BOOL       bWrite)
{
    // validate arguments in debug mode only
    ASSERT(eNodeID < nnINVALID_NODE);

    //CPU_INT_DISABLE;
    if (bWrite)
    {
        // Has to be 255. This node has to be write locked.
        //ASSERT(m_asNodeControlInfo[eNodeID].cLockCounter == WRITECOUNTER);
        m_asNodeControlInfo[eNodeID].cLockCounter = 0;
        m_asNodeControlInfo[eNodeID].cFlag |= DIRTY;
    }
    else
    {
        if (WRITECOUNTER == m_asNodeControlInfo[eNodeID].cLockCounter)
        {
            // alolows writing nothing
            m_asNodeControlInfo[eNodeID].cLockCounter = 0;
        }
        else
        {
            // Can't be 0. avoid unlock read on a node which is not read
            // locked
            //ASSERT(m_asNodeControlInfo[eNodeID].cLockCounter > 0);
            // Can't be 255. avoid unlock read on a node which is write locked
            //ASSERT(m_asNodeControlInfo[eNodeID].cLockCounter < WRITECOUNTER);

            m_asNodeControlInfo[eNodeID].cLockCounter--;
        }
    }
}


// ---------------------------------------------------------------------------
// This API enables or inhibits updates of the RAM mirror into non-volatile
// memory.
//
// Params:
//  bInhibit : TRUE to prevent updates, FALSE otherwise
//
// Return: none
// ---------------------------------------------------------------------------
void utilDataMgrUpdateInhibit(BOOL bInhibit)
{
    m_bUpdateInhibit = bInhibit;
}


// ---------------------------------------------------------------------------
// This API transfers data stored in the RAM mirror into non-volatile memory.
//
// Params:
//  eNodeID : The node identifier
//
// Return: none
// ---------------------------------------------------------------------------
void utilDataMgrUpdate(eNODE_NAME eNodeID)
{
#if USE_NVRAM_TYPE
    int                 i;

    // validate arguments in debug mode only
    ASSERT(eNodeID < nnINVALID_NODE || UPDATE_ALL_NODEID == eNodeID);

    // if update request is for a single node, update it if dirty
    if (UPDATE_ALL_NODEID != eNodeID)
    {
        if (m_asNodeControlInfo[eNodeID].cFlag & DIRTY)
        {
            // Read lock is used. Ram mirror is still available for read
            utilDataMgrReadLockData(eNodeID);
            setNodeCRC(eNodeID);

            // load NVRam data to Ram mirror when CRC is correct
            WriteRamMirrorToNvram(eNodeID);
            m_asNodeControlInfo[eNodeID].cFlag &= ~DIRTY;

            utilDataMgrUnlock(eNodeID,
                              FALSE);
        }
    }
    // otherwise (request is for all nodes), update if not inhibited
    else if (!m_bUpdateInhibit)
    {
        for (i = 0; i < nnINVALID_NODE; i++)
        {
            if (m_asNodeControlInfo[i].cFlag & DIRTY)
            {
                // Read lock is used. Ram mirror is still available for read
                utilDataMgrReadLockData((eNODE_NAME)i);
                setNodeCRC((eNODE_NAME)i);

                // load NVRam data to Ram mirror when CRC is correct
                WriteRamMirrorToNvram((eNODE_NAME)i);
                m_asNodeControlInfo[(eNODE_NAME)i].cFlag &= ~DIRTY;
                utilDataMgrUnlock((eNODE_NAME)i,
                                  FALSE);
            }
        }
    }
#endif
}


