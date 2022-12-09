/**
  ******************************************************************************
  * @file    version.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    09-05-2019
  * @brief   This file provides Version Information
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
  * <h2><center>&copy; Copyright 1997-2019 (c) MadeFactory Inc.</center></h2>
  ******************************************************************************
  */

#if !defined(_INCLUDED_VERSION_H_)
#define _INCLUDED_VERSION_H_

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

//---------------------------------------------------------------------------
// Software Version
//---------------------------------------------------------------------------
/* 201905016-1 */
#define _SOFTWARE_VERSION_MAJOR_      (0)
#define _SOFTWARE_VERSION_MINOR_      (6)
#define _SOFTWARE_VERSION_CUSTOM_     (2)
#define _SOFTWARE_VERSION_CANDIDATE_  (0) // unit is character

//---------------------------------------------------------------------------
// Software Release Version (Display only)
//---------------------------------------------------------------------------
#define _SOFTWARE_VERSION_REL_MAJOR_    (_SOFTWARE_VERSION_MAJOR_)
#define _SOFTWARE_VERSION_REL_MINOR_    (_SOFTWARE_VERSION_MINOR_)
#define _SOFTWARE_VERSION_REL_CUSTOM_   (_SOFTWARE_VERSION_CUSTOM_)

//---------------------------------------------------------------------------
// Data Manager Version
//---------------------------------------------------------------------------
#define _DATAMGR_VERSION_STRUCTURE_     (2) // change this will invoke a "complete reset"
#define _DATAMGR_VERSION_SYSTEM_        (1) // change this will invoke a "standard reset"

//---------------------------------------------------------------------------
// Host I/F Protocol  Version. Sync with Main Program
//---------------------------------------------------------------------------
#define _HOSTIF_PROTOCOL_VERSION_MAJOR_ (0)
#define _HOSTIF_PROTOCOL_VERSION_MINOR_ (1)

//---------------------------------------------------------------------------
// Syslog Protocol  Version
//---------------------------------------------------------------------------
#define _SYSLOG_PROTOCOL_VERSION_MAJOR_ (0)
#define _SYSLOG_PROTOCOL_VERSION_MINOR_ (1)

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************
//const char software_version[] = "0.0.1";

// The following is intended to be output as part of the main help text for
// each program. ``program_name`` is thus the name of the program.
#define REPORT_VERSION(program_name) \
  msgRaw(PRINT_FG_YELLOW PRINT_FG_BOLD "\r\n\r\n %s V%d.%d.%d built %s %s by Made Factory June.Yim \r\n\r\n" PRINT_ANSI_RESET, \
         (program_name), _SOFTWARE_VERSION_MAJOR_, _SOFTWARE_VERSION_MINOR_, _SOFTWARE_VERSION_CUSTOM_, \
         __DATE__,__TIME__)



// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************


#endif //#if !defined(_INCLUDED_VERSION_H_)

