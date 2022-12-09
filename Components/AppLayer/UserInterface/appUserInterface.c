
/**
  ******************************************************************************
  * @file    appUserInterface.c
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
#include "dvGPIOAPI.h"
#include "dvCPUAPI.h"
#include "AK4183.h"
#include "appUserInterfaceAPI.h"
#include "appUserInterface.h"
#include "utilHostIfAPI.h"
#include "appLayerData.h"
#include "utilExecutiveAPI.h"
#include "utilMiscAPI.h"
#include "appSystemStateAPI.h"
#include "halPowerCtrlAPI.h"
#include "appPowerMgrAPI.h"
#include "halLCDCtrlAPI.h"
#include "appAudioMgrAPI.h"
#include "halLEDCtrlAPI.h"

#include "../../lv_conf.h"
#include "../../lvgl/lvgl.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS **************
// ***************************************************************************
#define _MOD_NAME_ "\"UI\""

// ---------------------------------------------------------------------------
// Text formatting options
// ---------------------------------------------------------------------------
#define TU_DECIMAL_DIGITS   0x07        // mask for decimal digit count (0-7)
#define TU_LEFT_JUSTIFY     0x20        // flag for left justification
#define TU_LEADING_ZEROS    0x40        // flag for leading zeros (for example 001)
#define TU_LEADING_SPACE    0x08
#define TU_SIGN             0x80        // flag for sign (for example +30)

// ---------------------------------------------------------------------------
// Counter Period 
// ---------------------------------------------------------------------------
#define PERIOD_UI_RENDER_DELAY            100000L
#define PERIOD_UI_ACTIVE_CLEAR            5000000L
#define PERIOD_UI_ANIMATION_DEFAULT       250000
#define PERIOD_UI_ANIMATION_CAVI_STEP     100000
#define PERIOD_UI_FACTORY_TAP             2000000L
#define PERIOD_UI_ALBUM_SLIDE             5000000L

// ---------------------------------------------------------------------------
// Sentinel value for a deselected menu (no active item)
// ---------------------------------------------------------------------------
#define MS_NO_ACTIVE_ITEM  0xFF

// ---------------------------------------------------------------------------
// Color code for EGL_COLOR 
// ---------------------------------------------------------------------------
#define COLOR_WHITE         0xFFFFFF
#define COLOR_BLACK         0x000000
#define COLOR_GRAY          0x7F7F7F
#define COLOR_GRAY_DARK     0x595959
#define COLOR_RED           0xFF0000

// Main Menu Background
#define COLOR_MENU_BACK_UNSEL        0xD1CBC6
#define COLOR_MENU_BACK_SEL          0xD1CBC6
#define COLOR_MENU_BACK_DEACTIVATE   0x4E5664


// ---------------------------------------------------------------------------
// Image Object Index for numeration configuration. Refer the appLayerData.c
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// enum to event interface
// ---------------------------------------------------------------------------
typedef enum
{
  userEVENT_NONE,
  userEVENT_KEY,
  userEVENT_TOUCH,
  
  userEVENT_INVALID
}eUSER_EVENT_TYPE;

// ---------------------------------------------------------------------------
// Background state to draw (Special procedure)
// ---------------------------------------------------------------------------
typedef enum
{
  menuSTATE_NORMAL_UNSEL,   // selectable but not selected
  menuSTATE_NORMAL_SEL,     // selectable and selected
  menuSTATE_DEACTIVATE,     // Not selectable

  menuSTATE_INVALID
}eMENU_ITEM_STATE;

// ---------------------------------------------------------------------------
// Pane for Frame Surface
// ---------------------------------------------------------------------------
typedef struct tagSURFACEPANE
{
  SURFACE* psSurface1st;
  SURFACE* psSurface2nd;
}SURFACE_PANE, *PSURFACE_PANE;

// ---------------------------------------------------------------------------
// Structure to event interface
// ---------------------------------------------------------------------------
typedef struct tagUSEREVENT
{
  eUSER_EVENT_TYPE eEventType;
  union
  {
    KEY_EVENT sKeyEvent;
    TOUCH_EVENT sTouchEvent;    
  }uEvent;
}USER_EVENT, *PUSER_EVENT;
typedef CROMDATA USER_EVENT *PCUSER_EVENT;


// ***************************************************************************
// ****************** START OF PRIVATE CONFIGURATION SECTION *****************
// ***************************************************************************

// Surface pointer array to Image objects
static SURFACE* m_asSurfaceImage[IMAGE_OBJECT_COUNT];// = {NULL};

//----------------------------------------------------------------------------
// ----------------------------- SPLASH SCREEN -------------------------------
//----------------------------------------------------------------------------
static CROMDATA WORD m_awSplashStaticObjs[] =
{
  goSS_STATIC_Background,
  goSS_STATIC_Product,
  goSS_STATIC_Product_Sub,
  goSS_STATIC_Loading,
  goSS_STATIC_Vendor,
};

static MS_STATIC_MENU_OBJECTS m_asSplashStaticObjs[] =
{
		{sizeof(m_awSplashStaticObjs)/sizeof(WORD), (PVOID)&m_awSplashStaticObjs},
};

#define SS_ITEM_COUNT menuSS_INVALID

// Item Flags to Splash Screen
static WORD m_awSplashItemsFlags[SS_ITEM_COUNT+1]; // Save cActiveItem on index cItemCount
//----------------------------------------------------------------------------
// splash screen menu definition and state data
//----------------------------------------------------------------------------
static CROMDATA MS_MENU_INFO m_sMenuSplash =
{
	.cItemCount           = SS_ITEM_COUNT,
	.cMenuFlags           = 0,
	.eLayer               = ldSPLASH, 
	.pawItemState         = m_awSplashItemsFlags,
	.pasStaticMenuObjects = m_asSplashStaticObjs,
	.pasSurface           = m_asSurfaceImage,
	.pasItemArea          =	NULL,
	.cTouchAreaCount      = 0,
};

//----------------------------------------------------------------------------
// ------------------------------- LAUNCHBAR ---------------------------------
//----------------------------------------------------------------------------
// touch event area for Easthetic UI Menu Items
#define LB_TOUCH_AREA_COUNT 5
static CROMDATA ITEM_AREA m_asLBItemArea[LB_TOUCH_AREA_COUNT] =
{
	{(TOUCH_TAP_SIGNLE),  53, 79, 53+400, 79+681, 0, 0, menuLB_PHOTO_BTN_1},
	{(TOUCH_TAP_SIGNLE), 506,79, 506+400, 79+225, 0, 0, menuLB_PHOTO_BTN_2},
	{(TOUCH_TAP_SIGNLE), 956, 79, 956+262, 79+392, 0, 0, menuLB_PHOTO_BTN_3},
	{(TOUCH_TAP_SIGNLE), 506, 367, 506+262, 214+367, 0, 0, menuLB_PHOTO_BTN_4},
	{(TOUCH_TAP_SIGNLE), 818, 494, 818+400, 494+266, 0, 0, menuLB_PHOTO_BTN_5},
};

static CROMDATA WORD m_awLBStaticObjs[] =
{
  goLB_STATIC_Background,
  goLB_STATIC_Title,
  goLB_STATIC_Copyright,
};

static MS_STATIC_MENU_OBJECTS m_asLBStaticObjs[] =
{
	{sizeof(m_awLBStaticObjs)/sizeof(WORD), (PVOID)&m_awLBStaticObjs},
};

#define LB_ITEM_COUNT menuLB_INVALID

// Item Flags to Launchbar
static WORD m_awLBItemsFlags[LB_ITEM_COUNT + 1]; // Save cActiveItem on index cItemCount
//----------------------------------------------------------------------------
// Launchbar menu definition and state data
//----------------------------------------------------------------------------
static CROMDATA MS_MENU_INFO m_sMenuLaunchbar =
{
	.cItemCount           = LB_ITEM_COUNT,
	.cMenuFlags           = 0,
	.eLayer               = ldLAUNCHBAR, 
	.pawItemState         = m_awLBItemsFlags,
	.pasStaticMenuObjects = m_asLBStaticObjs,
	.pasSurface           = m_asSurfaceImage,
	.pasItemArea          =	m_asLBItemArea,
	.cTouchAreaCount      = LB_TOUCH_AREA_COUNT,
};

//-------------------------------------------------------------------------------------
// ----------------------------------- MAIN MENU --------------------------------------
//-------------------------------------------------------------------------------------
// touch event area for Easthetic UI Menu Items
#define MM_TOUCH_AREA_COUNT       3
static CROMDATA ITEM_AREA m_asMenuMainItemArea[MM_TOUCH_AREA_COUNT] =
{
	{(TOUCH_TAP_SIGNLE), 487, 698, 487+100, 698+72, 0, 0, menuMM_NAVI_BTN_Prev},		
	{(TOUCH_TAP_SIGNLE), 587, 698, 587+106, 698+72, 0, 0, menuMM_NAVI_BTN_Home},		
	{(TOUCH_TAP_SIGNLE), 693, 698, 693+100, 698+72, 0, 0, menuMM_NAVI_BTN_Next},		
};

static CROMDATA WORD m_awMenuMainStaticObjs[] =
{
  goMM_STATIC_Background,
};

static MS_STATIC_MENU_OBJECTS m_asMenuMainStaticObjs[] =
{
	{sizeof(m_awMenuMainStaticObjs)/sizeof(WORD), (PVOID)&m_awMenuMainStaticObjs},
};


#define MENU_MAIN_ITEM_COUNT menuMM_INVALID

// Item Flags to Menu
static WORD m_awMenuMainItemsFlags[MENU_MAIN_ITEM_COUNT + 1]; // Save cActiveItem on index cItemCount
//----------------------------------------------------------------------------
// menu definition and state data
//----------------------------------------------------------------------------
static CROMDATA MS_MENU_INFO m_sMenuMain =
{
	.cItemCount           = MENU_MAIN_ITEM_COUNT,
	.cMenuFlags           = 0,
	.eLayer               = ldMENU_MAIN, 
	.pawItemState         = m_awMenuMainItemsFlags,
	.pasStaticMenuObjects = m_asMenuMainStaticObjs,
	.pasSurface           = m_asSurfaceImage,
	.pasItemArea          =	m_asMenuMainItemArea,
	.cTouchAreaCount      = MM_TOUCH_AREA_COUNT,
};

// ***************************************************************************
// ******************* END OF PRIVATE CONFIGURATION SECTION ******************
// ***************************************************************************

// ***************************************************************************
// ******************** START OF PRIVATE DATA DECLARATIONS *******************
// ***************************************************************************
// ---------------------------------------------------------------------------
// current and next selected states of user interface
// ---------------------------------------------------------------------------
static eUI_STATE        m_eUIMgrState     = uiIDLE;
static eUI_STATE        m_eUIMgrStateNext = uiIDLE;
static eUI_STATE        m_eUIMgrStatePrev = uiIDLE;

// ---------------------------------------------------------------------------
// flag indicating that OSD graphics need to be updated (redrawn)
// ---------------------------------------------------------------------------
static BOOL             m_bUpdateUI = FALSE;

// ---------------------------------------------------------------------------
// Run-time variables
// ---------------------------------------------------------------------------
#if USE_FONT_DRAW
static EGL_FONT*    m_pasFont[fdINVALID] = {NULL};
#endif
///static SURFACE_PANE m_sSurfacePane; // Pane to Frame Surface
static WORD         m_wImageObjectLoaded; // Loaded Image Object Count
static BYTE         m_cLastActiveMenu = MS_NO_ACTIVE_ITEM;

/* Internal Touch Filter Count */
static BYTE         m_cSwipeFilerCount = 0;
static BYTE         m_cHoldFilterCount = 0;

/* Photo Viewer Index */
static ePHOTO_ID    m_ePhotoIdCur       = (ePHOTO_ID)0;
static BOOL         m_bPhotoPlayStart   = FALSE;

/* LVGL Draw buffer */
static lv_disp_drv_t disp_drv;
static volatile uint32_t t_saved = 0;
uint16_t DISP_fb[PANE_WIDTH_MAX*PANE_HEIGHT_MAX];

static lv_indev_drv_t indev_drv;

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************

/**
 *  @brief      Load Image and allocate to surface
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static SURFACE* UILoadImage(WORD wIndex)
{
  ASSERT(wIndex < IMAGE_OBJECT_COUNT);
  
  SURFACE* psReturnSurface = NULL;
  PCHAR pcFilePath = NULL;
  eFILE_FORMAT eFileFormat;
  
  pcFilePath = g_asImageObj[wIndex].pcPath;
  eFileFormat = g_asImageObj[wIndex].eFormat;

  if(pcFilePath != NULL && eFileFormat < ffmtINVALID)
  {
    switch(eFileFormat)
    {
      case ffmtBMP:
        psReturnSurface = loadbmp(pcFilePath);
      break;

      case ffmtPNG:
        psReturnSurface = loadpng(pcFilePath);
      break;

      case ffmtJPG:
        psReturnSurface = loadjpg(pcFilePath);
      break;

      case ffmtSUF:
        psReturnSurface = loadsurf(pcFilePath);
      break;
        
      default: break;
    }

    ///msgRaw(_MOD_NAME_": Image: %d(%s) Loaded", wIndex, pcFilePath);
   }

  return psReturnSurface;
}

/**
 *  @brief      Process UI Counter event
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       Do not include state transition in this
 *  @attention  None
 */
static void ProcessUICounterEvent(void)
{
  ///PSTATUS_SYS  psStatusSys = appSysStateStatusPtrGet();
  
  switch(m_eUIMgrState)
  {
    case uiIDLE:
    break;

    case uiSPLASH:
    break;

    case uiLAUNCHBAR:
    break;

    case uiMENU_MAIN:
    break;
    
    default: break;
  }
}

/**
 *  @brief      Process state changes on timeouts. If we are in a UI state that has a
                timeout and the timeout occurs, then transition to the idle state.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void ProcessStateChangeOnTimeout(void)
{
  ePOWERSTATE   ePowerState   = appPowerMgrPowerStateGet();
  ///PCSTATUS_SYS  pcsStatusSys  = (PCSTATUS_SYS)appSysStateStatusPtrGet(); 
  eSOUND_ID     eSoundId      = sndINVALID;
  
  if(!utilExecCounterGet(coUI_STATECHANGE_TIMEOUT))
  {
    switch(m_eUIMgrState)
    {
      case uiIDLE:
        if(pwrON == ePowerState)
        {
#if USE_LOADING_CONFIGIRM_FROM_HOST        
          if(utilHostIfGUIConnected())
          {
            /* Connection is established and then go to uiSPLASH */
            m_eUIMgrStateNext = uiSPLASH;
          }
          else if(!utilHostIfGUIConnected()
                  && IMAGE_OBJECT_SS_COUNT == m_wImageObjectLoaded)
          {
            /* To avoid Host I/F packet overflow or packet timeout during imageloading,
              it must be finished before loading remaining images */
            BYTE bConnect = TRUE;
  #if USE_CAN_BUS            
            static WORD wRetry = 0;
  #endif
              
            utilExecCounterSet(coUI_STATECHANGE_TIMEOUT, uS_TO_TICKS(500000));  // 500ms wait before re-send
  #if USE_CAN_BUS            
            utilHostIfGUISend((wRetry++ % 2) ? hiSERIAL_GUI : hiCANBUS
                                , hifGUI_CTH_ConnectionSet
                                , &bConnect, 1);
  #else
            utilHostIfGUISend(hiSERIAL_GUI, hifGUI_CTH_ConnectionSet, &bConnect, 1);
  #endif
          }
#else          
          m_eUIMgrStateNext = uiSPLASH;
#endif
      }
      break;

      case uiSPLASH:  /* ------------------ SPLASH STATE CHANGE ------------------------- */
      if(IMAGE_OBJECT_COUNT == m_wImageObjectLoaded)
      { 
#if USE_LOADING_CONFIGIRM_FROM_HOST
        if(utilHostIfGUIReadyGet())
        {          
          appSysStateStatusRequest(); // System Status Request

          eSoundId = sndWELCOME;

          m_eUIMgrStateNext = uiLAUNCHBAR;
          m_bUpdateUI       = TRUE;
        }
        else
        {          
          BYTE cReady = TRUE;
          
          /* Send Ready signal to Host every 500ms */
          utilExecCounterSet(coUI_STATECHANGE_TIMEOUT, uS_TO_TICKS(500000));  // 500ms wait before re-send
          utilHostIfGUISend(HOST_INSTANCE_AUTO, hifGUI_CTH_ReadySet, &cReady, 1);          
        }
#else      
        eSoundId = sndWELCOME;

        m_eUIMgrStateNext = uiLAUNCHBAR;
        m_bUpdateUI       = TRUE;

        utilExecSignalSet(sgGUI_IF, SIGNAL_GUI_READY);
#endif
      }
      break;

      case uiLAUNCHBAR:
        if(m_bPhotoPlayStart)
        {
          m_eUIMgrStateNext = uiMENU_MAIN;
          m_bUpdateUI       = TRUE;       
        }
      break;

      case uiMENU_MAIN:
        if(!m_bPhotoPlayStart)
        {
          m_eUIMgrStateNext = uiLAUNCHBAR;
          m_bUpdateUI       = TRUE;
        }
      break;

      default: break;
    }

    if(sndINVALID != eSoundId)
      appAudioMgrPlay(eSoundId, TRUE);
  }

}

/**
 *  @brief      Process Sound especially Shot Sound
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       Keep Play Priority
 *  @attention  None
 */
void ProcessSound(void)
{
}

/**
 *  @brief      Process Loding on Splash Screen UI
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void ProcessLoading(void)
{
  ASSERT(m_eUIMgrState == uiSPLASH)
    
  if(m_wImageObjectLoaded < IMAGE_OBJECT_COUNT)
  {
    m_asSurfaceImage[m_wImageObjectLoaded] = UILoadImage(m_wImageObjectLoaded);
    m_wImageObjectLoaded++;

    appMSMenuStateItemsFlagsSet(uiSPLASH, (0x1ULL << menuSS_PROGRESS), (MS_ITEM_UPDATE));
  }
}

//=============================================================================
// Start of Event API helper functions. Called from the UIStateEvent API.
//=============================================================================
/**
 *  @brief      translate the user event
 *  @param[in]  psKeyEvent : Pointer to event to translate
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void KeyRawEventTranslate(PKEY_EVENT psKeyEvent)
{    
  switch(psKeyEvent->eKey)
  {
    case keRAW_KEY1:
      appSysStateUnitTestKey_1();
      psKeyEvent->eKey = keNONE;
    break;

    case keRAW_KEY2:
      appSysStateUnitTestKey_2();
      psKeyEvent->eKey = keNONE;
    break;

    case keRAW_KEY3:
      appSysStateUnitTestKey_3();
      psKeyEvent->eKey = keNONE;
    break;
    
    default: break;
  }
}

/**
 *  @brief      Determine if event should be filter out and not handled by normal event processing.
 *  @param[in]  eKey : Event key input
 *  @param[out] None
 *  @return     TRUE= Event should be ignored, FALSE= Event should be handled by normal event processing.
 *  @note       None
 *  @attention  None
 */
static BOOL KeyEventFilter(eKEY eKey)
{
  BOOL        bFiltered = FALSE;
  ePOWERSTATE ePwrState = appPowerMgrPowerStateGet();  


  if(pwrSOFTRESET== ePwrState)
  {
    bFiltered = TRUE;
  }
  else if(pwrSTANDBY == ePwrState
    /*&& keVIR_POWEROFF != eKey
    && keVIR_POWERON != eKey*/)
  {
    bFiltered = TRUE;
  }

  return bFiltered;
}

/**
 *  @brief      Default event handler if not handled by normal event processing.
 *  @param[in]  eState : New user interface state
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void KeyEventDefault(eUI_STATE eState)
{

}

/**
 *  @brief      Touch event process to UIStateEvent sub callers
 *  @param[in]  pcsMenuInfo: Pointer to menu info                
 *  @param[out] psUserEvent : Pointer to user event to return back
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static BYTE TouchEventPreprocess(PCMS_MENU_INFO pcsMenuInfo, PUSER_EVENT psUserEvent)
{
  BOOL              bMenuItemFound    = FALSE;  	
  BYTE              cItem;
  BYTE              cFlags;
  PCITEM_AREA       pasItemArea       = pcsMenuInfo->pasItemArea;
  eLAYER_DATA       eLayer            = pcsMenuInfo->eLayer;
  BYTE              cTouchAreaCount   = pcsMenuInfo->cTouchAreaCount; 
  BYTE              cTouchAreaIndex;  
  PTOUCH_EVENT      psTouchEvent;
  ///PKEY_EVENT        psKeyEvent;  

  ASSERT(pcsMenuInfo != NULL && psUserEvent != NULL);

  psTouchEvent = (PTOUCH_EVENT)&psUserEvent->uEvent.sTouchEvent;
  ///psKeyEvent = (PKEY_EVENT)&psUserEvent->uEvent.sKeyEvent;  

  for(cTouchAreaIndex = 0; cTouchAreaIndex < cTouchAreaCount; cTouchAreaIndex++)
  {
    
    cItem = pasItemArea[cTouchAreaIndex].cMenuItem;
    cFlags = appMSMenuItemsFlagsGet(eLayer, cItem);

    if((cFlags & MS_ITEM_VISIBLE)
      && (cFlags & MS_ITEM_SELECTABLE))
    {
       /// msgDbg("X:%d, Y:%d", psTouchEvent->sPoint.wX, psTouchEvent->sPoint.wY)
      
      if(psTouchEvent->sPoint.wX >= pasItemArea[cTouchAreaIndex].wLeft
        && psTouchEvent->sPoint.wX <= pasItemArea[cTouchAreaIndex].wRight
        && psTouchEvent->sPoint.wY >= pasItemArea[cTouchAreaIndex].wTop
        && psTouchEvent->sPoint.wY <= pasItemArea[cTouchAreaIndex].wBottom
        && (psTouchEvent->wEventFlags & pasItemArea[cTouchAreaIndex].wEventFlags)
        )
      {
        /// msgDbg("0x%.4X", psTouchEvent->wEventFlags);

        if(TOUCH_TAP_SIGNLE & psTouchEvent->wEventFlags)
        {
          m_cSwipeFilerCount = 0;
          m_cHoldFilterCount = 0;      

          /* Filtering TOUCH_TAP_SIGNLE after TOUCH_HOLD */
          if((TOUCH_HOLD & pasItemArea[cTouchAreaIndex].wEventFlags) != TOUCH_HOLD
            || ((TOUCH_HOLD & pasItemArea[cTouchAreaIndex].wEventFlags)
                && psTouchEvent->wEventLevel < pasItemArea[cTouchAreaIndex].cHoldFilter))
          {
            bMenuItemFound = TRUE;
            break;
          }
        }
        else if(TOUCH_HOLD & psTouchEvent->wEventFlags)
        {
          if(m_cHoldFilterCount++ > pasItemArea[cTouchAreaIndex].cHoldFilter)
          {
            m_cSwipeFilerCount = 0;
            m_cHoldFilterCount = 0;    
            
            bMenuItemFound = TRUE;
            break;
          }
        }
        else if((TOUCH_SWIPE_LEFT | TOUCH_SWIPE_RIGHT) & psTouchEvent->wEventFlags)
        {
          if(m_cSwipeFilerCount++ > pasItemArea[cTouchAreaIndex].cSwipeFilter)
          {
            m_cSwipeFilerCount = 0;
            m_cHoldFilterCount = 0;    
            bMenuItemFound = TRUE;
            break;            
          }
        }
        else
        {            
          m_cSwipeFilerCount = 0;
          m_cHoldFilterCount = 0;    
        
          break;
        }
      }
    }
  }

  if(!bMenuItemFound) cItem = 0xFF;

  return cItem;
}
//=============================================================================
// End of Event API helper functions. Called from the UIStateEvent API.
//=============================================================================
/**
 *  @brief      Perform static bitmap draw in specified position
 *  @param[in]  psDrawable : pointer to draw object
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void MSMenuBitmapStaticDraw(PCDRAWABLE psDrawable)
{ 
  ASSERT(psDrawable != NULL);
  ASSERT(psDrawable->eType == dtBITMAP);
  ASSERT(psDrawable->wIndex < IMAGE_OBJECT_COUNT);

  if(!psDrawable->sRect.w || !psDrawable->sRect.h)
    draw_surface(m_asSurfaceImage[psDrawable->wIndex], psDrawable->sRect.x, psDrawable->sRect.y);
  else
    draw_surface_scale(m_asSurfaceImage[psDrawable->wIndex]
                                  , psDrawable->sRect.x
                                  , psDrawable->sRect.y
                                  , psDrawable->sRect.w
                                  , psDrawable->sRect.h);
}

/**
 *  @brief      Perform bitmap draw in specified position
 *  @param[in]  wIndex : Index to draw bitmap. refer appLayerData.c
                sRect : point to draw
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void MSMenuBitmapDraw(WORD wIndex, RECT sRect)
{  
  ASSERT(wIndex < IMAGE_OBJECT_COUNT);

  if(!sRect.w || !sRect.h)
    draw_surface(m_asSurfaceImage[wIndex], sRect.x, sRect.y);
  else
    draw_surface_scale(m_asSurfaceImage[wIndex], sRect.x, sRect.y, sRect.w, sRect.h);
}

/**
 *  @brief      Perform bitmap area to blank in specified position
 *  @param[in]  psDrawable : pointer to draw object
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void MSMenuBitmapStaticClear(PCDRAWABLE psDrawable)
{
#if 0
  SURFACE* psSurface = NULL;
  PBYTE pcPixelData = NULL;

  psSurface = get_front_buffer(); 
  pcPixelData = (PBYTE)psSurface->pixels;  
	memset(pcPixelData + (psSurface->pitch*100),0,psSurface->pitch*100);
#endif  
#if 0
  SURFACE* psSurface = NULL;
  PBYTE pcPixelData = NULL;  

  psSurface = create_surface(100, 100, 32);
  pcPixelData = (PBYTE)psSurface->pixels;  
  memset(pcPixelData,0,psSurface->pitch*psSurface->h);

  draw_surface(psSurface, 100, 100);

  release_surface(psSurface);
#endif  
}

#if 0
/**
 *  @brief      Perform static  rectagle draw in specified color
 *  @param[in]  psDrawable : pointer to draw object
                nColor : combinationed RGB color                
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void MSMenuRectStaticDrawColor(PCDRAWABLE psDrawable
                                      , EGL_COLOR nColor)
{
  ASSERT(psDrawable != NULL);
  ASSERT(psDrawable->eType == dtRECT);  
  
  draw_rectfill(psDrawable->sRect.x
                        , psDrawable->sRect.y
                        , psDrawable->sRect.w
                        , psDrawable->sRect.h
                        , nColor);
}    
#endif                                      

/**
 *  @brief      Perform Draw rectagle static
 *  @param[in]  psDrawable : pointer to draw object                
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void MSMenuRectStaticDraw(PCDRAWABLE psDrawable)
{  
  EGL_COLOR sEGLColor;
  
  ASSERT(psDrawable != NULL);
  ASSERT(psDrawable->eType == dtRECT);
  ASSERT(psDrawable->wIndex < RECT_OBJECT_COUNT);

  sEGLColor = MAKE_RGB888(g_asRectObj[psDrawable->wIndex].cRed
                                                  , g_asRectObj[psDrawable->wIndex].cGreen
                                                  , g_asRectObj[psDrawable->wIndex].cBlue);
  
  draw_rectfill(psDrawable->sRect.x
                          , psDrawable->sRect.y
                          , psDrawable->sRect.w
                          , psDrawable->sRect.h
                          , sEGLColor);
}

/**
 *  @brief      Perform Draw rectagle Dymanically as specified RECT and COLOR
 *  @param[in]  sRect : RECT to draw object
                nColor : combinationed RGB color
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void MSMenuRectDynamicDraw(RECT sRect, EGL_COLOR nColor)
{  
  draw_rectfill(sRect.x, sRect.y, sRect.w, sRect.h, nColor);
}

/**
 *  @brief      Draws the specified string as configured by the supplied rendering attributes.
 *  @param[in]  eFontIndex: Font Index
                sPoint: Point to draw
                sEGLColor: EGL_COLOR to draw
                pacString: Pointer to null-terminated array of character values
                bVerticalDraw: TRUE to Vertical Draw
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
 void MSMenuTextDynamicDraw(eFONT_DATA eFontIndex
                                  , POINT sPoint
                                  , EGL_COLOR sEGLColor
                                  , PCBYTE pacString
                                  , BOOL bVerticalDraw)
{
	ASSERT(eFontIndex < fdINVALID);

	egl_font_set_color(m_pasFont[eFontIndex],sEGLColor);
	bmpfont_settextencoding(m_pasFont[eFontIndex], NONE);
	bmpfont_setautokerning(m_pasFont[eFontIndex], true);

  if(bVerticalDraw)
  	bmpfont_draw_vleft(m_pasFont[eFontIndex], (int)sPoint.wX, (int)sPoint.wY, pacString);
  else
    bmpfont_draw(m_pasFont[eFontIndex], (int)sPoint.wX, (int)sPoint.wY, pacString);
}

/**
 *  @brief      Perform processing for entering a Splash Screen UI state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void SplashScreenEnter(void)
{
  eLAYER_DATA eLayer = m_sMenuSplash.eLayer;
  
  appMSMenuItemsFlagsSet(eLayer
                          , MS_ALL_ITEMS
                          , (MS_ITEM_VISIBLE | MS_ITEM_DRAW));

  appMSMenuActiveItemSet(&m_sMenuSplash, MS_NO_ACTIVE_ITEM);

  m_cLastActiveMenu = MS_NO_ACTIVE_ITEM;
  m_cSwipeFilerCount = 0;
  m_cHoldFilterCount = 0;                       
}

/**
 *  @brief      Perform event handling processing for Splash Screen UI state
 *  @param[in]  sUserEvent : event information structure
 *  @param[out] None
 *  @return     TRUE if UI Update needed, FALSE for others
 *  @note       None
 *  @attention  None
 */
static BOOL SplashScreenEvent(USER_EVENT sUserEvent)
{	
  BOOL bUpdateUI = FALSE;

  return bUpdateUI;
}

/**
 *  @brief      Perform renderng for Splash Screen UI state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void SplashScreenRender(void)
{
  PCDRAWABLE    pasDrawList;
  eLAYER_DATA   eLayer = m_sMenuSplash.eLayer;
  BYTE          cItem;
  BYTE          cFlags;
  POINT         sPoint = {0,0};
  RECT          sRect;

  pasDrawList = g_asLayers[eLayer].pasDrawList;
  for(cItem = 0; cItem < m_sMenuSplash.cItemCount; cItem++)
  {
    cFlags = appMSMenuItemsFlagsGet(m_sMenuSplash.eLayer, cItem);

    if((cFlags & MS_ITEM_VISIBLE) 
    && (cFlags & (MS_ITEM_UPDATE | MS_ITEM_DRAW)))//VISIBLE, UPDATE, DRAW Flag Set?
    {
      switch(cItem)
      {
        case menuSS_PROGRESS:
        {
          BYTE cPercent = (m_wImageObjectLoaded*100) / IMAGE_OBJECT_COUNT;
          EGL_COLOR sEGLColor 
            = MAKE_RGB888(g_asRectObj[pasDrawList[goSS_RECT_Progress].wIndex].cRed
                          , g_asRectObj[pasDrawList[goSS_RECT_Progress].wIndex].cGreen
                          , g_asRectObj[pasDrawList[goSS_RECT_Progress].wIndex].cBlue);
                    
          sRect = pasDrawList[goSS_RECT_ProgressBack].sRect;
          ///sPoint.wX = sRect.x + sRect.w;
          sRect.w = sRect.w / 100 * cPercent;
          ///sRect.x = sPoint.wX - sRect.w;
          
          MSMenuRectDynamicDraw(sRect, sEGLColor);
        }

        break;

        case menuSS_VERSION:
        {
#if USE_FONT_DRAW      
          BYTE acBuffer[100] = {0,};

          commArgsToStr(acBuffer
                        , sizeof(acBuffer)
                        ///, "[built %s] GUI Ver%d.%d.%d"
                        ///, __DATE__
                        , "GUI Ver%d.%d.%d"
                        , _SOFTWARE_VERSION_REL_MAJOR_
                        , _SOFTWARE_VERSION_REL_MINOR_
                        , _SOFTWARE_VERSION_REL_CUSTOM_);
          
          sPoint.wX = pasDrawList[goSS_TDV_Version].sRect.x;
          sPoint.wY = pasDrawList[goSS_TDV_Version].sRect.y + pasDrawList[goSS_TDV_Version].sRect.h;
          MSMenuTextDynamicDraw(fdMYRIADPRO_17
                                , sPoint
                                , (EGL_COLOR)0xDDDDDD
                                , acBuffer
                                , TRUE);
          sPoint.wX -= 1;
          sPoint.wY -= 1;
          MSMenuTextDynamicDraw(fdMYRIADPRO_17
                                , sPoint
                                , (EGL_COLOR)0x555555
                                , acBuffer
                                , TRUE);

#endif        
        }
        break;

        default : //ASSERT_ALWAYS(); 
        break;
      }
    }
    
    appMSMenuItemsFlagsClear(eLayer
                              , (1ULL << cItem)
                              , (MS_ITEM_UPDATE | MS_ITEM_DRAW));
    
  }

}

/**
 *  @brief      Perform processing for exiting a Splash Screen UI state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void SplashScreenExit(void)
{
	
}

/**
 *  @brief      Perform processing for entering a Main Menu UI state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void LaunchbarEnter(void)
{
  PCMS_MENU_INFO    pcsMenuInfo = &m_sMenuLaunchbar;
  eLAYER_DATA       eLayer = pcsMenuInfo->eLayer;
  PCDRAWABLE        pasDrawList;
  eGO_MENU_MAIN     eObject;
  BYTE              cObjIndex;

  ///set_draw_target(getbackframe());

  pasDrawList = g_asLayers[eLayer].pasDrawList;
  for(cObjIndex=0; cObjIndex < pcsMenuInfo->pasStaticMenuObjects ->cObjectCount; cObjIndex++)
  {
    eObject = pcsMenuInfo->pasStaticMenuObjects->pawStaticObjects[cObjIndex];

    if(dtBITMAP == pasDrawList[eObject].eType)
    {
      MSMenuBitmapStaticDraw(&pasDrawList[eObject]);
    }
    else if(dtRECT == pasDrawList[eObject].eType)
    {
      MSMenuRectStaticDraw(&pasDrawList[eObject]);
    }
  }

  ///flip();  
  
  appMSMenuItemsFlagsSet(eLayer
                          , MS_ALL_ITEMS
                          , (MS_ITEM_VISIBLE | MS_ITEM_DRAW | MS_ITEM_SELECTABLE));

  appMSMenuActiveItemSet(pcsMenuInfo, MS_NO_ACTIVE_ITEM);

  m_cLastActiveMenu = MS_NO_ACTIVE_ITEM;
  m_cSwipeFilerCount = 0;
  m_cHoldFilterCount = 0; 

  m_bPhotoPlayStart = FALSE; 
}

/**
 *  @brief      Perform event handling processing for Main Menu UI state
 *  @param[in]  sUserEvent : event information structure
 *  @param[out] None
 *  @return     TRUE if UI Update needed, FALSE for others
 *  @note       None
 *  @attention  None
 */
static BOOL LaunchbarEvent(USER_EVENT sUserEvent)
{	
  BOOL              bUpdateUI       = FALSE;  
  BOOL              bMenuItemFound  = FALSE;  	
  
  PCMS_MENU_INFO    pcsMenuInfo     = &m_sMenuLaunchbar;
  PCITEM_AREA       pasItemArea     = pcsMenuInfo->pasItemArea;
  eLAYER_DATA       eLayer          = pcsMenuInfo->eLayer;
  ///BYTE const        cActiveItem     = appMSMenuActiveItemGet(pcsMenuInfo);
  PCDRAWABLE        pasDrawList;  
  ///eGO_MENU_MAIN     eObjectId;
  ///WORD              wObjectIndex;
  BYTE              cItem;

  ///PCSTATUS_RUN      pcsStatusRun  = (PCSTATUS_RUN)&appSysStateStatusPtrGet()->sRun; // Reference the Real Time value
  ///PCSTATUS_USER     pcsStatusUser = (PCSTATUS_USER)&appSysStateStatusPtrGet()->sUser; // Reference the Real Time value
  ///PARAM_INFO        sParamInfo;
  eSOUND_ID         eSoundId = sndINVALID;

  /* -------------------------------- Touch Event Process -------------------------------- */
  if(userEVENT_TOUCH == sUserEvent.eEventType
    && NULL != pasItemArea)
  {
    ///PTOUCH_EVENT  psTouchEvent  = (PTOUCH_EVENT)&sUserEvent.uEvent.sTouchEvent;

    /* Touch Event Pre-process */
    cItem = TouchEventPreprocess(pcsMenuInfo, &sUserEvent);
    if(cItem < pcsMenuInfo->cItemCount) bMenuItemFound = TRUE;

    if(bMenuItemFound)
    {
      pasDrawList = g_asLayers[eLayer].pasDrawList;
      
      switch(cItem)
      {
        case menuLB_PHOTO_BTN_1:
        case menuLB_PHOTO_BTN_2:
        case menuLB_PHOTO_BTN_3:
        case menuLB_PHOTO_BTN_4:
        case menuLB_PHOTO_BTN_5:          
          MSMenuBitmapStaticDraw(&pasDrawList[goLB_PHOTO_BTN_1_Sel + (cItem - menuLB_PHOTO_BTN_1)*2]);
          /* Counter and flags set to select image draw until counter expired */          
          utilExecCounterSet(coUI_RENDER_DELAY, uS_TO_TICKS(PERIOD_UI_RENDER_DELAY*5));
          utilExecCounterSet(coUI_STATECHANGE_TIMEOUT, uS_TO_TICKS(PERIOD_UI_RENDER_DELAY*5));
          appMSMenuItemsFlagsSet(eLayer, 0x1 << cItem, MS_ITEM_UPDATE);

          m_ePhotoIdCur = (ePHOTO_ID)(cItem - menuLB_PHOTO_BTN_1);

          //appUIStateChange(uiMENU_MAIN);
          m_bPhotoPlayStart = TRUE;

          eSoundId = sndTOUCH;            
          
          bUpdateUI = TRUE;
        break;      
        
        default : /*ASSERT_ALWAYS();*/ 
        break;			
    	}
    }
  } 

  /* -------------------------------- Key Event Process -------------------------------- */
  if(userEVENT_KEY == sUserEvent.eEventType)
  {
    KEY_EVENT sKeyEvent = sUserEvent.uEvent.sKeyEvent;
    pasDrawList = g_asLayers[eLayer].pasDrawList;

    switch(sKeyEvent.eKey)
    {
      default: break;
    }
  }

  if(sndINVALID != eSoundId)
    appAudioMgrPlay(eSoundId, TRUE);
 
  return bUpdateUI;
}

/**
 *  @brief      Perform renderng for Main Menu UI state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void LaunchbarRender(void)
{
  PCDRAWABLE        pasDrawList;
  PCMS_MENU_INFO    pcsMenuInfo   = &m_sMenuLaunchbar;
  eLAYER_DATA       eLayer        = pcsMenuInfo->eLayer;
  BYTE              cItem;
  BYTE              cFlags;
  ///POINT sPoint = {0,0};
  ///RECT sRect;
  eGO_MENU_MAIN     eObjectId;
  ///WORD              wObjectIndex;
  ///eMENU_ITEM_STATE  eMenuState    = menuSTATE_NORMAL_UNSEL;
  ///STATUS_SYS        sStatusSys    = appSysStateStatusGet();
  ///PCSTATUS_RUN      pcsStatusRun  = (PCSTATUS_RUN)&sStatusSys.sRun;
  ///PCSTATUS_USER     pcsStatusUser = (PCSTATUS_USER)&sStatusSys.sUser;
  ///BYTE const        cActiveItem   = appMSMenuActiveItemGet(pcsMenuInfo);
  ///PARAM_INFO        sParamInfo;  

  pasDrawList = g_asLayers[eLayer].pasDrawList;
  for(cItem = 0; cItem < pcsMenuInfo->cItemCount; cItem++)
  {
    cFlags = appMSMenuItemsFlagsGet(eLayer, cItem);

    if((cFlags & MS_ITEM_VISIBLE) 
    && (cFlags & (MS_ITEM_UPDATE | MS_ITEM_DRAW)))
    {
      switch(cItem)
      {  
        case menuLB_PHOTO_BTN_1:
          eObjectId = goLB_PHOTO_BTN_1_UnSel;
          MSMenuBitmapStaticDraw((PCDRAWABLE)&pasDrawList[eObjectId]); 
        break;

        case menuLB_PHOTO_BTN_2:
          eObjectId = goLB_PHOTO_BTN_2_UnSel;
          MSMenuBitmapStaticDraw((PCDRAWABLE)&pasDrawList[eObjectId]); 
        break;

        case menuLB_PHOTO_BTN_3:
          eObjectId = goLB_PHOTO_BTN_3_UnSel;
          MSMenuBitmapStaticDraw((PCDRAWABLE)&pasDrawList[eObjectId]); 
        break;

        case menuLB_PHOTO_BTN_4:
          eObjectId = goLB_PHOTO_BTN_4_UnSel;
          MSMenuBitmapStaticDraw((PCDRAWABLE)&pasDrawList[eObjectId]); 
        break;

        case menuLB_PHOTO_BTN_5:
          eObjectId = goLB_PHOTO_BTN_5_UnSel;
          MSMenuBitmapStaticDraw((PCDRAWABLE)&pasDrawList[eObjectId]); 
        break;  
                
        default : //ASSERT_ALWAYS(); 
        break;
      }
    }

    appMSMenuItemsFlagsClear(eLayer
                              , (1ULL << cItem)
                              , (MS_ITEM_UPDATE | MS_ITEM_DRAW));
    
  }

}

/**
 *  @brief      Perform processing for exiting a Main Menu UI state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void LaunchbarExit(void)
{
  appMSMenuItemsFlagsClear(m_sMenuLaunchbar.eLayer
                              , MS_ALL_ITEMS
                              , (MS_ITEM_VISIBLE 
                                | MS_ITEM_DRAW 
                                | MS_ITEM_SELECTABLE 
                                | MS_ITEM_UPDATE));


  m_cLastActiveMenu = MS_NO_ACTIVE_ITEM;
}

#if 0
#define MENUBOX_SIZE_WH 5 // 5pixel by 5pixel image
/**
 *  @brief      Draws the Menu item Background to CWM-T10 (Special proceder)
 *  @param[in]  sRect: Rectangle info to draw
                eState: menu background state to draw
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void MenuMainItemBackgroundDraw(RECT sRect, eMENU_ITEM_STATE eState)
{
  PCDRAWABLE    pasDrawList;
  eLAYER_DATA   eLayer = m_sMenuMain.eLayer;
  RECT          sRectObj;
  eGO_MENU_MAIN eObjLeft1, eObjLeft2, eObjLeft3
                , eObjRight1, eObjRight2, eObjRight3
                , eObjTop, eObjBottom; 
  EGL_COLOR     sColor;
  BOOL          bDraw = FALSE;

  switch(m_eUIMgrState)
  {
    case uiMENU_MAIN:
    {
      /* Hard Coding to Product */
      switch(eState)
      {
        case menuSTATE_NORMAL_UNSEL:
          sColor      = COLOR_MENU_BACK_UNSEL;
          
          bDraw = TRUE;
        break;

        case menuSTATE_NORMAL_SEL:      
          sColor      = COLOR_MENU_BACK_SEL;
          
          bDraw = TRUE;
        break;

        case menuSTATE_DEACTIVATE:
          sColor      = COLOR_MENU_BACK_DEACTIVATE;

          bDraw = TRUE;
        break;

        default: ASSERT_ALWAYS(); break;    
      }
    }
    break;

    default: ASSERT_ALWAYS(); break;
  }


  if(bDraw)
  {
    pasDrawList = g_asLayers[eLayer].pasDrawList;

    /* Draw Left-Top Corner */  
    sRectObj.x = sRect.x;
    sRectObj.y = sRect.y;
    sRectObj.w = MENUBOX_SIZE_WH;
    sRectObj.h = MENUBOX_SIZE_WH;
    MSMenuBitmapDraw(pasDrawList[eObjLeft1].wIndex, sRectObj);

    /* Draw Top Middle */  
    sRectObj.x += sRectObj.w;
    sRectObj.w = sRect.w - MENUBOX_SIZE_WH*2;
    MSMenuBitmapDraw(pasDrawList[eObjTop].wIndex, sRectObj);

    /* Draw Right-Top Corner */  
    sRectObj.x += sRectObj.w;
    sRectObj.w = MENUBOX_SIZE_WH;
    MSMenuBitmapDraw(pasDrawList[eObjRight1].wIndex, sRectObj); 

    /* Draw Left Middle */
    sRectObj.x = sRect.x;
    sRectObj.y = sRect.y + MENUBOX_SIZE_WH;
    sRectObj.w = MENUBOX_SIZE_WH;
    sRectObj.h = sRect.h - MENUBOX_SIZE_WH*2;
    MSMenuBitmapDraw(pasDrawList[eObjLeft2].wIndex, sRectObj);  

    /* Draw Center */
    sRectObj.x += sRectObj.w;
    sRectObj.w = sRect.w - MENUBOX_SIZE_WH*2;
    MSMenuRectDynamicDraw(sRectObj, sColor);

    /* Draw Rigth Moddile */
    sRectObj.x += sRectObj.w;
    sRectObj.w = MENUBOX_SIZE_WH;
    MSMenuBitmapDraw(pasDrawList[eObjRight2].wIndex, sRectObj);


    /* Draw Left-Bottom Corner */
    sRectObj.x = sRect.x;
    sRectObj.y = sRect.y + MENUBOX_SIZE_WH + (sRect.h - MENUBOX_SIZE_WH*2);
    sRectObj.h = MENUBOX_SIZE_WH;
    sRectObj.w = MENUBOX_SIZE_WH;
    MSMenuBitmapDraw(pasDrawList[eObjLeft3].wIndex, sRectObj);

    /* Draw Bottom Middle */
    sRectObj.x += sRectObj.w;
    sRectObj.w = sRect.w - MENUBOX_SIZE_WH*2;
    MSMenuBitmapDraw(pasDrawList[eObjBottom].wIndex, sRectObj);

    /* Draw Right-Bottom Corner */
    sRectObj.x += sRectObj.w;
    sRectObj.w = MENUBOX_SIZE_WH;
    MSMenuBitmapDraw(pasDrawList[eObjRight3].wIndex, sRectObj);   
  }
}
#endif

/**
 *  @brief      Play the sentence the specified item when the active item Set
 *  @param[in]  cItem: Menu item to Enter active 
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void MenuMainActiveItemEnterPlay(BYTE cItem)
{

}


/**
 *  @brief      Play the sentence the specified item when the active item Clear
 *  @param[in]  cItem: Menu item to Exit active
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void MenuMainActiveItemExitPlay(BYTE cItem)
{

}

/**
 *  @brief      Relative with Active Item's Flags Set
 *  @param[in]  cItem: Menu item to Flags Set
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void MenuMainActiveItemRelativeUpdateSet(BYTE cItem)
{
  QWORD qwItemMaskUpdate = 0ULL;
  
  switch(cItem)
  { 
    default: /* NTD */ break;
  }

  if(qwItemMaskUpdate)
  {
    appMSMenuStateItemsFlagsSet(uiMENU_MAIN
                                , qwItemMaskUpdate
                                , MS_ITEM_UPDATE);  
  }
}


/**
 *  @brief      Perform processing for entering a Menu UI state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void MenuMainEnter(void)
{
  PCMS_MENU_INFO    pcsMenuInfo = &m_sMenuMain;
  eLAYER_DATA       eLayer = pcsMenuInfo->eLayer;
  PCDRAWABLE        pasDrawList;
  eGO_MENU_MAIN     eObject;
  BYTE              cObjIndex;

  ///set_draw_target(getbackframe());

  pasDrawList = g_asLayers[eLayer].pasDrawList;
  for(cObjIndex=0; cObjIndex < pcsMenuInfo->pasStaticMenuObjects ->cObjectCount; cObjIndex++)
  {
    eObject = pcsMenuInfo->pasStaticMenuObjects->pawStaticObjects[cObjIndex];

    if(dtBITMAP == pasDrawList[eObject].eType)
    {
      MSMenuBitmapStaticDraw(&pasDrawList[eObject]);
    }
    else if(dtRECT == pasDrawList[eObject].eType)
    {
      MSMenuRectStaticDraw(&pasDrawList[eObject]);
    }
  }

  ///flip();  
  
  appMSMenuItemsFlagsSet(eLayer
                          , MS_ALL_ITEMS
                          , (MS_ITEM_VISIBLE | MS_ITEM_DRAW | MS_ITEM_SELECTABLE));

  appMSMenuActiveItemSet(pcsMenuInfo, MS_NO_ACTIVE_ITEM);

  m_cLastActiveMenu = MS_NO_ACTIVE_ITEM;
  m_cSwipeFilerCount = 0;
  m_cHoldFilterCount = 0;   
}

/**
 *  @brief      Perform event handling processing for Menu UI state
 *  @param[in]  sUserEvent : event information structure
 *  @param[out] None
 *  @return     TRUE if UI Update needed, FALSE for others
 *  @note       None
 *  @attention  None
 */
static BOOL MenuMainEvent(USER_EVENT sUserEvent)
{	
  BOOL              bUpdateUI       = FALSE;  
  BOOL              bMenuItemFound  = FALSE;    
  
  PCMS_MENU_INFO    pcsMenuInfo     = &m_sMenuMain;
  PCITEM_AREA       pasItemArea     = pcsMenuInfo->pasItemArea;
  eLAYER_DATA       eLayer          = pcsMenuInfo->eLayer;
  ///BYTE const        cActiveItem     = appMSMenuActiveItemGet(pcsMenuInfo);
  PCDRAWABLE        pasDrawList;  
  ///eGO_MENU_MAIN     eObject;    
  BYTE              cItem;

  ///PCSTATUS_RUN      pcsStatusRun  = (PCSTATUS_RUN)&appSysStateStatusPtrGet()->sRun; // Reference the Real Time value
  ///PCSTATUS_USER     pcsStatusUser = (PCSTATUS_USER)&appSysStateStatusPtrGet()->sUser; // Reference the Real Time value
  ///PARAM_INFO        sParamInfo;
  eSOUND_ID         eSoundId      = sndINVALID;

  /* -------------------------------- Touch Event Process -------------------------------- */
  if(userEVENT_TOUCH == sUserEvent.eEventType
    && NULL != pasItemArea)
  {
    ///PTOUCH_EVENT  psTouchEvent  = (PTOUCH_EVENT)&sUserEvent.uEvent.sTouchEvent;

    /* Touch Event Pre-process */
    cItem = TouchEventPreprocess(pcsMenuInfo, &sUserEvent);
    if(cItem < pcsMenuInfo->cItemCount) bMenuItemFound = TRUE;

    if(bMenuItemFound)
    {
      pasDrawList = g_asLayers[eLayer].pasDrawList;
      
      switch(cItem)
      { 
        case menuMM_NAVI_BTN_Prev:
          MSMenuBitmapStaticClear(&pasDrawList[goMM_NAVI_BTN_Prev_Sel]);
          MSMenuBitmapStaticDraw(&pasDrawList[goMM_NAVI_BTN_Prev_Sel]);
          /* Counter and flags set to select image draw until counter expired */
          utilExecCounterSet(coUI_RENDER_DELAY, uS_TO_TICKS(PERIOD_UI_RENDER_DELAY));
          appMSMenuItemsFlagsSet(eLayer, 0x1 << cItem | 0x1 << menuMM_PHOTO, MS_ITEM_UPDATE);

          m_ePhotoIdCur++;
          if(m_ePhotoIdCur >= photoID_INVALID) m_ePhotoIdCur = (ePHOTO_ID)0;

          MSMenuBitmapStaticClear(NULL);

          eSoundId = sndTOUCH;
          
          bUpdateUI = TRUE;          
        break;
        
        case menuMM_NAVI_BTN_Home:
          MSMenuBitmapStaticClear(&pasDrawList[goMM_NAVI_BTN_Home_Sel]);
          MSMenuBitmapStaticDraw(&pasDrawList[goMM_NAVI_BTN_Home_Sel]);
          /* Counter and flags set to select image draw until counter expired */
          utilExecCounterSet(coUI_RENDER_DELAY, uS_TO_TICKS(PERIOD_UI_RENDER_DELAY*5));
          utilExecCounterSet(coUI_STATECHANGE_TIMEOUT, uS_TO_TICKS(PERIOD_UI_RENDER_DELAY*5));
          appMSMenuItemsFlagsSet(eLayer, 0x1 << cItem, MS_ITEM_UPDATE);

          ///appUIStateChange(uiLAUNCHBAR);
          m_bPhotoPlayStart = FALSE; 

          eSoundId = sndCHAP;
          
          bUpdateUI = TRUE;   
        break;
        
        case menuMM_NAVI_BTN_Next:
          MSMenuBitmapStaticClear(&pasDrawList[goMM_NAVI_BTN_Next_Sel]);
          MSMenuBitmapStaticDraw(&pasDrawList[goMM_NAVI_BTN_Next_Sel]);
          /* Counter and flags set to select image draw until counter expired */
          utilExecCounterSet(coUI_RENDER_DELAY, uS_TO_TICKS(PERIOD_UI_RENDER_DELAY));
          appMSMenuItemsFlagsSet(eLayer, 0x1 << cItem | 0x1 << menuMM_PHOTO, MS_ITEM_UPDATE);

          if(0 == (BYTE)m_ePhotoIdCur) m_ePhotoIdCur = (ePHOTO_ID)((BYTE)photoID_INVALID - 1);
          else m_ePhotoIdCur--;

          eSoundId = sndTOUCH;
          
          bUpdateUI = TRUE;            
        break;
        
        default : /*ASSERT_ALWAYS();*/ 
        break;      
      }
    }
  } 


  /* -------------------------------- Key Event Process -------------------------------- */
  if(userEVENT_KEY == sUserEvent.eEventType)
  {
    KEY_EVENT sKeyEvent = sUserEvent.uEvent.sKeyEvent;
    pasDrawList = g_asLayers[eLayer].pasDrawList;

    switch(sKeyEvent.eKey)
    {
      default: break;
    }
  }

  if(sndINVALID != eSoundId)
    appAudioMgrPlay(eSoundId, TRUE);
 
  return bUpdateUI;
}

/**
 *  @brief      Perform renderng for Menu UI state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void MenuMainRender(void)
{
  PCDRAWABLE        pasDrawList;
  PCMS_MENU_INFO    pcsMenuInfo   = &m_sMenuMain;
  eLAYER_DATA       eLayer        = pcsMenuInfo->eLayer;
  BYTE              cItem;
  BYTE              cFlags;
  ///POINT sPoint = {0,0};
  ///RECT sRect;
  eGO_MENU_MAIN     eObjectId;
  ///WORD              wObjectIndex;
  ///eMENU_ITEM_STATE  eMenuState    = menuSTATE_NORMAL_UNSEL;
  ///STATUS_SYS        sStatusSys    = appSysStateStatusGet();
  ///PCSTATUS_RUN      pcsStatusRun  = (PCSTATUS_RUN)&sStatusSys.sRun;
  ///PCSTATUS_USER     pcsStatusUser = (PCSTATUS_USER)&sStatusSys.sUser;
  ///BYTE const        cActiveItem   = appMSMenuActiveItemGet(pcsMenuInfo);
  ///PARAM_INFO        sParamInfo;  

  pasDrawList = g_asLayers[eLayer].pasDrawList;
  for(cItem = 0; cItem < pcsMenuInfo->cItemCount; cItem++)
  {
    cFlags = appMSMenuItemsFlagsGet(eLayer, cItem);

    if((cFlags & MS_ITEM_VISIBLE) 
    && (cFlags & (MS_ITEM_UPDATE | MS_ITEM_DRAW)))
    {
      switch(cItem)
      {   
        case menuMM_PHOTO:
          MSMenuRectStaticDraw((PCDRAWABLE)&pasDrawList[goMM_PHOTO_Erase]);          
          eObjectId = goMM_PHOTO_1 + m_ePhotoIdCur;
          MSMenuBitmapStaticDraw((PCDRAWABLE)&pasDrawList[eObjectId]); 
        break;

        case menuMM_NAVI_BTN_Prev:
          MSMenuBitmapStaticDraw((PCDRAWABLE)&pasDrawList[goMM_NAVI_BTN_Prev_UnSel]); 
        break;
        
        case menuMM_NAVI_BTN_Home:
          MSMenuBitmapStaticDraw((PCDRAWABLE)&pasDrawList[goMM_NAVI_BTN_Home_UnSel]); 
        break;
        
        case menuMM_NAVI_BTN_Next:
          MSMenuBitmapStaticDraw((PCDRAWABLE)&pasDrawList[goMM_NAVI_BTN_Next_UnSel]); 
        break;
                
        default : //ASSERT_ALWAYS(); 
        break;
      }
    }

    appMSMenuItemsFlagsClear(eLayer
                              , (1ULL << cItem)
                              , (MS_ITEM_UPDATE | MS_ITEM_DRAW));
    
  }
}

/**
 *  @brief      Perform processing for exiting a Menu UI state
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void MenuMainExit(void)
{
	appMSMenuItemsFlagsClear(m_sMenuMain.eLayer
                            , MS_ALL_ITEMS
                            , (MS_ITEM_VISIBLE 
															| MS_ITEM_DRAW 
															| MS_ITEM_SELECTABLE 
															| MS_ITEM_UPDATE));

  
  m_cLastActiveMenu = MS_NO_ACTIVE_ITEM;
}

/**
 *  @brief      Perform processing for entering a new UI state
 *  @param[in]  eState : Current state of the UIne
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void UIStateEnter(eUI_STATE eState)
{

/* -------------- BEGIN CUSTOM AREA [020] appUserInterface.c -------------- */
  switch (eState)
  {
    case uiIDLE:
      /* NTD */
    break;
      
    case uiSPLASH:
      SplashScreenEnter();
    break;

    case uiLAUNCHBAR:
      LaunchbarEnter();
    break;
   
    case uiMENU_MAIN:
      MenuMainEnter();
    break;
    
    default: break;
  }
/* ---------------------------- END CUSTOM AREA --------------------------- */
}

/**
 *  @brief      Perform event handling processing for current UI state
 *  @param[in]  eState    : current UI state
                sUserEvent : event information structure
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void UIStateEvent(eUI_STATE eState,
                         USER_EVENT sUserEvent)

{
  ///ePOWERSTATE ePwrState = appPowerMgrPowerStateGet();
  BOOL bUpdateUI = FALSE;

  if(userEVENT_KEY == sUserEvent.eEventType)
  {
    PKEY_EVENT psKeyEvent = (PKEY_EVENT)&sUserEvent.uEvent.sKeyEvent;

    KeyRawEventTranslate(psKeyEvent);

    // Do not Process keNONE
    if(keNONE == psKeyEvent->eKey) return;

    // Check if event should be filter out and ignored
    if (KeyEventFilter(psKeyEvent->eKey))
    {
      return; //---------------------> ignore event and EXIT
    }
    else
    {
      switch (psKeyEvent->eKey)
      {
        default: KeyEventDefault(eState); break;
      }
    }
  }

 
/* -------------- BEGIN CUSTOM AREA [030] appUserInterface.c -------------- */

  switch (eState)
  {
   case uiIDLE:
      /* NTD */
    break;
      
    case uiSPLASH:
      bUpdateUI = SplashScreenEvent(sUserEvent);
    break;

    case uiLAUNCHBAR:
      bUpdateUI = LaunchbarEvent(sUserEvent);
    break;    
   
    case uiMENU_MAIN:
      bUpdateUI = MenuMainEvent(sUserEvent);
    break; 
    
    default: break;
  }
/* ---------------------------- END CUSTOM AREA --------------------------- */

  if(bUpdateUI)
  {
    appUIUpdateSet();
  }
}

/**
 *  @brief      Perform renderng for current UI state
 *  @param[in]  eState : UI state to render
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void UIStateRender(eUI_STATE eState)
{
  ///BOOL bUpdateUI = FALSE;
  
/* -------------- BEGIN CUSTOM AREA [040] appUserInterface.c -------------- */
  switch (eState)
  {
    case uiIDLE:
      /* NTD */
    break;
      
    case uiSPLASH:      
      SplashScreenRender();
    break;

    case uiLAUNCHBAR:
      LaunchbarRender();
    
    case uiMENU_MAIN:
      MenuMainRender();
    break; 
    
    default: break;
  }
/* ---------------------------- END CUSTOM AREA --------------------------- */
}

/**
 *  @brief      Perform processing for exiting a UI state
 *  @param[in]  eState : UI state to render
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void UIStateExit(eUI_STATE eState)
{
/* -------------- BEGIN CUSTOM AREA [050] appUserInterface.c -------------- */
  switch (eState)
  {

    case uiIDLE:
      /* NTD */
    break;
      
    case uiSPLASH:
      SplashScreenExit();
    break;

    case uiLAUNCHBAR:
      LaunchbarExit();
    break;    
   
    case uiMENU_MAIN:
      MenuMainExit();
    break; 

    default:  break;
  }
/* ---------------------------- END CUSTOM AREA --------------------------- */
}

// ***************************************************************************
// **************** START OF PUBLIC PROCEDURE IMPLEMENTATIONS ****************
// ***************************************************************************
void monitor_cb(lv_disp_drv_t * d, uint32_t t, uint32_t p)
{
	t_saved = t;
}

static void tft_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p);
static BOOL touchpad_read(lv_indev_drv_t *drv, lv_indev_data_t *data);

/**
 * Flush a color buffer
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color_p pointer to an array of colors
 */
static void tft_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p)
{ 
	if(area->x2 < 0) return;
	if(area->y2 < 0) return;
	if(area->x1 > PANE_WIDTH_MAX - 1) return;
	if(area->y1 > PANE_HEIGHT_MAX - 1) return;

  int y = 0;
  int nWidth  = (area->x2 - area->x1)+1;
  int nHeight = (area->y2 - area->y1)+1;
  
  SURFACE* psSurfacePattern = create_surface(1280, 800, 16);
  // SURFACE* psSurfacePattern = get_front_buffer();
  U16* pcPixelData = (U16*)psSurfacePattern->pixels;
  
  for(y=area->y1; y<=area->y2; y++)
  {
    lv_memcpy(pcPixelData+y*psSurfacePattern->pitch/2+area->x1, (uint32_t)color_p, (area->x2-area->x1+1)*2);    
    color_p += area->x2-area->x1+1;
  }
  lv_disp_flush_ready(drv);
  draw_surface_scalerect(psSurfacePattern, area->x1, area->y1, nWidth, nHeight,
                                             area->x1, area->y1, nWidth, nHeight);
  release_surface(psSurfacePattern);
}

static BOOL touchpad_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
  static int16_t last_x = 0;
  static int16_t last_y = 0;

  bool detected;
  POINT sPoint;
  // int16_t x;
  // int16_t y;
  // detected = (rcSUCCESS == dvILI2511TouchRead(&sPoint)) ? TRUE : FALSE;
  detected = (rcSUCCESS == dvAK4183TouchGet(0,&sPoint)) ? TRUE : FALSE;

  if(detected)
  {
    data->point.x = sPoint.wX;
    data->point.y = sPoint.wY;
    last_x = data->point.x;
    last_y = data->point.y;

    data->state = LV_INDEV_STATE_PR;
  }
  else
  {
    data->point.x = last_x;
    data->point.y = last_y;

    data->state = LV_INDEV_STATE_REL;
  }

  return false;
}

/**
 *  @brief      This API sets up the internal state of this component.
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appUIInitialize(void)
{
  // WORD          wIndex;
  // PCDRAWABLE    pasDrawList;
  // eGO_SPLASH    eObject;

  m_bUpdateUI = FALSE;  

  // initialize user interface state
  m_eUIMgrState 
          = m_eUIMgrStatePrev 
          = m_eUIMgrStateNext = uiIDLE;  
 
  /* -------------- BEGIN CUSTOM AREA [060] appUserInterface.c -------------- */
  /*
    if LCD's resolution is 800x480.
    Frame-buffer size = 1024 * 512 * 2 (RGB565) * 2(doube buffer) = 2Mbyte
    Frame-buffer size = 1024 * 512 * 4 (RGB888) * 2(doube buffer) = 4Mbyte

    if LCD's resolution is 800x600.
    Frame-buffer size = 1024 * 1024 * 2 (RGB565) * 2(doube buffer) = 4Mbyte
    Frame-buffer size = 1024 * 1024 * 4 (RGB888) * 2(doube buffer) = 8Mbyte

    if LCD's resolution is 1280x800.
    Frame-buffer size = 2048 * 1024 * 2 (RGB565) * 2(doube buffer) = 8Mbyte
    Frame-buffer size = 2048 * 1024 * 4 (RGB888) * 2(doube buffer) = 16Mbyte (only single buffer)

    texture memory start address = frame buffer start + frame buffer size
  */ 
  ge_set_texture_mem(0xc2900000, 0x5700000);

  
  /* Panel Timing ---------------------------------------------- 
    HT(Horizontal Total)              = th(One Horizontal line) 
    HSS(Horizontal Sync Start)        = thfp(HS front porch) 
    HSE(Horizontal Sync End)          = HSS+thpw(HS pulse Width) 
    HAS(Horizontal Sync Active Start) = HSS+thb(HS back porch) 
    HAE(Horizontal Sync Active End)   = HAS+thd(HS display area) 
    
    VT(Vertical Total)                = tv(One Vertical line) 
    VSS(Vertical Sync Start)          = tvfp(VS front porch) 
    VSE(Vertical Sync End)            = VSS+tvpw(VS pulse Width) 
    VAS(Vertical Sync Active Start)   = VSS+tvb(VS back porch) 
    VAE(Vertical Sync Active End)     = VAS+tvd(VS display area)
  -------------------------------------------------------------- */
#if 1  
  setscreenex(0xc2100000          // Frame Buffer Address.
              , PANE_WIDTH_MAX    // LCD Horizontal Pixel Count
              , PANE_HEIGHT_MAX   // LCD Vertical Pixel Count
#if USE_DOUBLE_FRAME_BUFFER              
              , SCREENMODE_RGB888 | SCREENMODE_DOUBLEBUF // Screen Mode
#else
              , SCREENMODE_RGB565 | SCREENMODE_SINGLEBUF // Screen Mode
#endif
#if (USE_AMAZON_STK)  // HD50WV-PF1 800x480@60Hz
              // ------- Panel Timing to STK 800x480 ------- //
              , 928                   // HT
              , ( 40 << 16 ) | 88     // HSS << 16 | HSE
              , ( 128 << 16 ) | 928   // HAS << 16 | HAE
              , 525                   // VT
              , ( 13 << 16 ) | 16     // VSS << 16 | VSE
              , ( 45 << 16 ) | 525    // VAS << 16 | VAE           
              // -------------------------------------------- //
#elif (USE_LCD_1024x600)              
              // ----- Panel Timing to CD 1024x600@60Hz ----- //
              , 1344                   // HT
              , ( 100 << 16 ) | 220    // HSS << 16 | HSE
              , ( 320 << 16 ) | 1344   // HAS << 16 | HAE
              , 635                    // VT
              , ( 10 << 16 ) | 15      // VSS << 16 | VSE
              , ( 35 << 16 ) | 635     // VAS << 16 | VAE           
              // -------------------------------------------- //
#elif (USE_LCD_1280x800)
              // ----- Panel Timing to VA 1280x800@60Hz ----- //
              , 1440                   // HT
              , ( 40 << 16 ) | 120     // HSS << 16 | HSE
              , ( 160 << 16 ) | 1440   // HAS << 16 | HAE
              , 823                    // VT
              , ( 10 << 16 ) | 13      // VSS << 16 | VSE
              , ( 23 << 16 ) | 823     // VAS << 16 | VAE           
              // -------------------------------------------- //
#endif
              , VMODE_RGB             // Graphic(Video) Mode
              );  
#endif
  /* Clear Frame Buffer */
  appUIClear(TRUE);
  /* Draw Black Rectagle to remove GUI Garbage First (1st Buffer) */
  MSMenuRectDynamicDraw((RECT){0,0,PANE_WIDTH_MAX, PANE_HEIGHT_MAX} , (EGL_COLOR)COLOR_WHITE); 
  
#if 0//1
  static lv_color_t disp_buf1[PANE_WIDTH_MAX*PANE_HEIGHT_MAX];
  static lv_disp_draw_buf_t buf; 

  lv_disp_draw_buf_init(&buf, disp_buf1, NULL, PANE_WIDTH_MAX*PANE_HEIGHT_MAX);
  lv_disp_drv_init(&disp_drv);

  disp_drv.draw_buf = &buf;
  disp_drv.flush_cb = tft_flush;
  disp_drv.monitor_cb = monitor_cb;
  disp_drv.hor_res = PANE_WIDTH_MAX;
  disp_drv.ver_res = PANE_HEIGHT_MAX;
  // disp_drv.full_refresh = 1;
  lv_disp_drv_register(&disp_drv);

  lv_indev_drv_init(&indev_drv);
  indev_drv.read_cb = touchpad_read;
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  lv_indev_drv_register(&indev_drv);
#endif
  /* LVDS On */
  // Send >1ms of black video data; this allows the LVDS83B to be phase locked, 
  // and the display to show black data first.
  halLCDCtrlLVDSOnSet(TRUE);

#if USE_INTRO_MOVIE
  {
    /* Load Intro Movie */
    MOVIE* psIntroMovie = loadmovie(LAYER_MOVIE_NAME, TRUE);

    /* LCD ON */
    halLCDCtrlBrightnessSet(90);  
    halLCDCtrlBluOnSet(TRUE);
    
    if(psIntroMovie == NULL )
    {
      msgErr("Intro Movie Load Error !!");
    }  
    else
    {
      MOVIE_DRAW_RESULT eMovieDrawResult;

      // Start Movie Play
      if(movie_play(psIntroMovie, 0, 0, TRUE))
      {
        do
        {
          // Next frame
          eMovieDrawResult = movie_drawnext(psIntroMovie, 0, 0, TRUE);          
        }while(MOVIE_DRAW_RESULT_OK == eMovieDrawResult);

        if(MOVIE_DRAW_RESULT_FAIL == eMovieDrawResult)
        {
          msgErr("MOVIE Play ERROR");
        }
      }

      /* Release Movie Memory */
      release_movie(psIntroMovie);

      commDelayMS(2000);
    }
  }
#endif  

#if 0//USE_LCD_TEST_PATTERN  
    {
      SURFACE* psSurfacePattern = NULL;
      // PBYTE    pcPixelData = NULL;
      // int index2 = 0;  
      psSurfacePattern = loadbmp("TestPattern.bmp");
      // pcPixelData = (PBYTE)psSurfacePattern->pixels;

      // SURFACE* psSurfacePattern = create_surface(200,400,16);
      // psSurfacePattern = loadbmp("TestPattern.bmp");      
      PWORD pwPixelData = (PWORD)psSurfacePattern->pixels;
      int i,j;
      WORD wPixelBuf[400];
      memset(wPixelBuf, 0, sizeof(wPixelBuf));

      // for(j = 0; j < 400; j++)
      // {
      //   for(i = 0; i<200; i++)
      //   {
      //     wPixelBuf[i] = 0xf800;  
      //   }
      //   memcpy(psSurfacePattern->pixels + psSurfacePattern->pitch*j, wPixelBuf, 200*2);
      // }

      // release_surface(psSurfacePattern);

      SURFACE* psSurfacePattern_1 = create_surface(200,400,16);
      msgDbg("w = %d, 1:pitch = %d",psSurfacePattern_1->w, (psSurfacePattern_1->w*16)/8);
      msgDbg("pitch + 3 = %d, & ~3 = %d",(psSurfacePattern_1->w*16)/8 + 3, ((psSurfacePattern_1->w*16)/8 + 3) & ~3);
      msgDbg("pitch = %d", psSurfacePattern_1->pitch);
              
      if(NULL != psSurfacePattern)
      {       
        draw_surface_scalerect(psSurfacePattern, 0,0,100,100,0,0,100,100);
        // draw_surface(psSurfacePattern_1, 100, 0);
        // draw_surface(psSurfacePattern_dest, 500,0);
        
        //draw_surface(get_front_buffer(), 500,0);
       
        // release_surface(psSurfacePattern);
      }
      else
      {
        MSMenuRectDynamicDraw((RECT){0,0,PANE_WIDTH_MAX, PANE_HEIGHT_MAX} , (EGL_COLOR)COLOR_WHITE);  
        MSMenuRectDynamicDraw((RECT){(PANE_WIDTH_MAX - (PANE_WIDTH_MAX/2))/2 
                                      , (PANE_HEIGHT_MAX - (PANE_HEIGHT_MAX/2))/2 
                                      , PANE_WIDTH_MAX/2
                                      , PANE_HEIGHT_MAX/2}
                              , (EGL_COLOR)COLOR_BLACK);
      }
    }                                  
    
#endif
#if 0  
  /* Draw Logo Faster */  
  pasDrawList = g_asLayers[m_sMenuSplash.eLayer].pasDrawList;

  /* Splash Image Load */
  for(m_wImageObjectLoaded = 0
        ; m_wImageObjectLoaded < IMAGE_OBJECT_SS_COUNT
        ; m_wImageObjectLoaded++)
  {
	  m_asSurfaceImage[m_wImageObjectLoaded] = UILoadImage(m_wImageObjectLoaded);
  }
#if USE_DEBUG_UI      
	msgInfo(_MOD_NAME_": SplashScreen Image Load (%d) Done", IMAGE_OBJECT_SS_COUNT);
#endif
#endif
#if 0//!USE_LCD_TEST_PATTERN
  /* Splash Screen hard draw */
  eObject = goSS_STATIC_Background;
  MSMenuRectStaticDraw((PCDRAWABLE)&pasDrawList[eObject]); 

  eObject = goSS_STATIC_Product;
  MSMenuBitmapStaticDraw((PCDRAWABLE)&pasDrawList[eObject]); 
  release_surface(m_asSurfaceImage[pasDrawList[eObject].wIndex]); // NOTE: Do not Access Released Surface !

  eObject = goSS_STATIC_Product_Sub;
  MSMenuBitmapStaticDraw((PCDRAWABLE)&pasDrawList[eObject]);
  release_surface(m_asSurfaceImage[pasDrawList[eObject].wIndex]); // NOTE: Do not Access Released Surface !

  eObject = goSS_RECT_ProgressFrame;  
  MSMenuRectStaticDraw((PCDRAWABLE)&pasDrawList[eObject]);

  eObject = goSS_RECT_ProgressBack;  
  MSMenuRectStaticDraw((PCDRAWABLE)&pasDrawList[eObject]);

  eObject = goSS_STATIC_Loading;
  MSMenuBitmapStaticDraw((PCDRAWABLE)&pasDrawList[eObject]); 
  release_surface(m_asSurfaceImage[pasDrawList[eObject].wIndex]); // NOTE: Do not Access Released Surface !

  eObject = goSS_STATIC_Vendor;
  MSMenuBitmapStaticDraw((PCDRAWABLE)&pasDrawList[eObject]); 
  release_surface(m_asSurfaceImage[pasDrawList[eObject].wIndex]); // NOTE: Do not Access Released Surface !
#endif  
   
   
#if 0//USE_FONT_DRAW
  /* Load Font */
  for(wIndex = 0; wIndex < fdINVALID; wIndex++)
  {
    m_pasFont[wIndex] = create_bmpfont(g_asFontObj[wIndex].pcPath);
  }
#endif   

#if !USE_INTRO_MOVIE  
    /* LCD ON */
    halLCDCtrlBrightnessSet(90); 
    halLCDCtrlBluOnSet(TRUE);
#endif
	/* ---------------------------- END CUSTOM AREA --------------------------- */    
  
  utilExecSignalClear(sgUI, 0xFFFF);

  utilExecMsgClear(mqUSER_INPUT);
  utilExecMsgClear(mqUSER_TOUCH);  
}

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
void appUIProcess(void)
{
  BYTE				        bStateChanged;
  USER_EVENT          sUserEvent = {userEVENT_NONE,};
  
  /* -------------- BEGIN CUSTOM AREA [070] appUserInterface.c -------------- */
  if(!(uiIDLE == m_eUIMgrState 
       && uiSPLASH == m_eUIMgrStateNext))
  {
    ProcessStateChangeOnTimeout();

    if(uiSPLASH == m_eUIMgrState)
    {
      ProcessLoading();
    }
    else
    {
      halUICtrlProcess();
    }
  }
  /* ---------------------------- END CUSTOM AREA --------------------------- */       

  /* Process Counter Event */
  ProcessUICounterEvent();

  /* Process Sound */
  ProcessSound();

  // check if user input control has a new keypress
  if (utilExecMsgPresent(mqUSER_INPUT))
  {
    utilExecMsgRead(mqUSER_INPUT, (PBYTE)&sUserEvent.uEvent.sKeyEvent);
    utilExecMsgClear(mqUSER_INPUT);
    sUserEvent.eEventType = userEVENT_KEY;
  }
  else
  {
    // check if user touch control has a new event
    if(utilExecMsgPresent(mqUSER_TOUCH))
    {
      utilExecMsgRead(mqUSER_TOUCH, (PBYTE)&sUserEvent.uEvent.sTouchEvent);
      utilExecMsgClear(mqUSER_TOUCH);
      sUserEvent.eEventType = userEVENT_TOUCH;

      /*msgDbg("X:%d, Y:%d, F:0x%.4X, L:%d"
                    , sUserEvent.uEvent.sTouchEvent.sPoint.wX
                    , sUserEvent.uEvent.sTouchEvent.sPoint.wY
                    , sUserEvent.uEvent.sTouchEvent.wEventFlags
                    , sUserEvent.uEvent.sTouchEvent.wEventLevel);*/
    }
    else
    {
      sUserEvent.eEventType = userEVENT_NONE;
    }  
  }

  /* User Event Process */
  if (userEVENT_NONE != sUserEvent.eEventType)
  {
    UIStateEvent(m_eUIMgrState, sUserEvent);
  }

  bStateChanged = (m_eUIMgrStateNext != m_eUIMgrState);
  
/* -------------- BEGIN CUSTOM AREA [077] appUserInterface.c -----------------
 * Add code here to force a state change if required even though the event
 * handler didn't change the state. */
 
/* ---------------------------- END CUSTOM AREA --------------------------- */

  // if the user interface state has changed
  if (bStateChanged)
  {
    // execute exit code for current state
    UIStateExit(m_eUIMgrState);

    // execute entry code for next state
    UIStateEnter(m_eUIMgrStateNext);

/* -------------- BEGIN CUSTOM AREA [078] appUserInterface.c -----------------
* Add code here to force a state change if required even though the event
* handler didn't change the state. */

    // Clear Render delay
    utilExecCounterSet(coUI_RENDER_DELAY, 0);

/* ---------------------------- END CUSTOM AREA --------------------------- */
      
    // update state information and set flag to redraw graphics
    m_eUIMgrStatePrev = m_eUIMgrState;
    m_eUIMgrState = m_eUIMgrStateNext;
    m_bUpdateUI = TRUE;
  }

  // if the user interface graphics need to be updated,
  if (m_bUpdateUI
    && !utilExecCounterGet(coUI_RENDER_DELAY))
  {
    m_bUpdateUI = FALSE;

    // call the rendering routine for the current state
    UIStateRender(m_eUIMgrState);
  }
}

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
BOOL appUIStateIsIdle(void)
{
  return (m_eUIMgrState == uiIDLE);
}


/**
 *  @brief      Change the state of the user interface to the specified value.
 *  @param[in]  eState : state to transition to
 *  @param[out] None
 *  @return     None
 *  @note       This API must ONLY be called from within the User Interface component,
                     and only from appUIxxxEvent routines.
 *  @attention  None
 */
void appUIStateChange(eUI_STATE eState)
{
  m_eUIMgrStateNext = eState;
}

/**
 *  @brief      Set a flag indicating that the user interface graphics need to be updated
 *  @param[in]  eState : state to transition to
 *  @param[out] None
 *  @return     None
 *  @note       This API must ONLY be called from within the User Interface component,
                     and only from appUIxxxEvent routines.
 *  @attention  None
 */
void appUIUpdateSet(void)
{
  m_bUpdateUI = TRUE;
}

/**
 *  @brief      Enable or disable the UI
 *  @param[in]  bEnable : TRUE to enable the UI (reset the current state), FALSE to disable
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appUIEnable(BOOL bEnable)
{
  // If disabling, exit the current state
  if (!bEnable)
  {
    UIStateExit(m_eUIMgrState);
  }
  // Otherwise, re-enter the current state and flag the UI to be updated
  else
  {
    UIStateEnter(m_eUIMgrState);
    m_bUpdateUI = TRUE;
  }
}

/**
 *  @brief      Clear the UI Frame Buffer
 *  @param[in]  bBackFrameOnly : TRUE to Clear the Back Frame Buffer only (Double Buffer Mode only)
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appUIClear(BOOL bBackFrameOnly)
{
  SURFACE* psSurface = NULL;
  PBYTE pcPixelData = NULL;

  psSurface = get_front_buffer(); 
  pcPixelData = (PBYTE)psSurface->pixels;  
	memset(pcPixelData,0,psSurface->pitch*psSurface->h);

  if(bBackFrameOnly)
  {
#if USE_DOUBLE_FRAME_BUFFER  
    psSurface = get_back_buffer();
    pcPixelData = (PBYTE)psSurface->pixels;
  	memset(pcPixelData,0,psSurface->pitch*psSurface->h);
#endif    
  }
}

/**
 *  @brief      Return Next State
 *  @param[in]  None
 *  @param[out] None
 *  @return     eUI_STATE
 *  @note       None
 *  @attention  None
 */
eUI_STATE appUINextStateGet(void)
{
  return m_eUIMgrStateNext;
}

/**
 *  @brief      Return Current State
 *  @param[in]  None
 *  @param[out] None
 *  @return     eUI_STATE
 *  @note       None
 *  @attention  None
 */
eUI_STATE appUICurrentStateGet(void)
{
  return m_eUIMgrState;
}

/**
 *  @brief      Return Previous State
 *  @param[in]  None
 *  @param[out] None
 *  @return     eUI_STATE
 *  @note       None
 *  @attention  None
 */
eUI_STATE appUIPrevStateGet(void)
{
  return m_eUIMgrStatePrev;
}

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
                            BYTE           cItem)
{
  BOOL                bSuccess = FALSE;
  PMENU_PRIVATE_DATA  pPrivate 
      = (PMENU_PRIVATE_DATA)&psMenuInfo->pawItemState[psMenuInfo->cItemCount];

  // validate arguments in debug mode only
  ASSERT(NULL != psMenuInfo);
  ASSERT(cItem < psMenuInfo->cItemCount || MS_NO_ACTIVE_ITEM == cItem);

  // only change active item if item is visible and can be selected,
  // or if menu is being deactivated (active item set to MS_NO_ACTIVE_ITEM)
  if ((MS_NO_ACTIVE_ITEM == cItem) ||
      ((psMenuInfo->pawItemState[cItem] &
        (MS_ITEM_VISIBLE | MS_ITEM_SELECTABLE)) ==
       (MS_ITEM_VISIBLE | MS_ITEM_SELECTABLE)))
  {
    pPrivate->cActiveItem = cItem;
    bSuccess = TRUE;
  }

  return bSuccess;
}

/**
 *  @brief      This API returns the active item in the specified menu.
 *  @param[in]  psMenuInfo : Pointer to a menu configuration data structure
 *  @param[out] None
 *  @return     Active item of specified menu
 *  @note       None
 *  @attention  None
 */
BYTE appMSMenuActiveItemGet(PCMS_MENU_INFO psMenuInfo)
{
  PMENU_PRIVATE_DATA  pPrivate 
      = (PMENU_PRIVATE_DATA)&psMenuInfo->pawItemState[psMenuInfo->cItemCount];

  // validate arguments in debug mode only
  ASSERT(NULL != psMenuInfo);

  return pPrivate->cActiveItem;
}

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
                                    BYTE cItem)
{
  BYTE cFlags = 0xFF;

  switch(eLayer)
  {
    case ldSPLASH:
      if(cItem < menuSS_INVALID)
    		cFlags = m_awSplashItemsFlags[cItem];
    break; 

    case ldLAUNCHBAR:
      if(cItem < menuLB_INVALID)
        cFlags = m_awLBItemsFlags[cItem];      
    break;

    case ldMENU_MAIN:
      if(cItem < menuMM_INVALID)
    		cFlags = m_awMenuMainItemsFlags[cItem];
    break;     

    default: break;
  }

  return cFlags;
}

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
                            BYTE           cFlags)
{

  BYTE                cItem = 0;

  if(qwItemMask == MS_ALL_ITEMS)
  {
    switch(eLayer)
    {
      case ldSPLASH:
        for(cItem = 0; cItem < menuSS_INVALID; cItem++)
        {
          m_awSplashItemsFlags[cItem] |= cFlags;
        }
      break;

      case ldLAUNCHBAR:
        for(cItem = 0; cItem < menuLB_INVALID; cItem++)
        {
          m_awLBItemsFlags[cItem] |= cFlags;
        }
      break;       

      case ldMENU_MAIN:
        for(cItem = 0; cItem < menuMM_INVALID; cItem++)
        {
          m_awMenuMainItemsFlags[cItem] |= cFlags;
        }
      break;    
        
      default: break;
    }
  }
  else
  {
    switch(eLayer)
    {
      case ldSPLASH: 
        while(0 != qwItemMask && cItem < menuSS_INVALID)
        {
          if(qwItemMask & 0x00000001ULL)
          {
            m_awSplashItemsFlags[cItem] |= cFlags;
          }

          qwItemMask = qwItemMask >>1ULL;
          cItem++;
        }
      break;

      case ldLAUNCHBAR: 
        while(0 != qwItemMask && cItem < menuLB_INVALID)
        {
          if(qwItemMask & 0x00000001ULL)
          {
            m_awLBItemsFlags[cItem] |= cFlags;
          }

          qwItemMask = qwItemMask >>1ULL;
          cItem++;
        }
      break;      

      case ldMENU_MAIN: 
        while(0 != qwItemMask && cItem < menuMM_INVALID)
        {
          if(qwItemMask & 0x00000001ULL)
          {
            m_awMenuMainItemsFlags[cItem] |= cFlags;
          }

          qwItemMask = qwItemMask >>1ULL;
          cItem++;
        }
      break;   

      default: break;
    }   
  }
}

/**
 *  @brief      This API clears the specified item flags for the specified mask of menu items
 *  @param[in]  eLayer: Layer Id
                wItemMask: Item number bitmask to set flags for. Use MS_ALL_ITEMS to set
                                     flags for all items in menu
                cFlags: Flags to clear (see the Menu Item flag symbols TS_ITEM_XXX for valid values)
 *  @param[out] None
 *  @return     Current status flags for specified menu item
 *  @note       None
 *  @attention  None
 */
void appMSMenuItemsFlagsClear(eLAYER_DATA eLayer,
                              QWORD          qwItemMask,
                              BYTE           cFlags)
{
	BYTE cItem = 0;

	if(qwItemMask == MS_ALL_ITEMS)
	{
		switch(eLayer)
		{
			case ldSPLASH:
  			for(cItem = 0; cItem < menuSS_INVALID; cItem++)
  			{
  				m_awSplashItemsFlags[cItem] &= (0xff00 | (WORD)~cFlags);
  			}
			break;

			case ldLAUNCHBAR:
  			for(cItem = 0; cItem < menuLB_INVALID; cItem++)
  			{
  				m_awLBItemsFlags[cItem] &= (0xff00 | (WORD)~cFlags);
  			}
			break;      

			case ldMENU_MAIN:
  			for(cItem = 0; cItem < menuMM_INVALID; cItem++)
  			{
  				m_awMenuMainItemsFlags[cItem] &= (0xff00 | (WORD)~cFlags);
  			}
			break;      
      
			default : break;
		}
	}
	else
	{
		switch(eLayer)
		{
			case ldSPLASH:
				while(0 !=  qwItemMask && cItem < menuSS_INVALID)
				{
					if(qwItemMask & 0x00000001ULL)
					{
						m_awSplashItemsFlags[cItem] &= (0xff00 | (WORD)~cFlags);
					}

					qwItemMask = qwItemMask >> 1ULL;
					cItem++;
				}
			break;

      case ldLAUNCHBAR:
				while(0 !=  qwItemMask && cItem < menuLB_INVALID)
				{
					if(qwItemMask & 0x00000001ULL)
					{
						m_awLBItemsFlags[cItem] &= (0xff00 | (WORD)~cFlags);
					}

					qwItemMask = qwItemMask >> 11ULL;
					cItem++;
				}        
      break;      

      case ldMENU_MAIN:
				while(0 !=  qwItemMask && cItem < menuMM_INVALID)
				{
					if(qwItemMask & 0x00000001ULL)
					{
						m_awMenuMainItemsFlags[cItem] &= (0xff00 | (WORD)~cFlags);
					}

					qwItemMask = qwItemMask >> 11ULL;
					cItem++;
				}        
      break;

			default : break;
		}
	}
}

                              /**
 *  @brief      This API return Menu info the specified eUI_STATE
 *  @param[in]  eUIState: UI State to active item set
 *  @param[out] None
 *  @return     pointer to Menu info
 *  @note       None
 *  @attention  None
 */
PCMS_MENU_INFO appMSMenuStateInfoGet(eUI_STATE eUIState)
{
  PCMS_MENU_INFO pcsMenuInfo = NULL;

  switch(eUIState)
  {
    case uiSPLASH:    pcsMenuInfo = &m_sMenuSplash;     break;
    case uiLAUNCHBAR: pcsMenuInfo = &m_sMenuLaunchbar;  break;    
    case uiMENU_MAIN: pcsMenuInfo = &m_sMenuMain;       break;
        
    default: ASSERT_ALWAYS(); break;
  }

  return pcsMenuInfo;
}

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
BOOL appMSMenuStateActiveItemSet(eUI_STATE eUIState, BYTE cItem)
{
  BOOL            bReturn     = FALSE;
  PCMS_MENU_INFO  pcsMenuInfo = appMSMenuStateInfoGet(eUIState);


  if(NULL != pcsMenuInfo)
  {
    if(MS_NO_ACTIVE_ITEM == cItem)
      cItem = m_cLastActiveMenu;
      
    appMSMenuStateItemsFlagsSet(eUIState, 1ULL << cItem, MS_ITEM_UPDATE); 
    m_bUpdateUI = TRUE;

    switch(eUIState)
    {
      case uiMENU_MAIN:
        MenuMainActiveItemRelativeUpdateSet(appMSMenuActiveItemGet(pcsMenuInfo));
        MenuMainActiveItemRelativeUpdateSet(cItem);            
      break;    

      default: /* NTD */ break;
    }  

    /* Counter Set to Active Item Clear */
    utilExecCounterSet(coUI_ACTIVE_CLEAR, uS_TO_TICKS(PERIOD_UI_ACTIVE_CLEAR));  
  
    bReturn = appMSMenuActiveItemSet(pcsMenuInfo, cItem);  
  }

  return bReturn;
}

/**
 *  @brief      This API returns the active item specified eUI_STATE
 *  @param[in]  eUIState: UI State to active item set
 *  @param[out] None
 *  @return     Active item of MAIN MENU
 *  @note       None
 *  @attention  None
 */
BYTE appMSMenuStateActiveItemGet(eUI_STATE eUIState)
{
  PCMS_MENU_INFO pcsMenuInfo = appMSMenuStateInfoGet(eUIState);
  BYTE           cActiveItem = MS_NO_ACTIVE_ITEM;

  if(NULL != pcsMenuInfo)
   cActiveItem = appMSMenuActiveItemGet(pcsMenuInfo);

  return cActiveItem;
}

/**
 *  @brief      This API Clear the active item in the specified eUI_STATE
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void appMSMenuStateActiveItemClear(eUI_STATE eUIState)
{
  PCMS_MENU_INFO pcsMenuInfo = appMSMenuStateInfoGet(eUIState);

  if(NULL != pcsMenuInfo)
  {
    BYTE cActiveItem = appMSMenuStateActiveItemGet(eUIState);
    
    m_cLastActiveMenu = cActiveItem;

    if(cActiveItem != MS_NO_ACTIVE_ITEM)
    {
      appMSMenuStateItemsFlagsSet(eUIState, 1ULL << cActiveItem, MS_ITEM_UPDATE);    

      switch(eUIState)
      {
        case uiMENU_MAIN:
          MenuMainActiveItemRelativeUpdateSet(cActiveItem);
        break;  
        
        default: /* NTD */ break;
      }
      m_bUpdateUI = TRUE;
    }

    appMSMenuActiveItemSet(pcsMenuInfo, MS_NO_ACTIVE_ITEM);    
  }
}


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
void appMSMenuStateItemsFlagsSet(eUI_STATE eUIState, DWORD dwItemMask, BYTE cFlags)
{
  PCMS_MENU_INFO pcsMenuInfo = appMSMenuStateInfoGet(eUIState);

  if(NULL != pcsMenuInfo)
  {
    appMSMenuItemsFlagsSet(pcsMenuInfo->eLayer, dwItemMask, cFlags);
    m_bUpdateUI = TRUE;
  }
}

/**
 *  @brief      This API sets menu item flags from the main program status to userinterface change
 *  @param[in]  pcsSysStatusNew  : pointer to system status to userinterface to set
 *  @param[out] None
 *  @return     TRUE if processed
 *  @note       must be call before appSysStateStatusSet()
 *  @attention  None
 */
BOOL appUISysStatusUpdate(PCSTATUS_SYS pcsSysStatusNew)
{
  eLAYER_DATA   eLayer            = uiINVALID;
  QWORD         qwItemMask        = 0ULL;
  ///PCDRAWABLE    pasDrawList;  
  ///PCSTATUS_SYS  pcsSysStatusCur   = (PCSTATUS_SYS)appSysStateStatusPtrGet();
  BOOL          bProcessed        = FALSE;

  ASSERT(pcsSysStatusNew != NULL);

  if(m_eUIMgrState == m_eUIMgrStateNext) // Wait stable
  {
    switch(m_eUIMgrState)
    {  
      case uiLAUNCHBAR:
      break;
      
      case uiMENU_MAIN:
      break;

      default: break;
    }

    /* Menu item Flags Set */
    if(qwItemMask && eLayer != uiINVALID)
    {
      appMSMenuItemsFlagsSet(eLayer
                              , qwItemMask
                              , MS_ITEM_UPDATE);
      m_bUpdateUI = TRUE;
    }
  }

  return bProcessed;
}

/**
 *  @brief      This API return the UI Ready Status                
 *  @param[in]  None
 *  @param[out] None
 *  @return     UI is available to state change, it return TRUE. 
 *  @note       None
 *  @attention  None
 */
BOOL appUIReadyToStateChangeGet(void)
{
  return (IMAGE_OBJECT_COUNT == m_wImageObjectLoaded);
}


