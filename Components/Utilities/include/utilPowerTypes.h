/**
  ******************************************************************************
  * @file    appPowerMgrAPI.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    13-August-2014
  * @brief   This file provides APIs the Power Manager
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
#ifndef __UTILPOWERTYPES_H__
#define __UTILPOWERTYPES_H__

#include "common.h"


// ***************************************************************************
// **************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
// ---------------------------------------------------------------------------
// This type defines all of the possible system power states.
// ---------------------------------------------------------------------------
typedef enum
{
  pwrSTANDBY,    // UIM Board and Analog Power Off, Main uP is activating
  pwrON,              // UIM Board and Analog Power On
  pwrSOFTRESET, // Stop Kicking Watchdog 

  pwrINVALID
}GCC_PACKED ePOWERSTATE;


// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************



// ***************************************************************************
// ******************** END OF PUBLIC CONFIGURATION SECTION ******************
// ***************************************************************************

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

#endif // __UTILPOWERTYPES_H__

