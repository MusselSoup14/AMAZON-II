/**
  ******************************************************************************
  * @file    appLayerData.c
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
#include "appLayerData.h"

#define DEF_EXTEN     ".bmp"
#define DEF_FRMT  ffmtBMP
// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS **************
// ***************************************************************************
#include "appLayerDataPartial.c"

CROMDATA DRAWABLE g_asObjSplash[] = 
{
  {{0,0,PANE_WIDTH_MAX,PANE_HEIGHT_MAX}, dtRECT, 1}, // goSS_STATIC_Background,
    
  {{311,290,0,0}, dtBITMAP, 0},       // goSS_STATIC_Product,
  {{514,402,0,0}, dtBITMAP, 1},       // goSS_STATIC_Product_Sub,
  {{583,456,0,0}, dtBITMAP, 2},       // goSS_STATIC_Loading,
  {{568,737,0,0}, dtBITMAP, 3},       // goSS_STATIC_Vendor,

  {{439,436,402,9}, dtRECT, 2},       // goSS_RECT_ProgressFrame,
  {{440,437,400,7}, dtRECT, 3},       // goSS_RECT_ProgressBack,
  {{440,437,50,7}, dtRECT, 4},        // goSS_RECT_Progress,  

  {{16,737,33,21}, dtRECT, 0},        // goSS_TDV_Version,    
};

CROMDATA DRAWABLE g_asObjLaunchar[] = 
{

  {{0,0,PANE_WIDTH_MAX,PANE_HEIGHT_MAX}, dtRECT, 5}, // goLB_STATIC_Background,

  {{22,26,0,0}, dtBITMAP, 21},           // goLB_STATIC_Title,
  {{22,768,0,0}, dtBITMAP, 22},         // goLB_STATIC_Copyright,

  {{53,79,0,0}, dtBITMAP, 31},          // goLB_PHOTO_BTN_1_UnSel,
  {{53,79,0,0}, dtBITMAP, 32},          // goLB_PHOTO_BTN_1_Sel,

  {{506,79,0,0}, dtBITMAP, 33},         // goLB_PHOTO_BTN_2_UnSel,
  {{506,79,0,0}, dtBITMAP, 34},         // goLB_PHOTO_BTN_2_Sel,

  {{956,79,0,0}, dtBITMAP, 35},         // goLB_PHOTO_BTN_3_UnSel,
  {{956,79,0,0}, dtBITMAP, 36},         // goLB_PHOTO_BTN_3_Sel,

  {{506,367,0,0}, dtBITMAP, 37},        // goLB_PHOTO_BTN_4_UnSel,
  {{506,367,0,0}, dtBITMAP, 38},        // goLB_PHOTO_BTN_4_Sel,

  {{818,494,0,0}, dtBITMAP, 39},        // goLB_PHOTO_BTN_5_UnSel,
  {{818,494,0,0}, dtBITMAP, 40},        // goLB_PHOTO_BTN_5_Sel,
};


CROMDATA DRAWABLE g_asObjMenuMain[] = 
{

  {{0,0,PANE_WIDTH_MAX,PANE_HEIGHT_MAX}, dtRECT, 6}, // goMM_STATIC_Background, 

  {{0,0,PANE_WIDTH_MAX,PANE_HEIGHT_MAX}, dtRECT, 6}, // goMM_PHOTO_Erase,
  {{406,0,0,0}, dtBITMAP, 51},        // goMM_PHOTO_1,
  {{0,40,0,0}, dtBITMAP, 52},         // goMM_PHOTO_2,
  {{374,0,0,0}, dtBITMAP, 53},        // goMM_PHOTO_3,
  {{374,0,0,0}, dtBITMAP, 54},        // goMM_PHOTO_4,
  {{40,0,0,0}, dtBITMAP, 55},         // goMM_PHOTO_5,

  {{487,698,0,0}, dtBITMAP, 64},      // goMM_NAVI_BTN_Prev_UnSel,
  {{487,698,0,0}, dtBITMAP, 65},      // goMM_NAVI_BTN_Prev_Sel,

  {{587,698,0,0}, dtBITMAP, 66},      // goMM_NAVI_BTN_Home_UnSel,
  {{587,698,0,0}, dtBITMAP, 67},      // goMM_NAVI_BTN_Home_Sel,

  {{693,698,0,0}, dtBITMAP, 68},      // goMM_NAVI_BTN_Next_UnSel,
  {{693,698,0,0}, dtBITMAP, 69},      // goMM_NAVI_BTN_Next_Sel,
};

CROMDATA LAYER_DATA g_asLayers[] = 
{
  {g_asObjSplash},      // ldSPLASH
  {g_asObjLaunchar},    // ldLAUNCHBAR
  {g_asObjMenuMain},    // ldMENU_MAIN
};

