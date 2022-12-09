/**
  ******************************************************************************
  * @file    dvAR1000API.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    27-03-2019
  * @brief   This file contains all the functions prototypes for the Touch Driver
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
#if !defined(_INCLUDED_DVAR1000API_H_)
#define _INCLUDED_DVAR1000API_H_

 /* Includes ------------------------------------------------------------------*/
#include "common.h"

/** @addtogroup CWM-T10
  * @{
  */


/** @defgroup dvAR1000 
  * @brief AR1000 Driver
  * @{
  */

/** @defgroup dvAR1000_Exported_Types
  * @{
  */
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------  

/**
  * @}
  */


/** @defgroup dvAR1000_Exported_constants 
  * @{
  */
/* Device Instance Count */
#define AR1000_NUM_INSTANCES      1

/**
  * @}
  */

/** @defgroup dvAR1000_Interrupt_Service_Routine
  * @{
  */

/**
  * @}
  */


/** @defgroup dvAR1000_Exported_Functions
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
void dvAR1000Initialize(void);

/**
 *  @brief      This API Release the Device
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvAR1000Release(void);

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */
#endif //#if !defined(_INCLUDED_DVAR1000API_H_)  

/******************* (C) COPYRIGHT 1997-2019 (c) MadeFactory Inc. *****END OF FILE****/  
