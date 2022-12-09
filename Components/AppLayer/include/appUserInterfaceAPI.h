/**
  ******************************************************************************
  * @file    appUserInterfaceAPI.h
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
#ifndef _APPUSERINTERFACEAPI_H
#define _APPUSERINTERFACEAPI_H

#include "common.h"
#include "appLayerData.h"
#include "appFontData.h"
#include "halUserInputCtrlAPI.h"
#include "appSystemStateAPI.h"
//#include "utilDataMgrAPI.h"

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

//----------------------------------------------------------------------------
// Menu item state flags
//----------------------------------------------------------------------------
#define MS_ITEM_NO_FLAGS  	 	  0
#define MS_ITEM_SELECTABLE 	    (0x1 << 0)
#define MS_ITEM_VISIBLE         (0x1 << 1)
#define MS_ITEM_UPDATE     	    (0x1 << 2)
#define MS_ITEM_DRAW       	    (0x1 << 3)
// ---------------------------------------------------------------------------
// Sentinel value for all items in a menu
// ---------------------------------------------------------------------------
#define MS_ALL_ITEMS       0xFFFFFFFFFFULL

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************
//----------------------------------------------------------------------------
// list of possible user interface states
//----------------------------------------------------------------------------
typedef enum
{
  uiIDLE,  
/* -------------------------- BEGIN CUSTOM AREA ---------------------------
 * CONFIGURATION: Add user interface states here */  
  uiSPLASH,
  uiLAUNCHBAR,
  uiMENU_MAIN,
/* ---------------------------- END CUSTOM AREA --------------------------- */

  uiINVALID                          // Used for enumerated type range checking (DO NOT REMOVE)
} eUI_STATE;

// ---------------------------- CONFIGURATION --------------------------------
// menu items to SPLASH screen
// ---------------------------- CONFIGURATION --------------------------------
typedef enum
{
  menuSS_PROGRESS, // Loading Progress
  menuSS_VERSION,

  menuSS_INVALID // Sentinel
}eMENU_SPLASH;

// ---------------------------- CONFIGURATION --------------------------------
// menu items to Launchbar screen
// ---------------------------- CONFIGURATION --------------------------------
typedef enum
{
  menuLB_PHOTO_BTN_1,
  menuLB_PHOTO_BTN_2,
  menuLB_PHOTO_BTN_3,
  menuLB_PHOTO_BTN_4,
  menuLB_PHOTO_BTN_5,  

  menuLB_INVALID // Sentinel
}eMENU_LB;

// ---------------------------- CONFIGURATION --------------------------------
// menu items to Main Screen
// ---------------------------- CONFIGURATION --------------------------------
typedef enum
{  
  menuMM_PHOTO,  

  menuMM_NAVI_BTN_Prev,
  menuMM_NAVI_BTN_Home,
  menuMM_NAVI_BTN_Next,

  menuMM_INVALID // Sentinel
}eMENU_MAIN;

// ---------------------------------------------------------------------------
// Touch area to each item
// ---------------------------------------------------------------------------
typedef struct tagITEMAREA
{
  WORD  wEventFlags;     // Touch Event condition to accept
  WORD  wLeft;
  WORD  wTop;
  WORD  wRight;
  WORD  wBottom;
  BYTE  cHoldFilter;    // Hold Filter Counter
  BYTE  cSwipeFilter;   // Swipe Filter counter
  BYTE  cMenuItem;      // Menu Item Index  
}ITEM_AREA, *PITEM_AREA;
typedef const ITEM_AREA *PCITEM_AREA;
#define GAP_TOUCH_AREA 5 // was 10

// ---------------------------------------------------------------------------
// Structure used to define data at last word of flags array
// ---------------------------------------------------------------------------
typedef struct
{
    BYTE    cActiveItem;     // current active item index
    BYTE    cMenuStateFlags; // menu bit flags (see MI_xxx defines above)
} MENU_PRIVATE_DATA, *PMENU_PRIVATE_DATA;

//-------------------------------------------------------------------------
// Menu item static Graphic Ojbect(s) definition
//-------------------------------------------------------------------------
typedef struct
{
  BYTE                   cObjectCount;      // count of gfx objects in array
  PWORD                  pawStaticObjects;  // pointer to gfx array   
} MS_STATIC_MENU_OBJECTS, *PMS_STATIC_MENU_OBJECTS;
typedef const MS_STATIC_MENU_OBJECTS *PCMS_STATIC_MENU_OBJECTS;

//-------------------------------------------------------------------------
// TS Menu Definition
//-------------------------------------------------------------------------
typedef struct
{
  BYTE                        cItemCount;
  BYTE                        cMenuFlags;         // TS_NO_WRAP or TS_WRAP_XXX flags
  eLAYER_DATA                 eLayer;
  PWORD                       pawItemState;       // Low byte= state flags High byte= drawing order
  PCMS_STATIC_MENU_OBJECTS    pasStaticMenuObjects;
  SURFACE**                   pasSurface;
  PCITEM_AREA                 pasItemArea; // Item area for touch event
  BYTE                        cTouchAreaCount;  
} MS_MENU_INFO, *PMS_MENU_INFO;
typedef const MS_MENU_INFO *PCMS_MENU_INFO;
typedef PCMS_MENU_INFO const * CPPMS_MENU_INFO;

// ***************************************************************************
// ******************** END OF PUBLIC CONFIGURATION SECTION ******************
// ***************************************************************************

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *  @brief      This API sets up the internal state of this component.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appUIInitialize(void);

/**
 *  @brief      This API performs periodic processing for the user interface.  It checks
                    for user input (e.g. keypad or Touch) and updates the state of the user
                    interface appropriately.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appUIProcess(void);

/**
 *  @brief      Return TURE if current UI state is IDLE
                    and resetting of this components persistent data node by handling external
                    events triggered by the persistent data storage.
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if current UI state is IDLE
 *  @note       None
 *  @attention  None
 */
BOOL appUIStateIsIdle(void);

/**
 *  @brief      Change the state of the user interface to the specified value.
 *  @param[in]  eState : state to transition to
 *  @param[out] None
 *  @return     None
 *  @note       This API must ONLY be called from within the User Interface component,
                     and only from appUIxxxEvent routines.
 *  @attention  None
 */
void appUIStateChange(eUI_STATE State);

/**
 *  @brief      Set a flag indicating that the user interface graphics need to be updated
 *  @param[in]  eState : state to transition to
 *  @param[out] None
 *  @return     None
 *  @note       This API must ONLY be called from within the User Interface component,
                     and only from appUIxxxEvent routines.
 *  @attention  None
 */
void appUIUpdateSet(void);

/**
 *  @brief      Enable or disable the UI
 *  @param[in]  bEnable : TRUE to enable the UI (reset the current state), FALSE to disable
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appUIEnable(BOOL bEnable);

/**
 *  @brief      Clear the UI Frame Buffer
 *  @param[in]  bBackFrameOnly : TRUE to Clear the Back Frame Buffer only (Double Buffer Mode only)
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appUIClear(BOOL bBackFrameOnly);

/**
 *  @brief      Return Next State
 *  @param[in]  None
 *  @param[out] None
 *  @return     eUI_STATE
 *  @note       None
 *  @attention  None
 */
eUI_STATE appUINextStateGet(void);

/**
 *  @brief      Return Current State
 *  @param[in]  None
 *  @param[out] None
 *  @return     eUI_STATE
 *  @note       None
 *  @attention  None
 */
eUI_STATE appUICurrentStateGet(void);

/**
 *  @brief      Return Previous State
 *  @param[in]  None
 *  @param[out] None
 *  @return     eUI_STATE
 *  @note       None
 *  @attention  None
 */
eUI_STATE appUIPrevStateGet(void);

/**
 *  @brief      This API selects the specified item as the active item
 *  @param[in]  psMenuInfo : Pointer to a menu configuration data structure
                cItem      : Menu item to make active
 *  @param[out] None
 *  @return     TRUE if active item was set as specified, FALSE otherwise
 *  @note       None
 *  @attention  None
 */
BOOL appMSMenuActiveItemSet(PCMS_MENU_INFO psMenuInfo,
                            BYTE           cItem);

/**
 *  @brief      This API returns the active item in the specified menu.
 *  @param[in]  psMenuInfo : Pointer to a menu configuration data structure
 *  @param[out] None
 *  @return     Active item of specified menu
 *  @note       None
 *  @attention  None
 */
BYTE appMSMenuActiveItemGet(PCMS_MENU_INFO psMenuInfo);

/**
 *  @brief      This API reads the item flags for the specified menu item.
 *  @param[in]  eLayer: Layer Id
                cItem: Item to get flags for
 *  @param[out] None
 *  @return     Current status flags for specified menu item
 *  @note       None
 *  @attention  None
 */
BYTE appMSMenuItemsFlagsGet(eLAYER_DATA eLayer,
                           BYTE           cItem);

/**
 *  @brief      This API sets the specified item flags for the specified mask of menu items
 *  @param[in]  eLayer: Layer Id
                qwItemMask: Item number bitmask to set flags for. Use MS_ALL_ITEMS to set
                                     flags for all items in menu
                cFlags: Flags to set (see the Menu Item flag symbols TS_ITEM_XXX for valid values)
 *  @param[out] None
 *  @return     Current status flags for specified menu item
 *  @note       None
 *  @attention  None
 */
void appMSMenuItemsFlagsSet(eLAYER_DATA eLayer, 
                            QWORD          qwItemMask,
                            BYTE           cFlags);
                            
/**
 *  @brief      This API clears the specified item flags for the specified mask of menu items
 *  @param[in]  eLayer: Layer Id
                qwItemMask: Item number bitmask to set flags for. Use MS_ALL_ITEMS to set
                                     flags for all items in menu
                cFlags: Flags to clear (see the Menu Item flag symbols TS_ITEM_XXX for valid values)
 *  @param[out] None
 *  @return     Current status flags for specified menu item
 *  @note       None
 *  @attention  None
 */
void appMSMenuItemsFlagsClear(eLAYER_DATA eLayer,
                              QWORD          qwItemMask,
                              BYTE           cFlags);


/**
 *  @brief      This API return Menu info the specified eUI_STATE
 *  @param[in]  eUIState: UI State to active item set
 *  @param[out] None
 *  @return     pointer to Menu info
 *  @note       None
 *  @attention  None
 */
PCMS_MENU_INFO appMSMenuStateInfoGet(eUI_STATE eUIState);

/**
 *  @brief      This API selects the specified item as the active item in 
                specified eUI_STATE
 *  @param[in]  eUIState: UI State to active item set
                cItem: Menu item to make active
 *  @param[out] None
 *  @return     TRUE if active item was set as specified, FALSE otherwise
 *  @note       None
 *  @attention  None
 */
BOOL appMSMenuStateActiveItemSet(eUI_STATE eUIState, BYTE cItem);

/**
 *  @brief      This API returns the active item specified eUI_STATE
 *  @param[in]  eUIState: UI State to active item set
 *  @param[out] None
 *  @return     Active item of MAIN MENU
 *  @note       None
 *  @attention  None
 */
BYTE appMSMenuStateActiveItemGet(eUI_STATE eUIState);

/**
 *  @brief      This API Clear the active item in the specified eUI_STATE
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appMSMenuStateActiveItemClear(eUI_STATE eUIState);

/**
 *  @brief      This API sets the specified item flags for the specified mask of Launchbar menu items
 *  @param[in]  eUIState: UI State tot Flags Set
                wItemMask: Item number bitmask to set flags for. Use MS_ALL_ITEMS to set 
                           flags for all items in menu
                cFlags: Flags to set (see the Menu Item flag symbols TS_ITEM_XXX for valid values)
 *  @param[out] None
 *  @return     Current status flags for specified menu item
 *  @note       None
 *  @attention  None
 */
void appMSMenuStateItemsFlagsSet(eUI_STATE eUIState, DWORD dwItemMask, BYTE cFlags);

/**
 *  @brief      This API sets menu item flags from the main program status to userinterface change
 *  @param[in]  pcsSysStatusNew  : pointer to system status to userinterface to set
 *  @param[out] None
 *  @return     TRUE if processed
 *  @note       must be call before appSysStateStatusSet()
 *  @attention  None
 */
BOOL appUISysStatusUpdate(PCSTATUS_SYS pcsSysStatusNew);

/**
 *  @brief      This API return the UI Ready Status                
 *  @param[in]  None
 *  @param[out] None
 *  @return     UI is available to state change, it return TRUE. 
 *  @note       None
 *  @attention  None
 */
BOOL appUIReadyToStateChangeGet(void);

#endif // _APPUSERINTERFACEAPI_H

