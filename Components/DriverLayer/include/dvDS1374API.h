/**
  ******************************************************************************
  * @file    dvDS1374API.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    09-05-2019
  * @brief   This file contains all the functions prototypes for the Audio Amp Driver
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
#if !defined(_INCLUDED_DVDS1374API_H_)
#define _INCLUDED_DVDS1374API_H_

 /* Includes ------------------------------------------------------------------*/
#include "common.h"

#if ((USE_LCD_1024x600 ||  USE_LCD_1280x800) && USE_DS1374_RTC)
/** @addtogroup CWM-T10
  * @{
  */


/** @defgroup dvDS1374 
  * @brief DS1374 Driver
  * @{
  */

/** @defgroup dvDS1374_Exported_Types
  * @{
  */

/**
  * @}
  */


/** @defgroup dvDS1374_Exported_constants 
  * @{
  */
// ---------------------------------------------------------------------------  
// Device Instance Count
// ---------------------------------------------------------------------------
#define DS1374_NUM_INSTANCES      1

// ---------------------------------------------------------------------------  
// timestamp value on !rcSUCCESS
// ---------------------------------------------------------------------------  
#define DS1374_TIMESTAMP_ERROR  0xFFFFFFFF 


/**
  * @}
  */

/** @defgroup dvDS1374_Interrupt_Service_Routine
  * @{
  */

/**
  * @}
  */


/** @defgroup dvDS1374_Exported_Functions
  * @{
  */

/**
 *  @brief      This API initializes the device bus software and hardware
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
void dvDS1374Initialize(void);

/**
 *  @brief      This API must be called once during system Release
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvDS1374Release(void);

/**
 *  @brief       This API perform the H/W Reset via specified I/F
 *  @param[in]   None
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None
 */
void dvDS1374HWReset(void);

/**
 *  @brief      this API the presence of specified DS1374 instance
 *  @param[in]  cInstance:   Device instance
 *  @param[out] None
 *  @return     TRUE if 2-wire device is presence, FALSE for others
 *  @note       None
 *  @attention  None
 */ 
BOOL dvDS1374PresenceGet(BYTE cInstance);

/**
 *  @brief       This API the Get the timestamp from specified DS1374 instance
 *  @param[in]   cInstance:     Device instance
                 pdwTimestamp:  pointer to return back timestamp
 *  @param[out]  None
 *  @return      rcSUCCESS if the operation was successful.
                 rcERROR if communication to the hardware failed,  
 *  @note        None
 *  @attention   None
 */ 
eRESULT dvDS1374TimestampGet(BYTE cInstance, PDWORD pdwTimestamp);

/**
 *  @brief      This API the Set the timestamp from specified DS1374 instance
 *  @param[in]  cInstance:    Device instance
                pdwTimestamp: pointer to timestamp to write
 *  @param[out] None
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed,  
 *  @note       None
 *  @attention  None
 */ 
eRESULT dvDS1374TimestampSet(BYTE cInstance, DWORD dwTimestamp);


/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

#endif // #if ((USE_LCD_1024x600 ||  USE_LCD_1280x800) && USE_DS1374_RTC)

#endif // #if !defined(_INCLUDED_DVDS1374API_H_)  

/******************* (C) COPYRIGHT 1997-2019 (c) MadeFactory Inc. *****END OF FILE****/  
