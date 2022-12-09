/**
  ******************************************************************************
  * @file    appAudioMgr.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    4-April-2016
  * @brief   This file provides APIs the Audio Manager
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
#include "appAudioMgrAPI.h"
#include "utilExecutiveAPI.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************
// ---------------------------------------------------------------------------
// Volume Curve
// ---------------------------------------------------------------------------
static CROMDATA BYTE m_acVolumeTable[AUDIO_VOLUME_MAX+1] = 
{
  // --------------------------------CONFIGURATION------------------------------  
  HAL_AUDIO_VOLUME_MIN,      /* 0 */
  210,
  215,
  220,
  225,
  230,    /* 5 */
  235,
  240,
  245,
  250,
  HAL_AUDIO_VOLUME_MAX     /* 10 */
  // --------------------------------CONFIGURATION------------------------------    
};

//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************
/* Audio Volume */
static BYTE       m_cAudioVolLevel      = 0; 

/* Audio Loop Play */
static BOOL       m_bAudioLoopEnabled = 0;
static WORD       m_wAudioLoopTicks   = 0;
static eSOUND_ID  m_eAudioLoopSoundId = sndINVALID;

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
void appAudioMgrInitialize(void)
{
  eNODE_NAME            eNodeID     = nnAUDIOMGR_NVRAM_DATA;
  PAUDIOMGR_NVRAM_DATA  psNVRAMData = NULL;

  /* Load from NVRAM to internal Variables */
  psNVRAMData = utilDataMgrReadLockData(eNodeID);
  utilDataMgrUnlock(eNodeID, FALSE);
  
  /* Release Mute */
  appAudioMgrAmpMuteSet(psNVRAMData->bMute);
  appAudioMgrMuteSet(psNVRAMData->bMute);

  /* Default Volume Set */
  appAudioMgrVolumeLevelSet(psNVRAMData->cVolLevel);
}

/**
 *  @brief      This API Release the internal state of component.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appAudioMgrRelease(void)
{

}

/**
 *  @brief      This API completes the previously started audio broadcast standard detect
                    or volume adjust processing requiring non-blocking functionality.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appAudioMgrProcess(void)
{

  if(m_bAudioLoopEnabled
    && m_eAudioLoopSoundId != sndINVALID
    && m_wAudioLoopTicks    
    && !utilExecCounterGet(coSOUND_PLAY))
  {
    utilExecCounterSet(coSOUND_PLAY, m_wAudioLoopTicks);

    appAudioMgrPlay(m_eAudioLoopSoundId, TRUE);    
  }
}

/**
 *  @brief      This API Play the specified sound id
 *  @param[in]  eSoundId: Sound Id to play
                bPlayAfterStop: TRUE to Play after stop the last audio stream
 *  @param[out] None
 *  @return     TRUE if played, FALSE for others
 *  @note       None
 *  @attention  None
 */
BOOL appAudioMgrPlay(eSOUND_ID eSoundId, BOOL bPlayAfterStop)
{
  BOOL bSuccess = FALSE;
  
  if(rcSUCCESS == halAudioCtrlPlay(eSoundId, bPlayAfterStop)) bSuccess = TRUE;

  return bSuccess;
}

/**
 *  @brief      This API return loop Play enabled
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Loop play enabled, FALSE to others
 *  @note       None
 *  @attention  None
 */
BOOL appAudioMgrPlayLoopEnabled(void)
{
  return m_bAudioLoopEnabled;
}

/**
 *  @brief      This API loop Play the specified sound id 
 *  @param[in]  eSoundId: Sound Id to play
                        wTicks: Tick count to play
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appAudioMgrPlayLoop(eSOUND_ID eSoundId, WORD wTicks)
{
  
  ASSERT(eSoundId < sndINVALID);
  
  if(m_eAudioLoopSoundId != eSoundId 
    || m_wAudioLoopTicks != wTicks)
  {
    if(wTicks)
    {
      m_bAudioLoopEnabled = TRUE;
      m_eAudioLoopSoundId = eSoundId;
      m_wAudioLoopTicks = wTicks;
      
      utilExecCounterSet(coSOUND_PLAY, wTicks);

      /* Play First */
      appAudioMgrPlay(m_eAudioLoopSoundId, TRUE);   
    }
    else
    {
      m_bAudioLoopEnabled = FALSE;
      m_eAudioLoopSoundId = sndINVALID;
      m_wAudioLoopTicks = 0;
    }    
  }  
}

/**
 *  @brief      This API Set the volume
 *  @param[in]  cVolLevel: Volume Level to set
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appAudioMgrVolumeLevelSet(BYTE cVolLevel)
{
  eNODE_NAME            eNodeID     = nnAUDIOMGR_NVRAM_DATA;
  PAUDIOMGR_NVRAM_DATA  psNVRAMData = NULL;

  cVolLevel = MIN(AUDIO_VOLUME_MAX, cVolLevel);
  ///cVolLevel = MAX(AUDIO_VOLUME_MIN, cVolLevel);

  if(rcSUCCESS == halAudioCtrlVolumeSet(m_acVolumeTable[cVolLevel]))
  {
    psNVRAMData = utilDataMgrWriteLockData(eNodeID);
    psNVRAMData->cVolLevel = cVolLevel;
    m_cAudioVolLevel = cVolLevel;
    utilDataMgrUnlock(eNodeID, TRUE);
  }
}

/**
 *  @brief      This API Get the current volume
 *  @param[in]  None
 *  @param[out] None
 *  @return     current volume
 *  @note       None
 *  @attention  None
 */
BYTE appAudioMgrVolumeLevelGet(void)
{
  return m_cAudioVolLevel;
}

/**
 *  @brief      This API Set the Audio Mute
 *  @param[in]  bMute: TRUE to Mute, FALSE to unmute
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appAudioMgrMuteSet(BOOL bMute)
{
  eNODE_NAME            eNodeID     = nnAUDIOMGR_NVRAM_DATA;
  PAUDIOMGR_NVRAM_DATA  psNVRAMData = NULL;

  if(rcSUCCESS == halAudioCtrlMuteSet(bMute))
  {
    psNVRAMData = utilDataMgrWriteLockData(eNodeID);
    psNVRAMData->bMute = bMute;
    utilDataMgrUnlock(eNodeID, TRUE);    
  }
}

/**
 *  @brief      This API Get the Audio mute status
 *  @param[in]  None
 *  @param[out] None
 *  @return     Audio mute status
 *  @note       None
 *  @attention  None
 */
BOOL appAudioMgrMuteGet(void)
{
  return halAudioCtrlMuteGet();
}

/**
 *  @brief      This API Set the Audio Amp Mute
 *  @param[in]  bMute: TRUE to Mute, FALSE to unmute
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appAudioMgrAmpMuteSet(BOOL bMute)
{    
  halAudioCtrlAmpMuteSet(bMute);
}

/**
 *  @brief      This API Get the Audio Amp mute status
 *  @param[in]  None
 *  @param[out] None
 *  @return     Amp mute status
 *  @note       None
 *  @attention  None
 */
BOOL appAudioMgrAmpMuteGet(void)
{
  return halAudioCtrlAmpMuteGet();
}


/**
 *  @brief      This API is the persistent storage manager call back routine used by utilDataMgr.c. 
 *  @param[in]  eNodeID: Identifier of the data node associated with this event..
 *  @param[out] eEvent: Identifier of the external event.
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appAudioMgrNodeCallback(eNODE_NAME eNodeID,
                             eNV_EVENT  eEvent)
{
  AUDIOMGR_NVRAM_DATA *psNVRAMData;

  ASSERT(nnAUDIOMGR_NVRAM_DATA == eNodeID);

  switch(eEvent)
  {
    case nveRESETALL:
    case nveINITIALIZE:
    /* ---------------------------- BEGIN CUSTOM AREA --------------------------- 
    * Add custom code here to handle initialization of this component's Data
    * Manager node */
      // Lock data node and prepare to write
      psNVRAMData = utilDataMgrWriteLockData(nnAUDIOMGR_NVRAM_DATA);

      psNVRAMData->bMute    = FALSE;
      psNVRAMData->cVolLevel  = AUDIO_VOLUME_DEFAULT;

      /* Reserved area to Audio Manager Node */
      memset(psNVRAMData->acReserved, 0, sizeof(psNVRAMData->acReserved));
      
      utilDataMgrUnlock(nnAUDIOMGR_NVRAM_DATA, TRUE);
    /* ---------------------------- END CUSTOM AREA --------------------------- */
    break;

    default:
      ASSERT_ALWAYS();
  }
}



