/**
  ******************************************************************************
  * @file    halUserInputCtrlAPI.h
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
  * <h2><center>&copy; Copyright 1997-2019 (c) MadeFactory Inc.</center></h2>
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#if !defined(_HALUSERINPUTCTRLAPI_H_)
#define _HALUSERINPUTCTRLAPI_H_

#include "common.h"
#include "utilExecutiveAPI.h"
#include "dvAK4183API.h"
#include "dvILI2511API.h"

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
// --------------------------------CONFIGURATION------------------------------
// User input events
// --------------------------------CONFIGURATION------------------------------
typedef enum
{
/* ---------------- BEGIN CUSTOM AREA [010] halUICtrlAPI.h -------------------
 * CONFIGURATION: Add user input events here */
 /*--------------------BEGIN Common Events -------------------------------*/
  keNONE, 

  keRAW_KEY1,
  keRAW_KEY2,
  keRAW_KEY3,
  
  keINVALID // used for enumerated type range Checking (DO NOT REMOVE)
}GCC_PACKED eKEY;

// ---------------------------------------------------------------------------
// User input event structure (key and repeat count)
// ---------------------------------------------------------------------------
typedef struct
{
  eKEY                eKey;          // user input event
  BYTE                cCount;        // event repeat count
} KEY_EVENT, *PKEY_EVENT;

// ---------------------------------------------------------------------------
// Touch event condition flags
// ---------------------------------------------------------------------------
#define TOUCH_NONE              (0)
#define TOUCH_PRESS             (1 << 0)
#define TOUCH_TAP_SIGNLE				(1 << 1)
#define TOUCH_TAP_DOUBLE			  (1 << 2)
#define TOUCH_HOLD			        (1 << 3)
#define TOUCH_SWIPE_START       (1 << 4)
#define TOUCH_SWIPE_UP		      (1 << 5)
#define TOUCH_SWIPE_DOWN		    (1 << 6)
#define TOUCH_SWIPE_LEFT		    (1 << 7)
#define TOUCH_SWIPE_RIGHT		    (1 << 8)
#define TOUCH_ZOOM_IN		        (1 << 9)
#define TOUCH_ZOOM_OUT		      (1 << 10)
#define TOUCH_ROTATE            (1 << 11)

// ---------------------------------------------------------------------------
// Touch Event
// ---------------------------------------------------------------------------
typedef struct tagTOUCHEVENT
{
  WORD  wEventFlags;
  WORD  wEventLevel; // Hold, Swipe, Zoom, Rotate
  POINT sPoint;
}TOUCH_EVENT, *PTOUCH_EVENT;

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************


// ***************************************************************************
// ************************* START OF ISR  DEFINITIONS *************************
// ***************************************************************************


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************
/**
 *  @brief      This API sets up the internal state of component.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halUICtrlInitialize(void);

/**
 *  @brief      This API Release the internal state of component.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halUICtrlRelease(void);

/**
 *  @brief      This API Process the internal state of component..
 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halUICtrlProcess(void);

/**
 *  @brief      This API read GPIO field(s) and return assemble keycode
 *  @param      None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
WORD halUICtrlKeypadCodeGet(void);

/**
 *  @brief      This API handles timer events for keypad polling.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halUICtrlKeyEventHandler(void);

/**
 *  @brief      This API Gets the Touch Axis
 *  @param[in]  None
 *  @param[out] psTouchEvent: Pointer to return back Touch Event
 *  @return     TRUE if Event Occurred, else Event is not occurred
 *  @note       None
 *  @attention  None
 */
BOOL halUICtrlTouchEventGet(PTOUCH_EVENT psTouchEvent);

/**
 *  @brief      This API handles Touch events
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void halUICtrlTouchEventHandler(void);


#endif  // _HALUSERINPUTCTRLAPI_H_
