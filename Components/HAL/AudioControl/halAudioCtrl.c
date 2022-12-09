/**
  ******************************************************************************
  * @file    halAudioCtrl.c
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
#include "halAudioCtrlAPI.h"
#include "dvCPUAPI.h"
#include "dvGPIOAPI.h"
#include "utilExecutiveAPI.h"
#include "utilHostIfAPI.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************

//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************

// ---------------------------------------------------------------------------
// Wave file path (WAV File Only)
// ---------------------------------------------------------------------------
static CROMDATA PCHAR m_pacHalAudioFiles[sndINVALID] = 
{
  // --------------------------------CONFIGURATION------------------------------  
  "wav/1KHz.wav",       // snd1KHz  
  "wav/Welcome.wav",    // sndWELCOME
  "wav/System_On.wav",  // sndSYSTEM_ON
  "wav/Fail.wav",       // sndFAIL
  "wav/OK.wav",         // sndOK
  "wav/Touch.wav",      // sndTOUCH
  "wav/Ding.wav",       // sndDING
  "wav/DingPulse.wav",  // sndDINGPULSE   
  "wav/Chap.wav",       // sndCHAP  
  "wav/TimeRemain.wav", // sndTIME_REMAIN,
  "wav/TimeOut.wav",    // sndTIME_OUT,
  // --------------------------------CONFIGURATION------------------------------    
};

/* Pointer array to WAVE* structure */
static WAVE*      m_pasWaveObject[sndINVALID]; 

/* Internal Audio Vars */
static BYTE       m_cHalAudioGain;
static BOOL       m_bHalAudioMute;
static BOOL       m_bHalAudioMuteAmp;
static eSOUND_ID  m_eHalSoundIdPrev; // Last Played ID

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
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
void halAudioCtrlInitialize(void)
{
  WORD wI;

  /* Initialize Audio Device */
#if USE_AMAZON_STK
  wm8974_init();
  wm8974_mono_on();
#elif USE_TAS2505_AUDIO_CODEC
  dvTAS2505Initialize();
#endif    

  /* Init Vars */
  m_cHalAudioGain       = (BYTE)*R_SNDVOLOUT(0);
  m_bHalAudioMute       = FALSE;
  m_bHalAudioMuteAmp    = FALSE;
  m_eHalSoundIdPrev     = sndINVALID; // Last Played ID

  /* Load Audio Files */
  for(wI = 0; wI < sndINVALID; wI++)
  {    
    m_pasWaveObject[wI] = sound_loadwav(m_pacHalAudioFiles[wI]);
  }  

}

/**
 *  @brief      This API Release the internal state of component.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halAudioCtrlRelease(void)
{
  /* NONE */
}

/**
 *  @brief      This API completes the previously started Buzzer
                    or volume adjust processing requiring non-blocking functionality.
 *  @param[in]  None
 *  @param[out] None
 *  @return     nbPENDING= Result is pending
                nbSUCCESS= No error occurred and returned result is complete
                nbERROR= An error occurred during the operation
 *  @note       None
 *  @attention  None
 */
eNBRESULT halAudioCtrlProcess(void)
{
  eNBRESULT eNBResult = nbSUCCESS;


  return eNBResult;
}

/**
 *  @brief      This API Play the specified sound id
 *  @param[in]  eSoundId: Sound Id to play
                bPlayAfterStop: TRUE to Play after stop the last audio stream
 *  @param[out] None
 *  @return     rcSUCCESS if read operation succeeds, rcERROR otherwise
 *  @note       None
 *  @attention  None
 */
eRESULT halAudioCtrlPlay(eSOUND_ID eSoundId, BOOL bPlayAfterStop)
{
  eRESULT eResult = rcERROR;

  ASSERT(eSoundId < sndINVALID);
  
  if(m_pasWaveObject[eSoundId]->datasize > 0
    /*&& !sound_isplay(m_pasWaveObject[eSoundId])*/)
  {
    if(bPlayAfterStop && sndINVALID != m_eHalSoundIdPrev)
      sound_stop(m_pasWaveObject[m_eHalSoundIdPrev]);
    sound_play(m_pasWaveObject[eSoundId]);

    m_eHalSoundIdPrev = eSoundId;
    
    eResult = rcSUCCESS;
  }  

  return eResult;
}

/**
 *  @brief        This API perform the AMP Mute
 *  @param[in]    bMuteOn: TRUE to Mute On, FALSE to Mute Off
 *  @param[out]   None
 *  @return       rcSUCCESS if read operation succeeds, rcERROR otherwise
 *  @note         None
 *  @attention    None
 */
eRESULT halAudioCtrlAmpMuteSet(BOOL bMuteOn)
{
  eRESULT eResult = rcSUCCESS;

  /* TBD: Audio Codec Mute with "REG_TAS_DAC_CHANNEL2 -> Mute(0x0C), UnMute(0x04)" */
  
  m_bHalAudioMuteAmp = bMuteOn;

  return eResult;
}

/**
 *  @brief        This API return the AMP Mute status
 *  @param[in]    None
 *  @param[out]   None
 *  @return       TRUE if AMP Muted, FALSE if AMP unmuted
 *  @note         None
 *  @attention    None
 */
BOOL halAudioCtrlAmpMuteGet(void)
{
  return m_bHalAudioMuteAmp;
}

/**
 *  @brief      This API Set Mute  (Blocking Functionality)
 *  @param[in]  bMute: TRUE to Mute, FALSE to un-mute
 *  @param[out] None
 *  @return     rcSUCCESS if the operation was successful.
 *              rcERROR if others
 *  @note       None
 *  @attention  None
 */
eRESULT halAudioCtrlMuteSet(BOOL bMute)
{
  eRESULT eResult = rcSUCCESS;

  if(bMute) 
    *R_SNDVOLOUT(0) = 0; // Mixer Volume to 0
  else
    halAudioCtrlVolumeSet(m_cHalAudioGain); // Recover the Mixer Volume

  m_bHalAudioMute = bMute;

  return eResult;
}

/**
 *  @brief        This API return Mute Status
 *  @param[in]    None
 *  @param[out]   None
 *  @return       TRUE if AMP or ISC muted, FALSE to others
 *  @note         None
 *  @attention    None
 */
BOOL halAudioCtrlMuteGet(void)
{
  return m_bHalAudioMute;
}

/**
 *  @brief      This API Set Voume (Audio Gain. Blocking Functionality)
 *  @param[in]  cAudioGain: 0xFF(0dB) ~ 0x80(-63.5dB), 0x7F~0x0(-∞dB)
 *  @param[out] None
 *  @return     rcSUCCESS if the operation was successful.
 *              rcERROR if others
 *  @note       None
 *  @attention  None
 */
eRESULT halAudioCtrlVolumeSet(BYTE cAudioGain)
{
  eRESULT eResult = rcSUCCESS;

  ///cAudioGain = MIN(HAL_AUDIO_VOLUME_MAX, cAudioGain);
  cAudioGain = MAX(HAL_AUDIO_VOLUME_MIN, cAudioGain);

  *R_SNDVOLOUT(0) = ((UINT)cAudioGain << 8) | cAudioGain;  

  m_cHalAudioGain = cAudioGain;

  return eResult;
}

/**
 *  @brief      This API Get Voume
 *  @param[in]  None
 *  @param[out] None
 *  @return     AudioGain: 0xFF(0dB) ~ 0x80(-63.5dB), 0x7F~0x0(-∞dB)
 *  @note       None
 *  @attention  None
 */
 BYTE halAudioCtrlVolumeGet(void)
{
  return m_cHalAudioGain;
}

