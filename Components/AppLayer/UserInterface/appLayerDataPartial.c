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

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS **************
// ***************************************************************************
CROMDATA IMGINFO g_asImageObj[] = 
{
  // 0
  {LAYER_IMAGE_DIR"SS_STATIC_Product"DEF_EXTEN, DEF_FRMT},      // goSS_STATIC_Product
  {LAYER_IMAGE_DIR"SS_STATIC_Product_Sub"DEF_EXTEN, DEF_FRMT},  // goSS_STATIC_Product_Sub
  {LAYER_IMAGE_DIR"SS_STATIC_Loading"DEF_EXTEN, DEF_FRMT},      // goSS_STATIC_Loading
  {LAYER_IMAGE_DIR"SS_STATIC_Vendor"DEF_EXTEN, DEF_FRMT},       // goSS_STATIC_Vendor
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved  
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved

  // 11
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved           

  // 21
  {LAYER_IMAGE_DIR"LB_STATIC_Title"DEF_EXTEN, DEF_FRMT},      // goLB_STATIC_Title
  {LAYER_IMAGE_DIR"LB_STATIC_Copyright"DEF_EXTEN, DEF_FRMT},  // goLB_STATIC_Copyright
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved   

  // 31
  {LAYER_IMAGE_DIR"LB_PHOTO_BTN_1_UnSel"DEF_EXTEN, DEF_FRMT}, // goLB_PHOTO_BTN_1_UnSel
  {LAYER_IMAGE_DIR"LB_PHOTO_BTN_1_Sel"DEF_EXTEN, DEF_FRMT},   // goLB_PHOTO_BTN_1_Sel
  {LAYER_IMAGE_DIR"LB_PHOTO_BTN_2_UnSel"DEF_EXTEN, DEF_FRMT}, // goLB_PHOTO_BTN_2_UnSel
  {LAYER_IMAGE_DIR"LB_PHOTO_BTN_2_Sel"DEF_EXTEN, DEF_FRMT},   // goLB_PHOTO_BTN_2_Sel
  {LAYER_IMAGE_DIR"LB_PHOTO_BTN_3_UnSel"DEF_EXTEN, DEF_FRMT}, // goLB_PHOTO_BTN_3_UnSel
  {LAYER_IMAGE_DIR"LB_PHOTO_BTN_3_Sel"DEF_EXTEN, DEF_FRMT},   // goLB_PHOTO_BTN_3_Sel
  {LAYER_IMAGE_DIR"LB_PHOTO_BTN_4_UnSel"DEF_EXTEN, DEF_FRMT}, // goLB_PHOTO_BTN_4_UnSel
  {LAYER_IMAGE_DIR"LB_PHOTO_BTN_4_Sel"DEF_EXTEN, DEF_FRMT},   // goLB_PHOTO_BTN_4_Sel
  {LAYER_IMAGE_DIR"LB_PHOTO_BTN_5_UnSel"DEF_EXTEN, DEF_FRMT}, // goLB_PHOTO_BTN_5_UnSel
  {LAYER_IMAGE_DIR"LB_PHOTO_BTN_5_Sel"DEF_EXTEN, DEF_FRMT},   // goLB_PHOTO_BTN_5_Sel     

  // 41
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved     

  // 51
  {LAYER_IMAGE_DIR"MM_PHOTO_1"DEF_EXTEN, DEF_FRMT}, // goMM_PHOTO_1
  {LAYER_IMAGE_DIR"MM_PHOTO_2"DEF_EXTEN, DEF_FRMT}, // goMM_PHOTO_2
  {LAYER_IMAGE_DIR"MM_PHOTO_3"DEF_EXTEN, DEF_FRMT}, // goMM_PHOTO_3
  {LAYER_IMAGE_DIR"MM_PHOTO_4"DEF_EXTEN, DEF_FRMT}, // goMM_PHOTO_4
  {LAYER_IMAGE_DIR"MM_PHOTO_5"DEF_EXTEN, DEF_FRMT}, // goMM_PHOTO_5
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved  
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT}, // Reserved    

  // 61
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT},               // Reserved 
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT},               // Reserved 
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT},               // Reserved 
  {LAYER_IMAGE_DIR"MM_BTN_Navi_Prev_UnSel.png", ffmtPNG},     // goMM_NAVI_BTN_Prev_UnSel
  {LAYER_IMAGE_DIR"MM_BTN_Navi_Prev_Sel.png", ffmtPNG},       // goMM_NAVI_BTN_Prev_Sel
  {LAYER_IMAGE_DIR"MM_BTN_Navi_Home_UnSel.png", ffmtPNG},     // goMM_NAVI_BTN_Home_UnSel
  {LAYER_IMAGE_DIR"MM_BTN_Navi_Home_Sel.png", ffmtPNG},       // goMM_NAVI_BTN_Home_Sel
  {LAYER_IMAGE_DIR"MM_BTN_Navi_Next_UnSel.png", ffmtPNG},     // goMM_NAVI_BTN_Next_UnSel
  {LAYER_IMAGE_DIR"MM_BTN_Navi_Next_Sel.png", ffmtPNG},       // goMM_NAVI_BTN_Next_Sel
  {LAYER_IMAGE_DIR"Spacer"DEF_EXTEN, DEF_FRMT},               // Reserved    
};

CROMDATA RECTINFO g_asRectObj[] = 
{
  {0x00,0x00,0x00, 0},    // Common Rect.
  {0xD1,0xD2,0xD4, 0},    // goSS_STATIC_Background  
  {0x58,0x58,0x5A, 0},    // goSS_RECT_ProgressFrame
  {0xD1,0xD2,0xD4, 0},    // goSS_RECT_ProgressBack
  {0xB9,0xB9,0xC8, 0},    // goSS_RECT_Progress
  {0xFF,0xFF,0xFF, 0},    // goLB_STATIC_Background
  {0x99,0x99,0x99, 0},    // goMM_STATIC_Background, goMM_PHOTO_Erase
};


