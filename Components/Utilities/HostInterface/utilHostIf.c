/*
  ******************************************************************************
  * @file    utilHostIf.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    12-August-2014
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
  * <h2><center>&copy; Copyright 1997-2016 (c) MadeFactory Inc.</center></h2>
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "utilHostIfAPI.h"
#include "utilExecutiveAPI.h"
#include "utilCRC.h"
#include "dvCPUAPI.h"
#include "dvSerialAPI.h"

#define _MOD_NAME_ "\"HOST\""
// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
#define USE_ACKNACK_RETURN (0)

// ---------------------------------------------------------------------------
// Host Interface packet identifier
// ---------------------------------------------------------------------------
#define PACKET_ID               0xEFBE
#define PACKET_ID_LEN           2
#define RET_CODE_LEN            1

// ---------------------------------------------------------------------------
// Host Interface return codes
// ---------------------------------------------------------------------------
#define HOSTIF_RETURN_ACK      0x06
#define HOSTIF_RETURN_NAK      0x15
#define HOSTIF_RETURN_VAL      0x1D
#define HOSTIF_RETURN_PAK      0x1E
#define HOSTIF_RETURN_PARAM    0x20

// ---------------------------------------------------------------------------
// Host Interface default data length
// ---------------------------------------------------------------------------
#define HOSTIF_MSG_DATA_LEN_DEFAULT 2056  //include check 1 Byte. was 2056 spend 2053 bytes at DFU (8+2+2+2048+1)

//--------------------------------CONFIGURATION-------------------------------
// Host Interface communication timeout values for each interface
//--------------------------------CONFIGURATION-------------------------------
#define HOSTIF_SERIAL_WAIT_USEC    200000        // in 115200bps, we need to 144ms at least to exchange the flash memory 2048bytes+packet
#define HOSTIF_CANBUS_WAIT_USEC    700000        // in CAN[1MHz]@SPI[8MHz], we need to 700ms at least to exchange the flash memory 2048bytes+packet

// ---------------------------------------------------------------------------
// Timeout values in ticks
// ---------------------------------------------------------------------------
#define SERIAL_WAIT_TICKS      uS_TO_TICKS(HOSTIF_SERIAL_WAIT_USEC)
#define CANBUS_WAIT_TICKS      uS_TO_TICKS(HOSTIF_CANBUS_WAIT_USEC)


// ---------------------------------------------------------------------------
// Host Interface outgoing buffer type
// ---------------------------------------------------------------------------
typedef enum
{
  btRETURN,  // ACK/NACK buffer
  btTX,      // Msg Transmit buffer
  btNTF,
  
  btINVALID                          // Must be LAST value. Used for range
                                   // checking (DO NOT REMOVE)
}GCC_PACKED eOUT_BUFFER_TYPE;

// ---------------------------------------------------------------------------
// Host Interface incoming buffer states
// ---------------------------------------------------------------------------
typedef enum
{
  bsWAITING,
  bsRECEIVED,
  bsERROR,
  
  bsINVALID                          // Must be LAST value. Used for range
                                     // checking (DO NOT REMOVE)
}GCC_PACKED eIN_BUFFER_STATE;

// ---------------------------------------------------------------------------
// Host Interface States
// ---------------------------------------------------------------------------
typedef enum
{
  hsWAIT_FOR_PACKET_ID_L,
  hsWAIT_FOR_PACKET_ID_H,
  hsWAIT_FOR_PACKET_HEADER,
  hsWAIT_FOR_PACKET_PAYLOAD,
  hsPROCESS_MESSAGE,
  
  hsINVALID                          // Must be LAST value. Used for range
                                     // checking (DO NOT REMOVE)
}GCC_PACKED eHOSTIF_STATE;

// ---------------------------------------------------------------------------
// Host Interface supported packet type
// ---------------------------------------------------------------------------
typedef enum
{
  ptBAD_PACKET  = 0,    
  ptSETUP       = 1,
  ptGUI         = 2,    
  
  ptINVALID     = 3               // Must be LAST value. Used for range
                                       // checking (DO NOT REMOVE)
}GCC_PACKED ePACKET_TYPE;

// ---------------------------------------------------------------------------
// Packet header structure
// ---------------------------------------------------------------------------
typedef struct tagPACKETHEADER
{
  BYTE          cSeq;
  ePACKET_TYPE  eType;
  WORD          wPayloadSize;
  WORD          wCRCPacket;
}GCC_PACKED PACKET_HEADER;
#define PACKET_HEADER_LEN (sizeof(PACKET_HEADER))

// ---------------------------------------------------------------------------
// GUI Message packet content structure
// ---------------------------------------------------------------------------
typedef struct tagGUIMESSAGEHEADER
{
  WORD                wOPCode;
  WORD                wDataSize;    
}GCC_PACKED GUI_MESSAGE_HEADER, *PGUI_MESSAGE_HEADER;
#define MSG_GUI_HEADER_LEN (sizeof(GUI_MESSAGE_HEADER))

#define PACKET_MSG_GUI_DATA_LEN HOSTIF_MSG_DATA_LEN_DEFAULT
typedef struct tagGUIMESSAGE
{
  GUI_MESSAGE_HEADER  sHeader;
  BYTE                acData[PACKET_MSG_GUI_DATA_LEN];
}GCC_PACKED GUI_MESSAGE, *PGUI_MESSAGE;
typedef CROMDATA GUI_MESSAGE *PCGUI_MESSAGE;
#define PACKET_MSG_GUI_LEN (sizeof(GUI_MESSAGE))

// ============================================================================
// Setup Message packet content structure
// ============================================================================
typedef struct tagSETUPMESSAGEHEADER
{
  WORD          wOPCode;
  WORD          wDataSize;    
}GCC_PACKED SETUP_MESSAGE_HEADER, *PSETUP_MESSAGE_HEADER;

#define PACKET_MSG_SETUP_DATA_LEN HOSTIF_MSG_DATA_LEN_DEFAULT
typedef struct tagSETUPMESSAGE
{
  SETUP_MESSAGE_HEADER  sHeader;
  BYTE                  acData[PACKET_MSG_SETUP_DATA_LEN];
}GCC_PACKED SETUP_MESSAGE, *PSETUP_MESSAGE;
typedef CROMDATA SETUP_MESSAGE *PCSETUP_MESSAGE;
#define PACKET_MSG_SETUP_HEADER_LEN (sizeof(SETUP_MESSAGE_HEADER))
#define PACKET_MSG_SETUP_LEN (sizeof(SETUP_MESSAGE))

// ============================================================================
// Host Interface packet structure.
// ============================================================================
typedef struct tagHOSTIFPACKET
{
  WORD                wID;
  PACKET_HEADER       sHeader;
  union
  {
    BYTE              acPrintData;
    GUI_MESSAGE       sGUIMsg;
    SETUP_MESSAGE     sSetupMsg;
  }uPayload;
}GCC_PACKED HOSTIF_PACKET, *PHOSTIF_PACKET;
typedef const HOSTIF_PACKET *PCHOSTIF_PACKET;
#define PACKET_LEN (sizeof(HOSTIF_PACKET))

//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************

//--------------------------------CONFIGURATION-------------------------------
// Host enables - define each symbol to host that handles it
//--------------------------------CONFIGURATION-------------------------------
#define NUM_HOSTS                  hiINVALID       // total number of hosts
#define NUM_HOSTS_SERIAL           (hiSERIAL_SETUP+1)

// Device isntance to Host instances
static CROMDATA eCPU_SERIAL m_aeSerialInstance[NUM_HOSTS_SERIAL] = 
{
  cpuUART_GUI,    // hiSERIAL_GUI
  cpuUART_SETUP,  // hiSERIAL_SETUP
};

// Signal Group to Host instances
static CROMDATA eSIGNAL_GROUPS m_aeHostSignalGroup[NUM_HOSTS] = 
{
  sgSERIAL_IO2,   // hiSERIAL_GUI
  sgSERIAL_IO0,   // hiSERIAL_SETUP
#if USE_CAN_BUS
  sgCANTP,        // hiCANBUS
#endif  
};

// Counter
static CROMDATA eCOUNTERS m_aeHostCounter[NUM_HOSTS] = 
{
  coHOSTIF_SERIAL_2,    // hiSERIAL_GUI
  coHOSTIF_SERIAL_0,    // hiSERIAL_SETUP
#if USE_CAN_BUS
  coHOSTIF_CANBUS,      // hiCANBUS,
#endif  
};


//--------------------------------CONFIGURATION-------------------------------
// Buffer sizes (in bytes)
//--------------------------------CONFIGURATION-------------------------------
#define MAX_IN_MSG_BUF_SIZE		    PACKET_LEN
#define MAX_OUT_MSG_BUF_SIZE      PACKET_LEN

#define MAX_IN_MSG_BUF_SIZE_GUI     (PACKET_ID_LEN + PACKET_HEADER_LEN + PACKET_MSG_GUI_LEN)
#define MAX_IN_MSG_BUF_SIZE_SETUP   (PACKET_ID_LEN + PACKET_HEADER_LEN + PACKET_MSG_SETUP_LEN)

static CROMDATA WORD m_wHostInMsgSizeMax[NUM_HOSTS] =
{
  MAX_IN_MSG_BUF_SIZE_GUI,    // hiSERIAL_GUI 
  MAX_IN_MSG_BUF_SIZE_SETUP,  // hiSERIAL_SETUP 
#if USE_CAN_BUS
  MAX_IN_MSG_BUF_SIZE_GUI,  // hiCANBUS
#endif  
};

//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************

// ---------------------------------------------------------------------------
// Current state of each host
// ---------------------------------------------------------------------------
static eHOSTIF_STATE    m_aeHostIfCurrState[NUM_HOSTS];

// ---------------------------------------------------------------------------
// Data IO buffers
// ---------------------------------------------------------------------------
// GUI serial queue buffer
static BYTE             m_acSerialInMsgBufferQueGUI[MAX_IN_MSG_BUF_SIZE_GUI];

// SETUP serial queue buffer
static BYTE             m_acSerialInMsgBufferQueSETUP[MAX_IN_MSG_BUF_SIZE_SETUP];

/* GUI Message Process Buffer */
static BYTE             m_acSerialInMsgBufferGUI[MAX_IN_MSG_BUF_SIZE_GUI]; 

/* SETUP Message Process Buffer */
static BYTE             m_acSerialInMsgBufferSETUP[MAX_IN_MSG_BUF_SIZE_SETUP]; 

// ---------------------------------------------------------------------------
// pointers to incoming queue message buffers
// ---------------------------------------------------------------------------
static SERIAL_QUE m_asHostIfRxQue[NUM_HOSTS] = 
{
  {0, 0, 0,  m_acSerialInMsgBufferQueGUI},     // hiSERIAL_GUI
  {0, 0, 0, m_acSerialInMsgBufferQueSETUP},   // hiSERIAL_SETUP
#if USE_CAN_BUS
  {0, 0, 0,  NULL},                            // hiCANBUS  
#endif  
};

// ---------------------------------------------------------------------------
// Array of pointers to incoming message buffers
// ---------------------------------------------------------------------------
static CROMDATA PBYTE m_apcHostIfInMsgBuffer[NUM_HOSTS] =
{
  m_acSerialInMsgBufferGUI,     // hiSERIAL_GUI
  m_acSerialInMsgBufferSETUP,   // hiSERIAL_SETUP
#if USE_CAN_BUS
  m_acSerialInMsgBufferGUI,  // hiCANBUS 
#endif  
};

// ---------------------------------------------------------------------------
// Number of bytes currently occupied in each buffer
// ---------------------------------------------------------------------------
static WORD             m_awHostIfInMsgNumBytes[NUM_HOSTS];

// ---------------------------------------------------------------------------
// Packet Frame Sequence number
// ---------------------------------------------------------------------------
static BYTE m_acPacketSeqNum[NUM_HOSTS] = {0,}; 

// ---------------------------------------------------------------------------
// Serial Bus Instance that connected currently
// ---------------------------------------------------------------------------
static eHOSTIF_HOST_INSTANCE m_eHostInstanceConnected = hiINVALID;

// ---------------------------------------------------------------------------
// HOST I/F Main Device Information
// ---------------------------------------------------------------------------
static HOST_DEV_INFO m_sHostDevInfo;


// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************
/**
 *  @brief            This function processes the incoming buffer when communicating with the host using the Serial protocol.
 *  @param[in]    cInstance    : Host instance
                           bStartNewMsg : Flag indicates the start of a new packet
 *  @param[out]  None
 *  @return         State of the incoming buffer
 *  @note           None
 *  @attention     None 
 */
static eIN_BUFFER_STATE SerialInMsgBufferProcess(BYTE cInstance,
                                                 BOOL bStartNewMsg)
{
    WORD                wBytesRead, wBytesReadAvail;
    WORD                wEvents;
    WORD                wBytesExpecting;
    eIN_BUFFER_STATE    eReturnState;
    PHOSTIF_PACKET      psPacket =
        (PHOSTIF_PACKET)m_apcHostIfInMsgBuffer[cInstance];
    eSIGNAL_GROUPS eSignalGroup = m_aeHostSignalGroup[cInstance];


    if (bStartNewMsg)
    {
        // check for FIFO overflow
        if (utilExecSignalGet(eSignalGroup,
                              SI_SIGNAL_READ_FULL))
        {
            // flush out the FIFO
            dvSerialReadSet(m_aeSerialInstance[cInstance]
                            , m_wHostInMsgSizeMax[cInstance]
                            , m_wHostInMsgSizeMax[cInstance]
                            , &m_asHostIfRxQue[cInstance]);
        }

        // clear all READ related event flags
        utilExecSignalClear(eSignalGroup,
                            SI_SIGNAL_READ_THRESHOLD | SI_SIGNAL_READ_FULL);

        // reset the number of bytes read
        m_awHostIfInMsgNumBytes[cInstance] = 0;

        // wait for the packet ID
        //wBytesExpecting = PACKET_ID_LEN;
        wBytesExpecting = 1; // Read 1 bytes to no frame packet          

        // set up the read buffer
        dvSerialReadSet(m_aeSerialInstance[cInstance]
                        , m_wHostInMsgSizeMax[cInstance]
                        , wBytesExpecting
                        , &m_asHostIfRxQue[cInstance]);

        eReturnState = bsWAITING;
    }
    else
    {
      /* Read from Serial Queue */
      if(m_asHostIfRxQue[cInstance].wAvailableRxBytes)
      {
        // Read 1 bytes to no frame packet          
        wBytesReadAvail = dvSerialRead(m_aeSerialInstance[cInstance]
                                      , 1
                                      , &m_apcHostIfInMsgBuffer[cInstance][m_awHostIfInMsgNumBytes[cInstance]]);

        // Read 1 bytes successfully, then increase internal buffer index
        if(wBytesReadAvail) m_awHostIfInMsgNumBytes[cInstance]++;
      }    
      
        wEvents = utilExecSignalGet(eSignalGroup, 0xFFFF);
        
        // check to verify the previous expected data has arrived
        if (wEvents & SI_SIGNAL_READ_THRESHOLD)
        {
            // clear all READ related event flags
            utilExecSignalClear(eSignalGroup,
                            SI_SIGNAL_READ_THRESHOLD | SI_SIGNAL_READ_FULL);

            // check to verify the serial status is OK
            if (dvSerialStatus(m_aeSerialInstance[cInstance]
                                , &wBytesRead
                                , &wBytesReadAvail) == ssOK)
            {
              // assert if buffer is overrun
              ASSERT(wBytesRead <= m_wHostInMsgSizeMax[cInstance]);

              if (wBytesRead < (PACKET_ID_LEN))
              {// at hsWAIT_FOR_PACKET_ID_L to next
                  wBytesExpecting =
                      (WORD)(PACKET_ID_LEN);                  
              }
              else if (wBytesRead < (PACKET_ID_LEN + PACKET_HEADER_LEN))
              {// at hsWAIT_FOR_PACKET_ID_H to next
                  wBytesExpecting =
                      (WORD)(PACKET_ID_LEN + PACKET_HEADER_LEN);
              }
              else
              {// at hsWAIT_FOR_PACKET_HEADER to next
                  wBytesExpecting = (WORD)(PACKET_ID_LEN +
                                           PACKET_HEADER_LEN +
                                           psPacket->sHeader.wPayloadSize);
              }

              // Expecting Bytes can not be exceed Maximum buffer size
              if(wBytesExpecting <= m_wHostInMsgSizeMax[cInstance])
              {
                // set up the next threshold
                dvSerialSetThreshold(m_aeSerialInstance[cInstance],wBytesExpecting);

                // reset the timer
                utilExecCounterSet(m_aeHostCounter[cInstance],(WORD)SERIAL_WAIT_TICKS);

                // update the number of bytes read
                ///m_awHostIfInMsgNumBytes[cInstance] = wBytesRead;

                eReturnState = bsRECEIVED;
              }
              else
              {
                eReturnState = bsERROR;
              }
            }
            else
            {
                eReturnState = bsERROR;
#if USE_DEBUG_HOST
                msgErr(_MOD_NAME_": serial status is error");
#endif
                
            }
        }
        // check if FIFO overflow or
        // timeout has expired for the rest of the packet
        else if ((wEvents & SI_SIGNAL_READ_FULL) ||
                 (m_awHostIfInMsgNumBytes[cInstance] > 0 &&
                  0 == utilExecCounterGet(m_aeHostCounter[cInstance])))
        {

            eReturnState = bsERROR;
#if USE_DEBUG_HOST
            msgErr(_MOD_NAME_": check if FIFO overflow(%d) or timeout error(%d) : inMsgNumBytes (%d)"
                                  , wEvents & SI_SIGNAL_READ_FULL
                                  , (m_awHostIfInMsgNumBytes[cInstance] > 0 &&
                                                  0 == utilExecCounterGet(m_aeHostCounter[cInstance]))
                                  , m_awHostIfInMsgNumBytes[cInstance]);
#endif
        }
        // data yet to arrive
        else
        {
            eReturnState = bsWAITING;
        }
    }

    // flush the FIFO if error is encountered
    if (bsERROR == eReturnState)
    {
      dvSerialReadSet(m_aeSerialInstance[cInstance]
                      , m_wHostInMsgSizeMax[cInstance]
                      , m_wHostInMsgSizeMax[cInstance]
                      , &m_asHostIfRxQue[cInstance]);
    }

    return eReturnState;

}

#if USE_CAN_BUS
/**
 *  @brief       This function Convert Host I/F Instance to CAN-TP Node Id
 *  @param[in]   cInstance    : Host instance
 *  @param[out]  None
 *  @return      CAN ISO-TP Node Id
 *  @note        None
 *  @attention   None 
 */
static eCANTP_NODE InstanceToCANBusNodeId(BYTE cInstance)
{
  eCANTP_NODE eCANTPNodeId;
  
  switch(cInstance)
  {
    case hiCANBUS: eCANTPNodeId = tpNODE_T10; break;

    default: eCANTPNodeId = tpNODE_INVALID; break;
  }  

  return eCANTPNodeId;
}

/**
 *  @brief       This function processes the incoming buffer when communicating 
                 with the host using the ISO-TP protocol.
 *  @param[in]   cInstance    : Host instance
                 bStartNewMsg : Flag indicates the start of a new packet
 *  @param[out]  None
 *  @return      State of the incoming buffer
 *  @note        None
 *  @attention   None 
 */
static eIN_BUFFER_STATE CANBusInMsgBufferProcess(BYTE cInstance,
                                                            BOOL bStartNewMsg)
{
  WORD                wBytesRead;
  //WORD                wBytesReadAvail;
  WORD                wEvents;
  WORD                wBytesExpecting;
  eIN_BUFFER_STATE    eReturnState;
  PHOSTIF_PACKET      psPacket =
      (PHOSTIF_PACKET)m_apcHostIfInMsgBuffer[cInstance];
  eSIGNAL_GROUPS eSignalGroup = m_aeHostSignalGroup[cInstance];
  

  if(bStartNewMsg)
  {
    // check for FIFO overflow
    if (utilExecSignalGet(eSignalGroup,
                          CANTP_SIGNAL_READ_FULL))
    {
      // flush out the FIFO
      halCANCtrlReadSet(InstanceToCANBusNodeId(cInstance)
                        , m_wHostInMsgSizeMax[cInstance]
                        , m_wHostInMsgSizeMax[cInstance]
                        , m_apcHostIfInMsgBuffer[cInstance]);
    }

    // clear all READ related event flags
    utilExecSignalClear(eSignalGroup,
                        CANTP_SIGNAL_READ_THRESHOLD | CANTP_SIGNAL_READ_FULL);

    // reset the number of bytes read
    m_awHostIfInMsgNumBytes[cInstance] = 0;

    // wait for the packet ID
    wBytesExpecting = PACKET_ID_LEN;

    // set up the read buffer
    halCANCtrlReadSet(InstanceToCANBusNodeId(cInstance)
                        , m_wHostInMsgSizeMax[cInstance]
                        , wBytesExpecting
                        , m_apcHostIfInMsgBuffer[cInstance]);
    
    eReturnState = bsWAITING;
  }
  else
  {  
    wEvents = utilExecSignalGet(eSignalGroup, 0xFFFF);
    
    // check to verify the previous expected data has arrived
    if (wEvents & CANTP_SIGNAL_READ_THRESHOLD)
    {
      // clear all READ related event flags
      utilExecSignalClear(eSignalGroup,
                      CANTP_SIGNAL_READ_THRESHOLD | CANTP_SIGNAL_READ_FULL);

      // check to verify the serial status is OK
      if (halCANCtrlStatusGet(InstanceToCANBusNodeId(cInstance)
                        , &wBytesRead) == tpSTATUS_OK)
      {              
        // assert if buffer is overrun
        ASSERT(wBytesRead <= m_wHostInMsgSizeMax[cInstance]);
          
        if (wBytesRead < (PACKET_ID_LEN + PACKET_HEADER_LEN))
        {// at hsWAIT_FOR_PACKET_ID_L to next (no use hsWAIT_FOR_PACKET_ID_H)
          wBytesExpecting =
              (WORD)(PACKET_ID_LEN + PACKET_HEADER_LEN);
        }
        else
        {// at hsWAIT_FOR_PACKET_HEADER to next
          wBytesExpecting = (WORD)(PACKET_ID_LEN +
                                   PACKET_HEADER_LEN +
                                   psPacket->sHeader.wPayloadSize);
        }

        // Expecting Bytes can not be exceed Maximum buffer size
        if(wBytesExpecting <= m_wHostInMsgSizeMax[cInstance])
        {
          // set up the next threshold
          halCANCtrlThresholdSet(InstanceToCANBusNodeId(cInstance), wBytesExpecting);

          // reset the timer
          utilExecCounterSet(m_aeHostCounter[cInstance],(WORD)CANBUS_WAIT_TICKS);

          // update the number of bytes read
          m_awHostIfInMsgNumBytes[cInstance] = wBytesRead;

          eReturnState = bsRECEIVED;
        }
        else
        {
          eReturnState = bsERROR;
        }
      }
      else
      {
        eReturnState = bsERROR;
      }
    }
    // check if FIFO overflow or
    // timeout has expired for the rest of the packet
    else if ((wEvents & CANTP_SIGNAL_READ_FULL) ||
             (m_awHostIfInMsgNumBytes[cInstance] > 0 &&
            0 == utilExecCounterGet(m_aeHostCounter[cInstance])))
    {
      eReturnState = bsERROR;
    }
    // data yet to arrive
    else
    {
      eReturnState = bsWAITING;
    }
  }

  // flush the FIFO if error is encountered
  if (bsERROR == eReturnState)
  {
    halCANCtrlReadSet(InstanceToCANBusNodeId(cInstance)
                      , m_wHostInMsgSizeMax[cInstance]
                      , m_wHostInMsgSizeMax[cInstance]
                      , m_apcHostIfInMsgBuffer[cInstance]);
  }

  return eReturnState;

}
#endif // #if USE_CAN_BUS

/**
 *  @brief            This function processes the incoming buffer when communicating with the
                          host using the Serial protocol.
 *  @param[in]    cInstance    : Host instance
                            bStartNewMsg : Flag indicates the start of a new packet
 *  @param[out]  None
 *  @return         State of the incoming buffer
 *  @note           None
 *  @attention     None 
 */  
static eIN_BUFFER_STATE InMsgBufferProcess(BYTE cInstance, BOOL bStartNewMsg)
{
  eIN_BUFFER_STATE eReturnState = bsWAITING;

  switch(cInstance)
  {    
    case hiSERIAL_GUI:
    case hiSERIAL_SETUP:
      eReturnState = SerialInMsgBufferProcess(cInstance, bStartNewMsg);
    break;

#if USE_CAN_BUS
    case hiCANBUS:
      eReturnState = CANBusInMsgBufferProcess(cInstance, bStartNewMsg);
    break;
#endif

    default: ASSERT_ALWAYS(); break;
  }

  return eReturnState;
}

/**
 *  @brief       This function processes the error message
 *  @param[in]   cInstance    : Host instance
 *  @param[out]  pacBuffer   : pointer to storage
 *  @return      number of bytes in the return packet
 *  @note        None
 *  @attention   None 
 */
static WORD MsgErrorProcess(BYTE cInstance, PBYTE pacBuffer)
{
  return RET_CODE_LEN;
}


#include "utilHostIfSetup.c"
#include "utilHostIfGUI.c"

/**
 *  @brief       This function processes the serial host interface state machine
 *  @param[in]   cInstance : Host instance
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None 
 */  
static void SerialStateMachineProcess(BYTE cInstance)
{
  PHOSTIF_PACKET psPacket;
  PBYTE pacRetBuffer;
  WORD wBytesWrite = 0;
#if USE_HOSTIF_PACKET_CRC
    WORD                wCRC, wCRCResult;
#endif  

  psPacket = (PHOSTIF_PACKET)m_apcHostIfInMsgBuffer[cInstance];
  pacRetBuffer = NULL;

  switch(m_aeHostIfCurrState[cInstance])
  {
    case hsWAIT_FOR_PACKET_ID_L:
    {
      switch(InMsgBufferProcess(cInstance, FALSE))
      {
        case bsWAITING: // still waiting
          /* do nothing */
        break; 

        case bsRECEIVED:
          // if packet ID Low Byte is recognizable
          if((PACKET_ID & 0xFF) == (psPacket->wID & 0xFF))
          {
            // advance to the next state
            m_aeHostIfCurrState[cInstance] = hsWAIT_FOR_PACKET_ID_H;
            break;
          }

        case bsERROR:
          psPacket->sHeader.eType = ptBAD_PACKET;          
#if USE_HOSTIF_NOPACKETERR_PROC
          // go back to the initial state
          InMsgBufferProcess(cInstance, TRUE);
          //m_aeHostIfCurrState[cInstance] = hsWAIT_FOR_PACKET_ID_L;
#else          
          m_aeHostIfCurrState[cInstance] = hsPROCESS_MESSAGE;
#endif          
#if USE_DEBUG_HOST                        
          ///msgErr(_MOD_NAME_": bsERROR-1 (PACKET ID L [0x%.2X])", (psPacket->wID & 0xFF));
          //appSysStateUnitTest();
#endif
        break;

        default: ASSERT_ALWAYS(); break;
      }
    }      
    break;

    case hsWAIT_FOR_PACKET_ID_H:
    {
      switch(InMsgBufferProcess(cInstance, FALSE))
      {
        case bsWAITING: // still waiting
          /* do nothing */
        break; 

        case bsRECEIVED:
          // if packet ID is recognizable
          if(PACKET_ID == psPacket->wID)
          {
            // advance to the next state
            m_aeHostIfCurrState[cInstance] = hsWAIT_FOR_PACKET_HEADER;
            break;
          }

        case bsERROR:
          psPacket->sHeader.eType = ptBAD_PACKET;          
#if USE_HOSTIF_NOPACKETERR_PROC
          // go back to the initial state
          InMsgBufferProcess(cInstance, TRUE);
          m_aeHostIfCurrState[cInstance] = hsWAIT_FOR_PACKET_ID_L;
#else          
          m_aeHostIfCurrState[cInstance] = hsPROCESS_MESSAGE;
#endif          
#if USE_DEBUG_HOST                        
          msgErr(_MOD_NAME_": bsERROR-1 (PACKET ID H [0x%.4X])", psPacket->wID);
#endif
        break;

        default: ASSERT_ALWAYS(); break;
      }
    }      
    break;
    

    case hsWAIT_FOR_PACKET_HEADER:
    {
      switch (InMsgBufferProcess(cInstance,FALSE))
      {
        case bsWAITING:       // still waiting
          /* do nothing */
        break;

        case bsRECEIVED:
        // if Packet Type is supported
        switch (psPacket->sHeader.eType)
        {
          // supported packet types
          case ptGUI:
          case ptSETUP:
#if USE_HOSTIF_PACKET_CRC            
            // and then CRC check 
            wCRC = psPacket->sHeader.wCRCPacket;              

            psPacket->sHeader.wCRCPacket = 0;
            // During Image loading system is under blocked to load image. so m_awHostIfInMsgNumBytes can be bigger than expecting
            wCRCResult = utilCRC16Calc((PBYTE)psPacket, PACKET_ID_LEN + PACKET_HEADER_LEN) ;
            if(wCRCResult == wCRC)
            //if(utilCRC16Calc((PBYTE)psPacket, m_awHostIfInMsgNumBytes[cInstance]) == wCRC)
            //if(utilCRC16Calc((PBYTE)psPacket, PACKET_ID_LEN + PACKET_HEADER_LEN) == wCRC)               
#endif              
            {        
              //msgInfo("CRC OK");
              // advance to the next state
              m_aeHostIfCurrState[cInstance] = hsWAIT_FOR_PACKET_PAYLOAD;
              break;
            }              

          // everything else is not supported
          default:
            psPacket->sHeader.eType = ptBAD_PACKET;                        
#if USE_DEBUG_HOST              
           {
              msgDbg("wID 0x%.4X", psPacket->wID);
              msgDbg("cSeq %d", psPacket->sHeader.cSeq);
              msgDbg("eType %d", psPacket->sHeader.eType);
              msgDbg("PayloadSize 0x%.4X", psPacket->sHeader.wPayloadSize);
  #if USE_HOSTIF_PACKET_CRC              
              msgDbg("CRC Rx 0x%.4X", wCRC);
              msgDbg("CRC Cal 0x%.4X", wCRCResult);
  #endif              
              msgDbg("NUM %d(I:%d)", m_awHostIfInMsgNumBytes[cInstance], cInstance);
            }
            msgErr(_MOD_NAME_": bsERROR-2 (CRC or PacketType[%d])"
                                                    , psPacket->sHeader.eType);
#endif            
          break;
        }
          
        // if invalid packet type then fall through to send back a
        // NAK
        if (psPacket->sHeader.eType != ptBAD_PACKET)
        break; //  case bsRECEIVED:

        case bsERROR:
          psPacket->sHeader.eType = ptBAD_PACKET;          
#if USE_HOSTIF_NOPACKETERR_PROC
          // go back to the initial state
          InMsgBufferProcess(cInstance, TRUE);
          m_aeHostIfCurrState[cInstance] = hsWAIT_FOR_PACKET_ID_L;
#else          
          m_aeHostIfCurrState[cInstance] = hsPROCESS_MESSAGE;
#endif          
#if USE_DEBUG_HOST                        
          msgErr(_MOD_NAME_": bsERROR-3");
#endif
        break;
        
        case bsINVALID:
          ASSERT_ALWAYS();
        break;
      }
    }      
    break;

    case hsWAIT_FOR_PACKET_PAYLOAD:
    {
      switch(InMsgBufferProcess(cInstance, FALSE))
      {
        case bsWAITING: // still waiting
          /* do nothing */
        break;

        case bsRECEIVED:
#if 0 // CRC check Intire Packet
          wCRC = psPacket->sHeader.wCRCPacket;
          psPacket->sHeader.wCRCPacket = 0;
          // if CRC is matched
          if(utilCRC16Calc((PBYTE)psPacket, m_awHostIfInMsgNumBytes[cInstance]) == wCRC)
#endif                    
          {
            m_aeHostIfCurrState[cInstance] = hsPROCESS_MESSAGE;
            break;
          }

        case bsERROR:
          psPacket->sHeader.eType = ptBAD_PACKET;          
#if USE_HOSTIF_NOPACKETERR_PROC
          // go back to the initial state
          InMsgBufferProcess(cInstance, TRUE);
          m_aeHostIfCurrState[cInstance] = hsWAIT_FOR_PACKET_ID_L;
#else                    
          m_aeHostIfCurrState[cInstance] = hsPROCESS_MESSAGE;
#endif          
#if USE_DEBUG_HOST                        
          msgErr(_MOD_NAME_": bsERROR-4");
#endif
        break;

        default: ASSERT_ALWAYS(); break;
      }

    }      
    break;

    case hsPROCESS_MESSAGE:
    {
      switch(psPacket->sHeader.eType)
      {
        case ptINVALID:
        case ptBAD_PACKET:
          /* Error Message Process */
          wBytesWrite = MsgErrorProcess(cInstance, pacRetBuffer);
        break;   

        case ptGUI:
          wBytesWrite = MsgGUIProcess(cInstance, &psPacket->uPayload.sGUIMsg, pacRetBuffer);
        break;

        case ptSETUP:
          wBytesWrite = MsgSetupProcess(cInstance, &psPacket->uPayload.sSetupMsg, pacRetBuffer);
        break;

        default: ASSERT_ALWAYS(); break;        
      }
      
      // go back to the initial state
      InMsgBufferProcess(cInstance, TRUE);
      m_aeHostIfCurrState[cInstance] = hsWAIT_FOR_PACKET_ID_L;
    }      
    break;

    default: ASSERT_ALWAYS(); break;
  }

}

#if USE_CAN_BUS
/**
 *  @brief       This function processes the ISO-TP host interface state machine
 *  @param[in]   cInstance : Host instance
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None 
 */  
static void CANBusStateMachineProcess(BYTE cInstance)
{
  PHOSTIF_PACKET      psPacket;
  PBYTE               pacRetBuffer = NULL;
#if USE_HOSTIF_PACKET_CRC
  WORD                wCRC;
#endif  

  psPacket = (PHOSTIF_PACKET)m_apcHostIfInMsgBuffer[cInstance];

  //2 Process Host I/F Protocol
  switch(m_aeHostIfCurrState[cInstance])
  {
    case hsWAIT_FOR_PACKET_ID_L:
    {
      switch(InMsgBufferProcess(cInstance, FALSE))
      {
        case bsWAITING: // still waiting
          /* do nothing */
        break; 

        case bsRECEIVED:
        {
          if(PACKET_ID == psPacket->wID)
          {
            // advance to the next state
            m_aeHostIfCurrState[cInstance] = hsWAIT_FOR_PACKET_HEADER;
            break;
          }
        }

        case bsERROR:
          psPacket->sHeader.eType = ptBAD_PACKET;          
#if USE_HOSTIF_NOPACKETERR_PROC
          // go back to the initial state
          InMsgBufferProcess(cInstance, TRUE);
          m_aeHostIfCurrState[cInstance] = hsWAIT_FOR_PACKET_ID_L;
          #if USE_DEBUG_CANHOST
          msgErr("ERR: hsWAIT_FOR_PACKET_ID_L");
          #endif
#else
          m_aeHostIfCurrState[cInstance] = hsPROCESS_MESSAGE;
#endif          
        break;

        default: ASSERT_ALWAYS(); break;
      }
    }
    break;

    case hsWAIT_FOR_PACKET_HEADER:
    {
      switch (InMsgBufferProcess(cInstance,FALSE))
      {
        case bsWAITING:       // still waiting
          /* do nothing */
        break;

        case bsRECEIVED:
        // if Packet Type is supported
        switch (psPacket->sHeader.eType)
        {
          // supported packet types
          case ptGUI:
#if USE_HOSTIF_PACKET_CRC
            // and then CRC check 
            wCRC = psPacket->sHeader.wCRCPacket;
            psPacket->sHeader.wCRCPacket = 0;
            ///if(utilCRC16Calc((PBYTE)psPacket, m_awHostIfInMsgNumBytes[cInstance]) == wCRC)
            if(utilCRC16Calc((PBYTE)psPacket, PACKET_ID_LEN + PACKET_HEADER_LEN) == wCRC)
#endif
            {
              // advance to the next state
              m_aeHostIfCurrState[cInstance] = hsWAIT_FOR_PACKET_PAYLOAD;
              break;
            }

          // everything else is not supported
          default:
            psPacket->sHeader.eType = ptBAD_PACKET;
            #if USE_DEBUG_CANHOST
            msgErr("ERR: hsWAIT_FOR_PACKET_HEADER-CRC");
            #endif
          break;
        }
        
        // if invalid packet type then fall through to send back a
        // NAK
        if (psPacket->sHeader.eType != ptBAD_PACKET)
        break; // case bsRECEIVED:

        case bsERROR:
          psPacket->sHeader.eType = ptBAD_PACKET;          
#if USE_HOSTIF_NOPACKETERR_PROC
          // go back to the initial state
          InMsgBufferProcess(cInstance, TRUE);
          m_aeHostIfCurrState[cInstance] = hsWAIT_FOR_PACKET_ID_L;
          #if USE_DEBUG_CANHOST
          msgErr("ERR: hsWAIT_FOR_PACKET_HEADER");
          #endif
#else          
          m_aeHostIfCurrState[cInstance] = hsPROCESS_MESSAGE;
#endif          
        break;
        
        case bsINVALID:
          ASSERT_ALWAYS();
        break;
      }
    }
    break;

    case hsWAIT_FOR_PACKET_PAYLOAD:
    {
      switch(InMsgBufferProcess(cInstance, FALSE))
      {
        case bsWAITING: // still waiting
          /* do nothing */
        break;

        case bsRECEIVED:
        {
          m_aeHostIfCurrState[cInstance] = hsPROCESS_MESSAGE;
          break;
        }

        case bsERROR:
          psPacket->sHeader.eType = ptBAD_PACKET;          
#if USE_HOSTIF_NOPACKETERR_PROC
          // go back to the initial state
          InMsgBufferProcess(cInstance, TRUE);
          m_aeHostIfCurrState[cInstance] = hsWAIT_FOR_PACKET_ID_L;
          #if USE_DEBUG_CANHOST
          msgErr("ERR: hsWAIT_FOR_PACKET_PAYLOAD");
          #endif
#else
          m_aeHostIfCurrState[cInstance] = hsPROCESS_MESSAGE;
#endif          
        break;

        default: ASSERT_ALWAYS(); break;
      }

    }
    break;

    case hsPROCESS_MESSAGE:
    {
      switch(psPacket->sHeader.eType)
      {
        case ptINVALID:
        case ptBAD_PACKET:
          /* Error Message Process */ 
          MsgErrorProcess(cInstance, pacRetBuffer);
          #if USE_DEBUG_CANHOST
          msgErr("ERR: hsPROCESS_MESSAGE");
          #endif
        break;

        case ptGUI:
          MsgGUIProcess(cInstance, &psPacket->uPayload.sGUIMsg, pacRetBuffer);
        break;

        default: ASSERT_ALWAYS(); break;        
      }
      
      // go back to the initial state
      InMsgBufferProcess(cInstance, TRUE);
      m_aeHostIfCurrState[cInstance] = hsWAIT_FOR_PACKET_ID_L;
    }
    break;

    default: ASSERT_ALWAYS(); break;
  }
}
#endif // #if USE_CAN_BUS

/**
 *  @brief       This function processes the host interface state machine
 *  @param[in]   cInstance : Host instance
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None 
 */  
static void StateMachineProcess(BYTE cInstance)
{
  switch(cInstance)
  {
    case hiSERIAL_GUI:      
    case hiSERIAL_SETUP:
      SerialStateMachineProcess(cInstance);
    break;

#if USE_CAN_BUS
    case hiCANBUS:
      // Process CAN-TP 
      halCANCtrlProcess();

      // Process Host I/F Protocol
      CANBusStateMachineProcess(cInstance);
    break;
#endif    

    default: ASSERT_ALWAYS(); break;
  }  
}

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *  @brief      This API initializes static data structures that are used subsequently at
 *              runtime and Initializes the device by setting registers that do not change
 *              frequently at runtime.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilHostIfInitialize(void)
{  
  BYTE cInstance;  

  for(cInstance = 0; cInstance < NUM_HOSTS; cInstance++)
  {
    // initialize all host instances
    m_aeHostIfCurrState[cInstance] = hsWAIT_FOR_PACKET_ID_L;

    memset(m_apcHostIfInMsgBuffer[cInstance], 0, m_wHostInMsgSizeMax[cInstance]);

    // start polling the incoming buffer
    InMsgBufferProcess(cInstance, TRUE);

    // set the ready flag for the print buffer
    utilExecSignalClear(m_aeHostSignalGroup[cInstance], 0xFFFF);

    switch(cInstance)
    {
      case hiSERIAL_GUI:
      case hiSERIAL_SETUP:
        utilExecSignalSet(m_aeHostSignalGroup[cInstance], SI_SIGNAL_WRITE_COMPLETE);
      break;

#if USE_CAN_BUS
      case hiCANBUS:
        // inherit the signal group to lower layer
        halCANCtrlSignalGroupSet(InstanceToCANBusNodeId(cInstance), m_aeHostSignalGroup[cInstance]);
        utilExecSignalSet(m_aeHostSignalGroup[cInstance], CANTP_SIGNAL_WRITE_COMPLETE);
      break;
#endif

      default: break;
    }
  }

  // Clear SIGNAL to Connection Information
  utilExecSignalClear(sgSETUP_IF, 0xFFFF);
  utilExecSignalClear(sgGUI_IF, 0xFFFF);  

  memset(&m_sHostDevInfo, 0, sizeof(HOST_DEV_INFO));

#if USE_SYSLOG
  /* Set Default Value to SYSLOG. 
     It will be update by HOST after connection established */
  m_sHostDevInfo.sSyslogVars.bLogEnable           = FALSE; // Start as Disabled
  m_sHostDevInfo.sSyslogVars.bLogEnableOnShotOnly = TRUE;
  m_sHostDevInfo.sSyslogVars.wFlushPeriod         = SYSLOG_FLUSH_PERIOD_MAX;
  m_sHostDevInfo.sSyslogVars.wTimestampSyncPeriod = SYSLOG_SYNC_PERIOD_MAX;
  m_sHostDevInfo.sSyslogVars.wLogFileSizeMax      = 128;
#endif
}

/**
 *  @brief      This API Release this component
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilHostIfRelease(void)
{
}

/**
 *  @brief      This API maintains the internal state machine and processes the request
 *              messages by Serial I/F. 
 *              It also formulates the response and sends it back to the host.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
 void utilHostIfProcess(void)
{
  BYTE cInstance;

  for(cInstance = 0; cInstance < NUM_HOSTS; cInstance++)
  {
    // 'clock' the state machine
    StateMachineProcess(cInstance);
  }

  /* Host I/F Active LED Off */
  if(!utilExecCounterGet(coHOSTIF_ACTIVE_LED))
    utilHostIfMsgActiveIndicate(0, 0);
}

/**
 *  @brief       This API return Serial Intance as specified Host Instace
 *  @param[in]   cInstance : Host instance
 *  @param[out]  None
 *  @return      Serial Instance
 *  @note        None
 *  @attention   one 
 */
BYTE utilHostIfSerialInstanceGet(BYTE cInstance)
{
  ASSERT(cInstance < hiINVALID);

  return (BYTE)m_aeSerialInstance[cInstance];
}

/**
 *  @brief       This API Indicate the Host Active Status (Sucessive Packet Process)
 *  @param[in]   wHostActiveTimeMs: Time to display. unit is ms
                 bOn: TRUE to On, FALSE to Off
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None 
 */  
void utilHostIfMsgActiveIndicate(WORD wHostActiveTimeMs, BOOL bOn)
{
  if(bOn)
  {    
    if(wHostActiveTimeMs)
    {
      /* Host I/F Active LED On */
      halLEDCtrlOnOffSet(ledRAW_MSG, bOn);
      utilExecCounterSet(coHOSTIF_ACTIVE_LED
                    , uS_TO_TICKS((DWORD)wHostActiveTimeMs*1000));
    }
  }
  else
    halLEDCtrlOnOffSet(ledRAW_MSG, bOn);  
}


/**
 *  @brief      This API get the pointer of HOST Device information structure
 *  @param[in]  None
 *  @param[out] None
 *  @return     pointer of Connection information structure
 *  @note       None
 *  @attention  None
 */
PCHOST_DEV_INFO utilHostIfHostInfoPtrGet(void)
{
  return (PCHOST_DEV_INFO)&m_sHostDevInfo;
}



