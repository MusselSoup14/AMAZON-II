/**
  ******************************************************************************
  * @file    locales.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    16-August-2014
  * @brief   This file provides locales Type
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
#include "common.h"

#if !defined(_INCLUDED_LOCALES_H_)
#define _INCLUDED_LOCALES_H_


// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

#if !defined(NDEBUG)
// ---------------------------------------------------------------------------
// enum for string
// ---------------------------------------------------------------------------
typedef enum
{
  strSHARP,
  strMODEL_NAME,    
  strDRV_INIT_SUCCESS,
  strDRV_INT_FAIL,
  strCMD_WRONG,
  strVALUE_SAME,
  str_INVALID, // Sentinel
}eSTRING;

// ---------------------------------------------------------------------------
//  Locale String Configuration
// ---------------------------------------------------------------------------
EXTERN CROMDATA PCHAR g_pacLocales[];
#endif

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

#endif // _INCLUDED_LOCALES_H_

