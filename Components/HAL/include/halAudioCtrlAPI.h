/**
  ******************************************************************************
  * @file    halAudioCtrlAPI.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    14-05-2019
  * @brief   This file provides APIs the Audio Control
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
#if !defined(_HALAUDIOCTRLAPI_H_)
#define _HALAUDIOCTRLAPI_H_

#include "common.h"
#include "dvTAS2505API.h"
#if USE_AMAZON_STK
#include "wm8974.h"
#endif


// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
// ---------------------------------------------------------------------------
// enum for Sound File to play
// ---------------------------------------------------------------------------
typedef enum
{
  snd1KHz, // Test Pattern

  sndWELCOME,
  sndSYSTEM_ON,
  sndFAIL,
  sndOK,
  sndTOUCH,
  sndDING,
  sndDINGPULSE,    
  sndCHAP,
  sndTIME_REMAIN,
  sndTIME_OUT,
  
  sndINVALID
}GCC_PACKED eSOUND_ID;

// ---------------------------------------------------------------------------  
// HAL Audio Parameters
// ---------------------------------------------------------------------------  
#define HAL_AUDIO_VOLUME_MIN      127 // 0x7F~0x00: Mute, 0xFF(0dB) ~ 0x80(-63.5dB)
#define HAL_AUDIO_VOLUME_MAX      255
#define HAL_AUDIO_VOLUME_INC      5
#define HAL_AUDIO_VOLUME_DEFAULT  205

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************
/**
 *  @brief      This API sets up the internal state of component.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halAudioCtrlInitialize(void);

/**
 *  @brief      This API Release the internal state of component.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halAudioCtrlRelease(void);

/**
 *  @brief      This API completes the previously started audio broadcast standard detect
                    or volume adjust processing requiring non-blocking functionality.
 *  @param[in]  None
 *  @param[out] None
 *  @return     nbPENDING= Result is pending
                     nbSUCCESS= No error occurred and returned result is complete
                     nbERROR= An error occurred during the operation
 *  @note       None
 *  @attention  None
 */
eNBRESULT halAudioCtrlProcess(void);

/**
 *  @brief      This API Play the specified sound id
 *  @param[in]  eSoundId: Sound Id to play
                bPlayAfterStop: TRUE to Play after stop the last audio stream
 *  @param[out] None
 *  @return     rcSUCCESS if read operation succeeds, rcERROR otherwise
 *  @note       None
 *  @attention  None
 */
eRESULT halAudioCtrlPlay(eSOUND_ID eSoundId, BOOL bPlayAfterStop);

/**
 *  @brief        This API perform the AMP Mute
 *  @param[in]    bMuteOn: TRUE to Mute On, FALSE to Mute Off
 *  @param[out]   None
 *  @return       rcSUCCESS if read operation succeeds, rcERROR otherwise
 *  @note         None
 *  @attention    None
 */
eRESULT halAudioCtrlAmpMuteSet(BOOL bMuteOn);

/**
 *  @brief        This API return the AMP Mute status
 *  @param[in]    None
 *  @param[out]   None
 *  @return       TRUE if AMP Muted, FALSE if AMP unmuted
 *  @note         None
 *  @attention    None
 */
BOOL halAudioCtrlAmpMuteGet(void);

/**
 *  @brief      This API Set Mute  (Blocking Functionality)
 *  @param[in]  bMute: TRUE to Mute, FALSE to un-mute
 *  @param[out] None
 *  @return     rcSUCCESS if the operation was successful.
 *              rcERROR if others
 *  @note       None
 *  @attention  None
 */
eRESULT halAudioCtrlMuteSet(BOOL bMute);

/**
 *  @brief        This API return Mute Status
 *  @param[in]    None
 *  @param[out]   None
 *  @return       TRUE if AMP or ISC muted, FALSE to others
 *  @note         None
 *  @attention    None
 */
BOOL halAudioCtrlMuteGet(void);

/**
 *  @brief      This API Set Voume (Audio Gain. Blocking Functionality)
 *  @param[in]  cAudioGain: 0x00: Mute, 0x01: -48dB, 0x02: -47dB, ......, 0x43: +18dB
 *  @param[out] None
 *  @return     rcSUCCESS if the operation was successful.
 *              rcERROR if others
 *  @note       None
 *  @attention  None
 */
eRESULT halAudioCtrlVolumeSet(BYTE cAudioGain);

/**
 *  @brief      This API Get Voume
 *  @param[in]  None
 *  @param[out] None
 *  @return     AudioGain: 0x00: Mute, 0x01: -48dB, 0x02: -47dB, ......, 0x43: +18dB
 *  @note       None
 *  @attention  None
 */
 BYTE halAudioCtrlVolumeGet(void);

#endif //#if !defined(_HALAUDIOCTRLAPI_H_)


