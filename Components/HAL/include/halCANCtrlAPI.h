/**
  ******************************************************************************
  * @file    halCANCtrlAPI.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    29-May-2019
  * @brief   This file provides the HAL CAN Control with CAN-TP procedure
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
  * <h2><center>&copy; Copyright 1997-2018 (c) MadeFactory Inc.</center></h2>
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#ifndef _HALCANCTRLAPI_H_
#define _HALCANCTRLAPI_H_

#include "common.h"
#include "dvCPUAPI.h"
#include "utilExecutiveAPI.h"
#include "dvMCP25625API.h"

#if USE_CAN_BUS
// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

// ---------------------------------------------------------------------------
// enum to Node Id (Node Id == Raw CAN Message Id)
// ---------------------------------------------------------------------------
typedef enum
{
  tpNODE_T10,

  tpNODE_INVALID
}eCANTP_NODE;
#define CANTP_NODE_MAX tpNODE_INVALID

// ---------------------------------------------------------------------------
// The following enumeration describes the CAN-TP status values that can be
// returned from the halCANCtrlStatusGet API. 
// ---------------------------------------------------------------------------
typedef enum
{
  tpSTATUS_OK,                              // CAN status is ok (no read errors)
  
  tpSTATUS_INVALID                          // Must be LAST value. Used for range
                                     // checking (DO NOT REMOVE)
}eCANTP_STATUS;

// ***************************************************************************
// ******************** END OF PUBLIC CONFIGURATION SECTION ******************
// ***************************************************************************

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *  @brief      This API sets up the internal state of component..
 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halCANCtrlInitialize(void);

/**
 *  @brief      This API Release the internal state of component..
 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halCANCtrlRelease(void);

/**
 *  @brief         This API Process the internal state of component..
 
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halCANCtrlProcess(void);

/**
 *  @brief       This API Set Signal Groups on specified Node Id.
                 This API must be called by Protocol Task
 *  @param[in]   cNodeId: CAN Node Id
                 eSignalGroup: Signal group to Set
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None 
 */  
void halCANCtrlSignalGroupSet(BYTE cNodeId, eSIGNAL_GROUPS eSignalGroup);

/**
 *  @brief       This API reads data from the RS-232 Serial Port.
 *  @param[in]   cNodeId : CAN Node Id
                 wCount     : Number of bytes to available for storage in the pacBuffer parameter
                 wThreshold : Number of bytes to receive before signaling the protocol
                 pacBuffer : Pointer to Host I/F Buffer
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None 
 */  
void halCANCtrlReadSet(BYTE cNodeId , WORD  wCount , WORD wThreshold, PBYTE pacBuffer);

/**
 *  @brief       This API changes the threshold for signaling the protocol task when
                  receiving bytes on the Serial Port.
 *  @param[in]   eSerialId : CPU USART Device Id
                 wThreshold : Number of bytes to receive before signaling the protocol
                              task.  Must be less than or equal to amount of bytes
                              specified in the wCount parameter of the last call to
                              halCANCtrlMsgReceive()
                                                 
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None 
 */ 
void halCANCtrlThresholdSet(BYTE cNodeId, WORD wThreshold);

/**
 *  @brief      This API write as CAN-TP Message
 *  @param[in]  cNodeId: Node Id to Transmit
                wCount: Number of bytes to transmit
                pacBuffer: Pointer to an array of at least wCount bytes to transmit
 *  @param[out] None
 *  @return     rcSUCCESS= No error occurred and returned result is complete
                rcERROR= An error occurred during the operation
 *  @note       None
 *  @attention  None
 */
eRESULT halCANCtrlWrite(BYTE cNodeId, WORD wCount, PCBYTE pacBuffer);

/**
 *  @brief       This API acquires the current status of the CAN-TP
 *  @param[in]   cNodeId: Node Id
 *  @param[out]  pwCountRead  : Number of message read since the last call to halCANCtrlReadSet() 
 *  @return      Current status of CAN-TP
 *  @note        None
 *  @attention   None 
 */  
eCANTP_STATUS halCANCtrlStatusGet(BYTE cNodeId, PWORD pwCountRead);

#endif // #if USE_CAN_BUS

#endif // _HALCANCTRLAPI_H_

