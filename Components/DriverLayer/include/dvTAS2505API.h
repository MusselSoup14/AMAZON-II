/**
  ******************************************************************************
  * @file    dvTAS2505API.h
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
#if !defined(_INCLUDED_DVTAS2505API_H_)
#define _INCLUDED_DVTAS2505API_H_

 /* Includes ------------------------------------------------------------------*/
#include "common.h"

#if ((USE_LCD_1024x600 ||  USE_LCD_1280x800) && USE_TAS2505_AUDIO_CODEC)
/** @addtogroup CWM-T10
  * @{
  */


/** @defgroup dvTAS2505 
  * @brief TAS2505 Driver
  * @{
  */

/** @defgroup dvTAS2505_Exported_Types
  * @{
  */
// ---------------------------------------------------------------------------
// enum for Audio Input
// ---------------------------------------------------------------------------
typedef enum
{
  aiLR,       /* Analog L/R single ended input */
  aiASI,      /* Digital Input */
  
  aiINVALID   /* used for enumeration type range
                 checking (DO NOT REMOVE) */
}eTAS2505_INPUT_ID;

/**
  * @}
  */


/** @defgroup dvTAS2505_Exported_constants 
  * @{
  */
// ---------------------------------------------------------------------------  
// Device Instance Count
// ---------------------------------------------------------------------------
#define TAS2505_NUM_INSTANCES      1

/**
  * @}
  */

/** @defgroup dvTAS2505_Interrupt_Service_Routine
  * @{
  */

/**
  * @}
  */


/** @defgroup dvTAS2505_Exported_Functions
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
void dvTAS2505Initialize(void);

/**
 *  @brief      This API Release the Device
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvTAS2505Release(void);

/**
 *  @brief       This API perform the H/W Reset via specified I/F
 *  @param[in]   None
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None
 */
void dvTAS2505HWReset(void);

/**
 *  @brief            This API perform the S/W Reset via specified I/F
 *  @param[in]    cInstance   Device instance
 *  @param[out]  None
 *  @return         rcSUCCESS if the operation was successful.
 *  @return         rcERROR if communication to the hardware failed,  
 *  @note           None
 *  @attention     None
 */
eRESULT dvTAS2505SWReset(BYTE cInstance);

/**
 *  @brief        This API sets the registers in the AIC3268 to configure the specified
                  input as the source for the specified output.
 *  @param[in]    eAudioInputId  : Identifier of audio input source
                  eAudioOutputId : Identifier of audio output
 *  @param[out]   None
 *  @return       rcSUCCESS if the operation was successful.
 *  @return       rcERROR if communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
eRESULT dvTAS2505Configure(BYTE cInstance,
                                 eTAS2505_INPUT_ID eAudioInputId);

/**
 *  @brief       This API Control GPIO pin as bit bang output
 *  @param[in]   cInstance: Device instance
 *  @param[in]   bHigh:     TRUE to HIGH, FALSE to Low
 *  @param[out]  None
 *  @return      rcSUCCESS if the operation was successful.
 *  @return      rcERROR if communication to the hardware failed,  
 *  @note        None
 *  @attention   None
*/
eRESULT dvTAS2505GPIOBitBangOutSet(BYTE cInstance, BOOL bHigh);

/**
 *  @brief       This API is Wrapper of TAS2505RegsWrite()
 *  @param[in]   cInstance:     Device instance
 *  @param[in]   wPBRAddress:  Register Address(included Book/Page) to Write to
 *  @param[in]   cSize:         Number of bytes to write
 *  @param[in]   pacData:       pointer for Data to write
 *  @param[out]  None
 *  @return      rcSUCCESS if the operation was successful.
 *  @return      rcERROR if communication to the hardware failed,  
 *  @note        None
 *  @attention   None
 */
eRESULT dvTAS2505Writes(BYTE cInstance, 
                              WORD wPBRAddress, 
                              BYTE cSize, 
                              PCBYTE pacData);

/**
 *  @brief        This API Wrapper of TAS2505PageSet(), TAS2505RegsRead()
 *  @param[in]    cInstance:    Device instance
 *  @param[in]    wPBRAddress: Register Address(included Book/Page) to Read
 *  @param[in]    cSize:        Number of bytes to read
 *  @param[out]   pacData:      pointer for Data to read
 *  @return       rcSUCCESS if the operation was successful.
 *  @return       rcERROR if communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
eRESULT dvTAS2505Reads(BYTE cInstance, 
                            WORD wPBRAddress, 
                            BYTE cSize, 
                            PBYTE pacData);

/**
 *  @brief       This API is Wrapper of TAS2505PageSet(), TAS2505OneRegWrite()
 *  @param[in]   cInstance:     Device instance
 *  @param[in]   wPBRAddress:  Register Address(included Book/Page) to Write to
 *  @param[in]   cData:         Data to write
 *  @param[out]  None
 *  @return      rcSUCCESS if the operation was successful.
 *  @return      rcERROR if communication to the hardware failed,  
 *  @note        None
 *  @attention   None
 */
eRESULT dvTAS2505OneWrite(BYTE cInstance, 
                                WORD wPBRAddress, 
                                BYTE cData);

/**
 *  @brief        This API is Wrapper of TAS2505PageSet(), TAS2505OneRegRead()
 *  @param[in]    cInstance:    Device instance
 *  @param[in]    wPBRAddress: Register Address(included Book/Page) to Read to
 *  @param[out]   pacData:      pointer for Data to read
 *  @return       rcSUCCESS if the operation was successful.
 *  @return       rcERROR if communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
eRESULT dvTAS2505OneRead(BYTE cInstance, 
                              WORD wPBRAddress, 
                              PBYTE pacData);


/**
 *  @brief        This API is Wrapper of TAS2505PageSet(), TAS2505OneRegFieldWrite()
 *  @param[in]    cInstance:    Device instance
 *  @param[in]    wPBRAddress: Register Address(included Book/Page) to Write to
 *  @param[in]    cBitPosition: Bit Position
 *  @param[in]    cBitOffset:   Bit Size
 *  @param[out]   cData:        Data to write
 *  @return       rcSUCCESS if the operation was successful.
 *  @return       rcERROR if communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
eRESULT dvTAS2505OneFieldWrite(BYTE cInstance,
                                WORD wPBRAddress,
                                BYTE cBitPosition,
                                BYTE cBitOffset,
                                BYTE cData);

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

#endif // #if ((USE_LCD_1024x600 ||  USE_LCD_1280x800) && USE_TAS2505_AUDIO_CODEC)

#endif // #if !defined(_INCLUDED_DVTAS2505API_H_)  

/******************* (C) COPYRIGHT 1997-2019 (c) MadeFactory Inc. *****END OF FILE****/  
