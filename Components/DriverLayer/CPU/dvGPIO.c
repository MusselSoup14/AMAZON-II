/**
  ******************************************************************************
  * @file    dvGPIO.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    02-August-2014
  * @brief   This file provides basic the GPIO Configuration.
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
#include "dvGPIO.h"
#include "dvGPIOAPI.h"
#include "dvGPIOType.h"
//#include "utilExecutiveAPI.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
#define GPIO_AF_GP    (0x03)

//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************


//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************


// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************
/**
 *  @brief      GPIO Initialization
 *  @param[in]  None
 *  @param[out]  None
 *  @return     TURE of FALSE
 *  @note       None
 *  @attention  None
 */
void gpioDeviceInitialize(void)
{
  BYTE          cField;
  VUINT         vunTmp = 0;
  VUINT         vunAF = 0;
  BYTE          cIndex;
  PCGPIO_FIELD  pcsGPIO = NULL;

  for(cField = 0; cField < gfINVALID; cField++)
  {
    pcsGPIO = (PCGPIO_FIELD)&g_asGPIOFields[cField];
    
    /* Port Alternate Function Register Set	*/
    vunAF = (VUINT)*R_PAF(pcsGPIO->eChannel);
    //msgDbg("[%d-1] 0x%08lX", cField, dwAF);
    
    ///for(cIndex = 0; cIndex < GPIO_INVALID; cIndex++)
    for(cIndex = 0; cIndex < 8; cIndex++)
    {
      // Amazon-II features */
      if(GPIO_12 == pcsGPIO->eChannel && cIndex > 3) continue;
      
      if(g_asGPIOFields[cField].dwMask & ((DWORD)1 << cIndex))
      {
        vunAF &= ~(GPIO_AF_GP << (cIndex << 1));
        vunAF |= (GPIO_AF_GP << (cIndex << 1));
        //msgDbg("%d", cIndex);
      }
    }
    
    //msgDbg("[%d-2] 0x%04lX", cField, vunAF);
    *R_PAF(pcsGPIO->eChannel) = vunAF;

    /* Port Pull-up/Pull-Down Register Set */
    if(GPIO_PUPD_PU == pcsGPIO->ePuPd)
    {
      *R_GPPUEN(pcsGPIO->eChannel) = pcsGPIO->dwMask;
    }
    else if(GPIO_PUPD_PD == pcsGPIO->ePuPd)
    {
      //*R_GPPUDIS(pcsGPIO->eChannel) = pcsGPIO->dwMask;
      *R_GPPDEN(pcsGPIO->eChannel) = pcsGPIO->dwMask;
    }
    else if(GPIO_PUPD_OD == pcsGPIO->ePuPd)
    {
      *R_GPODM(pcsGPIO->eChannel) = pcsGPIO->dwMask;
    }
    else if(GPIO_PUPD_SCHI == pcsGPIO->ePuPd)
    {
      *R_GPSHMT(pcsGPIO->eChannel) = pcsGPIO->dwMask;
    }

    /* Port Direction Setting Register Set */
    if(GPIO_MODE_OUT == pcsGPIO->eMode)
    {
      /* Port Direction Output Mode */ 
      *R_GPODIR(pcsGPIO->eChannel) = pcsGPIO->dwMask;
#if 0 // Legacy  
      // Output Control by Port Mode Set      
      *R_GPODIR(pcsGPIO->eChannel) = (1 << 8);
      //msgDbg("0x%X - 0x%X", pcsGPIO->dwMask, *R_GPODIR(pcsGPIO->eChannel));

      // Port output Data Set
      vunTmp = *R_GPDOUT(pcsGPIO->eChannel);
      vunTmp &= ~(pcsGPIO->dwMask);
      vunTmp |= (pcsGPIO->dwInit << pcsGPIO->cShift);
      *R_GPDOUT(pcsGPIO->eChannel) = vunTmp;
#else
      vunTmp = (pcsGPIO->dwInit << pcsGPIO->cShift) ;
      *R_GPOHIGH(pcsGPIO->eChannel) = vunTmp & pcsGPIO->dwMask;
      vunTmp = ~vunTmp & pcsGPIO->dwMask;
      *R_GPOLOW(pcsGPIO->eChannel) = vunTmp & pcsGPIO->dwMask;
#endif
    }
    else
    {
      /* Port Direction Input Mode */
      *R_GPIDIR(pcsGPIO->eChannel) = (BYTE)pcsGPIO->dwMask;
    }
    
  }
}


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
void dvGPIOInitialize(void)
{
  gpioDeviceInitialize();
}

/**
 *  @brief              This API Release the CPU GPIO Device
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvGPIORelease(void)
{ 

}

 /**
 *  @brief              This API gets the value for a specified GPIO field.
 *  @param[in]  eField : Field to get the GPIO value for
 *  @param[out]  None
 *  @return     Value of the specified GPIO field
 *  @note       None
 *  @attention  None
 */
DWORD dvGPIOFieldGet(eGPIO_FIELD eField)
{
  DWORD         dwData    = 0;
  DWORD         dwMask;
  PCGPIO_FIELD  pcsGPIO   = (PCGPIO_FIELD)&g_asGPIOFields[eField];

  dwMask = pcsGPIO->dwMask;

  if(pcsGPIO->eMode == GPIO_MODE_OUT)
  {  
    dwData = (DWORD)*R_GPOLEV(pcsGPIO->eChannel);
  }
  else
  {
    dwData = (DWORD)*R_GPILEV(pcsGPIO->eChannel);
  }
  
  dwData &= dwMask;
  dwData >>= pcsGPIO->cShift;
  
  return dwData;
}
 

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
                     DWORD        dwData)
{
  DWORD         dwMask;
  DWORD         dwCurrentPortData = 0;
  PCGPIO_FIELD  pcsGPIO           = (PCGPIO_FIELD)&g_asGPIOFields[eField];

  ASSERT(gfINVALID > eField);
  ASSERT(GPIO_MODE_OUT == pcsGPIO->eMode);
  
  dwMask = pcsGPIO->dwMask;

  if(pcsGPIO->eMode == GPIO_MODE_OUT)
  {  
#if 0 // Legacy
    dwCurrentPortData = (DWORD)*R_GPOLEV(pcsGPIO->eChannel);

    dwCurrentPortData &= ~dwMask;
    dwData <<= pcsGPIO->cShift;
    dwData &= dwMask;
    dwCurrentPortData |= dwData;


    *R_GPDOUT(pcsGPIO->eChannel) = (BYTE)dwCurrentPortData;
#else
    dwCurrentPortData = (dwData << pcsGPIO->cShift) ;
    *R_GPOHIGH(pcsGPIO->eChannel) = dwCurrentPortData & pcsGPIO->dwMask;
    dwCurrentPortData = ~dwCurrentPortData & pcsGPIO->dwMask;
    *R_GPOLOW(pcsGPIO->eChannel) = dwCurrentPortData & pcsGPIO->dwMask;    
#endif
  }
}



