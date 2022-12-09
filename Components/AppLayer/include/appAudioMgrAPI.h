/**
  ******************************************************************************
  * @file    appAudioMgrAPI.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    4-April-2019
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
#if !defined(_APPAUDIOMGRAPI_H_)
#define _APPAUDIOMGRAPI_H_

#include "common.h"
#include "halAudioCtrlAPI.h"
#include "utilDataMgrAPI.h"

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

//--------------------------------CONFIGURATION-------------------------------
// Audio Manager parameter storage used by data manager (16Bytes)
//--------------------------------CONFIGURATION-------------------------------
typedef struct
{    
/* --------------------------- BEGIN CUSTOM AREA ------------------------------
 * CONFIGURATION: Add types that should be saved in persistent storage by Data
 * Manager */
  BYTE          bMute;          /* Mute. unit is BOOL. Def: FALSE */
  BYTE          cVolLevel;   /* Volume Level. unit is Lv. Range: AUDIO_VOLUME_MIN~MAX */
  BYTE          acReserved[16-2];
/* ---------------------------- END CUSTOM AREA --------------------------- */
}GCC_PACKED AUDIOMGR_NVRAM_DATA, *PAUDIOMGR_NVRAM_DATA;

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************

// ---------------------------------------------------------------------------  
// Audio Parameters
// ---------------------------------------------------------------------------  
#define AUDIO_VOLUME_MIN      0
#define AUDIO_VOLUME_MAX      10
#define AUDIO_VOLUME_INC      1
#define AUDIO_VOLUME_DEFAULT  5

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
void appAudioMgrInitialize(void);

/**
 *  @brief      This API Release the internal state of component.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appAudioMgrRelease(void);

/**
 *  @brief      This API completes the previously started audio broadcast standard detect
                    or volume adjust processing requiring non-blocking functionality.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appAudioMgrProcess(void);

/**
 *  @brief      This API Play the specified sound id
 *  @param[in]  eSoundId: Sound Id to play
                bPlayAfterStop: TRUE to Play after stop the last audio stream
 *  @param[out] None
 *  @return     TRUE if played, FALSE for others
 *  @note       None
 *  @attention  None
 */
BOOL appAudioMgrPlay(eSOUND_ID eSoundId, BOOL bPlayAfterStop);

/**
 *  @brief      This API return loop Play enabled
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Loop play enabled, FALSE to others
 *  @note       None
 *  @attention  None
 */
BOOL appAudioMgrPlayLoopEnabled(void);

/**
 *  @brief      This API loop Play the specified sound id 
 *  @param[in]  eSoundId: Sound Id to play
                        wTicks: Tick count to play
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appAudioMgrPlayLoop(eSOUND_ID eSoundId, WORD wTicks);

/**
 *  @brief      This API Set the volume
 *  @param[in]  cVolLevel: Volume Level to set
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appAudioMgrVolumeLevelSet(BYTE cVolume);

/**
 *  @brief      This API Get the current volume
 *  @param[in]  None
 *  @param[out] None
 *  @return     current volume
 *  @note       None
 *  @attention  None
 */
BYTE appAudioMgrVolumeLevelGet(void);

/**
 *  @brief      This API Set the Audio Mute
 *  @param[in]  bMute: TRUE to Mute, FALSE to unmute
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appAudioMgrMuteSet(BOOL bMute);

/**
 *  @brief      This API Get the Audio mute status
 *  @param[in]  None
 *  @param[out] None
 *  @return     Audio mute status
 *  @note       None
 *  @attention  None
 */
BOOL appAudioMgrMuteGet(void);

/**
 *  @brief      This API Set the Audio Amp Mute
 *  @param[in]  bMute: TRUE to Mute, FALSE to unmute
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appAudioMgrAmpMuteSet(BOOL bMute);

/**
 *  @brief      This API Get the Audio Amp mute status
 *  @param[in]  None
 *  @param[out] None
 *  @return     Amp mute status
 *  @note       None
 *  @attention  None
 */
BOOL appAudioMgrAmpMuteGet(void);

/**
 *  @brief      This API is the persistent storage manager call back routine used by utilDataMgr.c. 
 *  @param[in]  eNodeID: Identifier of the data node associated with this event..
 *  @param[out] eEvent: Identifier of the external event.
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appAudioMgrNodeCallback(eNODE_NAME eNodeID,
                             eNV_EVENT  eEvent);

#endif //#if !defined(_APPAUDIOMGRAPI_H_)

