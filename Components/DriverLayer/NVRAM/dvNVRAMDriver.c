/**
  ******************************************************************************
  * @file    dvNVRAMDriver.c
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

/* Includes ------------------------------------------------------------------*/
#include "dvDeviceBusAPI.h"
#include "dvNVRAMDriverAPI.h"
#include "utilExecutiveAPI.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS **************
// ***************************************************************************
// This file does not contain any non-configurable definitions.

// ***************************************************************************
// ****************** START OF PRIVATE CONFIGURATION SECTION *****************
// ***************************************************************************
/* --------------- BEGIN CUSTOM AREA [010] dvNVRAMDriver.c -------------------
 * CONFIGURATION: Configurable default value definitions */

//--------------------------------CONFIGURATION-------------------------------
// Identifier of I2C bus used to communicate with NVRAM device
//--------------------------------CONFIGURATION-------------------------------
#define NVRAM_BUS_ID                  cpuTWI_SW_0
#define NV_2WIRE_DEVICE_ADDR          0x50

//--------------------------------CONFIGURATION-------------------------------
// Select NVRAM device hardware type
//--------------------------------CONFIGURATION-------------------------------
//#define _DEF_NV24LC32A
//#define _DEF_NV24LC64

//--------------------------------CONFIGURATION-------------------------------
// parameters for each supported NVRAM device type
//--------------------------------CONFIGURATION-------------------------------
#if USE_NVRAM_TYPE == 1
  #if defined(_DEF_NV24LC32A)  
  #define NV_ADDRESS_FIELD_SIZE     16
  #define NV_BLOCK_SIZE             4096
  #define NV_BLOCK_COUNT            1
  #define NV_PAGE_WRITE_LEN         32
  #define NV_WRITE_RETRY		        50
  #define NV_READ_RETRY		          50
  #define NV_FLAG_WRITE				(DEVICE_BUS_ADDR_TRANSACTION | DEVICE_BUS_WORD_ADDR | DEVICE_BUS_FORCE_SW)
  #define NV_FLAG_READ				(DEVICE_BUS_SEND_REPEAT_AFTER_ADDR | DEVICE_BUS_ADDR_TRANSACTION | DEVICE_BUS_WORD_ADDR | DEVICE_BUS_FORCE_SW)
  #define NV_WRITE_DELAY            0
  #elif defined(_DEF_NV24LC64)
  #define NV_ADDRESS_FIELD_SIZE     16
  #define NV_BLOCK_SIZE             8192
  #define NV_BLOCK_COUNT            1
  #define NV_PAGE_WRITE_LEN         32
  #define NV_WRITE_RETRY		        50
  #define NV_READ_RETRY		          50
  #define NV_FLAG_WRITE				(DEVICE_BUS_ADDR_TRANSACTION | DEVICE_BUS_WORD_ADDR | DEVICE_BUS_FORCE_SW)
  #define NV_FLAG_READ				(DEVICE_BUS_SEND_REPEAT_AFTER_ADDR | DEVICE_BUS_ADDR_TRANSACTION | DEVICE_BUS_WORD_ADDR | DEVICE_BUS_FORCE_SW)
  #define NV_WRITE_DELAY            0
  #elif defined(_DEF_NV24LC04)
  #define NV_ADDRESS_FIELD_SIZE     8
  #define NV_BLOCK_SIZE             256
  #define NV_BLOCK_COUNT            2
  #define NV_PAGE_WRITE_LEN         16
  #define NV_WRITE_DELAY            10000
  #define NV_WRITE_RETRY		        50
  #define NV_READ_RETRY		          50  
  #define NV_FLAG_WRITE				      (DEVICE_BUS_ADDR_TRANSACTION | DEVICE_BUS_FAST_MODE)
  #define NV_FLAG_READ				      (DEVICE_BUS_SEND_REPEAT_AFTER_ADDR | DEVICE_BUS_ADDR_TRANSACTION | DEVICE_BUS_FAST_MODE)
  #elif defined(_DEF_NV24C02)
  #define NV_ADDRESS_FIELD_SIZE     8
  #define NV_BLOCK_SIZE             256
  #define NV_BLOCK_COUNT            1
  #define NV_PAGE_WRITE_LEN         16
  #define NV_WRITE_RETRY		        50
  #define NV_READ_RETRY		          50
  #define NV_FLAG_WRITE				(DEVICE_BUS_ADDR_TRANSACTION | DEVICE_BUS_WORD_ADDR | DEVICE_BUS_FORCE_SW)
  #define NV_FLAG_READ				(DEVICE_BUS_SEND_REPEAT_AFTER_ADDR | DEVICE_BUS_ADDR_TRANSACTION | DEVICE_BUS_WORD_ADDR | DEVICE_BUS_FORCE_SW)
  #define NV_WRITE_DELAY            0
  #else
  #error "NEED TO DEFINE NVRAM"
  #endif
#endif  
/* ---------------------------- END CUSTOM AREA --------------------------- */

// ***************************************************************************
// ******************* END OF PRIVATE CONFIGURATION SECTION ******************
// ***************************************************************************

// ***************************************************************************
// ******************** START OF PRIVATE DATA DECLARATIONS *******************
// ***************************************************************************
// This file does not contain any private data declarations.

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************
// This file does not contain any private procedure implementations.

// ***************************************************************************
// **************** START OF PUBLIC PROCEDURE IMPLEMENTATIONS ****************
// ***************************************************************************

/**
 *  @brief      This API initializes the NVRAM device driver.
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvNVRAMInitialize(void)
{
  return;
}

/**
 *  @brief      This API Release the NVRAM device driver.
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvNVRAMRelease(void)
{
  return;
}


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
                          PCBYTE pcData)
{
  eRESULT             eResult = rcINVALID;
#if USE_NVRAM_TYPE == 1
  BYTE                cRetries;
  WORD                wCurrentAddress, wEnd;
  WORD                wStartPage, wEndPage, wMidPages;
  WORD                wNextPageAddr, wPagePremainder, wPageRemainder;
  INT                 i;

  ASSERT(pcData != NULL);

  // Notes:
  // This routine deals with NVRAM on a page basis, which for 32Kb EEPROMs
  // is every 32 bytes starting at address 0.  Up to the page size may be
  // written to the device in one transaction, but only if the starting
  // address is on a page boundary.  If this is not the case, then the
  // internal address counter overflows and the bytes that go beyond the
  // page boundary are written starting from the beginning of the same page, 
  // effectively corrupting the data the used to be there.
  //
  // All transfers are broken up into three major groupings:
  //
  // Premainder: the number of bytes, if any, that exist in a transfer
  // before the next page boundary.  Mid Pages: the number of whole pages,
  // if any, that exist in a transfer following the Premainder and
  // preceeding the Remainder.  The Mid Pages are inherently page aligned.
  // Remainder: the number of bytes, if any, that follow the Mid Pages but
  // do not consume a whole page.  The Remainder is inherently page aligned.
  //
  // Since 32Kb EEPROMs use 12-bit addressing, there are no restrictions
  // such as block sizes that make dealing with smaller devices more
  // tedious.

  // Precalculate all our data used in the write loops that follow.
  wCurrentAddress = wStart;
  wEnd = wStart + wSize - 1;         // end address of this transfer in
                                     // NVRAM
  wStartPage = wStart / NV_PAGE_WRITE_LEN;    // starting block number for
                                              // transfer
  wEndPage = wEnd / NV_PAGE_WRITE_LEN;    // ending block number for
                                          // transfer

  // If this operation is entirely within one NVRAM page, then setup for
  // that situation.
  if (wStartPage == wEndPage)
  {
    wPagePremainder = wSize;       // this transfer is handled entirely as 
                                   // premainder
    wMidPages = 0;                 // no chunks in the middle
    wPageRemainder = 0;            // no remainder bytes
  }
  else
  {
    // We're not entirely within one page
    // Calculate the address of the next page boundary.
    wNextPageAddr = ((wStart / NV_PAGE_WRITE_LEN) + 1) * NV_PAGE_WRITE_LEN;

    // Premainder is the number of bytes before the first page boundary
    wPagePremainder = wNextPageAddr - wStart;

    // Check if any bytes follow the last page boundary in the transfer
    if ((wEnd + 1) % NV_PAGE_WRITE_LEN)
    {
      // Remainder is the number of bytes after the last page boundary
      wPageRemainder = wEnd - ((wEnd /
                                NV_PAGE_WRITE_LEN) *
                               NV_PAGE_WRITE_LEN) + 1;
    }
    else
    {
      // Tranfer ends at a page boundary so no remainder
      wPageRemainder = 0;
    }
  }

  // Calculate number of whole pages are in the middle of this transfer
  wMidPages = (wSize - wPagePremainder - wPageRemainder) / NV_PAGE_WRITE_LEN;

  // -----------------------------------------------------------------------
  // BEGIN ACCESSING THE NVRAM
  // -----------------------------------------------------------------------

  // Write out the Premainder
  if (wPagePremainder)
  {
    // Attempt to write up to the configured number of read retries
    for (cRetries = 0; (rcSUCCESS != eResult) &&
         (cRetries < NV_WRITE_RETRY); cRetries++)
    {

      // Report failure only after max retries have been exeeded
      eResult = dvDeviceBusWrite(NVRAM_BUS_ID,
                                 NV_2WIRE_DEVICE_ADDR,
                                 wCurrentAddress,
                                 wPagePremainder,
                                 pcData,
                                 NV_FLAG_WRITE);
      if (rcSUCCESS != eResult)
      {
        //utilExecSleep(uS_TO_TICKS(NV_WRITE_DELAY));
        commDelayUS(NV_WRITE_DELAY);
      }
    }
    wCurrentAddress += wPagePremainder;
    pcData += wPagePremainder;
    
    /*pcData =
        utilMemoryLinToSegConv(utilMemorySegToLinConv(pcData) +
                               wPagePremainder);*/
  }

  // Write out the Mid Pages
  for (i = 0; i < wMidPages; i++)
  {
      // Attempt to write up to the configured number of read retries
      eResult = rcINVALID;
      for (cRetries = 0; (rcSUCCESS != eResult) &&
           (cRetries < NV_WRITE_RETRY); cRetries++)
      {

        // Report failure only after max retries have been exeeded
        eResult = dvDeviceBusWrite(NVRAM_BUS_ID,
                                   NV_2WIRE_DEVICE_ADDR,
                                   wCurrentAddress,
                                   NV_PAGE_WRITE_LEN,
                                   pcData,
                                   NV_FLAG_WRITE);
        if (rcSUCCESS != eResult)
        {
          //utilExecSleep(uS_TO_TICKS(NV_WRITE_DELAY));
          commDelayUS(NV_WRITE_DELAY);
        }

    }
    wCurrentAddress += NV_PAGE_WRITE_LEN;
    pcData += NV_PAGE_WRITE_LEN;
    /*pcData =
        utilMemoryLinToSegConv(utilMemorySegToLinConv(pcData) +
                               NV_PAGE_WRITE_LEN);*/
  }

  // Write out the Remainder
  if (wPageRemainder)
  {
    // Attempt to write up to the configured number of read retries
    eResult = rcINVALID;
    for (cRetries = 0; (rcSUCCESS != eResult) &&
         (cRetries < NV_WRITE_RETRY); cRetries++)
    {

      // Report failure only after max retries have been exeeded
      eResult = dvDeviceBusWrite(NVRAM_BUS_ID,
                                 NV_2WIRE_DEVICE_ADDR,
                                 wCurrentAddress,
                                 wPageRemainder,
                                 pcData,
                                 NV_FLAG_WRITE);
      if (rcSUCCESS != eResult)
      {    
        //utilExecSleep(uS_TO_TICKS(NV_WRITE_DELAY));
        commDelayUS(NV_WRITE_DELAY);
      }
    }

  }
#endif //#if USE_NVRAM_TYPE == 1

  return eResult;
}

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
                         PBYTE pcData)
{
  eRESULT             eResult = rcINVALID;
#if USE_NVRAM_TYPE == 1  
  BYTE                cRetries;

  ASSERT(pcData != NULL);
  ASSERT((wStart+wSize)<dvNVRAMSizeGet())

#if 1
  // Attempt to read up to the configured number of read retries
  for (cRetries = 0; (rcSUCCESS != eResult) &&
                                    (cRetries < NV_READ_RETRY); cRetries++)
  {

    // Report failure only after max retries have been exeeded
    eResult = dvDeviceBusRead(NVRAM_BUS_ID,
                                                NV_2WIRE_DEVICE_ADDR,
                                                wStart,
                                                wSize,
                                                pcData,
                                                NV_FLAG_READ);
    if (rcSUCCESS != eResult)
    {
      //utilExecSleep(uS_TO_TICKS(NV_WRITE_DELAY));
      commDelayUS(NV_WRITE_DELAY);
    }
  }
#else // for NVRAM Address Test
  for(cRetries = 0x50; (rcSUCCESS != eResult) &&
                                    (cRetries < 0x58); cRetries++)
  {
        eResult = dvDeviceBusRead(NVRAM_BUS_ID,
                                                cRetries,
                                                wStart,
                                                wSize,
                                                pcData,
                                                NV_FLAG_READ);

  }
#endif

#endif // #if USE_NVRAM_TYPE == 1

  return eResult;
}

/**
 *  @brief      This API function returns the defined NVRAMSIZE.
 *                 beginning at the specified address.
 *  @param[in]  None
 *  @param[out]  None
 *  @return     Size of NVRAM, in bytes
 *  @note       None
 *  @attention  None
 */
WORD dvNVRAMSizeGet(void)
{
#if USE_NVRAM_TYPE == 1
  return (WORD)(NV_BLOCK_SIZE * NV_BLOCK_COUNT);
#else
  return 0;
#endif
}

