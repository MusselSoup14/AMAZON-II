/**
  ******************************************************************************
  * @file    utilCRC.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    24-March-2016
  * @brief   This file provides the CRC procedure header
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

#if !defined(_INCLUDED_UTILCRC_H_)
#define _INCLUDED_UTILCRC_H_

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
// This file does not contain any non-configurable definitions. 

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************
// This file does not contain any public configuration options.

// ***************************************************************************
// ******************** END OF PUBLIC CONFIGURATION SECTION ******************
// ***************************************************************************

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *  @brief      This function calculates the 16-bit CRC of the nCount bytes pointed to by pData.
 *  @param[in]  pData  : Pointer to the data
                        nCount : Number of bytes
 *  @param[out]  None
 *  @return     CRC value
 *  @note       None
 *  @attention  None
 */
WORD utilCRC16Calc(PBYTE pData,
                   INT   nCount);

/**
 *  @brief        This API Make a checksum field
 *  @param[in]    pacBuffer : pointer to the desired host protocol
                            wPayloadSize: Payload Size (not included checksum)
 *  @param[out]   None
 *  @return         Checksum byte
 *  @note           Check Sum of all fields excluding Check Sum itself will be stored as Check Sum.
                         In case of response message (Ack/Nack), Check Sum field will be omitted.
                         Calculation method of Check Sum is :
                         (1) All Bytes of all data will be added.
                         (2) Take least significant one Byte alone out
                         (3) Calculate twos complement of the value taken out
                         (4) Carry-over will be neglected.
 *  @attention     None 
 */  
BYTE utilCRCCheckSumBuild(PCBYTE pacBuffer, WORD wPayloadSize);

/**
 *  @brief       This API Verify a cheksum field
 *  @param[in]   pacBuffer : pointer to the desired host protocol
                           wPayloadSize: Payload Size to calculate (include checksum)
 *  @param[out]  None
 *  @return         TRUE if Checksum verified successfully, FALSE for others
 *  @note           Check Sum of all fields excluding Check Sum itself will be stored as Check Sum.
                         In case of response message (Ack/Nack), Check Sum field will be omitted.
                         Calculation method of Check Sum is :
                         (1) All Bytes of all data will be added.
                         (2) Take least significant one Byte alone out
                         (3) Calculate twos complement of the value taken out
                         (4) Carry-over will be neglected.
 *  @attention     None 
 */  
BOOL utilCRCCheckSumVerify(PCBYTE pacBuffer, WORD wPayloadSize);

#endif //#if !defined(_INCLUDED_UTILCRC_H_)

