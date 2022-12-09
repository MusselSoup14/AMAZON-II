/**
  ******************************************************************************
  * @file    dvAK4183API.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    25-07-2018
  * @brief   This file contains all the functions prototypes for the CPU Driver
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
  * <h2><center>&copy; Copyright 1997-2018 (c) MadeFactory Inc.</center></h2>
  ******************************************************************************
  */  

#ifndef _DVAK4183API_H_
#define _DVAK4183API_H_

/* Includes ------------------------------------------------------------------*/  
#include "common.h"

#if USE_TOUCH_AK4183
// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

/* Device Instance Count */
#define AK4183_NUM_INSTANCES        1


/* Default Register Value */

/* Default Value */
#define AK4183_CONFIG_ADC_X_MIN_DEF       100
#define AK4183_CONFIG_ADC_X_MAX_DEF       4000
#define AK4183_CONFIG_ADC_Y_MIN_DEF       80
#define AK4183_CONFIG_ADC_Y_MAX_DEF       3850
#define AK4183_CONFIG_ADC_PRESS_MIN_DEF   10
#define AK4183_CONFIG_ADC_AVG_TOL_DEF     30

// ---------------------------------------------------------------------------
// Structure to Device Configuration
// ---------------------------------------------------------------------------    
typedef struct tagAK4183CONFIG
{
  WORD wAdcXMin;
  WORD wAdcXMax;
  WORD wAdcYMin;
  WORD wAdcYMax;
  WORD wAdcPressMin;
  WORD wAdcAVGTol;  
  //BYTE cSamplingCount;
}AK4183_CONFIG, *PAK4183_CONFIG;
typedef CROMDATA AK4183_CONFIG *PCAK4183_CONFIG;

// ---------------------------------------------------------------------------
// Structure to Device Status
// ---------------------------------------------------------------------------    
typedef struct tagAK4183DEVSTATUS
{
  BYTE b8BitMode;
  BYTE bPowerDown;
}AK4183_DEV_STATUS, *PAK4183_DEV_STATUS;
typedef CROMDATA AK4183_DEV_STATUS *PCAK4183_DEV_STATUS;

typedef struct tagAK4183READSTATUS
{
  BYTE    bEventOccurred;  
  POINT   sPoint;
  INT     nAdcX;
  INT     nAdcY;
  INT     nAdcPress;  
}AK4183_READ_STATUS, *PAK4183_READ_STATUS;
typedef CROMDATA AK4183_READ_STATUS *PCAK4183_READ_STATUS;

typedef struct tagAK4183STATUS
{
  BYTE                bConfigured;    /* TRUE if Configured */
  AK4183_DEV_STATUS   sDevStatus;
  AK4183_CONFIG       sConfig;
  AK4183_READ_STATUS  sReadStatus;
}AK4183_STATUS, *PAK4183_STATUS;
typedef CROMDATA AK4183_STATUS *PCAK4183_STATUS;

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************

// ***************************************************************************
// ******************** END OF PUBLIC CONFIGURATION SECTION ******************
// ***************************************************************************

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *  @brief       This API initializes Device and Driver that do not change frequently at runtime.
 *  @param[in]   None
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None
 */
void dvAK4183Initialize(void);

/**
 *  @brief        This API perform that Free Resources that Used on this component
 *  @param[in]    None
 *  @param[out]   None
 *  @return       None
 *  @note         None
 *  @attention    None
 */
void dvAK4183Release(void);

/**
 *  @brief         This API Process the internal state of component..
 
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvAK4183Process(void);

/**
 *  @brief        This API perform the H/W Reset via GPO
 *  @param[in]    None
 *  @param[out]   None
 *  @return       None
 *  @note         None
 *  @attention    one
 */
void dvAK4183HWReset(void);

/**
 *  @brief       This API perform the S/W Reset via specified I/F
 *  @param[in]   cInstance   Device instance
 *  @param[out]  None
 *  @return      rcSUCCESS if the operation was successful.
 *  @return      rcERROR if communication to the hardware failed,  
 *  @note        None
 *  @attention   None
 */
eRESULT dvAK4183SWReset(BYTE cInstance);

/**
 *  @brief        This API perform Reads a Touch Point
 *  @param[in]    cInstance: Device instance
                  psPoint: Pointer to read point return back
 *  @param[out]   None
 *  @return       rcSUCCESS if the operation was successful.
                  rcERROR if communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
eRESULT dvAK4183TouchGet(BYTE cInstance, PPOINT psPoint);

/**
 *  @brief      This API Set Device Status as specified
 *  @param      cInstance: Device instance
                sConfigNew: Device Status to set
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed.
 *  @note       None
 *  @attention  None
 */
eRESULT dvAK4183ConfigSet(BYTE cInstance, AK4183_CONFIG sConfigNew);

/**
 *  @brief      This API return the Device Read Status
 *  @param      cInstance: Device instance
 *  @return     Device Status
 *  @note       None
 *  @attention  None
 */
AK4183_READ_STATUS dvAK4183ReadStatusGet(BYTE cInstance);

#endif // #if USE_TOUCH_AK4183

#endif /* _DVAK4183API_H_ */

