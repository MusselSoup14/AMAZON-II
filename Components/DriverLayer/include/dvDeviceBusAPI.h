/**
  ******************************************************************************
  * @file    dvDeviceBusAPI.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    03-August-2014
  * @brief   This file provides the APIs to other components
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
#include "common.h"
#include "dvCPUAPI.h"

#if !defined(_INCLUDED_DVDEVICEBUSAPI_H_)
#define _INCLUDED_DVDEVICEBUSAPI_H_

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
// ---------------------------------------------------------------------------
// The following enumeration contains the list of possible hardware/software I2C Bus
// used for interfacing with the CPU DeviceBus APIs.
// ---------------------------------------------------------------------------
typedef enum
{
  cpuTWI_HW,
  cpuTWI_SW_0,
  
  cpuTWI_INVALID, // Sentinel
}eCPU_TWI;

#define TWI_NUM_INSTANCES cpuTWI_INVALID

// ---------------------------------------------------------------------------
// The following structures are used when utilizing the read and write
// register functions. 
// ---------------------------------------------------------------------------
typedef struct tagREGISTER
{
    BYTE                cRegister;     // Register to access
    BYTE                cData;         // Data to set register to or storage
                                       // for read value.
} DEVICE_BUS_REGISTER, *PDEVICE_BUS_REGISTER;
typedef CROMDATA DEVICE_BUS_REGISTER *PCDEVICE_BUS_REGISTER;

// ------------------------------------------------------------------
// Bitfield flags utilized within the wFlags parameter of dvDeviceBusRead
// and dvDeviceBusWrite
// ------------------------------------------------------------------
// Flags the transaction to use wRegisterAddress (not used for
// register APIs).
#define DEVICE_BUS_ADDR_TRANSACTION       0x0001

// Flags the transaction to use all 16 bits of wRegisterAddress (not used for
// register APIs).
#define DEVICE_BUS_WORD_ADDR              0x0002

// Flags the transaction to use all 16 bits of wData (not used for
// register APIs).
#define DEVICE_BUS_WORD_DATA              0x0004

// Sends a stop after a dummy write used to set up a read
// address.  Forces use of software bus.
#define DEVICE_BUS_SEND_STOP_AFTER_ADDR   0x0010

// Flags the transaction to not verify acknowledges are received.
#define DEVICE_BUS_IGNORE_ACK             0x0020

// Forces the transaction to use the software bus
#define DEVICE_BUS_FORCE_SW               0x0040

// Flags the transaction to allow the hardware 2-Wire to operate in fast
// mode.  Ignored for software-based transactions.
#define DEVICE_BUS_FAST_MODE              0x0080

// Flags the transaction to be 5KHz instead of 100. only for software-based
// transactions
#define DEVICE_BUS_SLOW_MODE              0x0100

// Sends a repeat start after a "DEVICE_BUS_ADDR_TRANSACTION" write
#define DEVICE_BUS_SEND_REPEAT_AFTER_ADDR   0x0200

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************
/**
 *  @brief      This API initializes the device bus software and hardware
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
void dvDeviceBusInitialize(void);

/**
 *  @brief      This API must be called once during system Release
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvDeviceBusRelease(void);

/**
 *  @brief      Reads data from a device connected to an external 2-Wire device bus 
 *  @param[in]  cBusId         : Bus ID to execute the read / write on
                wDeviceAddress   : Device address to read / write
                wRegisterAddress : Internal device register address to read / write. 
                     Optional parameter, unused if the transaction is not an
                     addressed transaction.  Only the lower 8 bits are used
                     for addressed transactions unless word address is
                     specified.
                wCount           : Number of bytes to read / write from / to the external device
                wFlags           : Flags for the 2 wire transaction (see Definitions)
 *  @param[out] pacBuffer        : Pointer to an array of bytes for storing read data or data to be written 
 *  @return     rcSUCCESS if operation succeeds, rcERROR otherwise
 *  @note       None
 *  @attention  None
 */
eRESULT dvDeviceBusRead(BYTE cBusId,
                                WORD  wDeviceAddress,
                                WORD  wRegisterAddress,
                                WORD  wCount,
                                PBYTE pacBuffer,
                                WORD  wFlags);

/**
 *  @brief      Writes data from a device connected to an external 2-Wire device bus
 *  @param[in]  cBusId         : Bus ID to execute the read / write on
                wDeviceAddress   : Device address to read / write
                wRegisterAddress : Internal device register address to read / write. 
                     Optional parameter, unused if the transaction is not an
                     addressed transaction.  Only the lower 8 bits are used
                     for addressed transactions unless word address is specified.
                wCount           : Number of bytes to read / write from / to the external device
                wFlags           : Flags for the 2 wire transaction (see Definitions)
 *  @param[out] pacBuffer        : Pointer to an array of bytes for storing read data or data to be written 
 *  @return     rcSUCCESS if operation succeeds, rcERROR otherwise
 *  @note       None
 *  @attention  None
 */
eRESULT dvDeviceBusWrite(BYTE cBusId,
                                 WORD   wDeviceAddress,
                                 WORD   wRegisterAddress,
                                 WORD   wCount,
                                 PCBYTE pacBuffer,
                                 WORD   wFlags);


#endif //#if !defined(_INCLUDED_DVDEVICEBUSAPI_H_)


