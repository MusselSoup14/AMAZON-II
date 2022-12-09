/**
  ******************************************************************************
  * @file    utilStructs.h
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
#include "common.h"

#if !defined(_INCLUDED_UTILSTRUCTS_H_)
#define _INCLUDED_UTILSTRUCTS_H_

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
// ---------------------------------------------------------------------------
// Result returned from functions in the system
// ---------------------------------------------------------------------------
typedef enum 
{
	rcSUCCESS, 
  rcERROR, 
  
  rcINVALID
}GCC_PACKED eRESULT;

typedef enum 
{
	nbSUCCESS,  // Request successfully completed
	nbERROR,    // A failure occurred
	nbPENDING,  // Non-blocking request is pending
			        // (operation has not completed yet)
	nbINVALID
}GCC_PACKED eNBRESULT;

// ---------------------------------------------------------------------------
// Structure for Host IF Colsole Command
// ---------------------------------------------------------------------------
typedef struct tagLOCAL_CMD
{
//  BOOL bCaseMatch;
  PCHAR pcCmdString;                     		// command string
  int (*fpCallBack)(int argc, char **argv);		// callback function
} LOCAL_CMD, *PLOCAL_CMD;

#if 1
typedef struct 
{
  WORD wLeft;
  WORD wTop;
  WORD wRight;
  WORD wBottom;
} RECTG, *PRECTG;
#endif

typedef const RECT *PCRECT; // declares pointer to constant data
typedef RECT * const CPRECT; // declares constant pointer
typedef PCRECT const CPCRECT; // declares constant pointer to
// constant data

typedef struct
{
	WORD wX;
	WORD wY;
} SIZE, *PSIZE;
typedef const SIZE *PCSIZE;

typedef struct 
{
	WORD wX;
	WORD wY;
} POINT, *PPOINT;
typedef const POINT *PCPOINT;

// ---------------------------------------------------------------------------
// color definition data strucutre
// ---------------------------------------------------------------------------
typedef struct tagRGB
{
    BYTE                cRed;          // red color component
    BYTE                cGreen;        // green color component
    BYTE                cBlue;         // blue color component
}GCC_PACKED RGB, *PRGB;
typedef const RGB *PCRGB;

// ---------------------------------------------------------------------------
// 32bits SoC Register and Value
// ---------------------------------------------------------------------------
typedef struct tagSOCREGVAL
{
  UINT          nAddress;
  UINT          nValue;
}GCC_PACKED SOC_REGVAL, *PSOC_REGVAL;
typedef CROMDATA SOC_REGVAL *PCSOC_REGVAL;

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************


#endif //#if !defined(_INCLUDED_UTILSTRUCTS_H_)


