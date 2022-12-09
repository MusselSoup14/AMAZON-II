/**
  ******************************************************************************
  * @file    utilHostIfGUI.c
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
#include "utilHostIfAPI.h"
#include "utilCRC.h"
#include "utilExecutiveAPI.h"
#include "utilMiscAPI.h"
#include "dvSerialAPI.h"
#include "halLEDCtrlAPI.h"
#include "halLCDCtrlAPI.h"
#include "halRTCCtrlAPI.h"
#include "appUserinterfaceAPI.h"
#include "appSystemStateAPI.h"
#include "appPowerMgrAPI.h"
#include "appAudioMgrAPI.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
#define PERIOD_SYSTEM_STATUS_REQUEST          500000

// ---------------------------------------------------------------------------
// structure to Setup Device Diagnostics Monitor
// ---------------------------------------------------------------------------
typedef struct tagGUIDIAGDEVMONITOR
{
  WORD            wKeyPressed;
  WORD            wTouchEventFlags;
  WORD            wTouchEventLevel; // Hold, Swipe, Zoom, Rotate
  WORD            wTouchX;
  WORD            wTouchY;
}GCC_PACKED GUI_DIAGDEV_MONITOR, *PGUI_DIAGDEV_MONITOR;

//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************


//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************
static eHOSTIF_CONNMODE_GUI m_eHostIfModeGUI = hifCONNMODE_GUI_IDLE;


// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************

/**
 *  @brief       Build return GUI Packet
 *  @param[in]   cInstance    : Host instance
                 wOPCode: opcode in payload
 *               wDataSize: data size in payload (Excluded checksum 1byte)
 *  @param[out]  pacBuffer: Buffer to build
 *  @return      built packet size
 *  @note        None
 *  @attention   None
 */
static WORD MsgGUIReturnPacketBuild(BYTE cInstance, WORD wOPCode, WORD wDataSize, PBYTE pacBuffer)
{
  WORD wReturnSize = 0;
  
  if(pacBuffer != NULL)
  {
    PHOSTIF_PACKET psPacket = (PHOSTIF_PACKET)pacBuffer;
    PGUI_MESSAGE psMsg = (PGUI_MESSAGE)&psPacket->uPayload;    
    WORD wPayloadSize = sizeof(GUI_MESSAGE_HEADER) + wDataSize + 1; // Add Checksum
  
    /* Return Packet Build */
    psPacket->wID = PACKET_ID;
#if USE_HOSTIF_PACKET_SEQ_INC
    psPacket->sHeader.cSeq = m_acPacketSeqNum[cInstance]++;
#else
    psPacket->sHeader.cSeq = m_acPacketSeqNum[cInstance];
#endif

    psPacket->sHeader.eType = ptGUI;

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

  ASSERT(wReturnSize <= PACKET_LEN);

  return wReturnSize;
}

/**
 *  @brief       This API processes the GUI message
 *  @param[in]   cInstance    : Host instance
                 psGUIMsg : GUI message                            
 *  @param[out]  pacBuffer   : pointer to storage
 *  @return      number of bytes in the return packet
 *  @note        None
 *  @attention   None 
 */
WORD MsgGUIProcess(BYTE cInstance, PCGUI_MESSAGE pcsMsg, PBYTE pacBuffer)
{

  HOSTIF_PACKET   sPacket           = {0};
  PHOSTIF_PACKET  psRetPacket       = (PHOSTIF_PACKET )&sPacket;
  PGUI_MESSAGE    psRetMsg          = (PGUI_MESSAGE)&psRetPacket->uPayload;

  WORD            wReturnSize       = 0; // built packet size to return
  eRESULT         eResult           = rcSUCCESS;

  eHIF_GUI_CMD    eOPCodeIn; // Converted OPCode in PAYLOAD
  WORD            wOPCodeReturn;
  PBYTE           pacRetData        = NULL; // Pointer to Data in PAYLOAD
  WORD            wDataIndexIn      = 0; // Data Index in PAYLOAD (to psRetMsg)
  WORD            wDataIndexOut     = 0; // Data Index in PAYLOAD (excluded checksum, psRetPacket)
  WORD            wHostActiveTimeMs  = 0;

  if(pcsMsg->sHeader.wDataSize <= PACKET_MSG_GUI_DATA_LEN // Avoid out of buffer index when verify the checksum
    && utilCRCCheckSumVerify((PCBYTE)pcsMsg
                        , sizeof(GUI_MESSAGE_HEADER)+pcsMsg->sHeader.wDataSize)
  )
  {
    /* OPCode Conversion */
    eOPCodeIn = (eHIF_GUI_CMD)pcsMsg->sHeader.wOPCode;
    wOPCodeReturn = (WORD)eOPCodeIn | HIF_MSK_RSP;

    pacRetData = psRetMsg->acData;

    /* Set the Default Buzzer Play Duration*/
    ///dwBuzzerTimeMs = AUD_BUZ_GUI_DEFAULT;

    wHostActiveTimeMs = 10;

    /* Restore Instance */
    m_eHostInstanceConnected = (eHOSTIF_HOST_INSTANCE)cInstance;
    
    /* Command Process Here */
    switch(eOPCodeIn)
    {      
      /* --------------------- BEGIN CUSTOM AREA --------------------------
        * CONFIGURATION: Add Command process  here */  
        
      //----------------------------------- Connection -----------------------------------//
      case hifGUI_CTH_ConnectionSet: /* C->H. Connection Request to Host */
      {
        BOOL bConnect = (BOOL)pcsMsg->acData[wDataIndexIn++];

        if(bConnect)
        {
          utilExecSignalSet(sgGUI_IF, SIGNAL_GUI_CONNECTED);

          /* Device Info Set from Main Parameter Packet */
          memcpy(&m_sHostDevInfo
                    , &pcsMsg->acData[wDataIndexIn]
                    , sizeof(HOST_DEV_INFO));
          wDataIndexIn += sizeof(HOST_DEV_INFO);
          utilExecSignalSet(sgGUI_IF, SIGNAL_GUI_DEVINFO_RECEIVED);
        }
        else
        {
          utilExecSignalClear(sgGUI_IF, SIGNAL_GUI_CONNECTED | SIGNAL_GUI_DEVINFO_RECEIVED);
        }

        /* No Return Packet */
      }        
      break;

      case hifGUI_CTH_HostDevInfoGet: /* C->H. Host Device Information Get */
        if(utilHostIfGUIConnected())
        {
          /* Device Info Set from Main Parameter Packet */
          memcpy(&m_sHostDevInfo
                    , &pcsMsg->acData[wDataIndexIn]
                    , sizeof(HOST_DEV_INFO));
          wDataIndexIn += sizeof(HOST_DEV_INFO);
          
          utilExecSignalSet(sgGUI_IF, SIGNAL_GUI_DEVINFO_RECEIVED);

          /* No Return Packet */
        }
      break;

      case hifGUI_CTH_ReadySet:           /* C->H. Loading finished notification to Host */
      {
        BOOL bReady = (BOOL)pcsMsg->acData[wDataIndexIn++];

        if(bReady) utilExecSignalSet(sgGUI_IF, SIGNAL_GUI_READY);
        else utilExecSignalClear(sgGUI_IF, SIGNAL_GUI_READY);

        /* No Return Packet */
      }
      break;

      case hifGUI_CTH_Reboot:              /* C->H. Request Host Program reboot */
      {
        if(pcsMsg->acData[wDataIndexIn++] == HOSTIF_RETURN_ACK)
          utilExecSignalSet(sgMISC, SIGNAL_MISC_HOST_REBOOT_ACK);
      
        /* No Return Packet */
      }
      break;

      //----------------------------------- DFU Host -----------------------------------//
      case hifGUI_CTH_DFUHost_Enable:       /* C->H. Host Program DFU Enable Request */
      case hifGUI_CTH_DFUHost_Send:         /* C->H. Host Program DFU Data Send  */
        /* TBD */
      break;

      //----------------------------------- DFU Voice -----------------------------------//
      case hifGUI_CTH_DFUVoice_Enable:        /* C->H. Voice IC DFU Enable Request */
      case hifGUI_CTH_DFUVoice_ChipErase:     /* C->H. Voice IC ChipErase Reuqest */
      case hifGUI_CTH_DFUVoice_Send:          /* C->H. Voice IC DFU Data Send */
      case hifGUI_HTC_DFUVoice_EnableNoti:    /* H->C. Noti. to Voice IC DFU Data Enable */
      case hifGUI_HTC_DFUVoice_ChipEraseNoti: /* H->C. Noti. to Voice IC ChipErase Reuqest */
        /* TBD */
      break;

      //----------------------------------- DFU Module (Reserved) -----------------------------------//
      case hifGUI_CTH_DFUModule_Enable:        /* C->H. Additional Module DFU Enable Request */
      case hifGUI_CTH_DFUModule_Send:          /* C->H. Additional Module DFU Data Send */
      case hifGUI_HTC_DFUModule_EnableNoti:    /* H->C. Noti. to Additional Module DFU Enable Request */
        /* TBD */
      break;

      //----------------------------------- System Status -----------------------------------//
      case hifGUI_CTH_SysStatus_Get: /* C->H. System Status Request */
      case hifGUI_HTC_SysStatus_Noti: /* H->C. System Status Noti. */
      if(sizeof(STATUS_SYS) == pcsMsg->sHeader.wDataSize-1/* Excluded Checksum */)
      {
        STATUS_SYS sSysStatusNew = *((PCSTATUS_SYS)pcsMsg->acData);
        ///STATUS_SYS sSysStatusCur = appSysStateStatusGet();

        appUISysStatusUpdate(&sSysStatusNew);
        appSysStateStatusSet(&sSysStatusNew);   

        /* No Return Packet */
      }
      break;

      case hifGUI_CTH_Therapy_ModeSet:   /* C->H. Therapy Mode Change Request */
      case hifGUI_CTH_Therapy_ReadySet:  /* C->H. Therapy Ready/Standby Request */
      case hifGUI_CTH_Therapy_StartSet:  /* C->H. Therapy Start/Stop Request */
      case hifGUI_CTH_Therapy_RSV0:      /* C->H. Reserved */
      case hifGUI_CTH_Therapy_RSV1:      /* C->H. Reserved */
      case hifGUI_CTH_Therapy_RSV2:      /* C->H. Reserved */
      case hifGUI_CTH_Therapy_RSV3:      /* C->H. Reserved */
      case hifGUI_CTH_Therapy_RSV4:      /* C->H. Reserved */
        /* TBD */
      break;

      case hifGUI_HTC_KeyEvent_Noti:     /* H->C. Key Event Noti. */
        /* TBD */
      break;

      case hifGUI_CTH_Error_Clear:       /* C->H. Error Flags Clear Request. */
        /* TBD */
      break;
      
      case hifGUI_CTH_Info_Clear:        /* C->H. Info Flags Clear Request */
        /* TBD */
      break;

      case hifGUI_CTH_Parameter_Set:     /* C->H. System Parameter Set Request */
      break;
      
      case hifGUI_CTH_Parameter_IncDec:  /* C->H. System Parameter Inc/Dec Request */
      break;


      //---------------------- Client(GUI Module) Device Diagnotic ----------------------//
      case hifGUI_HTC_ClientDiagDev_Enter:
      {
        BOOL bEnter = (BOOL)pcsMsg->acData[wDataIndexIn++] && appSysStateDiagAvailable();        

        if(bEnter)
          utilHostIfGUIConnModeSet(hifCONNMODE_GUI_DIAGDEV);
        else if(hifCONNMODE_GUI_DIAGDEV == utilHostIfGUIConnModeGet())          
          utilHostIfGUIConnModeSet(hifCONNMODE_GUI_IDLE);

        /* Return Packet Build */
        pacRetData[wDataIndexOut++] = bEnter;      
      }
      break;
      
      case hifGUI_HTC_ClientDiagDev_LEDSet:
      {
        ///WORD wLEDCtrlFlags = *(PWORD)&pcsMsg->acData[wDataIndexIn]; // compile error??
        WORD wLEDCtrlFlags;

        memcpy(&wLEDCtrlFlags, &pcsMsg->acData[wDataIndexIn], sizeof(WORD));

        halLEDCtrlFlagsSet(wLEDCtrlFlags);
      }        
      ///break;
        
      case hifGUI_HTC_ClientDiagDev_LEDGet:
      {
        WORD wLEDCtrlFlags = halLEDCtrlFlagsGet();

        /* return packet build */
        memcpy(&pacRetData[wDataIndexOut], &wLEDCtrlFlags, sizeof(WORD));
        wDataIndexOut += sizeof(WORD);
      }        
      break;

      
      case hifGUI_HTC_ClientDiagDev_LCDSet:
      {
        HAL_LCD_STATUS sLCDStatusNew = *((PCHAL_LCD_STATUS)pcsMsg->acData);

        halLCDCtrlStatusSet(sLCDStatusNew, FALSE);
      }
      ///break;

      case hifGUI_HTC_ClientDiagDev_LCDGet:
      {
        HAL_LCD_STATUS sLCDStatusNew = halLCDCtrlStatusGet();

        /* Return Packet Build */
        wDataIndexOut += sizeof(HAL_LCD_STATUS);
        memcpy(pacRetData, &sLCDStatusNew, wDataIndexOut);        
      }
      break;

      case hifGUI_HTC_ClientDiagDev_AudioSet:
      {
        halAudioCtrlVolumeSet(pcsMsg->acData[wDataIndexIn++]);
        halAudioCtrlMuteSet((BOOL)pcsMsg->acData[wDataIndexIn++]);
        halAudioCtrlAmpMuteSet((BOOL)pcsMsg->acData[wDataIndexIn++]);

        /* Return Packet Build */
        pacRetData[wDataIndexOut++] = halAudioCtrlVolumeGet();
        pacRetData[wDataIndexOut++] = halAudioCtrlMuteGet();
        pacRetData[wDataIndexOut++] = halAudioCtrlAmpMuteGet();        
      }
      break;
      
      case hifGUI_HTC_ClientDiagDev_AudioGet:
      {
        /* Return Packet Build */
        pacRetData[wDataIndexOut++] = halAudioCtrlVolumeGet();
        pacRetData[wDataIndexOut++] = halAudioCtrlMuteGet();
        pacRetData[wDataIndexOut++] = halAudioCtrlAmpMuteGet();        
        pacRetData[wDataIndexOut++] = sndINVALID;        
      }
      break;
      
      case hifGUI_HTC_ClientDiagDev_AudioPlay:
      {
        eSOUND_ID eSoundId = (eSOUND_ID)pcsMsg->acData[wDataIndexIn++];

        halAudioCtrlPlay(eSoundId, TRUE);

        /* Return Packet Build */
        pacRetData[wDataIndexOut++] = halAudioCtrlVolumeGet();
        pacRetData[wDataIndexOut++] = halAudioCtrlMuteGet();
        pacRetData[wDataIndexOut++] = halAudioCtrlAmpMuteGet();
      }
      break;

      case hifGUI_HTC_ClientDiagDev_RTCGet:
      case hifGUI_HTC_ClientDiagDev_RTCSet:
      {
        HAL_RTC_STATUS sHalRTCStatus;
        
        if(hifGUI_HTC_ClientDiagDev_RTCSet == eOPCodeIn)
        {
          sHalRTCStatus = *(PCHAL_RTC_STATUS)pcsMsg->acData; 
          
          halRTCCtrlTimestampSet(sHalRTCStatus.dwSysTimestamp, TRUE);
          halRTCCtrlTimestampSyncPeriodSet(sHalRTCStatus.wSysTimeSyncPeriod);
        }

        sHalRTCStatus = halRTCCtrlStatusGet(TRUE);

        /* Return Packet Build */
        wDataIndexOut += sizeof(HAL_RTC_STATUS);
        memcpy(pacRetData, &sHalRTCStatus, wDataIndexOut);
      }
      break;

      case hifGUI_HTC_ClientDiagDev_MonitorGet:
      {
        PGUI_DIAGDEV_MONITOR  psDiagDevMon = (PGUI_DIAGDEV_MONITOR)pacRetData;
        TOUCH_EVENT           sTouchEvent = {0,};        

        /* Return Packet Build */
        psDiagDevMon->wKeyPressed = halUICtrlKeypadCodeGet();
        
        halUICtrlTouchEventGet(&sTouchEvent);
        psDiagDevMon->wTouchEventFlags  = sTouchEvent.wEventFlags;
        psDiagDevMon->wTouchEventLevel  = sTouchEvent.wEventLevel;
        psDiagDevMon->wTouchX           = sTouchEvent.sPoint.wX;
        psDiagDevMon->wTouchY           = sTouchEvent.sPoint.wY;
        
        wDataIndexOut = sizeof(GUI_DIAGDEV_MONITOR);
      }
      break;

      //---------------------- Client(GUI Module) System Configuration ----------------------//
      case hifGUI_HTC_ClientSysConf_MiscSet:
      {
        SYSCONF_MISC sSysConfMisc = *(PCSYSCONF_MISC)&pcsMsg->acData[wDataIndexIn];        

        appSysConfigMiscSet(sSysConfMisc);
      }        
      ///break;
      
      case hifGUI_HTC_ClientSysConf_MiscGet:
      {
        SYSCONF_MISC sSysConfMisc = appSysConfigMiscGet();

        wDataIndexOut += sizeof(SYSCONF_MISC);

        /* return packet build */
        memcpy(pacRetData, &sSysConfMisc, wDataIndexOut);
      }        
      break;
      
      case hifGUI_HTC_ClientSysConf_ElapTimeReset:
      {
        BYTE cFlags = pcsMsg->acData[wDataIndexIn++];

        /* Return Packet Build */
        pacRetData[wDataIndexOut++] = (appSysConfigElapsedTimesReset(cFlags)) 
                                                            ? HOSTIF_RETURN_ACK : HOSTIF_RETURN_NAK;       
      }
      break;
      
      case hifGUI_HTC_ClientSysConf_FactoryReset:
      {
        BOOL bProtect, bReboot;

        bProtect = (BOOL)!pcsMsg->acData[wDataIndexIn++];
        bReboot = (BOOL)pcsMsg->acData[wDataIndexIn++];

#if USE_NVRAM_PROTECT_AREA
        appSysConfigProtectSet(bProtect);

        utilDataMgrReset(nveRESETALL);
#else
        if(!bProtect)
          utilDataMgrReset(nveRESETALL);
        else
          utilDataMgrReset(nveINITIALIZE);
#endif
        utilDataMgrUpdate(UPDATE_ALL_NODEID);        

        /* Stop Kicking Watchdog to Watchdog Reset */
        if(bReboot)
          utilExecSignalSet(sgPWR_MGR, SIGNAL_PM_RESET);                

        /* return packet build */
        pacRetData[wDataIndexOut++] = HOSTIF_RETURN_ACK;
      }        
      break;

      //---------------------- Client(GUI Module) System Data ----------------------//
      case hifGUI_HTC_ClientSysData_Set:      
      {
        SYSTEM_PROP sSysProp = *(PSYSTEM_PROP)&pcsMsg->acData[wDataIndexIn];   
      
        appSysStateSystemPropSet(sSysProp);
      }        
      ///break;

      case hifGUI_HTC_ClientSysData_Get:
      {
        SYSTEM_PROP sSysProp = appSysStateSystemPropGet();

        wDataIndexOut += sizeof(SYSTEM_PROP);

        /* Return Packet Build */
        memcpy(pacRetData, &sSysProp, wDataIndexOut);
      }        
      break;

      //---------------------- Client(GUI Module) Bus Debugging ----------------------//
      case hifGUI_HTC_ClientDebugBus_SoCWrite:
      case hifGUI_HTC_ClientDebugBus_SoCRead:
      {
        SOC_REGVAL    sRegVal;
        register UINT nAddress;

        sRegVal = *(PCSOC_REGVAL)pcsMsg->acData;
        
        nAddress = sRegVal.nAddress;

        if(hifGUI_HTC_ClientDebugBus_SoCWrite == eOPCodeIn)
        {
          // Write Register Value
          *(VPUINT)(nAddress) = sRegVal.nValue;
        }

        // Read Register Value
        sRegVal.nValue = *(VPUINT)(nAddress); 

        /* Return Packet Build */
        wDataIndexOut += sizeof(SOC_REGVAL);
        memcpy(pacRetData, &sRegVal, wDataIndexOut);         
      }
      break;
      
      case hifGUI_HTC_ClientDebugBus_TwiRead:
      {
        BYTE cBusId;
        WORD wDeviceAddress;
        WORD wRegisterAddress;
        WORD wCount;
        WORD wFlags = 0;
        BYTE acBuffer[256] = {0,};
        eRESULT eResult = rcINVALID;

        cBusId = pcsMsg->acData[wDataIndexIn++];
        ///wDeviceAddress = *(PWORD)&pcsMsg->acData[wDataIndex];
        memcpy(&wDeviceAddress, &pcsMsg->acData[wDataIndexIn], sizeof(wDeviceAddress));
        wDataIndexIn += sizeof(wDeviceAddress);
        ///wRegisterAddress = *(PWORD)&pcsMsg->acData[wDataIndex];
        memcpy(&wRegisterAddress, &pcsMsg->acData[wDataIndexIn], sizeof(wRegisterAddress));
        wDataIndexIn += sizeof(wRegisterAddress);
        ///wCount = *(PWORD)&pcsMsg->acData[wDataIndex];        
        memcpy(&wCount, &pcsMsg->acData[wDataIndexIn], sizeof(wCount));
        wDataIndexIn += sizeof(wCount);
        ///wFlags = *(PWORD)&pcsMsg->acData[wDataIndex];
        memcpy(&wFlags, &pcsMsg->acData[wDataIndexIn], sizeof(wFlags));
        wDataIndexIn += sizeof(wFlags);        

        if(wCount && wCount <= 256)
        {
          eResult = dvDeviceBusRead(cBusId
                                  , wDeviceAddress
                                  , wRegisterAddress
                                  , wCount
                                  , acBuffer
                                  , wFlags);
        }

        /* Return Packet Build */
        pacRetData[wDataIndexOut++] = eResult;
        if(eResult == rcSUCCESS)
        {
          memcpy(&pacRetData[wDataIndexOut], &wCount, sizeof(wCount));          
          wDataIndexOut += sizeof(wCount);
          memcpy(&pacRetData[wDataIndexOut], acBuffer, wCount);
          wDataIndexOut += wCount;
        }
        else
        {
          memset(&pacRetData[wDataIndexOut], 0, sizeof(wCount));
          wDataIndexOut += sizeof(wCount);
        }
      }
      break;
        
      case hifGUI_HTC_ClientDebugBus_TwiWrite:
      {
        BYTE cBusId;
        WORD wDeviceAddress;
        WORD wRegisterAddress;
        WORD wCount;
        WORD wFlags = 0;
        BYTE acBuffer[256] = {0,};
        eRESULT eResult = rcINVALID;

        cBusId = pcsMsg->acData[wDataIndexIn++];
        ///wDeviceAddress = *(PWORD)&pcsMsg->acData[wDataIndex];
        memcpy(&wDeviceAddress, &pcsMsg->acData[wDataIndexIn], sizeof(wDeviceAddress));        
        wDataIndexIn += sizeof(wDeviceAddress);
        ///wRegisterAddress = *(PWORD)&pcsMsg->acData[wDataIndex];
        memcpy(&wRegisterAddress, &pcsMsg->acData[wDataIndexIn], sizeof(wRegisterAddress));
        wDataIndexIn += sizeof(wRegisterAddress);
        ///wCount = *(PWORD)&pcsMsg->acData[wDataIndex];
        memcpy(&wCount, &pcsMsg->acData[wDataIndexIn], sizeof(wCount));
        wDataIndexIn += sizeof(wCount);        
        ///wFlags = *(PWORD)&pcsMsg->acData[wDataIndex];
        memcpy(&wFlags, &pcsMsg->acData[wDataIndexIn], sizeof(wFlags));
        wDataIndexIn += sizeof(wFlags);

        if(wCount && wCount <= 256)
        {
          memcpy(acBuffer, &pcsMsg->acData[wDataIndexIn], wCount);
          wDataIndexIn += wCount;

          eResult = dvDeviceBusWrite(cBusId
                                    , wDeviceAddress
                                    , wRegisterAddress
                                    , wCount
                                    , (PCBYTE)acBuffer
                                    , wFlags);
        }
        
        /* Return Packet Build */
        pacRetData[wDataIndexOut++] = eResult;
        if(eResult == rcSUCCESS)
        {
          memcpy(&pacRetData[wDataIndexOut], &wCount, sizeof(wCount));          
          wDataIndexOut += sizeof(wCount);
          memcpy(&pacRetData[wDataIndexOut], acBuffer, wCount);
          wDataIndexOut += wCount;
        }
        else
        {
          memset(&pacRetData[wDataIndexOut], 0, sizeof(wCount));
          wDataIndexOut += sizeof(wCount);
        }
      }
      break;
        
      case hifGUI_HTC_ClientDebugBus_SPIRead:
        /* TBD */
      break;
        
      case hifGUI_HTC_ClientDebugBus_SPIWrite:
        /* TBD */
      break;
        
      case hifGUI_HTC_ClientDebugBus_UARTRead:
        /* TBD */
      break;
        
      case hifGUI_HTC_ClientDebugBus_UARTWrite: 
        /* TBD */
      break;
      
      /* ------------------- END CUSTOM AREA --------------------------- */

      default: wHostActiveTimeMs = 0; break;
    }
    
  }
  else
  {
    eResult = rcERROR;
  }    

  if(rcSUCCESS == eResult 
      && wDataIndexOut)
  {                
    wReturnSize = MsgGUIReturnPacketBuild(cInstance, wOPCodeReturn, wDataIndexOut, (PBYTE)psRetPacket);

    if(wReturnSize)
    {
      switch(cInstance)
      {
        case hiSERIAL_GUI:
          dvSerialWrite((eCPU_SERIAL)utilHostIfSerialInstanceGet(cInstance)
                    , wReturnSize
                    , (PCBYTE)psRetPacket);
        break;
#if USE_CAN_BUS
        case hiCANBUS:
          halCANCtrlWrite(InstanceToCANBusNodeId(cInstance), wReturnSize, (PCBYTE)psRetPacket);
        break;
#endif        

        default: ASSERT_ALWAYS(); break;
      }
      
    }
  }

  /* Indicate Host Active */
  utilHostIfMsgActiveIndicate(wHostActiveTimeMs, TRUE);      

  return wReturnSize;  
}        


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************
/**
 *  @brief      This API return Host I/F GUI Connection status.
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Connected, FALSE for others
 *  @note       None
 *  @attention  None
 */
BOOL utilHostIfGUIConnected(void)
{
  return (utilExecSignalGet(sgGUI_IF
                        , SIGNAL_GUI_CONNECTED)) ? TRUE : FALSE;
}

/**
 *  @brief      This API return Host I/F GUI Ready (Image loading done) status.
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Ready, FALSE for others
 *  @note       None
 *  @attention  None
 */
BOOL utilHostIfGUIReadyGet(void)
{
  return (utilExecSignalGet(sgGUI_IF
                        , SIGNAL_GUI_READY)) ? TRUE : FALSE;
}

/**
 *  @brief      This API return Host I/F Device information received status.
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Connected, FALSE for others
 *  @note       None
 *  @attention  None
 */
BOOL utilHostIfGUIDevInfoReceived(void)
{
  return (utilExecSignalGet(sgGUI_IF
                        , SIGNAL_GUI_DEVINFO_RECEIVED)) ? TRUE : FALSE;
}

/**
 *  @brief      This API send the message via Config I/F
 *  @param[in]  cInstance    : Host instance. OxFF to latest connected instance
                eOpCode : OPCODE Id
                pacBuffer : DATA Buffer
                wBufferLength : DATA Size
 *  @param[out] None
 *  @return     Size of message
 *  @note       None
 *  @attention  None
 */
WORD utilHostIfGUISend(BYTE               cInstance
                             , eHIF_GUI_CMD eOPCode 
                             , PCBYTE       pacBuffer 
                             , WORD         wBufferLength)
{

  HOSTIF_PACKET   sPacket     = {0};
  PHOSTIF_PACKET  psRetPacket = (PHOSTIF_PACKET )&sPacket;
  PGUI_MESSAGE    psRetMsg    = (PGUI_MESSAGE)&psRetPacket->uPayload;

  BOOL            bReturnBuild    = FALSE;
  WORD            wDataIndexOut   = 0;
  WORD            wReturnSize     = 0;
  WORD            wHostActiveTimeMs  = 10;
  WORD            wOPCode             = ((WORD)eOPCode) | HIF_MSK_NOTI;

  if(HOST_INSTANCE_AUTO == cInstance)
  {
    cInstance = m_eHostInstanceConnected;

    if(hiINVALID == m_eHostInstanceConnected) cInstance = 0;
  }  

  switch(eOPCode)
  {
    case hifGUI_CTH_ConnectionSet:
    case hifGUI_CTH_ReadySet:
    case hifGUI_CTH_DFUHost_Enable:
    case hifGUI_CTH_DFUHost_Send:
    case hifGUI_CTH_DFUVoice_Enable:
    case hifGUI_CTH_DFUVoice_ChipErase:
    case hifGUI_CTH_DFUModule_Enable:
    case hifGUI_CTH_DFUModule_Send:
    case hifGUI_CTH_Therapy_ModeSet:
    case hifGUI_CTH_Therapy_ReadySet:
    case hifGUI_CTH_Therapy_StartSet:
    case hifGUI_CTH_Therapy_RSV0:
    case hifGUI_CTH_Therapy_RSV1:
    case hifGUI_CTH_Therapy_RSV2:
    case hifGUI_CTH_Therapy_RSV3:
    case hifGUI_CTH_Therapy_RSV4:
    case hifGUI_CTH_Error_Clear:
    case hifGUI_CTH_Info_Clear:
    case hifGUI_CTH_Parameter_Set:
    case hifGUI_CTH_Parameter_IncDec:
    if(pacBuffer != NULL
      && wBufferLength)
    {
      memcpy(psRetMsg->acData, pacBuffer, wBufferLength);
      wDataIndexOut = wBufferLength;
      
      bReturnBuild = TRUE;
    }      
    break;

    case hifGUI_CTH_Reboot:
    case hifGUI_CTH_HostDevInfoGet:
    case hifGUI_CTH_SysStatus_Get:
    
    {
      bReturnBuild = TRUE;
    }
    break;

    default: wHostActiveTimeMs = 0; break;
  }

  if(bReturnBuild)
  {
    wReturnSize = MsgGUIReturnPacketBuild(cInstance, wOPCode, wDataIndexOut, (PBYTE)psRetPacket);  
    
    if(wReturnSize)
    {
#if USE_REQUEST_STATUS    
      // Delay System Status Request
      utilExecCounterSet(coHOST_STATUS_REQ, uS_TO_TICKS(PERIOD_SYSTEM_STATUS_REQUEST)); 
#endif

      switch(cInstance)
      {
        case hiSERIAL_GUI:
          // Send Packet
          dvSerialWrite((eCPU_SERIAL)m_aeSerialInstance[cInstance], wReturnSize, (PCBYTE)psRetPacket);                
        break;
#if USE_CAN_BUS
        case hiCANBUS:
          halCANCtrlWrite(InstanceToCANBusNodeId(cInstance), wReturnSize, (PCBYTE)psRetPacket);
        break;  
#endif
        default: ASSERT_ALWAYS(); break;
      }

      /* Indicate Host Active */
      utilHostIfMsgActiveIndicate(wHostActiveTimeMs, TRUE);
    }    
  }

  return wReturnSize;
}

/**
 *  @brief      This API Set the GUI Connection Mode
 *  @param[in]  eMode : GUI Connection Mode to Set
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilHostIfGUIConnModeSet(eHOSTIF_CONNMODE_GUI eMode)
{
  ASSERT(eMode < hifCONNMODE_GUI_INVALID);
  
  m_eHostIfModeGUI = eMode;
}

/**
 *  @brief      This API Return the GUI Connection Mode
 *  @param[in]  None
 *  @param[out] None
 *  @return     GUI Connection Mode 
 *  @note       None
 *  @attention  None
 */
eHOSTIF_CONNMODE_GUI utilHostIfGUIConnModeGet(void)
{  
  return m_eHostIfModeGUI;
}                                            


