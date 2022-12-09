/**
  ******************************************************************************
  * @file    common.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    24-March-2016
  * @brief   This file provides common TYPES, SYMBOLS, MACROS and so on
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

#if !defined(_INCLUDED_COMMON_H_)
#define _INCLUDED_COMMON_H_

/* Includes ------------------------------------------------------------------*/
//#include <stdio.h>
//#include <stdlib.h>
#include <stddef.h>
//#include <string.h>
//#include "sdk.h"
#include "BSPDefs.h"

// TODO: Move to PreCompiler to Compiler Option belows
//#define NDEBUG
#define USE_FULL_ASSERT

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif // __cplusplus

#ifndef TRUE
#define FALSE    0
#define TRUE     (!FALSE)
#endif

#ifndef NULL
#define NULL     ((void *) 0)
#endif

#define BREAK_PT while(1){ ; }

#define  until(exp)  while(!(exp))   /* Usage:  do { ... } until (exp);  */

/*
 *   Some of these typedefs may need to be changed so that the data sizes implied by the
 *   names of the generic types, int8, int16, etc, are correct for the target processor.
 *   Use uint16 and uint32 in preference to ushort and ulong (resp) in new projects.
 */
typedef void VOID, *PVOID; // void
typedef char /*CHAR,*/ *PCHAR; // signed 8bit
typedef short /*SHORT,*/ *PSHORT; // signed 16bit
typedef int /*INT,*/ *PINT;
typedef long /*LONG,*/ *PLONG; // signed 32bit

typedef unsigned char /*BYTE,*/ *PBYTE; // unsigned 8bit
typedef const BYTE *PCBYTE;
typedef unsigned short /*WORD,*/ *PWORD; // unsigned 16bit
typedef const WORD *PCWORD;
/*typedef unsigned int BIT8, UINT;*/
typedef unsigned long /*DWORD,*/ *PDWORD; // unsigned 32bit
typedef unsigned long long QWORD, *PQWORD; // unsigned 64bit
typedef float FLOAT, *PFLOAT; // floating number

typedef volatile void VVOID, *VPVOID; // volatile void
typedef volatile char VCHAR, *VPCHAR; // volatile signed 8bit
typedef volatile short VSHORT, *VPSHORT; // volatile signed 16bit
typedef volatile unsigned int VUINT, *VPUINT; // volatile unsigned int
typedef volatile long VLONG, *VPLONG; // volatile signed 32bit

typedef volatile BYTE VBYTE, *VPBYTE; // volatile unsigned 8bit
typedef volatile WORD VWORD, *VPWORD; // volatile unsigned 16bit
typedef volatile DWORD VDWORD, *VPDWORD; // volatile unsigned 32bit

/* avoid confliction
#ifndef bool
typedef unsigned char bool, BOOL, *PBOOL;
#else
typedef bool BOOL, *PBOOL;
#endif
*/

typedef void (*pfnvoid)(void); // pointer to void function

#define CROMDATA const

/*
 *   Macros for byte/word/register bit manipulation.
 *   NB: TEST_BIT(...) evaluates to *any* non-zero value to indicate TRUE.
 */
#define  TEST_BIT(entity, bitmask)   ((entity) & (bitmask))
//#define  SET_BIT(entity, bitmask)    ((entity) |= (bitmask)) // Defined in "stm32f10x.h"
//#define  CLEAR_BIT(entity, bitmask)  ((entity) &= ~(bitmask)) // Defined in "stm32f10x.h"

#define  BIT_0   0x01     /*** BIT MASKS ***/
#define  BIT_1   0x02
#define  BIT_2   0x04
#define  BIT_3   0x08
#define  BIT_4   0x10
#define  BIT_5   0x20
#define  BIT_6   0x40
#define  BIT_7   0x80
#define  BIT_8   0x0100
#define  BIT_9   0x0200
#define  BIT_10  0x0400
#define  BIT_11  0x0800
#define  BIT_12  0x1000
#define  BIT_13  0x2000
#define  BIT_14  0x4000
#define  BIT_15  0x8000

/*
 *   Other useful macros...
 */
#ifdef  LITTLE_ENDIAN   /* If bytes of unsigned word are in little-endian order... */
#define SWAP(w)   (w)   /* ... no change  */
#else                   /* If big-endian ... swap order of bytes */
#define SWAP(w)   ((((w) & 0xFF) << 8) | (((w) >> 8) & 0xFF))
#endif

// ---------------------------------------------------------------------------
// MSB (Most Significant Byte) LSB (Least Significant Byte) Macros operate on
// WORDs and return a BYTEs
// ---------------------------------------------------------------------------
#define MSB(d)              ((BYTE)((d) >> 8))
#define LSB(d)              ((BYTE)((d) & 0x00ff))
#define HI_BYTE(w)  (((w) >> 8) & 0xFF)   /* Extract high-order byte from unsigned word */
#define LO_BYTE(w)  ((w) & 0xFF)          /* Extract low-order byte from unsigned word */

#define LESSER_OF(arg1,arg2) ((arg1)<=(arg2)?(arg1):(arg2))

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define MATH_PI       3.141592654f // PI
#define MATH_2PI      6.283185307f // PI*2
#define MATH_PI_DIV2  1.570796327f // PI/2

// ---------------------------------------------------------------------------
// MACRO to return the absolute value of x
// ---------------------------------------------------------------------------
#define ABS(x) ( (x)<0 ? -(x) : (x))

// ---------------------------------------------------------------------------
// MACRO to return the absolute value of the difference of x and y
// ---------------------------------------------------------------------------
#define ABS_DIFF( x, y) ( (x)>=(y) ? ((x)-(y)) : ((y)-(x)) )

// ---------------------------------------------------------------------------
// MACRO to calculate the standard deviation of x
// Percentage deviation calculated like: |x - y|/y
// Units are tenths of percent, i.e. a result of 10 indicates 1% deviation
// ---------------------------------------------------------------------------
#define ABS_PERCENT_DEVIATION( x, y) (ABS_DIFF((x),(y))*1000)/(y)

// ---------------------------------------------------------------------------
// MACRO to return the minimum of 2 numbers
// ---------------------------------------------------------------------------
#define MIN(A,B)    ((A) < (B) ? (A) : (B))

// ---------------------------------------------------------------------------
// MACRO to return the maximum of 2 numbers
// ---------------------------------------------------------------------------
#define MAX(A,B)    ((A) > (B) ? (A) : (B))

// ---------------------------------------------------------------------------
// MACRO to devide two numbers x/y and round up when the remainder is >= 0.5
// ---------------------------------------------------------------------------
#define ROUND(x,y)   (((x)+((y)>>1))/(y))

// ---------------------------------------------------------------------------
// MACRO to devide two numbers x/y and round up when there is any remainder
// ---------------------------------------------------------------------------
#define CEILING(x,y) (((x)+((y)-1))/(y))

// ---------------------------------------------------------------------------
// MACRO to round up to nearest even number
// ---------------------------------------------------------------------------
#define ROUND_UP_EVEN(x) ((ROUND((x),2)) * 2)

/***** Commonly used symbollic constants *****/

//#define  ERROR   (-1) // Defined in "stm32f10x.h"
//#define  FAIL    (-1) // Defined in "stm32f10x.h"
//#define  SUCCESS   0 // Defined in "stm32f10x.h"
#define  OFF       0
#define  ON        1
//#define  DISABLE   0 // Defined in "stm32f10x.h"
//#define  ENABLE    1 // Defined in "stm32f10x.h"
#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1

// ---------------------------------------------------------------------------
// ASSERT
// ---------------------------------------------------------------------------
#if defined(NDEBUG)
#define ASSERF(isOk, comment)
#define ASSERT_APP(isOk, comment)
#define ASSERT(isOk)
#define ASSERT_ALWAYS()
#else
#define ASSERTF(isOk, comment)\
	{\
		if(!(isOk)){\
			commAssertFailed(comment,__FILE__,__LINE__,__FUNCTION__);\
		}\
	}
#define ASSERT_APP(isOk, comment)    ASSERTF(isOk, comment)
#define ASSERT(isOk)                                  ASSERTF(isOk, "")
#define ASSERT_ALWAYS()      ASSERT(0)
#endif


// ---------------------------------------------------------------------------
// Delay
// ---------------------------------------------------------------------------
#if 0
#define commDelayUS(DELAY) usleep(DELAY)
#define commDelayMS(DELAY) usleep(DELAY*1000)
#define commDelayS(DELAY) sleep(DELAY)
#else
#define commDelayUS(DELAY) DelayUs((WORD)DELAY)
#define commDelayMS(DELAY) DelayMs((WORD)DELAY)
//#define commDelayS(DELAY) DelayMs(DELAY*1000)
#endif

// ---------------------------------------------------------------------------
// msgxxx() Level
// ---------------------------------------------------------------------------
extern BYTE g_cDebugPrintLevel;

#define COMN_MSG_NONE     0x00
#define COMN_MSG_ERR      (0x01 << 0)
#define COMN_MSG_WARN   (0x01 << 1)
#define COMN_MSG_DBG      (0x01 << 2)
#define COMN_MSG_INFO     (0x01 << 3)
#define COMN_MSG_RAW      (0x01 << 4)
#define COMN_MSG_ALL       0xFF

//---------------------------------------------------------------------------
// Set structure to 0's
//---------------------------------------------------------------------------
#define ZERO_STRUCTURE(x) memset((PVOID)&x, 0, sizeof(x))

#define GCC_PACKED __attribute__ ((packed)) // for GCC
//#define IAR_PACKED __packed // for IAR

//---------------------------------------------------------------------------
// Color ANSI code for Terminal
//---------------------------------------------------------------------------
#if 0
#define PRINT_ANSI_RESET    "\33[0m"
#define PRINT_FG_BLACK      "\33[30m"
#define PRINT_FG_RED        "\33[31m"
#define PRINT_FG_GREEN      "\33[32m"
#define PRINT_FG_YELLOW     "\33[33m"
#define PRINT_FG_BLUE       "\33[34m"
#define PRINT_FG_MAGENTA    "\33[35m"
#define PRINT_FG_CYAN       "\33[36m"
#define PRINT_FG_WHITE      "\33[37m"
#define PRINT_FG_UNDERLINE  "\33[4m"
#define PRINT_FG_BOLD       "\33[1m"

#define PRINT_BG_BLACK      "\33[40m"
#define PRINT_BG_RED        "\33[41m"
#define PRINT_BG_GREEN      "\33[42m"
#define PRINT_BG_YELLOW     "\33[43m"
#define PRINT_BG_BLUE       "\33[44m"
#define PRINT_BG_MAGENTA    "\33[45m"
#define PRINT_BG_CYAN       "\33[46m"
#define PRINT_BG_WHITE      "\33[47m"
#else
#define PRINT_ANSI_RESET ""
#define PRINT_FG_BLACK ""
#define PRINT_FG_RED ""
#define PRINT_FG_GREEN ""
#define PRINT_FG_YELLOW ""
#define PRINT_FG_BLUE ""
#define PRINT_FG_MAGENTA ""
#define PRINT_FG_CYAN ""
#define PRINT_FG_WHITE ""
#define PRINT_FG_UNDERLINE ""
#define PRINT_FG_BOLD ""

#define PRINT_BG_BLACK ""
#define PRINT_BG_RED ""
#define PRINT_BG_GREEN ""
#define PRINT_BG_YELLOW ""
#define PRINT_BG_BLUE ""
#define PRINT_BG_MAGENTA ""
#define PRINT_BG_CYAN ""
#define PRINT_BG_WHITE ""
#endif


// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************
#if defined(NDEBUG)
#define PRINTF(ARGS...)
// Message Info printf
#define msgInfo(ARGS...) 
// Message Error printf 
#define msgErr(ARGS...) 
// Message Warnning printf 
#define msgWarn(ARGS...) 
// Message Debug printf 
#define msgDbgR(ARGS...) 
// Message Debug printf 
#define msgDbg(ARGS...) 
// Message Raw printf 
#define msgRawR(ARGS...) 
// Message Raw printf without "\n" 
#define msgRaw(ARGS...) 
#else
#define PRINTF(ARGS...) printf(ARGS)
// Message Info printf
#define msgInfo(ARGS...) if(g_cDebugPrintLevel & COMN_MSG_INFO)\
                                               {\
                                                  printf(PRINT_FG_GREEN "[INF] ");\
                                                  printf(ARGS); \
                                                  printf("\r\n" PRINT_ANSI_RESET);\
                                                }
// Message Error printf 
#define msgErr(ARGS...)  if(g_cDebugPrintLevel & COMN_MSG_ERR)\
                                              {\
                                                printf(PRINT_FG_RED "[ERR:%s] ", __FUNCTION__);\
                                                printf(ARGS);\
                                                printf(PRINT_ANSI_RESET "\r\n");\
                                              }
// Message Warnning printf 
#define msgWarn(ARGS...)  if(g_cDebugPrintLevel & COMN_MSG_WARN)\
                                                  {\
                                                    printf(PRINT_FG_YELLOW "[WRN] ");\
                                                    printf(ARGS);\
                                                    printf(PRINT_ANSI_RESET "\r\n");\
                                                  }
// Message Debug printf 
#define msgDbgR(ARGS...)  if(g_cDebugPrintLevel & COMN_MSG_DBG)\
                                                  {\
                                                    printf(PRINT_FG_WHITE "[DBG] ");\
                                                    printf(ARGS);\
                                                    printf(PRINT_ANSI_RESET);\
                                                  }
// Message Debug printf 
#define msgDbg(ARGS...)  if(g_cDebugPrintLevel & COMN_MSG_DBG)\
                                                {\
                                                  printf(PRINT_FG_UNDERLINE PRINT_FG_WHITE "[DBG] ");\
                                                  printf(ARGS); \
                                                  printf(PRINT_ANSI_RESET"\r\n");\
                                                }
// Message Raw printf 
#define msgRawR(ARGS...) if(g_cDebugPrintLevel & COMN_MSG_RAW){printf(ARGS);} 
// Message Raw printf without "\n" 
#define msgRaw(ARGS...) if(g_cDebugPrintLevel & COMN_MSG_RAW){printf(ARGS); printf("\r\n");}
#endif

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *	@brief		Delay per micro second.
 *	@param[in]	wTimeUS
 *	@return	None
 */
void DelayUs(WORD wTimeUS );

/**
 *	@brief		Delay per mili second.
 *	@param[in]	time_ms
 *	@return	None
 */
void DelayMs(WORD wTimeMS);

/**
 *	@brief		Print progress
 *	@param[in]	pcString	 string of printing
 *	@return	None
 */
EXTERN void msgProg( PCHAR pcString );

/**
 *	@brief		string to Upper-case
 *	@param[in]	pcString	 string to convert
 *	@return	None
 */
EXTERN void commStrtoUp( PCHAR pcString );

/**
 *	@brief		string to Lower-case
 *	@param[in]	pcString	 string to convert
 *	@return	None
 */
EXTERN void commStrtoLow( PCHAR pcString );

/**
 *	@brief		message convert to string
 *	@param[in]	pacBuffer: pointer to return back string
                        cLength: size to convert
                        pcStr: string to convert as args
 *	@return	None
 */
EXTERN void commArgsToStr(PBYTE pacBuffer, BYTE cLength, PCHAR pcStr, ...);

#if !defined(NDEBUG)
/**
 *	@brief		Assert function
 *	@param[in]	comment	 string of printing
 *	@return	None
 */
EXTERN void commAssertFailed ( const char* comment, const char* file, int line,
					 const char* function );
#endif

/**
 *  @brief      This API perform get strings from specified interface
 *  @param[out] pacBuffer : Pointer to return back string
 *  @param[in]  cCount : Size to read-out
 *  @param[in]  cEcho : 0 for no echo, 1 for normal, other for wild
 *  @return     Read-out size
 *  @note       None
 *  @attention  None
 */
BYTE commGets(PCHAR pacBuffer, BYTE cCount, BYTE cEcho);


// ***************************************************************************
// *********************** COMPONENT HEADERS INCLUDE ************************
// ***************************************************************************
#include "version.h" // version definitions, From ctrlSRC
#include "locales.h"
//#include "path.h" // Path definitions
#include "utilStructs.h" // Structure declaration
#include "CustomDefs.h" // Custom definitions

#endif // _INCLUDED_COMMON_H_

