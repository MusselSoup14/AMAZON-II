/**
  ******************************************************************************
  * @file    dvDeviceBus3WireAPI.c
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

#if !defined(_INCLUDED_DVDEVICEBUS3WIREAPI_H_)
#define _INCLUDED_DVDEVICEBUS3WIREAPI_H_

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
// ---------------------------------------------------------------------------
// The following enumeration contains the list of possible hardware SPI Bus
// used for interfacing with the CPU DeviceBus APIs.
// ---------------------------------------------------------------------------
typedef enum
{
  cpuSPI1,
 
  cpuSPI_INVALID, // Sentinel
}eCPU_SPI;

#define SPI_NUM_INSTANCES cpuSPI_INVALID

// ---------------------------------------------------------------------------
// Device Bus 3Wire Init Type
// ---------------------------------------------------------------------------
typedef struct
{       
  BYTE  cSPICon; // Not include SPICON_EN
  DWORD dwFreq;  // Unit is Hz
}SPI_INIT_TYPE, *PSPI_INIT_TYPE;

// ---------------------------------------------------------------------------
// Device Bus 3Wire Init Structure
// ---------------------------------------------------------------------------
typedef struct
{
  WORD          wFlags; // Flags to Init
  SPI_INIT_TYPE sInitType; // ST's Init structure
}SPI_INIT_STRUCT, *PSPI_INIT_STRUCT;
typedef CROMDATA SPI_INIT_STRUCT *PCSPI_INIT_STRUCT;

// ------------------------------------------------------------------
// Bitfield flags utilized within the wFlags parameter of 
// dvDeviceBus3WireTransaction
// ------------------------------------------------------------------
// Flags the dummy data to transaction. Set to Send 0x00, Default is 0xFF
#define DEVICE_3WIRE_DUMMY_00       (1 << 0)


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
void dvDeviceBus3WireInitialize(void);

/**
 *  @brief      This API must be called once during system Release
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvDeviceBus3WireRelease(void);

/**
 *  @brief      This API Enable the specified 3wire bus
 *  @param[in]  eSPId: SPI identifier for hardware
                bEnable: TRUE to Enable, FALSE to Disable
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvDeviceBus3WireEnable(eCPU_SPI eSPId, BOOL bEnable);

/**
 *  @brief      Reads and writes individual bits to a hardware three wire bus
 *  @param[in]  eSPId : SPI identifier for hardware
                dwWriteCount: Write Buffer Size. Can be 0
                pacWriteBuffer: Pointer to Write Buffer. Can be NULL
                dwReadCount: Read Buffer Size. Can be 0
                pcsInitStructNew: pointer to SPI Init structure (readonly. can be NULL)
 *  @param[out] pacReadBuffer: Pointer to Read Buffer. Can be NULL
 *  @return     rcSUCCESS if operation succeeds, rcERROR otherwise
 *  @note       None
 *  @attention  None
 */
eRESULT dvDeviceBus3WireTransaction(eCPU_SPI         eSPId
                                                , DWORD           dwWriteCount
                                                , PCBYTE          pacWriteBuffer
                                                , DWORD           dwReadCount
                                                , PBYTE           pacReadBuffer
                                                , PCSPI_INIT_STRUCT pcsInitStructNew);

/**
 *  @brief      Reads and writes One byte to a hardware three wire bus
                eSPId : SPI identifier for hardware
                cWriteData: Write Data
                pcsInitStructNew: pointer to SPI Init structure (readonly. can be NULL)
 *  @return     Read 1Byte Data
 *  @note       None
 *  @attention  None
 */
BYTE dvDeviceBus3WireOneByteSend(eCPU_SPI           eSPId
                                                , BYTE              cWriteData
                                                , PCSPI_INIT_STRUCT pcsInitStructNew);

/**
 *  @brief      Writes One WORD to a hardware three wire bus
                eSPId : SPI identifier for hardware
                wWriteData: Write Data
                pcsInitStructNew: pointer to SPI Init structure (readonly. can be NULL)
 *  @return     rcSUCCESS if operation succeeds, rcERROR otherwise
 *  @note       None
 *  @attention  None
 */
eRESULT dvDeviceBus3WireOneWordTxOnly(eCPU_SPI eSPId
                                                    , WORD wWriteData
                                                    , PCSPI_INIT_STRUCT pcsInitStructNew);


#endif //#if !defined(_INCLUDED_DVDEVICEBUS3WIREAPI_H_)


