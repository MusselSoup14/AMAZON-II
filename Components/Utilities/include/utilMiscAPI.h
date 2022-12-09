/**
  ******************************************************************************
  * @file    utilMiscAPI.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    12-August-2014
  * @brief   This file provides APIs for Misc.
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
  
#if !defined(_INCLUDED_UTILMISCAPI_H_)
#define _INCLUDED_UTILMISCAPI_H_

/* Includes ------------------------------------------------------------------*/
#include "common.h"

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
typedef enum
{
  miscMASS_SRC_DFU,
  miscMASS_SRC_LOG,

  miscMASS_SRC_INVALID
}eMISC_MASS_SRC;

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************
// ------------------------------------------------------------------
// enum for MISC State machine
// ------------------------------------------------------------------
typedef enum
{
  miscSTATE_IDLE,
/* --------------------- BEGIN CUSTOM AREA --------------------------
 * CONFIGURATION: Add misc states here */

  miscSTATE_MASS,
/* ------------------- END CUSTOM AREA --------------------------- */

  miscSTATE_INVALID                    // Used for enumerated type range
                                       // checking (DO NOT REMOVE)
} eMISC_STATE;

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *  @brief      This API initializes static data structures that are used subsequently at
                runtime and Initializes the device by setting registers that do not change
                frequently at runtime.                          
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilMiscInitialize(void);

/**
 *  @brief      This API maintains the internal state machine and processes the request
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilMiscProcess(void);

/**
 *  @brief      This API Release the internal Components
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilMiscRelease(void);

/**
 *  @brief      This API Set to USB Mass-storage source
 *  @param[in]  bToSDCard: TRUE to SD Card, FALSE to NAND
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilMiscMassStorageSourceSet(eMISC_MASS_SRC eMassSource);

/**
 *  @brief      This API Get to USB Mass-storage source
 *  @param[in]  None
 *  @param[out] None
 *  @return     USB Mass-storage source
 *  @note       None
 *  @attention  None
 */
eMISC_MASS_SRC utilMiscMassStorageSourceGet(void);

/**
 *  @brief      This API Perform connecting to USB Mass-storage source
 *  @param[in]  bConnect: TRUE to Connect, FALSE to Disconnect
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilMiscMassStorageConnectSet(BOOL bConnect);

/**
 *  @brief      This API Perform connecting to USB Mass-storage source
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Connected, FALSE to Disconnected
 *  @note       None
 *  @attention  None
 */
BOOL utilMiscMassStorageConnectGet(void);

#endif //#if !defined(_INCLUDED_UTILMISCAPI_H_)

