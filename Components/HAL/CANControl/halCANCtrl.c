/**
  ******************************************************************************
  * @file    halCANCtrl.c
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
#include "halCANCtrlAPI.h"
#include "dvGPIOAPI.h"
#include "utilExecutiveAPI.h"
#include "utilParameterTable.h"

#if USE_CAN_BUS
// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS **************
// ***************************************************************************


// ***************************************************************************
// ****************** START OF PRIVATE CONFIGURATION SECTION *****************
// ***************************************************************************

// CAN Flags to write
#define HAL_CAN_WRITE_FLAG                    0 // must be Standard Id and DATA Frame

// Max CAN frame Rx Count. it is a frame size to driver layer
#define HAL_CAN_FRAME_RX_COUNT                260 // 130(1024Byte) * X

// ***************************************************************************
// ******************* END OF PRIVATE CONFIGURATION SECTION ******************
// ***************************************************************************
// CAN-TP Node Id. must be 11bits (0x001 ~ 0x7FF). 0x000 to broadcast
static CROMDATA WORD m_awCANTPNodeIdConfig[CANTP_NODE_MAX] =
{
  CANTP_NODE_ID_T10,
};

// ***************************************************************************
// ******************** START OF PRIVATE DATA DECLARATIONS *******************
// ***************************************************************************

// CAN Frame Queue and Buffer
static CAN_FRAME_QUE_RX m_sCanFrameRxQue;
static CAN_FRAME_RX     m_asCanFrameRx[HAL_CAN_FRAME_RX_COUNT];

// Current count of byte received
static WORD           m_awHostRxBytes[CANTP_NODE_MAX];

// Number of byte available in receive buffer
static WORD           m_awHostRxBytesMax[CANTP_NODE_MAX];

// Number of bytes than can be received
static WORD           m_awHostRxThreshold[CANTP_NODE_MAX];

// pointer to CAN received message Que
static PBYTE          m_pacHostRxBuffer[CANTP_NODE_MAX];

// Signal groups to Host I/F CAN. Inherit from protocol task
static eSIGNAL_GROUPS m_aeHalCANSignalGroups[CANTP_NODE_MAX];

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************

#if USE_DEBUG_CANTP
/**
 *  @brief      Debug Rx frame
 *  @param[in]  pcsCANMsgRx: pointer to raw CAN frame to debug
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void CANTPFrameRxDebug(PCCAN_FRAME_RX pcsCANFrameRx)
{
  static DWORD dwCANFrameRxDebugCount = 0;  

  msgDbg("---> CAN FRAME RX: [%ld]", dwCANFrameRxDebugCount++);
    
  #if 1
  {
    BYTE cI;
    
    if(pcsCANFrameRx->IDE == CAN_ID_EXT)
    {
      msgRawR("ExtId: 0x%X", pcsCANFrameRx->ExtId);
    }
    else
    {
      msgRawR("StdId: 0x%X", pcsCANFrameRx->StdId);      
    }

    msgRawR("\tRTR: %d", pcsCANFrameRx->RTR);
    msgRawR("\tDLC: %d", pcsCANFrameRx->DLC);
    msgRaw("\t");
    for(cI = 0; cI < pcsCANFrameRx->DLC; cI++)
    {
      msgRawR("[0x%.2X]", pcsCANFrameRx->Data[cI]);
      }
    ///msgRaw("\tFMI: %d", pcsCANFrameRx->FMI);
  }
  #endif  
}
#endif

/**
 *  @brief      Check the CAN raw frame has valid Node Id
 *  @param[in]  wNodeId: Node Id to Check
 *  @param[out] pcNodeId: Node Id to return back
 *  @return     TRUE if VALID, FALSE to others
 *  @note       None
 *  @attention  None
 */
static BOOL CANTPIsValidNodeId(WORD wNodeId, PBYTE pcNodeId)
{
  BOOL bValid = FALSE;
  BYTE cIndex;  

  for(cIndex = 0; cIndex < CANTP_NODE_MAX; cIndex++)
  {
    if(wNodeId == m_awCANTPNodeIdConfig[cIndex])
    {
      bValid = TRUE;      
      if(pcNodeId != NULL) *pcNodeId = cIndex;
      break;
    }
  }

  return bValid;
}

/**
 *  @brief      Check the CAN raw frame is valid
 *  @param[in]  pcsCanMsg: Pointer Raw CAN frame
 *  @param[out] None
 *  @return     TRUE if VALID, FALSE to others
 *  @note       None
 *  @attention  None
 */
static BOOL CANTPIsValidFrame(PCCAN_FRAME_RX pcsCanFrame, PBYTE pcNodeId)
{
  BOOL bValid = FALSE;

  if(NULL != pcsCanFrame
    && pcsCanFrame->RTR == CAN_RTR_Data
    && pcsCanFrame->IDE == CAN_Id_Standard
    && pcsCanFrame->DLC
    && CANTPIsValidNodeId(pcsCanFrame->StdId, pcNodeId)
    && NULL != m_pacHostRxBuffer
  )
  {
    bValid = TRUE;
  }    

  return bValid;
}

/**
 *  @brief      Convert the CAN raw frame to CAN-TP
 *  @param[in]  pcsCanMsg: Pointer Raw CAN frame
 *  @param[out] None
 *  @return     rcSUCCESS= No error occurred and returned result is complete
                rcERROR= An error occurred during the operation
 *  @note       None
 *  @attention  None
 */
static eRESULT CANTPConvertFromCANFrame(PCCAN_FRAME_RX pcsCanFrame)
{
  eRESULT eResult = rcERROR; 
  BYTE cReadSize = 0;

  do
  {
    BYTE cNodeId;
    PWORD pwHostRxBytes;
    PWORD pwHostRxBytesMax;
    PBYTE pacHostRxBuffer;

    // Check raw CAN frame and read Node Id
    if(!CANTPIsValidFrame(pcsCanFrame, &cNodeId))
    {
      break; //----------->
    }

    pwHostRxBytes = &m_awHostRxBytes[cNodeId];
    pwHostRxBytesMax = &m_awHostRxBytesMax[cNodeId];
    pacHostRxBuffer = m_pacHostRxBuffer[cNodeId];
    
    if(*pwHostRxBytes >= *pwHostRxBytesMax)
    {
      utilExecSignalSet(m_aeHalCANSignalGroups[cNodeId], CANTP_SIGNAL_READ_FULL);
      ///dvDeviceBusCANInterruptEnable(FALSE);

      eResult = rcERROR;
      break;      
    }
          
    if((*pwHostRxBytes + pcsCanFrame->DLC) > *pwHostRxBytesMax)
      cReadSize = *pwHostRxBytesMax - *pwHostRxBytes;      
    else
      cReadSize = pcsCanFrame->DLC;

    memcpy(&pacHostRxBuffer[*pwHostRxBytes], pcsCanFrame->Data, cReadSize);
    *pwHostRxBytes += cReadSize;
      
    if(*pwHostRxBytes >= *pwHostRxBytesMax)
    {
      utilExecSignalSet(m_aeHalCANSignalGroups[cNodeId], CANTP_SIGNAL_READ_FULL);
      ///dvDeviceBusCANInterruptEnable(FALSE);
    }
    else if(*pwHostRxBytes >= m_awHostRxThreshold[cNodeId])
    {
      utilExecSignalSet(m_aeHalCANSignalGroups[cNodeId], CANTP_SIGNAL_READ_THRESHOLD);      
    }

    //if(utilExecSignalGet(m_aeHalCANSignalGroups[cNodeId], CANTP_SIGNAL_READ_FULL))
    //  dvDeviceBusCANInterruptEnable(TRUE);    

    /* SUCCESS */
    eResult = rcSUCCESS;

  }while(0);

  return eResult;
}

/**
 *  @brief      Process the CAN raw incomming frame 
 *  @param[in]  None
 *  @param[out] None
 *  @return     rcSUCCESS= No error occurred and returned result is complete
                rcERROR= An error occurred during the operation
 *  @note       None
 *  @attention  None
 */
static eRESULT CANInFrameProcess(void)
{
  eRESULT eResult = rcSUCCESS;  
  CAN_FRAME_RX sCANFrameRx;  

  // Read all of raw CAN frames
  while(m_sCanFrameRxQue.wAvailFrameRx)
  {
    // Read 1 raw CAN frame
    dvDeviceBusCANRead(1, &sCANFrameRx);
    
#if USE_DEBUG_CANTP
    CANTPFrameRxDebug(&sCANFrameRx);
#endif

    // if possible, Convert the raw CAN frame to CAN-TP
    eResult |= CANTPConvertFromCANFrame((PCCAN_FRAME_RX)&sCANFrameRx);

    if(!m_sCanFrameRxQue.wAvailFrameRx)
    {
      // Set Read to new raw CAN frame
      dvDeviceBusCANReadSet(HAL_CAN_FRAME_RX_COUNT
                          , &m_sCanFrameRxQue);

      // Clear SIGNALs Device Layer
      utilExecSignalClear(sgCANSPI
                          , (CANIO_SIGNAL_READ_FULL));  
    }      
  }

  return eResult;
}

// ***************************************************************************
// **************** START OF PUBLIC PROCEDURE IMPLEMENTATIONS ****************
// ***************************************************************************

/**
 *  @brief      This API sets up the internal state of component.. 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halCANCtrlInitialize(void)
{
  /* Initialize the Raw CAN frame Que */
  memset(&m_sCanFrameRxQue, 0, sizeof(CAN_FRAME_QUE_RX));
  memset(m_asCanFrameRx, 0, sizeof(CAN_FRAME_RX) * HAL_CAN_FRAME_RX_COUNT);
  m_sCanFrameRxQue.pasFrameRx = m_asCanFrameRx;  

  /* Set Read to new raw CAN frame */
  dvDeviceBusCANReadSet(HAL_CAN_FRAME_RX_COUNT
                      , &m_sCanFrameRxQue);
  
  /* Clear SIGNALs Device Layer */
  utilExecSignalClear(sgCANSPI
                      , (CANIO_SIGNAL_READ_FULL));  

  /* CAN Filter and Mask Set */
  // Refer the CANSPIDeviceInitialize()
}

/**
 *  @brief      This API Release the internal state of component..
 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halCANCtrlRelease(void)
{

}

/**
 *  @brief      This API Process the internal state of component.. 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halCANCtrlProcess(void)
{
  // Process incomming raw CAN frames
  CANInFrameProcess();
}

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
void halCANCtrlSignalGroupSet(BYTE cNodeId, eSIGNAL_GROUPS eSignalGroup)
{
  ASSERT(cNodeId < CANTP_NODE_MAX);
  
  m_aeHalCANSignalGroups[cNodeId] = eSignalGroup;
}

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
void halCANCtrlReadSet(BYTE cNodeId , WORD  wCount , WORD wThreshold, PBYTE pacBuffer)
{
  ASSERT(cNodeId < CANTP_NODE_MAX);
  
  m_awHostRxBytes[cNodeId] = 0;
  m_awHostRxBytesMax[cNodeId] = wCount;
  m_awHostRxThreshold[cNodeId] = wThreshold;
  m_pacHostRxBuffer[cNodeId] = pacBuffer;
}

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
void halCANCtrlThresholdSet(BYTE cNodeId, WORD wThreshold)
{
  ASSERT(cNodeId < CANTP_NODE_MAX);
  ASSERT(wThreshold <= m_awHostRxBytesMax[cNodeId]);

  m_awHostRxThreshold[cNodeId] = wThreshold;

  // We have already received wThreshold bytes. Immediatly signal the
  // protocol task
  if (wThreshold <= m_awHostRxBytes[cNodeId])
  {
    utilExecSignalSet(m_aeHalCANSignalGroups[cNodeId], SI_SIGNAL_READ_THRESHOLD);
  }

  /* Interrupt Disable */
  dvDeviceBusCANInterruptEnable(TRUE);
}


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
eRESULT halCANCtrlWrite(BYTE cNodeId, WORD wCount, PCBYTE pacBuffer)
{
  eRESULT eResult = rcSUCCESS;

  WORD    wIndex;
  BYTE    cCount = 0;

  ASSERT(cNodeId < CANTP_NODE_MAX && NULL != pacBuffer);

  for(wIndex = 0; wIndex < wCount; )
  {
    if(wIndex + BUS_CAN_BUFFER_SIZE > wCount) cCount = wCount - wIndex;
    else cCount = BUS_CAN_BUFFER_SIZE;

    /* Watchdog Kicking to avoid reset in huge data transfer */
    dvCPUWatchDogKick();
    
    /* Transmit fragmented Frame as 8bytes max */
    eResult = dvDeviceBusCANWrite(m_awCANTPNodeIdConfig[cNodeId]
                                        , cCount
                                        , &pacBuffer[wIndex]
                                        , HAL_CAN_WRITE_FLAG);

    /* if transmit failed */
    if(rcERROR == eResult) break; //----------->
  
    wIndex += cCount;
  }

  
  return eResult;
}

/**
 *  @brief       This API acquires the current status of the CAN-TP
 *  @param[in]   cNodeId: Node Id
 *  @param[out]  pwCountRead  : Number of message read since the last call to halCANCtrlReadSet() 
 *  @return      Current status of CAN-TP
 *  @note        None
 *  @attention   None 
 */  
eCANTP_STATUS halCANCtrlStatusGet(BYTE cNodeId, PWORD pwCountRead)
{  
  eCANTP_STATUS eCANStatus = tpSTATUS_OK;  

  ASSERT(cNodeId < CANTP_NODE_MAX);

  if(pwCountRead != NULL)
    *pwCountRead = m_awHostRxBytes[cNodeId];
  
  return eCANStatus;
}

#endif // #if USE_CAN_BUS

