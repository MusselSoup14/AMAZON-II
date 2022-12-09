/**
  ******************************************************************************
  * @file    halLEDCtrl.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    02-August-2014
  * @brief   This file provides APIs the Power Manager
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
#include "halLEDCtrlAPI.h"
#include "dvCPUAPI.h"
#include "dvGPIOAPI.h"
#include "utilExecutiveAPI.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS **************
// ***************************************************************************

// ***************************************************************************
// ****************** START OF PRIVATE CONFIGURATION SECTION *****************
// ***************************************************************************

// ***************************************************************************
// ******************* END OF PRIVATE CONFIGURATION SECTION ******************
// ***************************************************************************
static CROMDATA eGPIO_FIELD m_aeLEDGPIOs[ledRAW_END] =
{
  gfGPO_LED_RED,       // ledRAW_RED
  gfGPO_LED_GREEN,     // ledRAW_GREEN
  gfGPO_LED_BLUE,      // ledRAW_BLUE
  gfGPO_LED_MSG,       // ledRAW_MSG  
};

// ***************************************************************************
// ******************** START OF PRIVATE DATA DECLARATIONS *******************
// ***************************************************************************

/* LED On/Off to Monitoring */
static BOOL m_abLEDOnOff[ledRAW_END]; // Monitoring purpose
static WORD m_wLEDCtrlFlags;

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************

/**
 *  @brief        LED Driver Control 
 *  @param[in]    eLEDId: LED id to On or Off
                  bOn: TRUE to ON, FALSE to OFF
 *  @param[out]   None
 *  @return       None
 *  @note         None
 *  @attention    None
 */
void LEDCtrlOnSet(eLED_ID eLEDId, BOOL bOn)
{
  switch(eLEDId)
  {
    case ledRAW_RED: 
    case ledRAW_GREEN:
    case ledRAW_BLUE:  
    case ledRAW_MSG:
      dvGPIOFieldSet(m_aeLEDGPIOs[eLEDId], (BOOL)!bOn);
    break;

    default: break;
  }

  // Sync also Ctrl Flags
  m_abLEDOnOff[eLEDId] = bOn;
  if(bOn) m_wLEDCtrlFlags |= LED_FLAG(eLEDId);
  else m_wLEDCtrlFlags &= ~((WORD)LED_FLAG(eLEDId));
}

// ***************************************************************************
// **************** START OF PUBLIC PROCEDURE IMPLEMENTATIONS ****************
// ***************************************************************************

/**
 *  @brief         This API sets up the internal state of component..
 
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLEDCtrlInitialize(void)
{
  BYTE cLEDId;    
  
  m_wLEDCtrlFlags = 0;

  for(cLEDId = 0; cLEDId < ledRAW_END; cLEDId++)
  {
    LEDCtrlOnSet(cLEDId, FALSE);
  }    
}

/**
 *  @brief         This API Release the internal state of component..
 
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLEDCtrlRelease(void)
{

}

/**
 *  @brief         This API Process the internal state of component..
 
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halLEDCtrlProcess(void)
{
  /* NTD */
}

/**
 *  @brief      This API is On/Off status of specified LED 
 *  @param[in]  eLEDId : LED Id to On or Close
 *  @param[out] None
 *  @return     TRUE if LED is On, FALSE if LED is Off
 *  @note       None
 *  @attention  None
 */
BOOL halLEDCtrlOnOffGet(eLED_ID eLEDId)
{
  ASSERT(eLEDId < ledRAW_END);
  
  return m_abLEDOnOff[eLEDId];
}

/**
 *  @brief        This API is On/Off the specified LED
 
 *  @param[in]    eLEDId : LED Id to On or Close
                  wOn: Value to On/Off
 *  @param[out]   None
 *  @return       None
 *  @note         None
 *  @attention    None
 */
void halLEDCtrlOnOffSet(eLED_ID eLEDId, BOOL bOn)
{
  WORD wLEDCtrlFlags = m_wLEDCtrlFlags;
  
  ASSERT(eLEDId < ledINVALID);

  switch(eLEDId)
  {
    default: 
      if(m_abLEDOnOff[eLEDId] != bOn)
      {
        LEDCtrlOnSet(eLEDId, bOn);
      }      
    break;

    /* --------------------------- BEGIN CUSTOM AREA --------------------------- */    
    case ledRAW_END:
      /* Nothing to do */
    break;      

    case ledGRP_ALL:
    {
      if(bOn) wLEDCtrlFlags |= LED_FLAGS_ALL;
      else wLEDCtrlFlags &= ~(LED_FLAGS_ALL);
      
      halLEDCtrlFlagsSet(wLEDCtrlFlags);        
    }      
    break;
  /* ---------------------------- END CUSTOM AREA --------------------------- */
  }
}


/**
 *  @brief         This API return the LED control flags 
 *  @param[in]  None
 *  @param[out]  None
 *  @return     complex of Led ctrl flags
 *  @note       None
 *  @attention  None
 */
WORD halLEDCtrlFlagsGet(void)
{  
  return m_wLEDCtrlFlags;
}

/**
 *  @brief         This API control the LED GPIOs as specified flags 
 *  @param[in]  wLEDCtrlFlags: complex of Led ctrl flags
 *  @param[out]  None
 *  @return     rcSUCCESS= No error occurred and returned result is complete
                       rcERROR= An error occurred during the operation
 *  @note       None
 *  @attention  None
 */
eRESULT halLEDCtrlFlagsSet(WORD wLEDCtrlFlags)
{
  eRESULT eResult = rcSUCCESS;
  BYTE cI;
  WORD wFlagsPrev, wFlagsNew;
  

  if(m_wLEDCtrlFlags != wLEDCtrlFlags)
  {

    for(cI = 0; cI < ledRAW_END; cI++)
    {
      wFlagsPrev = m_wLEDCtrlFlags & LED_FLAG(cI);
      wFlagsNew = wLEDCtrlFlags & LED_FLAG(cI);

      if(wFlagsPrev != wFlagsNew)
      {
        LEDCtrlOnSet((eLED_ID)cI, wFlagsNew);        
      }      
    }

    ///m_wLEDCtrlFlags = wLEDCtrlFlags;
  }

  return eResult;
}

