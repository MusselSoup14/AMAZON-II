/**
  ******************************************************************************
  * @file    utilDataMgrAPI.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    04-August-2014
  * @brief   This file provides APIs the Data Manager Header
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
#include "common.h"

#if !defined(_INCLUDED_UTILDATAMGRAPI_H_)
#define _INCLUDED_UTILDATAMGRAPI_H_

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

// ---------------------------------------------------------------------------
// sentinel value indicating all data manager nodes
// ---------------------------------------------------------------------------
#define UPDATE_ALL_NODEID 					nnINVALID_NODE

// --------------------------------CONFIGURATION------------------------------
// list of all supported data manager events
// --------------------------------CONFIGURATION------------------------------
typedef enum
{
/* --------------- BEGIN CUSTOM AREA [010] utilDataMgrAPI.h ------------------
 * CONFIGURATION: Add data manager events here */
/* ---------------------------- END CUSTOM AREA --------------------------- */
    nveINITIALIZE,                     // Standard reset (not calibration
                                       // nodes)
    nveRESETALL,                       // Complete reset including calibration 
                                       // nodes
    nveINVALID                         // used for enumerated type range
                                       // checking (DO NOT REMOVE)
} eNV_EVENT;

// --------------------------------CONFIGURATION------------------------------
// list of all data manager data nodes
// --------------------------------CONFIGURATION------------------------------
typedef enum
{
/* ---------------------- BEGIN CUSTOM AREA utilDataMgrAPI.h ------------------
 * CONFIGURATION: Add data manager nodes here.
 *
 * Note: The order of these values must match the data defined in the
 * m_asNodeInfo array (defined in utilDataMgr.c) */
  nnSYSCONFIG_NVRAM_DATA,
  nnSYSSTATE_NVRAM_DATA, 
  nnAUDIOMGR_NVRAM_DATA,
/* ---------------------------- END CUSTOM AREA --------------------------- */
  nnINVALID_NODE
} eNODE_NAME;

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************


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
void utilDataMgrInitialize(void);
/**
 *  @brief      This API Release the components
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilDataMgrRelease(void);

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
void utilDataMgrReset(eNV_EVENT eResetType);

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
void utilDataMgrResetFactory(eNV_EVENT eResetType);
#endif

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
PVOID utilDataMgrReadLockData(eNODE_NAME eNodeID);

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
PVOID utilDataMgrWriteLockData(eNODE_NAME eNodeID);

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
void utilDataMgrUnlock(eNODE_NAME eNodeID, BOOL       bWrite);

// ---------------------------------------------------------------------------
// This API enables or inhibits updates of the RAM mirror into non-volatile
// memory.
//
// Params:
//  bInhibit : TRUE to prevent updates, FALSE otherwise
//
// Return: none
// ---------------------------------------------------------------------------
void utilDataMgrUpdateInhibit(BOOL bInhibit);

// ---------------------------------------------------------------------------
// This API transfers data stored in the RAM mirror into non-volatile memory.
//
// Params:
//  eNodeID : The node identifier
//
// Return: none
// ---------------------------------------------------------------------------
void utilDataMgrUpdate(eNODE_NAME eNodeID);

#endif //#if !defined(_INCLUDED_UTILDATAMGRAPI_H_)


