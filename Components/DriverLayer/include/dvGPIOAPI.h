/**
  ******************************************************************************
  * @file    dvGPIOConfig.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    02-August-2014
  * @brief   This file provides basic the GPIO Configuration Header.
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
#include "dvGPIOConfig.h"

#if !defined(_INCLUDED_DVGPIOAPI_H_)
#define _INCLUDED_DVGPIOAPI_H_

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
// Port Open Drain Mode Control Registers
#define R_GPODM(ch) ((volatile unsigned int*)(GPIO_BASE+0x2C+(0x40*(ch))))

// Port Shcmitt Input Enable Registers
#define R_GPSHMT(ch) ((volatile unsigned int*)(GPIO_BASE+0x34+(0x40*(ch))))

// Port Pull-Down Enable Registers
#define R_GPPDEN(ch) ((volatile unsigned int*)(GPIO_BASE+0x30+(0x40*(ch))))

// Port Pull-Down Disable Registers
#define R_GPPDDIS(ch) ((volatile unsigned int*)(GPIO_BASE+0x1C+(0x40*(ch))))


// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************
/**
 *  @brief              This API Initializ the CPU GPIO Device
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvGPIOInitialize(void);

/**
 *  @brief              This API Release the CPU GPIO Device
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvGPIORelease(void);

 /**
 *  @brief              This API gets the value for a specified GPIO field.
 *  @param[in]  eField : Field to get the GPIO value for
 *  @param[out]  None
 *  @return     Value of the specified GPIO field
 *  @note       None
 *  @attention  None
 */
DWORD dvGPIOFieldGet(eGPIO_FIELD eField); 

  /**
 *  @brief              This API sets the value for a specified GPIO field.
 *  @param[in]  eField : Field to set the GPIO value for
 *  @param[in]  dwData : Data to set the field value to
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvGPIOFieldSet(eGPIO_FIELD eField,
                     DWORD        dwData);

#endif //#if !defined(_INCLUDED_DVGPIOAPI_H_)


