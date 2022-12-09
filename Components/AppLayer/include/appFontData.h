/**
  ******************************************************************************
  * @file    appFontData.h
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
#ifndef _APPFONTDATA_H
#define _APPFONTDATA_H

#include "appGUITypes.h"

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

#if USE_FONT_DRAW
typedef enum
{
  fdMYRIADPRO_17,
  fdARIAL_24,
    
  fdINVALID
}eFONT_DATA;

extern CROMDATA FONTINFO g_asFontObj[fdINVALID];
#endif

#endif // _APPFONTDATA_H

