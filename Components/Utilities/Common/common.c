/**
  ******************************************************************************
  * @file    common.c
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

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <limits.h>
#include <ctype.h> //tolower(), toupper()
#include <stdarg.h>
#include "common.h"
//#include "dvCPUAPI.h"
//#include "dvGPIOAPI.h"

BYTE g_cDebugPrintLevel = COMN_MSG_ALL;
// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
#define COMN_MSG_MAX 1024
#define INPUT_KEY_LIST_SIZE_MAX		2

//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************


//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************


// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

#if defined(USE_FULL_ASSERT)
/**
 *	@brief		overriding assert_failed()
 *	@param[in]	comment	 string of printing
 *	@return	None
 */
void assert_failed(const char* pFile, int dwLine)
{   
  //dvGPIOFieldSet(gfLED_ERR, FALSE); // ERROR LED on
#if 1    
  /* Infinite looping expecting watchdog reset */
  while (1)
  {
  }
#else
  exit ( EXIT_FAILURE );
#endif
}
#endif

/**
 *	@brief		Delay per micro second.
 *	@param[in]	wTimeUS
 *	@return	None
 */
void DelayUs(WORD wTimeUS)
{
#if 1
  register WORD wI;

  /* 166CLK * 0.00990099us = 1us */
  for(wI = 0; wI < wTimeUS; wI++)    
  {
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");       //10CLK    
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");       //20CLK    
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");       //30CLK    
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");       //40CLK    
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");       //50CLK    
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");       //60CLK    
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");       //70CLK
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");       //80CLK
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");       //90CLK
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");       //100CLK
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");       //110CLK    
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");       //120CLK    
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");       //130CLK    
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");       //140CLK    
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");       //150CLK    
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");       //160CLK    
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop"); //166CLK
  }
#else
  delayus((int)wTimeUS);
#endif
}

/**
 *	@brief		Delay per mili second.
 *	@param[in]	time_ms
 *	@return	None
 */
void DelayMs(WORD wTimeMS)
{
#if 1
  register WORD wI;
  
  for(wI = 0; wI < wTimeMS; wI++)
    DelayUs(1000);

#else
  delayms(wTimeMS);
#endif
}

#if !defined(NDEBUG)
/**
 *	@brief		Assert function
 *	@param[in]	comment	 string of printing
 *	@return	None
 */
void commAssertFailed ( const char* comment, const char* file, int line,
					 const char* function )
{
  int cur_errno;
  //cur_errno = errno;  
  cur_errno = 0;  
  
  debugprintf("\r\n%s\r\n", g_pacLocales[strSHARP]);
  debugprintf("### ASSERT FAILED %s(%i):%s\r\n", file, line, function );
  debugprintf("### %s\r\n", comment );
  if ( cur_errno != 0 ) {
  	debugprintf("### errno:%d(%s)\r\n", cur_errno, strerror ( cur_errno ) );
  }
  debugprintf("%s\r\n\r\n", g_pacLocales[strSHARP]);
  
  //exit ( EXIT_FAILURE );
  
#if defined(USE_FULL_ASSERT)
  assert_failed(file, line);
#endif
}
#endif

/**
 *	@brief		Print progress
 *	@param[in]	pcString	 string of printing
 *	@return	None
 */
void msgProg( PCHAR pcString )
{
#if !defined(NDEBUG)  
  static int nProgressNum = 1;
  debugprintf("\r%s%s%s<<%.2d>> %s%s\r\n", 
    PRINT_FG_BOLD,
    PRINT_FG_BLUE, 
    PRINT_BG_WHITE, 
    nProgressNum++, 
    pcString ,
    PRINT_ANSI_RESET);
#endif  
}

/**
 *	@brief		string to upper-case
 *	@param[in]	pcString	 string to convert
 *	@return	None
 */
void commStrtoUp( PCHAR pcString )
{
  while( *pcString ){ *pcString = toupper( *pcString ); pcString++; }
}

/**
 *	@brief		string to Lower-case
 *	@param[in]	pcString	 string to convert
 *	@return	None
 */
void commStrtoLow( PCHAR pcString )
{
  while( *pcString ){ *pcString = tolower( *pcString ); pcString++; }
}

/**
 *	@brief		message convert to string
 *	@param[in]	pacBuffer: pointer to return back string
                        cLength: size to convert
                        pcStr: string to convert as args
 *	@return	None
 */
void commArgsToStr(PBYTE pacBuffer, BYTE cLength, PCHAR pcStr, ...)
{
  if(pacBuffer != NULL && pcStr != NULL)
  {
    va_list args;

    va_start(args, pcStr);
    vsnprintf(pacBuffer, cLength, pcStr, args);
    va_end(args);    
  }  
}

/**
 *  @brief      This API perform get strings from specified interface
 *  @param[out] pacBuffer : Pointer to return back string
 *  @param[in]  cCount : Size to read-out
 *  @param[in]  cEcho : 0 for no echo, 1 for normal, other for wild
 *  @return     Read-out size
 *  @note       None
 *  @attention  None
 */
BYTE commGets(PCHAR pacBuffer, BYTE cCount, BYTE cEcho)
{
  BYTE cIndex;
  int nChar;

  for(cIndex = 0; cIndex < cCount; cIndex++)
  {
#if 0
    nChar = getchar();
#else
    uart_getch(0, (PCHAR)&nChar);
#endif

    if(nChar == 0x0D) // CR
    {
      break;
    }
    else if(nChar == 0x08) // BS
    {
      if(cIndex != 0)
      {
#if 0      
        putchar('\b');
        putchar(' ');
        putchar('\b');
#else
        uart_putch(0, '\b');
        uart_putch(0, ' ');
        uart_putch(0, '\b');
#endif
        pacBuffer[--cIndex] = 0; // Clear previously character
        cIndex--;
      }
    }
    else
    {      
      pacBuffer[cIndex] = (BYTE)nChar;
      if(cEcho == 2)   
#if 0        
        putchar('*');        
#else
        uart_putch(0, '*');
#endif
      else if (cEcho == 1)
#if 0        
        putchar(nChar);
#else
        uart_putch(0, nChar);
#endif
    }
  }

  return cIndex;
}

