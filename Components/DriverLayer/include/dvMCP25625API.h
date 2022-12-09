/**
  ******************************************************************************
  * @file    dvMCP25625API.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    27-03-2019
  * @brief   This file contains all the functions prototypes for the CAN Controller
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
#if !defined(_INCLUDED_DVMCP25625API_H_)
#define _INCLUDED_DVMCP25625API_H_

 /* Includes ------------------------------------------------------------------*/
#include "common.h"

#if USE_CAN_BUS
/** @addtogroup CWM-T10
  * @{
  */


/** @defgroup dvMCP25625 
  * @brief MCP25625 Driver
  * @{
  */

/** @defgroup dvMCP25625_Exported_Types
  * @{
  */

// ---------------------------------------------------------------------------
// CAN SPI exported Object definitions
// ---------------------------------------------------------------------------
//! Number of TX Channels
typedef enum 
{
  canspiTX_CH0,
  canspiTX_CH1,
  canspiTX_CH2,
  
  canspiTX_INVALID
}eCANSPI_TX_CHANNEL;

//! Number of RX Channels
typedef enum 
{
  canspiRX_CH0 = 0,
  canspiRX_CH1,

  canspiRX_INVALID
}eCANSPI_RX_CHANNEL;

//! Number of Filters
typedef enum 
{
  canspiFILTER_0,
  canspiFILTER_1,
  canspiFILTER_2,
  canspiFILTER_3,
  canspiFILTER_4,
  canspiFILTER_5,
  
  canspiFILTER_INVALID
}eCANSPI_FILTER;

//! Number of Masks
typedef enum 
{
  canspiMASK_0,
  canspiMASK_1,
  
  canspiMASK_INVALID
}eCANSPI_MASK;

//! Operation Modes */
typedef enum 
{
  canspiMODE_NORMAL,
  canspiMODE_SLEEP,
  canspiMODE_LOOPBACK,
  canspiMODE_LISTEN_ONLY,
  canspiMODE_CONFIGURATION,
  
  canspiMODE_INVALID // 0xFF
}eCANSPI_OPERATION_MODE;


//! CAN Filter Object ID
typedef struct tagCANSPI_FILTEROBJ_ID 
{
  WORD  SID   : 11; // Filter to Standard identifier
  DWORD EID   : 18; // Filter to Extended identifier
  BYTE  EXIDE : 1;  // 0: Standard identifier, 1: Extended identifier
}CANSPI_FILTEROBJ_ID, *PCANSPI_FILTEROBJ_ID;
typedef CROMDATA CANSPI_FILTEROBJ_ID *PCCANSPI_FILTEROBJ_ID;

//! CAN Mask Object ID
typedef struct tagCANSPI_MASKOBJ_ID 
{
  WORD  MSID : 11;    // Mask to Standard identifier
  DWORD MEID : 18;    // Mask to Extended identifier
}CANSPI_MASKOBJ_ID, *PCANSPI_MASKOBJ_ID;
typedef CROMDATA CANSPI_MASKOBJ_ID *PCCANSPI_MASKOBJ_ID;


// ---------------------------------------------------------------------------
// The following enumeration describes the serial status values that can be
// returned from the dvDeviceBusCANStatus API. 
// ---------------------------------------------------------------------------
typedef enum
{
    canSTATUS_OK,                      // CAN status is ok (no read errors)
    
    canSTATUS_INVALID                  // Must be LAST value. Used for range
                                       // checking (DO NOT REMOVE)
}eCAN_STATUS;

/** 
  * @brief  CAN Tx message structure definition  
  */
typedef struct
{
  UINT StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

  UINT ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */

  BYTE IDE;     /*!< Specifies the type of identifier for the message that 
                        will be transmitted. This parameter can be a value 
                        of @ref CAN_identifier_type */

  BYTE RTR;     /*!< Specifies the type of frame for the message that will 
                        be transmitted. This parameter can be a value of 
                        @ref CAN_remote_transmission_request */

  BYTE DLC;     /*!< Specifies the length of the frame that will be 
                        transmitted. This parameter can be a value between 
                        0 to 8 */

  BYTE Data[8]; /*!< Contains the data to be transmitted. It ranges from 0 
                        to 0xFF. */
} CAN_TX_MSG, *PCAN_TX_MSG;
typedef CROMDATA CAN_TX_MSG *PCCAN_TX_MSG;                    

/** 
  * @brief  CAN Rx message structure definition  
  */
typedef struct
{
  UINT StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

  UINT ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */

  BYTE IDE;     /*!< Specifies the type of identifier for the message that 
                        will be received. This parameter can be a value of 
                        @ref CAN_identifier_type */

  BYTE RTR;     /*!< Specifies the type of frame for the received message.
                        This parameter can be a value of 
                        @ref CAN_remote_transmission_request */

  BYTE DLC;     /*!< Specifies the length of the frame that will be received.
                        This parameter can be a value between 0 to 8 */

  BYTE Data[8]; /*!< Contains the data to be received. It ranges from 0 to 
                        0xFF. */
} CAN_RX_MSG, *PCAN_RX_MSG;
typedef CROMDATA CAN_RX_MSG *PCCAN_RX_MSG;                        


/* type to CAN Frame */
typedef CAN_RX_MSG CAN_FRAME_RX, *PCAN_FRAME_RX;
typedef CROMDATA CAN_FRAME_RX *PCCAN_FRAME_RX;

/* structure to CAN frame Ring Queue */
typedef struct tagCANMSGQUERX
{
  WORD          wHeader;
  WORD          wTail;
  WORD          wAvailFrameRx;     // number of available Received Message. it decresed when dvDeviceBusCANRead()
  PCAN_FRAME_RX pasFrameRx; // Received Frame buffer. must be array pointer
}CAN_FRAME_QUE_RX, *PCAN_FRAME_QUE_RX;
typedef CROMDATA CAN_FRAME_QUE_RX *PCCAN_FRAME_QUE_RX;


/**
  * @}
  */


/** @defgroup dvMCP25625_Exported_constants 
  * @{
  */
/* Device Instance Count */
#define MCP25625_NUM_INSTANCES      1

/* Max CAN Buffer Size */
#define BUS_CAN_BUFFER_SIZE    8

#define CAN_Id_Standard             (0)  /*!< Standard Id */
#define CAN_Id_Extended             (1)  /*!< Extended Id */
#define IS_CAN_IDTYPE(IDTYPE) (((IDTYPE) == CAN_Id_Standard) || \
                               ((IDTYPE) == CAN_Id_Extended))

/* Legacy defines */
#define CAN_ID_STD      CAN_Id_Standard           
#define CAN_ID_EXT      CAN_Id_Extended

#define CAN_RTR_Data                (0)  /*!< Data frame */
#define CAN_RTR_Remote              (1)  /*!< Remote frame */
#define IS_CAN_RTR(RTR) (((RTR) == CAN_RTR_Data) || ((RTR) == CAN_RTR_Remote))

/* Legacy defines */
#define CAN_RTR_DATA     CAN_RTR_Data         
#define CAN_RTR_REMOTE   CAN_RTR_Remote


// ---------------------------------------------------------------------------
// Bitfield flags utilized within the wFlags parameter of dvDeviceBusCANRead()
// and dvDeviceBusCANWrite()
// ---------------------------------------------------------------------------
// Flags the ID is Extended(29bits, CAN2.0B) or Standard(11bits, CAN2.0A)
#define BUS_CAN_20B       0x0001
// Flags the Packet is the REMOTE REQUEST
#define BUS_CAN_RTR       0x0002 




/**
  * @}
  */

/** @defgroup dvMCP25625_Interrupt_Service_Routine
  * @{
  */

/**
  * @}
  */


/** @defgroup dvMCP25625_Exported_Functions
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
void dvDeviceBusCANInitialize(void);

/**
 *  @brief      This API Release the Device
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvDeviceBusCANRelease(void);

/**
 *  @brief       This API reads data from the CAN Bus
 *  @param[in]   wCount     : Number of bytes to Read
                 pasCanFrame  : Pointer to an array of at least wCount frame to receive data into
 *  @param[out]  None
 *  @return      Count of Read frame
 *  @note        None
 *  @attention   None 
 */  
WORD dvCANSPIReadFrame(WORD   wCount, PCAN_FRAME_RX pasCanFrame);

/**
 *  @brief      This API Configures the CAN Bus Filter
 *  @param      eFilter: Filter Id to Set to
                sFilterObj: CAN Filter to Set
 *  @return     rcSUCCESS = No error occurred and returned result is complete
                rcERROR   = An error occurred during the operation 
 *  @note       None
 *  @attention  None
 */
eRESULT dvDeviceBusCANFilterSet(eCANSPI_FILTER eFilter, CANSPI_FILTEROBJ_ID sFilterObj);

/**
 *  @brief      This API return the CAN Bus Filter settings as specified Filter Id
 *  @param      eFilter: Filter Id to Return Back
 *  @return     CAN filter Setting
 *  @note       None
 *  @attention  None
 */
CANSPI_FILTEROBJ_ID dvDeviceBusCANFilterGet(eCANSPI_FILTER eFilter);

/**
 *  @brief      This API Configures the CAN Bus Mask
 *  @param      eMask: Mask Id to Set to
                sMaskObj: CAN Mask to Set
 *  @return     rcSUCCESS = No error occurred and returned result is complete
                rcERROR   = An error occurred during the operation 
 *  @note       None
 *  @attention  None
 */
eRESULT dvDeviceBusCANMaskSet(eCANSPI_MASK eMask, CANSPI_MASKOBJ_ID sMaskObj);

/**
 *  @brief      This API return the CAN Bus Filter settings as specified Filter Id
 *  @param      eFilter: Filter Id to Return Back
 *  @return     CAN filter Setting
 *  @note       None
 *  @attention  None
 */
CANSPI_MASKOBJ_ID dvDeviceBusCANMaskGet(eCANSPI_MASK eMask);

/**
 *  @brief      This API Enables or Disable the specified CAN Interrupt
 *  @param[in]  bEnable : TRUE for Enable Interrupt, FALSE for Disable
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvDeviceBusCANInterruptEnable(BOOL bEnable);

/**
 *  @brief       This API reads data from the CAN Bus.
                 wCount     : Number of message to available for storage in the pacBuffer parameter
                 psQueRx : Pointer to CAN Rx Que   
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None 
 */  
void dvDeviceBusCANReadSet(WORD  wCount, PCAN_FRAME_QUE_RX psQueRx);

/**
 *  @brief       This API reads data from the CAN Bus
 *  @param[in]   wCount     : Number of bytes to Read
                 pasCanFrame  : Pointer to an array of at least wCount frame to receive data into
 *  @param[out]  None
 *  @return      Count of Read frame
 *  @note        None
 *  @attention   None 
 */  
WORD dvDeviceBusCANRead(WORD wCount, PCAN_FRAME_RX pasCanFrame);

/**
 *  @brief       This API Transmit the buffer via CAN Bus
 *  @param[in]   dwMsgId: CAN Message Id to write
                 cCount: Number of bytes to transmit. Can not be exceed 8
                 pacBuffer: Pointer to an array of at least wCount bytes to transmit
                 wFlags: Flags for the CAN transaction (see Definitions)
 *  @param[out]  None
 *  @return      rcSUCCESS= No error occurred and returned result is complete
                 rcERROR= An error occurred during the operation
 *  @note        cCount must be lower than 8bytes
 *  @attention   None 
 */  
eRESULT dvDeviceBusCANWrite(DWORD dwMsgId, BYTE cCount, PCBYTE pacBuffer, WORD wFlags);

/**
 *  @brief       This API acquires the current status of the CAN Bus hardware.
 *  @param[in]   None
 *  @param[out]  pwCountRead  : Number of message read since the last call to dvDeviceBusCANRead() 
                 pwCountAvail : Number of message read available from Queue
 *  @return      Current status of serial interface (ok, read overflow, read error, or read overflow and error)
 *  @note        None
 *  @attention   None 
 */  
eCAN_STATUS dvDeviceBusCANStatus(PWORD pwCountRead, PWORD pwCountAvail);

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

#endif // #if USE_CAN_BUS

#endif //#if !defined(_INCLUDED_DVMCP25625API_H_)  

/******************* (C) COPYRIGHT 1997-2019 (c) MadeFactory Inc. *****END OF FILE****/  
