/**
  ******************************************************************************
  * @file    halLEDCtrlAPI.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    24-March-2016
  * @brief   This file provides the main procedure
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
#ifndef _HALLEDCTRLAPI_H_
#define _HALLEDCTRLAPI_H_

#include "common.h"
#include "utilStructs.h"

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
/* enum for IO LED Id */
typedef enum
{
  ledRAW_RED,
  ledRAW_GREEN, 
  ledRAW_BLUE,
  ledRAW_MSG,
  
  ledRAW_END, // RAW LED Sentinel only
  
  ledGRP_ALL, // All RAW LEDs
  
  ledINVALID // Sentinel  
}GCC_PACKED eLED_ID;

/* LED Ctrl Flags */
#define LED_FLAG(ID)  (1 << (ID))
#define LED_FLAGS_ALL (0x0F)

// ***************************************************************************
// ******************** END OF PUBLIC CONFIGURATION SECTION ******************
// ***************************************************************************

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *  @brief      This API sets up the internal state of component..
 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLEDCtrlInitialize(void);

/**
 *  @brief      This API Release the internal state of component..
 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLEDCtrlRelease(void);

/**
 *  @brief      This API Process the internal state of component..
 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLEDCtrlProcess(void);

/**
 *  @brief      This API is On/Off status of specified LED
 
 *  @param[in]  eLEDId : LED Id to On or Close
 *  @param[out] None
 *  @return     TRUE if LED is On, FALSE if LED is Off
 *  @note       None
 *  @attention  None
 */
BOOL halLEDCtrlOnOffGet(eLED_ID eLEDId);

/**
 *  @brief        This API is On/Off the specified LED
 
 *  @param[in]    eLEDId : LED Id to On or Close
                  bOn: Value to On/Off
 *  @param[out]   None
 *  @return       None
 *  @note         None
 *  @attention    None
 */
void halLEDCtrlOnOffSet(eLED_ID eLEDId, BOOL bOn);

/**
 *  @brief         This API return the LED control flags 
 *  @param[in]  None
 *  @param[out]  None
 *  @return     complex of Led ctrl flags
 *  @note       None
 *  @attention  None
 */
WORD halLEDCtrlFlagsGet(void);

/**
 *  @brief      This API control the LED GPIOs as specified flags 
 *  @param[in]  wLEDCtrlFlags: complex of Led ctrl flags
 *  @param[out] None
 *  @return     rcSUCCESS= No error occurred and returned result is complete
                rcERROR= An error occurred during the operation
 *  @note       None
 *  @attention  None
 */
eRESULT halLEDCtrlFlagsSet(WORD wLEDCtrlFlags);

#endif // _HALLEDCTRLAPI_H_

