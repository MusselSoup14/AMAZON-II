/**
  ******************************************************************************
  * @file    utilHostIfAPI.h
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
  
#if !defined(_INCLUDED_UTILHOSTIFAPI_H_)
#define _INCLUDED_UTILHOSTIFAPI_H_

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "utilHostIfCommandTable.h"
#include "appSystemStateAPI.h"
#include "halCANCtrlAPI.h"

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
#define HOST_INSTANCE_AUTO 0xFF

// ---------------------------------------------------------------------------
// CONFIGURATION: Host interface communication settings
// ---------------------------------------------------------------------------
typedef enum
{
/* ----------------- BEGIN CUSTOM AREA [020] utilHostIf.c --------------------
 * CONFIGURATION: List of concurrent host instances - include all hosts used
 * in system */
    hiSERIAL_GUI,
    hiSERIAL_SETUP,
#if USE_CAN_BUS
    hiCANBUS,
#endif    
/* ---------------------------- END CUSTOM AREA --------------------------- */
    hiINVALID                          // Must be LAST value. Used for range
                                       // checking (DO NOT REMOVE)
}GCC_PACKED eHOSTIF_HOST_INSTANCE;

// ---------------------------------------------------------------------------
// enum to UI Mode Enable/Disable by Mode Entering.
// ---------------------------------------------------------------------------
typedef enum
{
  hifCONNMODE_GUI_IDLE,

  hifCONNMODE_GUI_DIAGDEV,
  hifCONNMODE_GUI_HOSTSIM,

  hifCONNMODE_GUI_INVALID    // setinel
}GCC_PACKED eHOSTIF_CONNMODE_GUI;

// ---------------------------------------------------------------------------
// Stucture to HOST Device Information
// ---------------------------------------------------------------------------
typedef struct tagHOSTDEVINFO
{  
  BYTE        acSWVer[6];    /* 0:Major, 1:Minor, 2:Custom, 3:Candidate, 4:NVRAM Major, 5:NVRAM Minor */
  BYTE        acHWVer[2];
  BYTE        acSerialNo[DEVINFO_SERIAL_SIZE];
  DWORD       dwUnitOnTimes; /* Unit is Minute */
  BYTE        cVolume;
  BYTE        cExpanderFlags;
  SYSLOG_VARS sSyslogVars;
  PARAM_INFO  asSysParamInfo[sysPAR_INVALID]; // System Parameter info
}GCC_PACKED HOST_DEV_INFO, *PHOST_DEV_INFO;
typedef CROMDATA HOST_DEV_INFO *PCHOST_DEV_INFO;

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************
/**
 *  @brief      This API initializes static data structures that are used subsequently at
 * runtime and Initializes the device by setting registers that do not change
 * frequently at runtime.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilHostIfInitialize(void);

/**
 *  @brief      This API Release this component
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilHostIfRelease(void);

/**
 *  @brief      This API maintains the internal state machine and processes the request
 * messages by Serial I/F. It also formulates the response and sends it back to the host.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilHostIfProcess(void);

/**
 *  @brief       This API return Serial Intance as specified Host Instace
 *  @param[in]   cInstance : Host instance
 *  @param[out]  None
 *  @return      Serial Instance
 *  @note        None
 *  @attention   one 
 */  
BYTE utilHostIfSerialInstanceGet(BYTE cInstance);

/**
 *  @brief       This API Indicate the Host Active Status (Sucessive Packet Process)
 *  @param[in]   wHostActiveTimeMs: Time to display. unit is ms
                 bOn: TRUE to On, FALSE to Off
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None 
 */  
void utilHostIfMsgActiveIndicate(WORD wHostActiveTimeMs, BOOL bOn);

/**
 *  @brief      This API get the pointer of HOST Device information structure
 *  @param[in]  None
 *  @param[out] None
 *  @return     pointer of Connection information structure
 *  @note       None
 *  @attention  None
 */
PCHOST_DEV_INFO utilHostIfHostInfoPtrGet(void);

/**
 *  @brief      This API return Host I/F GUI Connection status.
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Connected, FALSE for others
 *  @note       None
 *  @attention  None
 */
BOOL utilHostIfGUIConnected(void);

/**
 *  @brief      This API return Host I/F GUI Ready (Image loading done) status.
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Ready, FALSE for others
 *  @note       None
 *  @attention  None
 */
BOOL utilHostIfGUIReadyGet(void);

/**
 *  @brief      This API return Host I/F Device information received status.
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Connected, FALSE for others
 *  @note       None
 *  @attention  None
 */
BOOL utilHostIfGUIDevInfoReceived(void);

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
                             , WORD         wBufferLength);


/**
 *  @brief      This API Set the GUI Connection Mode
 *  @param[in]  eMode : GUI Connection Mode to Set
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilHostIfGUIConnModeSet(eHOSTIF_CONNMODE_GUI eMode);

/**
 *  @brief      This API Return the GUI Connection Mode
 *  @param[in]  None
 *  @param[out] None
 *  @return     GUI Connection Mode 
 *  @note       None
 *  @attention  None
 */
eHOSTIF_CONNMODE_GUI utilHostIfGUIConnModeGet(void);

#endif //#if !defined(_INCLUDED_UTILHOSTIFAPI_H_)


