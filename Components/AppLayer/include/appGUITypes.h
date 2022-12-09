/**
  ******************************************************************************
  * @file    appGUITypes.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    24-March-2016
  * @brief   This file provides types to GUI
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
#ifndef _APPGUITYPES_H
#define _APPGUITYPES_H

#include "common.h"
#include "utilStructs.h"

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
// ---------------------------------------------------------------------------
// Types of drawable graphic items
// ---------------------------------------------------------------------------
typedef enum
{
  dtRECT,                            // rectangle object
  dtBITMAP,                          // bitmap object
  dtMULTI_TEXT,                      // multi-language text
  dtSIMPLE_TEXT,                     // single-language text
} eDRAWTYPES;

// ---------------------------------------------------------------------------
// Graphic format
// ---------------------------------------------------------------------------
typedef enum
{
  ffmtBMP,
  ffmtPNG,
  ffmtJPG,  
  ffmtSUF,
  
  ffmtINVALID
} eFILE_FORMAT;

// ---------------------------------------------------------------------------
// String justification settings
// ---------------------------------------------------------------------------
typedef enum
{
  lTOPLEFT, lTOP, lTOPRIGHT,
  lLEFT, lCENTER, lRIGHT,
  lBOTTOMLEFT, lBOTTOM, lBOTTOMRIGHT,
  lCUSTOM, lINVALID
} eLOCATE;

typedef struct tagIMGINFO
{
  PCHAR              pcPath;
  eFILE_FORMAT       eFormat;
}IMGINFO;

typedef struct tagRECTINFO
{
  BYTE cRed;
  BYTE cGreen;
  BYTE cBlue;
  BYTE cFlags; // 0x00 to transparent
}RECTINFO;

// ---------------------------------------------------------------------------
// Graphic object definition structure
// ---------------------------------------------------------------------------
typedef struct tagDRAWABLE
{
  RECT              sRect; // bounding recangle of the item
  eDRAWTYPES        eType; // Types
  WORD              wIndex;
} DRAWABLE, *PDRAWABLE;
typedef const DRAWABLE *PCDRAWABLE;

// ---------------------------------------------------------------------------
// User interface layer definition structure
// ---------------------------------------------------------------------------
typedef struct tagLAYER_DATA
{
//  WORD                wHeight;       // layer height (pixels)
//  WORD                wWidth;        // layer width (pixels)
//  BYTE                cBitCount;     // # of bits per pixel
  PCDRAWABLE          pasDrawList;   // pointer to layer draw list
} LAYER_DATA, *PLAYER_DATA;
typedef const LAYER_DATA *PCLAYER_DATA;

// Font Info Structure
typedef struct tagFONTINFO
{
  PCHAR              pcPath;
}FONTINFO;


#endif // _APPGUITYPES_H
