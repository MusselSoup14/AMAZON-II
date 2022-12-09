/**
  ******************************************************************************
  * @file    utilHostIfSetup.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    8-June-2018
  * @brief   This file provides APIs for Host Interface
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
#include "halPowerCtrlAPI.h"
#include "halLEDCtrlAPI.h"
#include "dvDeviceBusAPI.h"

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************
/**
 *  @brief       Build return Setup Packet
 *  @param[in]   wOPCode: opcode in payload
 *               wDataSize: data size in payload (Excluded checksum 1byte)
 *  @param[out]  pacBuffer: Buffer to build
 *  @return      built packet size
 *  @note        None
 *  @attention   None
 */
WORD MsgSetupReturnPacketBuild(WORD wOPCode, WORD wDataSize, PBYTE pacBuffer)
{
  WORD wReturnSize = 0;
  
  if(pacBuffer != NULL)
  {
    PHOSTIF_PACKET psPacket = (PHOSTIF_PACKET)pacBuffer;
    PSETUP_MESSAGE psMsg = (PSETUP_MESSAGE)&psPacket->uPayload;    
    WORD wPayloadSize = sizeof(SETUP_MESSAGE_HEADER) + wDataSize + 1; // Add Checksum
  
    /* Return Packet Build */
    psPacket->wID = PACKET_ID;
#if USE_HOSTIF_PACKET_SEQ_INC
    psPacket->sHeader.cSeq = m_acPacketSeqNum[hiSERIAL_SETUP]++;
#else
    psPacket->sHeader.cSeq = m_acPacketSeqNum[hiSERIAL_SETUP];
#endif
    psPacket->sHeader.eType = ptSETUP;

    psPacket->sHeader.wPayloadSize = wPayloadSize;
    psPacket->sHeader.wCRCPacket = 0;
#if USE_HOSTIF_PACKET_CRC
    psPacket->sHeader.wCRCPacket = utilCRC16Calc((PBYTE)psPacket, PACKET_ID_LEN + PACKET_HEADER_LEN);
#endif

    psMsg->sHeader.wOPCode = (WORD)wOPCode;
    psMsg->sHeader.wDataSize = wDataSize + 1; // Add Checksum
    psMsg->acData[wDataSize] = utilCRCCheckSumBuild((PCBYTE)psMsg, wPayloadSize);

    wReturnSize = PACKET_ID_LEN + PACKET_HEADER_LEN + wPayloadSize;
  }

  return wReturnSize;
}

/**
 *  @brief       This function processes the Setup message
 *  @param[in]   cInstance    : Host instance
                 pcsMsg : Setup message to process
 *  @param[out]  pacBuffer   : pointer to storage
 *  @return      number of bytes in the return packet
 *  @note        None
 *  @attention   None 
 */
static WORD MsgSetupProcess(BYTE cInstance, PCSETUP_MESSAGE pcsMsg, PBYTE pacBuffer)
{
  HOSTIF_PACKET       sPacket     = {0};
  PHOSTIF_PACKET      psRetPacket = (PHOSTIF_PACKET )&sPacket;
  PSETUP_MESSAGE      psRetMsg    = (PSETUP_MESSAGE)&psRetPacket->uPayload;

  WORD                wReturnSize     = 0; // built packet size to return
  eRESULT             eResult         = rcSUCCESS;

  eHIF_SETUP_CMD      eOPCodeIn, eOPCodeReturn; // Converted OPCode in PAYLOAD
  PBYTE               pacRetData      = NULL; // Pointer to Data in PAYLOAD
  ///WORD                wDataIndexIn = 0;
  WORD                wDataIndexOut = 0;
  WORD                wHostActiveTimeMs = 0;

  if(pcsMsg->sHeader.wDataSize <= PACKET_MSG_SETUP_DATA_LEN // avoid out of buffer index when checksum verify
    && utilCRCCheckSumVerify((PCBYTE)pcsMsg
                        , sizeof(SETUP_MESSAGE_HEADER)+pcsMsg->sHeader.wDataSize)
  )
  {
    /* OPCode Conversion */
    eOPCodeIn = (eHIF_SETUP_CMD)pcsMsg->sHeader.wOPCode;
    eOPCodeReturn = eOPCodeIn;

    pacRetData = psRetMsg->acData;

    wHostActiveTimeMs = 10;

    /* Command Process Here */
    switch(eOPCodeIn)
    {      
      
      /* ------------------- END CUSTOM AREA --------------------------- */

      default: break;
    }
    
  }
  else
  {
    eResult = rcERROR;
  }    

  if(rcSUCCESS == eResult 
      && wDataIndexOut)
  {                
      wReturnSize = MsgSetupReturnPacketBuild((WORD)eOPCodeReturn, wDataIndexOut, (PBYTE)psRetPacket);

      if(wReturnSize)
        dvSerialWrite(m_aeSerialInstance[cInstance], wReturnSize, (PCBYTE)psRetPacket);
  }

  /* Indicate Host Active */
  utilHostIfMsgActiveIndicate(wHostActiveTimeMs, TRUE);            

  return wReturnSize;  
}


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************
/**
 *  @brief      This API send the message via Setup I/F
 *  @param[in]  eOpCode : OPCODE Id
                pacBuffer : DATA Buffer
                wBufferLength : DATA Size
 *  @param[out] None
 *  @return     Size of message
 *  @note       None
 *  @attention  None
 */
WORD utilHostIfSetupSend(eHIF_SETUP_CMD eOPCode, 
                                  PCBYTE pacBuffer, 
                                  WORD wBufferLength)
{

  HOSTIF_PACKET       sPacket     = {0};
  PHOSTIF_PACKET      psRetPacket = (PHOSTIF_PACKET )&sPacket;
  ///PSETUP_MESSAGE      psRetMsg = (PSETUP_MESSAGE)&psRetPacket->uPayload;
  BOOL                bReturnBuild = FALSE;
  WORD                wDataSize         = 0;
  WORD                wReturnSize       = 0;
  WORD                wHostActiveTimeMs = 10;

  /* Only avaiable in CONFIG connected */
  if(utilExecSignalGet(sgSETUP_IF, SIGNAL_CONF_SETUP_ENABLE))
  {
    switch(eOPCode)
    {
      /* Examples
        case hifSETUP_DiagDev_RFNoti:      
        if(pacBuffer != NULL && wBufferLength)
        {
          memcpy(psRetMsg->acData, pacBuffer, wBufferLength);          
          wDataSize = wBufferLength;
          bReturnBuild = TRUE;
        }      
      break;*/

      default: break;
    }
        
    if(bReturnBuild)
    {
      wReturnSize = MsgSetupReturnPacketBuild((WORD)eOPCode, wDataSize, (PBYTE)psRetPacket);      

      if(wReturnSize)
      {   
        // Send Packet
        dvSerialWrite(m_aeSerialInstance[hiSERIAL_SETUP], wReturnSize, (PCBYTE)psRetPacket);

        /* Indicate Host Active */
        utilHostIfMsgActiveIndicate(wHostActiveTimeMs, TRUE);
      }
    }    

    
  }

  return wReturnSize;
}


