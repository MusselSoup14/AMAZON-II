/**
  ******************************************************************************
  * @file    dvTAS2505.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    08-05-2019
  * @brief   This file provides basic the Touch Driver
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
  * <h2><center>&copy; Copyright 1997-2019 (c) MadeFactory Inc.</center></h2>
  ******************************************************************************
  */  
/* Includes ------------------------------------------------------------------*/
#include "dvILI2511API.h"
#include "dvGPIOAPI.h"
#include "dvDeviceBusAPI.h"

#if USE_TOUCH_ILI2511
/** @addtogroup CWM-T10
  * @{
  */

/** @defgroup dvILI2511 
  * @brief ILI2511 Driver
  * @{
  */

/** @defgroup dvILI2511_Private_Defines
  * @{
  */

// ---------------------------------------------------------------------------
// Interrupt Input GP Channel, Pin(Mask) and Interrupt Type */
// ---------------------------------------------------------------------------
#define ILITP_INT_GP_CH          GPIO_11
#define ILITP_INT_GP_PIN_MASK    GPIO_Pin_6
#define ILITP_INT_GP_AF          AF11_6_GP6
#define ILITP_INT_NUM            INTNUM_GPIO11
#define ILITP_INT_IRQHANDLER     ILI2511InterruptHandler


// ---------------------------------------------------------------------------
// Device Bus Flags
// ---------------------------------------------------------------------------
#define ILITP_FLAG_WRITE (DEVICE_BUS_ADDR_TRANSACTION | DEVICE_BUS_FAST_MODE)
#define ILITP_FLAG_READ  (DEVICE_BUS_FAST_MODE)

/**
  * @}
  */

/** @defgroup dvILI2511_Private_TypesDefinitions
  * @{
  */
  
/**
  * @}
  */

/** @defgroup dvILI2511_Private_Macros
  * @{
  */

/* ILI2511 Device search count */
#define ILITP_DEVICE_SEARCH_COUNT 5

// ---------------------------------------------------------------------------  
// Protocol V2.x/V3.x Command List
// ---------------------------------------------------------------------------  
#define	ILITP_CMD_TC_NUM		      0x10
#define ILITP_CMD_TC_INFO         0x11 // V2.x.x only
#define ILITP_CMD_TC_REPORT       0x11 // V2.x.x only
#define ILITP_CMD_PANEL_INFO      0x20
#define ILITP_CMD_ENTER_SLEEP     0x30
#define ILITP_CMD_VER_FIRMWARE    0x40
#define ILITP_CMD_VER_PROTOCOL    0x42

// ---------------------------------------------------------------------------  
// Touch Axis to Screen Axis Pitch
// ---------------------------------------------------------------------------  
#define ILITP_AXIS_PITCH_X        12.8 /* Touch Panel Info Width / Screen Width */
#define ILITP_AXIS_PITCH_Y        12   /* Touch Panel Info Height / Screen Height */ 
#define ILITP_AXIS_PITCH_X_HALF   6.4 /* To Calculation Round Up */
#define ILITP_AXIS_PITCH_Y_HALF   6   /* To Calculation Round Up */ 

/**
  * @}
  */

/** @defgroup dvILI2511_Private_Variables
  * @{
  */
/* ------------------------- BEGIN CUSTOM AREA  -------------------------------
  CONFIGURATION: Device bus for each driver instance */
static CROMDATA BYTE m_cDeviceBus      = cpuTWI_HW; /* ILI2511 Device buses */
static CROMDATA BYTE m_cDeviceAddress  = 0x41;      /* ILI2511 Device Address */
/* ---------------------------- END CUSTOM AREA --------------------------- */

/* Internal Device Status */
static ILITP_STATUS       m_sILITPStatus;

/* Internal Device Touch Status to Higher Layer */
static ILITP_STATUS_TOUCH m_sILITPStatusTouch;
/**
  * @}
  */

/** @defgroup dvILI2511_Private_FunctionPrototypes
  * @{
  */

/**
  * @}
  */

/** @defgroup dvILI2511_Private_Functions
  * @{
  */

/**
 *  @brief         Write to specified register address (Multi-Byte Write)
 *  @param[in]     wRegisterAddress: Register address to write
                   cSize: BYTE size to write
                   pacBuffer:   Pointer to Data to write
 *  @param[out]    None
 *  @return        rcSUCCESS if the operation was successful.
 *  @return        rcERROR if communication to the hardware failed,  
 *  @note          None
 *  @attention     None
 */
static eRESULT ILITPRegsWrite(WORD wRegisterAddress, BYTE cSize , PCBYTE pacBuffer)
{
  eRESULT eResult = rcERROR;

  do
  {
    eResult = dvDeviceBusWrite(m_cDeviceBus,
                          m_cDeviceAddress,
                         (WORD)wRegisterAddress,
                         cSize,
                         pacBuffer,
                         ILITP_FLAG_WRITE);
  }while(0);
  
  return eResult;
}

/**
 *  @brief         Read from the specified register address (Multi-Byte Read)
 *  @param[in]     wRegisterAddress: Register address to Read
                   cSize: BYTE size to Read 
 *  @param[out]    pacBuffer:   pointer for return back read data
 *  @return        rcSUCCESS if the operation was successful.
 *  @return        rcERROR if communication to the hardware failed,  
 *  @note          None
 *  @attention     None
 */
static eRESULT ILITPRegsRead(WORD wRegisterAddress, BYTE cSize, PBYTE pacBuffer)
{
  eRESULT eResult = rcSUCCESS;

  ASSERT(pacBuffer != NULL);

  do
  {
    /* Write Command */
    eResult |= dvDeviceBusWrite(m_cDeviceBus,
                                 m_cDeviceAddress,
                                 wRegisterAddress,
                                 0,
                                 NULL,
                                 ILITP_FLAG_WRITE); 

    if(rcSUCCESS != eResult) break; //-------->                                 

    /* Read Data */
    eResult = dvDeviceBusRead(m_cDeviceBus,
                         m_cDeviceAddress,
                        wRegisterAddress, // unused by ILITP_FLAG_READ
                        (WORD)cSize,
                        (PBYTE)pacBuffer,
                        ILITP_FLAG_READ);
  }while(0);

  return eResult;
}

/**
 *  @brief      Convert the X/Y Panel Coordinate Axis to Screen Axis
 *  @param      wPanelX: Panel X-Direction Coordinate Axis
                wPanelY: Panel Y-Direction Coordinate Axis
                psPoint: Pointer to return back screen position
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed. 
 *  @note       None
 *  @attention  None
 */
static eRESULT ILI2511PanelToScreenAxis(WORD wPanelX
                                                  , WORD wPanelY
                                                  , PPOINT psPoint)
{
  eRESULT             eResult       = rcSUCCESS;

  ASSERT(NULL != psPoint);  

  psPoint->wX = (WORD)(((FLOAT)wPanelX + ILITP_AXIS_PITCH_X_HALF) / ILITP_AXIS_PITCH_X);
  psPoint->wY = (WORD)(((FLOAT)wPanelY + ILITP_AXIS_PITCH_Y_HALF) / ILITP_AXIS_PITCH_Y);

  /*
  psPoint->wX = MIN(psPoint->wX, PANE_WIDTH_MAX);
  psPoint->wY = MIN(psPoint->wY, PANE_HEIGHT_MAX);
  */

  return eResult;
}

/**
 *  @brief      Interrupt Handler to ILITP nINT (Falling Edge Detect)
                None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void ILI2511InterruptHandler(void)
{
  /* CANSPI Interrupt Level is LOW */
  if(!(*R_GPILEV(ILITP_INT_GP_CH) & ILITP_INT_GP_PIN_MASK))
  {
    /* Set Interrupt Flags */
    m_sILITPStatus.bIntOccurred = TRUE;
  }

  /* Clear Status */
  *R_GPEDS(ILITP_INT_GP_CH) |= ILITP_INT_GP_PIN_MASK;
}

/**
 *  @brief      Initialize the Driver
 *  @param      None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void ILI2511DriverInitialize(void)
{
  ZERO_STRUCTURE(m_sILITPStatus);
  ZERO_STRUCTURE(m_sILITPStatusTouch);
}

/**
 *  @brief      Configures the Device 
 *  @param      None
 *  @return     rcSUCCESS if the operation was successful.
 *  @note       None
 *  @attention  None
 */
static eRESULT ILI2511DeviceInitialize(void)
{
  eRESULT eResult = rcSUCCESS;
  BYTE    cRetry = 0;  

  /* H/W Reset */
  dvILI2511HWReset();  

  /* nINT Interrupt Set 
     EIRQ1 is not Supported in AMAZON-II */
  *R_PAF(ILITP_INT_GP_CH)     |= ILITP_INT_GP_AF;
  *R_GPIDIR(ILITP_INT_GP_CH)  = ILITP_INT_GP_PIN_MASK;

  *R_GPFED(ILITP_INT_GP_CH)   |= (VUINT)ILITP_INT_GP_PIN_MASK;  // Set Falling Edge
  *R_GPRED(ILITP_INT_GP_CH)   &= ~ILITP_INT_GP_PIN_MASK;        // Clear Rising Edge

  set_interrupt(ILITP_INT_NUM, ILITP_INT_IRQHANDLER);

  /* Start as Interrupt Disabled */
  dvILITPInterruptEnable(FALSE);

  /* Read Device Information */
  do
  {
    /* Read Protocol Version */
    eResult = dvILI2511ProtocolVersionRead(&m_sILITPStatus.sProtocolInfo);
    if(rcSUCCESS != eResult) continue; //--------------^

    /* Support only Protocol V3.x.x */
    if(m_sILITPStatus.sProtocolInfo.cMajor != 0x03)
    {
      msgErr("Do not Support ILI-TP Protocol V%d.%d.%d currently !"
                , m_sILITPStatus.sProtocolInfo.cMajor
                , m_sILITPStatus.sProtocolInfo.cMinor
                , m_sILITPStatus.sProtocolInfo.cRelease); 
      eResult = rcERROR;
      break; //--------->
    }

    /* Read Firmware Version */
    eResult = dvILI2511FWVersionRead(&m_sILITPStatus.sFWInfo);
    if(rcSUCCESS != eResult) continue; //--------------^

    /* Read Panel Info */
    eResult = dvILI2511PanelInfoRead(&m_sILITPStatus.sPanelInfo);
    if(rcSUCCESS != eResult) continue; //--------------^

    /* Panel Info Check */
    if(!m_sILITPStatus.sPanelInfo.wWidth
      || !m_sILITPStatus.sPanelInfo.wHeight)
    {
      msgErr("Not Available Panel Info [W:%d, H:%d] !"
                , m_sILITPStatus.sPanelInfo.wWidth
                , m_sILITPStatus.sPanelInfo.wHeight); 
      eResult = rcERROR;
      break; //--------->
    }    
    
  }while(cRetry++ < ILITP_DEVICE_SEARCH_COUNT && rcSUCCESS != eResult);

  /* Device Presence Set if there was no error during initialization */
  if(rcSUCCESS == eResult)
  {
    msgInfo("ILI-TP is Available");    
    
    msgInfo("\t Protocol Version: V%d.%d.%d"
              , m_sILITPStatus.sProtocolInfo.cMajor
              , m_sILITPStatus.sProtocolInfo.cMinor
              , m_sILITPStatus.sProtocolInfo.cRelease);  
    
    msgInfo("\t FW Version: V%d.%d.%d.%d (CV:%d.%d.%d.%d)"
              , m_sILITPStatus.sFWInfo.cChipId
              , m_sILITPStatus.sFWInfo.cMajor
              , m_sILITPStatus.sFWInfo.cMinor
              , m_sILITPStatus.sFWInfo.cRelease
              , m_sILITPStatus.sFWInfo.acCustomer[0]
              , m_sILITPStatus.sFWInfo.acCustomer[1]
              , m_sILITPStatus.sFWInfo.acCustomer[2]
              , m_sILITPStatus.sFWInfo.acCustomer[3]);

    msgInfo("\t Panel Info: Size: %dx%d, Channel: X[%d]/Y[%d], Point Max: %d, KeyInfo: 0x%.4X"
              , m_sILITPStatus.sPanelInfo.wWidth
              , m_sILITPStatus.sPanelInfo.wHeight
              , m_sILITPStatus.sPanelInfo.cChannelNumX
              , m_sILITPStatus.sPanelInfo.cChannelNumY
              , m_sILITPStatus.sPanelInfo.cPointMax
              , m_sILITPStatus.sPanelInfo.wTouchKeyInfo);              

    /* FLAG set to Available */
    m_sILITPStatus.bAvailable = TRUE;
  }    
  /* error occurred during initialization */
  else
  {
    msgErr("ILI-TP IS NOT AVAILABLE ! SKIP TOUCH CONTROL !");
  }

  return eResult;
}

/**
  * @}
  */

/** @defgroup dvILI2511_Interrupt_Service_Routine
  * @{
  */

/**
  * @}
  */



/** @defgroup dvILI2511_Public_Functions
  * @{
  */

/**
 *  @brief      This API must be called once during system startup, 
                before calls are made to any other APIs in the Chip Driver component.
 *  @param      None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvILI2511Initialize(void)
{
  ILI2511DriverInitialize();
  if(rcSUCCESS != ILI2511DeviceInitialize())
  {
    /* Error Process Here */
  }
  else
  {    
    /* Enable Interrupt */ 
    dvILITPInterruptEnable(TRUE);
  }
}

/**
 *  @brief       This API Release the Device
 *  @param[in]   None
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None
 */
void dvILI2511Release(void)
{ 

}

/**
 *  @brief       This API Process the Device
 *  @param[in]   None
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None
 */
void dvILI2511Process(void)
{   
  if(m_sILITPStatus.bIntOccurred)
  {
    /* Read Touch Info */
    dvILI2511TouchRead(NULL);
    
    /* Clear Interrupt Flags */
    m_sILITPStatus.bIntOccurred = FALSE;
  }
}

/**
 *  @brief      This API Set the Touch IC HW Reset
 *  @param[in]  bReset : TRUE to Reset, FALSE for others
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvILI2511HWReset(void)
{
  /* RESET hold: up to 10uS */
  dvGPIOFieldSet(gfGPO_LCD_nRESET, LOW);
  commDelayMS(10); 
  
  /* RESET Release: Need 150ms+300ms to stabilize after RESET release */
  dvGPIOFieldSet(gfGPO_LCD_nRESET, HIGH);
  commDelayMS(150); 
#if USE_WATCHDOG
  dvCPUWatchDogKick();
#endif  
  commDelayMS(300);
}

/**
 *  @brief      This API Read the Protocol Version
 *  @param[in]  None
 *  @param[out] psProtocolInfo : Pointer to return back Protocol Version
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed,  
 *  @note       None
 *  @attention  None
 */
eRESULT dvILI2511ProtocolVersionRead(PILITP_PROTOCOL_INFO psProtocolInfo)
{
  eRESULT     eResult = rcSUCCESS;

  ASSERT(NULL != psProtocolInfo);

  eResult |= ILITPRegsRead(ILITP_CMD_VER_PROTOCOL
                          , sizeof(ILITP_PROTOCOL_INFO)
                          , (PBYTE)psProtocolInfo);

  return eResult;
}

/**
 *  @brief      This API Read the Firmware Version
 *  @param[in]  None
 *  @param[out] psFWInfo : Pointer to return back Firmware Version
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed,  
 *  @note       None
 *  @attention  None
 */
eRESULT dvILI2511FWVersionRead(PILITP_FW_INFO psFWInfo)
{
  eRESULT     eResult = rcSUCCESS;

  ASSERT(NULL != psFWInfo);

  eResult |= ILITPRegsRead(ILITP_CMD_VER_FIRMWARE
                          , sizeof(ILITP_FW_INFO)
                          , (PBYTE)psFWInfo);

  return eResult;
}

/**
 *  @brief      This API Read the Panel Information
 *  @param[in]  None
 *  @param[out] sPanelInfo : Pointer to return back Panel Information
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed,  
 *  @note       None
 *  @attention  None
 */
eRESULT dvILI2511PanelInfoRead(PILITP_PANEL_INFO psPanelInfo)
{
  eRESULT     eResult = rcSUCCESS;

  ASSERT(NULL != psPanelInfo);

  eResult |= ILITPRegsRead(ILITP_CMD_PANEL_INFO
                          , sizeof(ILITP_PANEL_INFO)
                          , (PBYTE)psPanelInfo);
  
  return eResult;
}

/**
 *  @brief      This API Read the Touch Information (Protocol V3.x.x only)
 *  @param[in]  None
 *  @param[out] psTouchStatus : Pointer to return back Touch Information
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed,  
 *  @note       None
 *  @attention  None
 */
eRESULT dvILI2511TouchInfoRead(PILITP_TOUCH_INFO psTouchStatus)
{
  eRESULT     eResult = rcSUCCESS;

  ASSERT(NULL != psTouchStatus);

  eResult |= ILITPRegsRead(ILITP_CMD_TC_NUM
                          , sizeof(ILITP_TOUCH_INFO)
                          , (PBYTE)psTouchStatus);

  return eResult;
}

/**
 *  @brief      This API Set the Sleep Mode
 *  @param[in]  bSleep: TRUE to Sleep, FALSE to Wake up
 *  @param[out] None
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed,  
 *  @note       None
 *  @attention  None
 */
eRESULT dvILI2511SleepModeSet(BOOL bSleep)
{
  eRESULT     eResult = rcSUCCESS;

  if(bSleep)
  {
    eResult |= ILITPRegsWrite(ILITP_CMD_ENTER_SLEEP, 0, NULL);  
  }
  else
  {
    /* Wake up by HW Reset. */
    dvILI2511HWReset();
  }

  return eResult;    
}

/**
 *  @brief        This API perform Reads a Touch Point
 *  @param[in]    psPoint: Pointer to read point return back
 *  @param[out]   None
 *  @return       rcSUCCESS if Event occurred and the operation was successful
                  rcERROR if Event is not occurred or communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
eRESULT dvILI2511TouchRead(PPOINT psPoint)
{
  eRESULT             eResult       = rcERROR;  
  PILITP_STATUS_TOUCH psStatusTouch = &m_sILITPStatusTouch;

  /* Procedure enter only when Device is Available and Check Level is LOW (Event Occurred)
     EIRQ1 is not Supported in AMAZON-II */
  if(!(*R_GPILEV(ILITP_INT_GP_CH) & ILITP_INT_GP_PIN_MASK)
    && m_sILITPStatus.bAvailable) 
  {   
    do
    {
      BYTE                  cIndex = 0;
      BYTE                  acBuffer[ILITP_TOUCH_INFO_SIZE] = {0,};    
      BYTE                  cTouchId;
      PILITP_TOUCHID_INFO   pasTouchId = (PILITP_TOUCHID_INFO)m_sILITPStatus.sTouchInfo.asTouchIdInfo;       

      if(rcSUCCESS != dvILI2511TouchInfoRead((PILITP_TOUCH_INFO)acBuffer))
        break; //------------->

      m_sILITPStatus.sTouchInfo.cPacketNumer = acBuffer[cIndex++];

      /* Convert Read Buffer to Touch Id Info RAW */
      for(cTouchId = 0; cTouchId < ILITP_TOUCH_POINT_MAX; cTouchId++)
      {      
        pasTouchId[cTouchId].TouchDown    = (acBuffer[cIndex] >> 7) & 0x01;
        
        pasTouchId[cTouchId].MustBeZeroX  = (acBuffer[cIndex] >> 6) & 0x01;
        pasTouchId[cTouchId].PanelX       = ((acBuffer[cIndex] & 0x3F) << 8) | acBuffer[cIndex+1];
        cIndex += 2;
        
        pasTouchId[cTouchId].MustBeZeroY  = (acBuffer[cIndex] >> 6) & 0x03;
        pasTouchId[cTouchId].PanelY       = ((acBuffer[cIndex] & 0x3F) << 8) | acBuffer[cIndex+1];
        cIndex += 2;

        pasTouchId[cTouchId].TouchPressure = acBuffer[cIndex++];
      }
      
#if 0 // Debug Purpose Only
      {
        msgDbg(" --------------------- Touch Info. ----------------------");
        ///msgRaw("SIZEOF(ILITP_TOUCHID_INFO) = %d", sizeof(ILITP_TOUCHID_INFO));
        ///msgRaw("SIZEOF(ILITP_TOUCH_INFO) = %d", sizeof(ILITP_TOUCH_INFO));
        msgRaw("\t -> Packet Number: %d\n", m_sILITPStatus.sTouchInfo.cPacketNumer);        
        for(cTouchId = 0; cTouchId < ILITP_TOUCH_POINT_MAX; cTouchId++)
        {
          PCBYTE pcacTouchId = (PCBYTE)&m_sILITPStatus.sTouchInfo.asTouchIdInfo[cIndex];

          if(pasTouchId[cTouchId].TouchDown)
          {          
            msgRaw("\t ##### ID-%d [%s]", cTouchId, (pasTouchId[cTouchId].TouchDown) ? "DOWN" : "OFF");
            msgRaw("\t PanelX: %d, Zero-X: %d"
                        , pasTouchId[cTouchId].PanelX
                        , pasTouchId[cTouchId].MustBeZeroX
                        );
            msgRaw("\t PanelY: %d, Zero-Y: %d"
                        , pasTouchId[cTouchId].PanelY
                        , pasTouchId[cTouchId].MustBeZeroY
                        );
            msgRaw("\t TouchPressure: %d", pasTouchId[cTouchId].TouchPressure); 

            msgRaw("\t [0x%.2X][0x%.2X][0x%.2X][0x%.2X][0x%.2X]"
                        , pcacTouchId[0]
                        , pcacTouchId[1]
                        , pcacTouchId[2]
                        , pcacTouchId[3]
                        , pcacTouchId[4]
                        );
          }
        }    
      }
#endif

      /* Convert Panel Axis to Screen Axis. 
         We Support only 1 point only currently */
      if(pasTouchId[0].TouchDown
          && rcSUCCESS == ILI2511PanelToScreenAxis(pasTouchId[0].PanelX
                                               , pasTouchId[0].PanelY
                                               , &psStatusTouch->sPoint))
      {
        eResult = rcSUCCESS;

        psStatusTouch->bEventOccurred = TRUE;
    
        if(NULL != psPoint)
          *psPoint = psStatusTouch->sPoint;

#if 0 // Debug Purpose Only
        msgDbg("X:%d, Y:%d"
            , psStatusTouch->sPoint.wX
            , psStatusTouch->sPoint.wY);
#endif
      }
      else /* Touch Released */
      {
        ZERO_STRUCTURE(m_sILITPStatusTouch);
      }

        
    }while(0);    
  }

  return eResult;
}

/**
 *  @brief      This API return the Device Touch Status
 *  @param      None
 *  @return     Device Status
 *  @note       None
 *  @attention  None
 */
ILITP_STATUS_TOUCH dvILITPStatusTouchGet(void)
{
  
  return m_sILITPStatusTouch;
}

/**
 *  @brief      This API Enables nINT Interrupt
 *  @param[in]  bEnable: TRUE to Enable, FALSE to Disable
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvILITPInterruptEnable(BOOL bEnable)
{
  enable_interrupt(ILITP_INT_NUM, bEnable);
}

#endif // #if USE_TOUCH_ILI2511

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 1997-2019 (c) MadeFactory Inc. *****END OF FILE****/  
