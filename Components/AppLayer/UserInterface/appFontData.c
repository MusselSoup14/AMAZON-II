/**
  ******************************************************************************
  * @file    appFontData.c
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
#include "appFontData.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS **************
// ***************************************************************************
#if USE_FONT_DRAW
CROMDATA FONTINFO g_asFontObj[fdINVALID] = 
{
  {LAYER_FONT_DIR"MyriadPro_17.fnt"}, // fdMYRIADPRO_17
  {LAYER_FONT_DIR"Arial_24.fnt"}, // fdARIAL_24
};
#endif

