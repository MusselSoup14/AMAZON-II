/**
  ******************************************************************************
  * @file    dvAR1000.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    27-03-2019
  * @brief   This file provides basic the Touch Driver
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
/* Includes ------------------------------------------------------------------*/
#include "dvAR1000API.h"
#include "dvGPIOAPI.h"
#include "dvSerialAPI.h"

/** @addtogroup CWM-T10
  * @{
  */

/** @defgroup dvAR1000 
  * @brief AR1000 Driver
  * @{
  */

/** @defgroup dvAR1000_Private_Defines
  * @{
  */
/**
  * @}
  */

/** @defgroup dvAR1000_Private_TypesDefinitions
  * @{
  */
  
/**
  * @}
  */

/** @defgroup dvAR1000_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup dvAR1000_Private_Variables
  * @{
  */

/**
  * @}
  */

/** @defgroup dvAR1000_Private_FunctionPrototypes
  * @{
  */

/**
  * @}
  */

/** @defgroup dvAR1000_Private_Functions
  * @{
  */

/**
 *  @brief      Initialize the Driver
 *  @param      None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void AR1000DriverInitialize(void)
{

}


/**
 *  @brief      Configures the Device 
 *  @param      None
 *  @return     rcSUCCESS if the operation was successful.
 *  @note       None
 *  @attention  None
 */
static eRESULT AR1000DeviceInitialize(void)
{
  eRESULT eResult = rcSUCCESS;

  return eResult;
}

/**
  * @}
  */

/** @defgroup dvAR1000_Interrupt_Service_Routine
  * @{
  */

/**
  * @}
  */



/** @defgroup dvAR1000_Public_Functions
  * @{
  */

/**
 *  @brief      This API must be called once during system startup, 
                before calls are made to any other APIs in the Chip Driver component.
 *  @param      None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvAR1000Initialize(void)
{
  AR1000DriverInitialize();
  if(rcSUCCESS != AR1000DeviceInitialize())
  {
    /* Error Process Here */
  }
}

/**
 *  @brief       This API Release the Device
 *  @param[in]   None
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None
 */
void dvAR1000Release(void)
{ 

}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 1997-2019 (c) MadeFactory Inc. *****END OF FILE****/  
