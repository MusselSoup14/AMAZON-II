/**
  ******************************************************************************
  * @file    dvILI2511API.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    08-05-2019
  * @brief   This file contains all the functions prototypes for the Touch Driver
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#if !defined(_INCLUDED_DVILI2511API_H_)
#define _INCLUDED_DVILI2511API_H_

 /* Includes ------------------------------------------------------------------*/
#include "common.h"

#if USE_TOUCH_ILI2511
/** @addtogroup CWM-T10
  * @{
  */


/** @defgroup dvILI2511 
  * @brief ILI2511 Driver
  * @{
  */

/** @defgroup dvILI2511_Exported_Types
  * @{
  */

/* Maximum Touch Point. availabe up to 10point but we use only 1 point */
#define ILITP_TOUCH_POINT_MAX     1     

// ---------------------------------------------------------------------------  
// Touch Status (Protocol V3.x.x. Read Only)
// ---------------------------------------------------------------------------  
typedef struct tagILITPTOUCHIDINFO
{
  WORD  TouchDown       : 1;      /* 1 = Touch Down, 0 = Touch Off */
  WORD  MustBeZeroX     : 1;      /* Must be 0 */  
  WORD  PanelX          : 14;     /* X Direction Coordinate*/  

  WORD  MustBeZeroY     : 2;      /* Must be 0 */  
  WORD  PanelY          : 14;     /* Y Direction Coordinate High */  
  
  BYTE  TouchPressure;            /* Touch Pressure */
}GCC_PACKED ILITP_TOUCHID_INFO, *PILITP_TOUCHID_INFO;
typedef CROMDATA ILITP_TOUCHID_INFO *PCILITP_TOUCHID_INFO;

// ---------------------------------------------------------------------------  
// Touch Information (Protocol V3.x.x. Read Only)
// ---------------------------------------------------------------------------  
typedef struct tagILITPTOUCHINFO
{
  BYTE                cPacketNumer;       /* Packet Number
                                             0: No Touch (in Real-world, Couldn't have this value. Why? )
                                             1: Last Report at ID 0 to ID 5 (include release status)
                                             2: Last Report at ID 6 to ID 9 (include release status)
                                          */
  ILITP_TOUCHID_INFO   asTouchIdInfo[ILITP_TOUCH_POINT_MAX];  /* Touch Id Information */
}GCC_PACKED ILITP_TOUCH_INFO, *PILITP_TOUCH_INFO;
typedef CROMDATA ILITP_TOUCH_INFO *PCILITP_TOUCH_INFO;
#define ILITP_TOUCH_INFO_SIZE   (5*ILITP_TOUCH_POINT_MAX + 1)     /* Touch Info Byte Size */

// ---------------------------------------------------------------------------  
// Panel Information (Protocol V3.x.x. Read Only)
// ---------------------------------------------------------------------------  
typedef struct tagILITPPANELINFO
{
  WORD    wWidth;           /* The Maxmimum X Coordinate */
  WORD    wHeight;          /* The Maxmimum Y Coordinate */
  BYTE    cChannelNumX;     /* The Channel Numbers of X Direciton */
  BYTE    cChannelNumY;     /* The Channel Numbers of X Direciton */
  BYTE    cPointMax;        /* The maximum report points */
  WORD    wTouchKeyInfo;    /* The Channel Numbres of TouchKey/Scrolling Bar */
}GCC_PACKED ILITP_PANEL_INFO, *PILITP_PANEL_INFO;
typedef CROMDATA ILITP_PANEL_INFO *PCILITP_PANEL_INFO;

// ---------------------------------------------------------------------------  
// Firmware Version (Protocol V3.x.x. Read Only)
// ---------------------------------------------------------------------------  
typedef struct tagILITPFWINFO
{
  BYTE    cChipId;          /* Chip ID Code */
  BYTE    cMajor;           /* Major Firmware Version */
  BYTE    cMinor;           /* Minor Firmware Version */
  BYTE    cRelease;         /* Release Firmware Version */
  BYTE    acCustomer[4];    /* For Customer Firmware Version 0 ~ 3 */
}GCC_PACKED ILITP_FW_INFO, *PILITP_FW_INFO;
typedef CROMDATA ILITP_FW_INFO *PCILITP_FW_INFO;

// ---------------------------------------------------------------------------  
// Protocol Version (Read Only)
// ---------------------------------------------------------------------------  
typedef struct tagILITPPROTOCOLINFO
{
  BYTE    cMajor;           /* Major Protocol Version: must be 0x03 */
  BYTE    cMinor;           /* Minor Protocol Version */
  BYTE    cRelease;         /* Release Protocol Version */
}GCC_PACKED ILITP_PROTOCOL_INFO, *PILITP_PROTOCOL_INFO;
typedef CROMDATA ILITP_PROTOCOL_INFO *PCILITP_PROTOCOL_INFO;

// ---------------------------------------------------------------------------  
// Device Status
// ---------------------------------------------------------------------------  
typedef struct tagILITPSTATUS
{
  BYTE                bAvailable;       /* TRUE if Device is Available */
  BOOL                bIntOccurred;     /* TRUE if Interrupt Occurred */
  ILITP_TOUCH_INFO    sTouchInfo;       /* Touch Status */
  ILITP_PANEL_INFO    sPanelInfo;       /* Panel Information (Protocol V3.x.x. Read Only) */
  ILITP_FW_INFO       sFWInfo;          /* Firmware Version (Protocol V3.x.x. Read Only) */
  ILITP_PROTOCOL_INFO sProtocolInfo;    /* Protocol Version (Protocol V3.x.x. Read Only) */
}GCC_PACKED ILITP_STATUS, *PILITP_STATUS;
typedef CROMDATA ILITP_STATUS *PCILITP_STATUS;

// ---------------------------------------------------------------------------  
// Device Touch Status
// ---------------------------------------------------------------------------  
typedef struct tagILITPSTATUSTOUCH
{
  BYTE    bEventOccurred;
  POINT   sPoint;
}ILITP_STATUS_TOUCH, *PILITP_STATUS_TOUCH;
typedef CROMDATA ILITP_STATUS_TOUCH *PCILITP_STATUS_TOUCH;


/**
  * @}
  */


/** @defgroup dvILI2511_Exported_constants 
  * @{
  */

/**
  * @}
  */

/** @defgroup dvILI2511_Interrupt_Service_Routine
  * @{
  */

/**
  * @}
  */


/** @defgroup dvILI2511_Exported_Functions
  * @{
  */

/**
 *  @brief      This API must be called once during system startup, before calls are made
                to any other APIs in the Chip Driver component.
 *  @param      None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvILI2511Initialize(void);

/**
 *  @brief      This API Release the Device
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvILI2511Release(void);

/**
 *  @brief       This API Process the Device
 *  @param[in]   None
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None
 */
void dvILI2511Process(void);

/**
 *  @brief      This API Set the Touch IC HW Reset
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvILI2511HWReset(void);

/**
 *  @brief      This API Read the Protocol Version
 *  @param[in]  None
 *  @param[out] psProtocolInfo : Pointer to return back Protocol Version
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed,  
 *  @note       None
 *  @attention  None
 */
eRESULT dvILI2511ProtocolVersionRead(PILITP_PROTOCOL_INFO psProtocolInfo);

/**
 *  @brief      This API Read the Fimware Version
 *  @param[in]  None
 *  @param[out] psFWInfo : Pointer to return back Firmware Version
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed,  
 *  @note       None
 *  @attention  None
 */
eRESULT dvILI2511FWVersionRead(PILITP_FW_INFO psFWInfo);

/**
 *  @brief      This API Read the Panel Information
 *  @param[in]  None
 *  @param[out] psPanelInfo : Pointer to return back Panel Information
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed,  
 *  @note       None
 *  @attention  None
 */
eRESULT dvILI2511PanelInfoRead(PILITP_PANEL_INFO psPanelInfo);

/**
 *  @brief      This API Read the Touch Information (Protocol V3.x.x only)
 *  @param[in]  None
 *  @param[out] psTouchStatus : Pointer to return back Touch Information
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed,  
 *  @note       None
 *  @attention  None
 */
eRESULT dvILI2511TouchInfoRead(PILITP_TOUCH_INFO psTouchStatus);

/**
 *  @brief      This API Set the Sleep Mode
 *  @param[in]  bSleep: TRUE to Sleep, FALSE to Wake up
 *  @param[out] None
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed,  
 *  @note       None
 *  @attention  None
 */
eRESULT dvILI2511SleepModeSet(BOOL bSleep);

/**
 *  @brief        This API perform Reads a Touch Point
 *  @param[in]    psPoint: Pointer to read point return back
 *  @param[out]   None
 *  @return       rcSUCCESS if Event occurred and the operation was successful
                  rcERROR if Event is not occurred or communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
eRESULT dvILI2511TouchRead(PPOINT psPoint);

/**
 *  @brief      This API return the Device Touch Status
 *  @param      None
 *  @return     Device Status
 *  @note       None
 *  @attention  None
 */
ILITP_STATUS_TOUCH dvILITPStatusTouchGet(void);


/**
 *  @brief      This API Enables nINT Interrupt
 *  @param[in]  bEnable: TRUE to Enable, FALSE to Disable
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvILITPInterruptEnable(BOOL bEnable);

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */
#endif // #if USE_TOUCH_ILI2511  

#endif //#if !defined(_INCLUDED_DVILI2511API_H_)  

/******************* (C) COPYRIGHT 1997-2019 (c) MadeFactory Inc. *****END OF FILE****/  
