/**
  ******************************************************************************
  * @file    appLayerData.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    24-March-2016
  * @brief   This file provides the main procedure
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
#ifndef _APPLAYERDATA_H
#define _APPLAYERDATA_H

#include "appGUITypes.h"

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

typedef enum
{
  ldSPLASH, 
  ldLAUNCHBAR,
  ldMENU_MAIN,
  
  ldINVALID,
}eLAYER_DATA;

typedef enum
{
  goSS_STATIC_Background,
    
  goSS_STATIC_Product,
  goSS_STATIC_Product_Sub,
  goSS_STATIC_Loading,
  goSS_STATIC_Vendor,
  
  goSS_RECT_ProgressFrame,
  goSS_RECT_ProgressBack,
  goSS_RECT_Progress,
  
  goSS_TDV_Version,
  
  goSS_INVALID // Sentinel
}eGO_SPLASH;

typedef enum
{
  goLB_STATIC_Background,

  goLB_STATIC_Title,
  goLB_STATIC_Copyright,

  goLB_PHOTO_BTN_1_UnSel,
  goLB_PHOTO_BTN_1_Sel,

  goLB_PHOTO_BTN_2_UnSel,
  goLB_PHOTO_BTN_2_Sel,

  goLB_PHOTO_BTN_3_UnSel,
  goLB_PHOTO_BTN_3_Sel,

  goLB_PHOTO_BTN_4_UnSel,
  goLB_PHOTO_BTN_4_Sel,

  goLB_PHOTO_BTN_5_UnSel,
  goLB_PHOTO_BTN_5_Sel,

  goLB_INVALID  
}eGO_LAUNCHBAR;

typedef enum
{
  goMM_STATIC_Background,

  goMM_PHOTO_Erase,
  goMM_PHOTO_1,
  goMM_PHOTO_2,
  goMM_PHOTO_3,
  goMM_PHOTO_4,
  goMM_PHOTO_5,
  
  goMM_NAVI_BTN_Prev_UnSel,
  goMM_NAVI_BTN_Prev_Sel,

  goMM_NAVI_BTN_Home_UnSel,
  goMM_NAVI_BTN_Home_Sel,

  goMM_NAVI_BTN_Next_UnSel,
  goMM_NAVI_BTN_Next_Sel,
  
  goMM_INVALID  
}eGO_MENU_MAIN;


#define IMAGE_OBJECT_COUNT      71 // do not forget index+1
#define RECT_OBJECT_COUNT       7  // do not forget index+1
#define IMAGE_OBJECT_SS_COUNT   21  // do not forget index+1

extern CROMDATA IMGINFO     g_asImageObj[];
extern CROMDATA RECTINFO    g_asRectObj[];
extern CROMDATA LAYER_DATA  g_asLayers[];

#endif // _APPLAYERDATA_H
