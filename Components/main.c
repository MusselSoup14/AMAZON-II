/**
  ******************************************************************************
  * @file    main.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    03-01-2019
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
#include "common.h"

#include "dvCPUAPI.h"
#include "dvGPIOAPI.h"
#include "utilExecutiveAPI.h"
#include "halUserInputCtrlAPI.h"
#include "halPowerCtrlAPI.h"
#include "halLEDCtrlAPI.h"
#include "halLCDCtrlAPI.h"
#include "halCANCtrlAPI.h"
#include "halAudioCtrlAPI.h"
#include "halRTCCtrlAPI.h"
#include "utilMiscAPI.h"
#include "appSystemStateAPI.h"
#include "appPowerMgrAPI.h"
#include "appAudioMgrAPI.h"
#include "appUserInterfaceAPI.h"
#include "utilDataMgrAPI.h"
#include "utilHostIfAPI.h"


//#include "lv_conf.h"
//include "lv_log.h"
// #include "lvgl.h"
// #include "lvgl/examples/lv_examples.h"
// #include "lvgl/demos/lv_demos.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS **************
// ***************************************************************************

//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************


//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************
/**
 *	@brief		Handler of exit() function.
 *	@param[in]	None
 *	@return		None
 */
static void mainExitHandler(void)
{  

}

BOOL dma_ahb_memcpy_custom( void* dest, void* src, U32 bytelen )
{
    U32 conval = 0;
    // int dmach = get_free_ahb_dma();
    // if( dmach < 0 )
    // {
    //     debugstring( "All DMA Channel is busy\r\n" );
    //     return FALSE;
    // }
    U32 bitwidth = 8;
    U32 transfer_size = bytelen;
    //dcache_invalidate_way();
    if( bytelen > 4 )
    {
        if( ( ( U32 )dest & 0x3 ) == 0 ) //32bit
        {
            conval |= 1 << 25;
        }
        else if( ( ( U32 )dest & 0x1 ) == 0 ) //16bit
        {
            conval |= 1 << 24;
        }

        if( ( ( U32 )src & 0x3 ) == 0 ) //32bit
        {
            conval |= 1 << 21;
            transfer_size = bytelen >> 2;
            bitwidth = 32;
        }
        else if( ( ( U32 )src & 0x1 ) == 0 ) //16bit
        {
            conval |= 1 << 20;
            transfer_size = bytelen >> 1;
            bitwidth = 16;
        }
    }
    conval |= DMA_CON_SRC_BST_256;
    conval |= DMA_CON_DST_BST_256;

    //increment mode
    conval |= ( 1 << 29 );
    conval |= ( 1 << 28 );

    *R_DMASRC( dmach ) = ( U32 )src;
    *R_DMADST( dmach ) = ( U32 )dest;

    U32 reloadcnt = transfer_size >> 11; // devide by 0x800
    if( reloadcnt )
    {
        //use auto reload
        conval |= 0x800;
        *R_DMASGA( dmach ) = 0;
        *R_DMADSA( dmach ) = 0;
        *R_DMAARCNT( dmach ) = reloadcnt - 1;
        *R_DMACON( dmach ) = conval;
        *R_DMACFG( dmach ) = 1;
        while( ( *R_DMACFG( dmach ) ) & 1 );
        set_free_ahb_dma( dmach );

        U32 totaltx = ( bitwidth / 8 ) * 0x800 * reloadcnt;
        U32 remain = bytelen - totaltx;
        if( remain )
        {
            memcpy( dest + totaltx, src + totaltx, remain );
        }
    }
    else
    {
        conval |= transfer_size;
        *R_DMACON( dmach ) = conval;
        *R_DMACFG( dmach * 8 ) = 1;
        while( ( *R_DMACFG( dmach ) ) & 1 );
        set_free_ahb_dma( dmach );

        U32 totaltx = ( bitwidth / 8 ) * transfer_size;
        U32 remain = bytelen - totaltx;
        if( remain )
        {
            memcpy( dest + totaltx, src + totaltx, remain );
        }
    }
    return TRUE;
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /* CPU Interrupt Enable */  
  CPU_INT_DISABLE;
  atexit(mainExitHandler);
  
  //4 ----------------------- Components Initialization ---------------------------      
  utilExecInitialize(); 
  utilExecSignalClear(sgSYSTEM, 0xFFFF);
  
  dvCPUInitialize();    
  REPORT_VERSION("CWM-T12");
  utilMiscInitialize();
  
  halPowerCtrlInitialize();
  utilDataMgrInitialize();
  appPowerMgrInitialize();
  
  // lv_init();

#if USE_DS1374_RTC
  dvDS1374Initialize();
#endif  

  halUICtrlInitialize();  
  halLEDCtrlInitialize();
  halLCDCtrlInitialize();
  halRTCCtrlInitialize();

#if USE_CAN_BUS
  dvDeviceBusCANInitialize();
  halCANCtrlInitialize();
#endif

  halAudioCtrlInitialize();

  appSysStateInitialize();
  appAudioMgrInitialize();  
  appUIInitialize(); 

  /* CPU Interrupt Enable */  
  CPU_INT_ENABLE;

  utilHostIfInitialize(); 
  
#if USE_CONFIGIF_MODE
  msgInfo("Configuration I/F Mode Activated");
  utilExecSignalSet(sgSETUP_IF, SIGNAL_CONF_SETUP_ENABLE);  
#endif

#if USE_WATCHDOG
  #if USE_DEBUG_RUNTIME
  msgInfo("Watchdog Enabled (%s)"
          , (dvCPUWatchDogResetOccurredGet()) ? "WATCHDOG RESET OCCURRED !" : "-");
  #endif
  dvCPUWatchDogEnable(TRUE);    
#endif  

  /* System Status to Ready */
  utilExecSignalSet(sgSYSTEM, SIGNAL_SYSTEM_INITIALIZED);
  dvGPIOFieldSet(gfGPO_SYS_STDBY, LOW);

#if 0
  char *arr = malloc(sizeof(char) * 10);
  msgDbg("동적할당변수 Memory Address : \t %p \r\n", arr);
  lv_malloc(10);
#endif

  //lv_example_anim_2();
  // lv_example_anim_3();
  // lv_demo_widgets();   
  // lv_demo_music();
 
  // ------------------------------ MAIN LOOP  ----------------------------------    
  while(TRUE)
  {  
    // if(!utilExecCounterGet(coLED_APP))
    // {      
    //   utilExecCounterSet(coLED_APP, uS_TO_TICKS(1000000));
    //   halLEDCtrlOnOffSet(ledRAW_GREEN, !halLEDCtrlOnOffGet(ledRAW_GREEN));
    // }
#if 1
    if(!utilExecCounterGet(coHAL_UI_LVGL))
    {
      utilExecCounterSet(coHAL_UI_LVGL, uS_TO_TICKS(1000));
      // lv_tick_inc(2);
    }
#endif
    /* Key Event Handler */
    //if(!utilExecCounterGet(coHAL_UI_KEY)) 
      //halUICtrlKeyEventHandler();
   
    // appSysStateProcess();    
    // utilHostIfProcess();
    // appPowerMgrProcess();

    // if(pwrON == appPowerMgrPowerStateGet())
    // {   
    //   /* Touch Event Handler. Perform after GUI is READY */
    //   // if(!utilExecCounterGet(coHAL_UI_TOUCH)
    //   //     && utilExecSignalGet(sgGUI_IF, SIGNAL_GUI_READY))
    //   //   halUICtrlTouchEventHandler();
      
    //   //appUIProcess();
    //   appAudioMgrProcess();
    //   utilMiscProcess();            
    // }   

    // lv_task_handler();

    // update NVRAM every 5s
    // if(!utilExecCounterGet(coDATA_MGR_UPDATE))
    // {      
    //   utilExecCounterSet(coDATA_MGR_UPDATE, 
    //                             uS_TO_TICKS(NVRAM_UPDATE_COUNTER));
    //   utilDataMgrUpdate(UPDATE_ALL_NODEID);
    // }    
  }
  
  return 0;
}
  
