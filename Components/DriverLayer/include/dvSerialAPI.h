/**
  ******************************************************************************
  * @file    dvSerialAPI.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    07-Feb-2017
  * @brief   This file contains all the functions prototypes for the CPU Driver
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
  * <h2><center>&copy; Copyright 1997-2017 (c) MadeFactory Inc.</center></h2>
  ******************************************************************************
  */  
  
/* Define to prevent recursive inclusion -------------------------------------*/
#if !defined(_INCLUDED_DVSERIALAPI_H_)
#define _INCLUDED_DVSERIALAPI_H_

 /* Includes ------------------------------------------------------------------*/
#include "common.h"

/** @addtogroup CWM-T10
  * @{
  */


/** @defgroup dvUART 
  * @brief CPU Serial Driver
  * @{
  */


/** @defgroup dvSerial_Exported_Types
  * @{
  */
#define SERIAL_RX_QUE_BUFF_SIZE 2054
//#define SERIAL_TX_QUE_BUFF_MAX 256


// ---------------------------------------------------------------------------
// The following enumeration describes the serial status values that can be
// returned from the dvSerialStatus API. 
// ---------------------------------------------------------------------------
typedef enum
{
  ssOK,                              // Serial status is ok (no read errors)
  ssREAD_OVERFLOW,                   // The serial read FIFO has overflowed
  ssREAD_ERROR,                      // A serial read framing error has
                                     // occured
  ssREAD_OVERFLOW_ERROR,             // Both the read FIFO has overflowed
                                     // and a read framing error has occured
  ssINVALID                          // Must be LAST value. Used for range
                                     // checking (DO NOT REMOVE)
} eSERIAL_STATUS;

/** 
  * @brief  The following enumeration contains the list of possible hardware timers,
                  used for interfacing with the CPU hardware PWM APIs.
  */
typedef enum
{
  cpuUART_SETUP,      /* I/F to Maintenance */
  cpuUART_GUI,        /* I/F to GUI */
  cpuUART_TOUCH,      /* I/F to Touch IC */
  
  cpuUART_INVALID     /* SENTINEL (DO NOT REMOVE) */
} eCPU_SERIAL;
#define SERIAL_NUM_INSTANCES cpuUART_INVALID

// structure to Ring Queue
typedef struct tagSERIALQUE
{
  WORD wHeader;
  WORD wTail;
  WORD wAvailableRxBytes;
  PBYTE pacBuffer;
}SERIAL_QUE, *PSERIAL_QUE;
typedef CROMDATA SERIAL_QUE *PCSERIAL_QUE;

/**
  * @}
  */


/** @defgroup dvSerial_Exported_constants 
  * @{
  */

/**
  * @}
  */

/** @defgroup dvSerial_Interrupt_Service_Routine
  * @{
  */

/**
  * @}
  */


/** @defgroup dvSerial_Exported_Functions
  * @{
  */

/**
 *  @brief      This API must be called once during system startup, before calls are made
 to any other APIs in the Chip Driver component.
 *  @param  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvSerialInitialize(void);

/**
 *  @brief              This API Release the CPU UART Device
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvSerialRelease(void);

/**
 *  @brief      This API Configures the Vector Table base location.
 *  @param  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvSerialNVICInitialize(void);

/**
 *  @brief      This API Enables or Disable the specified USART Interrupt
 *  @param[in]  eSerialId : CPU USART Device Id
 *  @param[in]  bEnable : TRUE for Enable Interrupt, FALSE for Disable
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvSerialInterruptEnable(eCPU_SERIAL eSerialId, BOOL bEnable);

/**
 *  @brief            This API reads data from the RS-232 Serial Port.
 *  @param[in]    eSerialId : CPU USART Device Id
                            wCount     : Number of bytes to available for storage in the pacBuffer parameter
                            wThreshold : Number of bytes to receive before signaling the protocol
                                                task.  Must be less than or equal to amount of bytes
                                                specified in the wCount parameter.
                             psQue : Pointer to Serial Que   
 *  @param[out]  None
 *  @return         None
 *  @note           None
 *  @attention     None 
 */  
void dvSerialReadSet(eCPU_SERIAL eSerialId
                                            , WORD  wCount
                                            , WORD  wThreshold
                                            , PSERIAL_QUE psQue);

/**
 *  @brief            This API reads data from the RS-232 Serial Port.
 *  @param[in]    eSerialId : CPU USART Device Id
                            wCount     : Number of bytes to Read
                            pacBuffer  : Pointer to an array of at least wCount bytes to receive data into
 *  @param[out]  None
 *  @return         None
 *  @note           None
 *  @attention     None 
 */  
WORD dvSerialRead(eCPU_SERIAL eSerialId
                                            , WORD  wCount
                                            , PBYTE pacBuffer);

/**
 *  @brief            This API Transmit the buffer via specified UART instance
 *  @param[in]    eSerialId : CPU USART Device Id
                            wCount: Number of bytes to transmit
                            pacBuffer: Pointer to an array of at least wCount bytes to transmit
 *  @param[out]  None
 *  @return         None
 *  @note           None
 *  @attention     None 
 */  
void dvSerialWrite(eCPU_SERIAL eSerialId
                                                    , WORD wCount
                                                    , PCBYTE pacBuffer);

/**
 *  @brief            This API changes the threshold for signaling the protocol task when
                            receiving bytes on the Serial Port.
 *  @param[in]    eSerialId : CPU USART Device Id
                            wThreshold : Number of bytes to receive before signaling the protocol
                                                 task.  Must be less than or equal to amount of bytes
                                                 specified in the wCount parameter of the last call to
                                                 dvSerialRead()
                                                 
 *  @param[out]  None
 *  @return         None
 *  @note           None
 *  @attention     None 
 */ 
void dvSerialSetThreshold(eCPU_SERIAL eSerialId, WORD wThreshold);

/**
 *  @brief            This API acquires the current status of the Serial Port hardware.
 *  @param[in]    eSerialId : CPU USART Device Id
 *  @param[out]  pwBytesRead    : Number of bytes read since the last call to dvSerialRead() 
                             pwBytesAvail : Number of bytes read available from Queue
 *  @return         Current status of serial interface (ok, read overflow, read error, or read overflow and error)
 *  @note           None
 *  @attention     None 
 */  
eSERIAL_STATUS dvSerialStatus(eCPU_SERIAL eSerialId
                                                                  , PWORD pwBytesRead
                                                                  , PWORD pwBytesAvail);


/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */
#endif //#if !defined(_INCLUDED_DVSERIALAPI_H_)  

/******************* (C) COPYRIGHT 1997-2017 (c) MadeFactory Inc. *****END OF FILE****/  
