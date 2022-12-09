/**
  ******************************************************************************
  * @file    utilParameterTable.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    09-Mar-2018
  * @brief   This file provides the Host interface command
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
  * <h2><center>&copy; Copyright 1997-2018 (c) MadeFactory Inc.</center></h2>
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#ifndef _UTILPARAMETERTABLE_H
#define _UTILPARAMETERTABLE_H

#include "common.h"

// ---------------------------------------------------------------------------
// parameter descriptor structure
// ---------------------------------------------------------------------------
typedef struct
{
  INT                 nMin;          // min value in display ordinates
  INT                 nMax;          // max value in display ordinates
  INT                 nInc;          // inc/dec value in display ordinates
  INT                 nPageInc;          // inc/dec page(jog-press) value in display ordinates
  INT                 nDefault;      // default value in display ordinates
}GCC_PACKED PARAM_INFO, *PPARAM_INFO;
typedef const PARAM_INFO *PCPARAM_INFO;

// ------------------------------------------------------------------
// enum for Param Info
// ------------------------------------------------------------------
typedef enum
{ 
  /* Application Specified */
  sysPAR_UI_PhotoId,

  /* Sentinel. do not remove */
  sysPAR_INVALID
}GCC_PACKED eSYS_PARAM;

// ============================================================================
// CONFIGURATION: Custom parameter defines. Define
//                min, max, inc, and default values in display ordinates.
// ============================================================================

#endif // _UTILPARAMETERTABLE_H
