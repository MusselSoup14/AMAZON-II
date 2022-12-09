/**
  ******************************************************************************
  * @file    dvNVRAMDriverAPI.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    04-August-2014
  * @brief   This file provides APIs the NVRAM Driver
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

#ifndef _DVNVRAMDRIVERAPI_H_
#define _DVNVRAMDRIVERAPI_H_

/* Includes ------------------------------------------------------------------*/
#include "common.h"

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *  @brief      This API initializes the NVRAM device driver.
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvNVRAMInitialize(void);

/**
 *  @brief      This API Release the NVRAM device driver.
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvNVRAMRelease(void);

/**
 *  @brief      This API function writes a block of sequential bytes to the NVRAM device
 *                 beginning at a specified address.
 *  @param[in]  Beginning byte address for block write
 *  @param[in]  Block size to write in bytes; 0 < wNumBytes <= NVRAMSIZE
 *  @param[in]  Pointer to array of data to write
 *  @param[out]  None
 *  @return     rcSUCCESS if writing was successful else rcERROR if error occurred.
 *  @note       None
 *  @attention  None
 */
eRESULT dvNVRAMBytesWrite(WORD   wStart,
                          WORD   wSize,
                          PCBYTE pcData);

/**
 *  @brief      This API function reads a number of sequential bytes from the NVRAM device
 *                 beginning at the specified address.
 *  @param[in]  wStart : Beginning byte address for block read
 *  @param[in]  wSize  : Block size to read in bytes; 0 < wNumBytes <= NVRAMSIZE
 *  @param[in]  pcData : Pointer to array for return data
 *  @param[out]  None
 *  @return     rcSUCCESS if writing was successful else rcERROR if error occurred.
 *  @note       None
 *  @attention  None
 */ 
eRESULT dvNVRAMBytesRead(WORD  wStart,
                         WORD  wSize,
                         PBYTE pcData);

/**
 *  @brief      This API function returns the defined NVRAMSIZE.
 *                 beginning at the specified address.
 *  @param[in]  None
 *  @param[out]  None
 *  @return     Size of NVRAM, in bytes
 *  @note       None
 *  @attention  None
 */
WORD dvNVRAMSizeGet(void);

#endif // _DVNVRAMDRIVERAPI_H_

