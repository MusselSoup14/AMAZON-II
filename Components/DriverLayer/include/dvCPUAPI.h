/**
  ******************************************************************************
  * @file    dvCPU.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    02-August-2014
  * @brief   This file provides basic the CPU firmware functions.
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

#if !defined(_INCLUDED_DVCPUAPI_H_)
#define _INCLUDED_DVCPUAPI_H_

 /* Includes ------------------------------------------------------------------*/
#include "common.h"

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

// ---------------------------------------------------------------------------
// Enable all interrupts (sets the master enable bit)
// ---------------------------------------------------------------------------
#define CPU_INT_ENABLE INTERRUPT_ENABLE

// ---------------------------------------------------------------------------
// Disable all interrupts (clears the master enable bit)
// ---------------------------------------------------------------------------
#define CPU_INT_DISABLE INTERRUPT_DISABLE

// ---------------------------------------------------------------------------
// The following enumeration contains the list of possible hardware timers,
// used for interfacing with the CPU hardware timer APIs.
// ---------------------------------------------------------------------------
typedef enum
{
#if 0  
  cpuTIMER0,        /* TIM0, Reserved */
  cpuTIMER_LCDBLU,  /* TIM1, used to LCD Backlight Dimming PWM output */  
#endif  
  //cpuTIMER_LVGL,    /* TIM2, LVGL tick counter, 1ms */
  cpuTIMER_SYSTEM,  /* TIM3, Internal System Counter. 2sec slow per 20minutes */
  
  cpuTIMER_INVALID // Must be LAST value. Used for range checking (DO NOT REMOVE)
} eCPU_TIMER;
#define TIMER_NUM_INSTANCES cpuTIMER_INVALID

// ---------------------------------------------------------------------------
// The following enumeration contains the list of possible hardware PWMs,
// used for interfacing with the CPU hardware PWM APIs.
// ---------------------------------------------------------------------------
typedef enum
{
  cpuPWM_LCD_DIMMING, /* TIM1, used to LCD Backlight Dimming PWM output */

  cpuPWM_INVALID  // Must be LAST value. Used for range checking (DO NOT REMOVE)
} eCPU_PWM;
#define PWM_NUM_INSTANCES cpuPWM_INVALID

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************
/**
 *  @brief      This API must be called once during system startup, before calls are made
                to any other APIs in the Chip Driver component.  If this API is
                not called before using other Chip Driver functions, the other
                functions will not behave as specified. It will initialize all subsystems
                including CPU, GPIO, Device Bus, Serial Port, Infrared and so on
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvCPUInitialize(void);

/**
 *  @brief      This API must be called once during system Release
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvCPURelease(void);

/**
 *  @brief      This API Enable/Disable the specified timer.
 *  @param[in]  eCpuTimer : Timer get the time remaining for
 *  @param[in]  bEnable : TRUE for Enable, FALSE for Disable
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvCPUTimerEnable(eCPU_TIMER eCpuTimer, BOOL bEnable);

/**
 *  @brief      This API Enable/Disable interrupt of the specified timer.
 *  @param[in]  eCpuTimer : Timer get the time remaining for
 *  @param[in]  bEnable : TRUE for Enable, FALSE for Disable
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvCPUTimerINTEnableSet(eCPU_TIMER eCpuTimer, BOOL bEnable);

/**
 *  @brief      This API Set the Timer Period of the specified timer.
 *  @param[in]  eCpuTimer : Timer Identifier
 *  @param[in]  wCounter : Period value to set
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvCPUTimerPeriodSet(eCPU_TIMER eCpuTimer, WORD wPeriod);

/**
 *  @brief      This API gets the number of timer ticks remaining on the specified timer.
 *  @param[in]  eCpuTimer : Timer get the time remaining for
 *  @param[in]  dwCounter : specifies the Counter register new value
 *  @param[out] None
 *  @return     Number of ticks cycles remaining for the timer
 *  @note       None
 *  @attention  None
 */
void dvCPUTimerSet(eCPU_TIMER eCpuTimer, DWORD dwCounter);

/**
 *  @brief      This API gets the number of timer ticks remaining on the specified timer.
 *  @param[in]  eCpuTimer : Timer get the time remaining for
 *  @param[out] None
 *  @return     Number of ticks cycles remaining for the timer
 *  @note       None
 *  @attention  None
 */
DWORD dvCPUTimerGet(eCPU_TIMER eCpuTimer);

/**
 *  @brief      This API Enable/Disable the specified PWM.
 *  @param[in]  bEnable : TRUE for Enable, FALSE for Disable
                eCpuPWM : Identifier of CPU hardware PWM
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvCPUPWMEnableSet(eCPU_PWM eCpuPWM, BOOL bEnable);

/**
 *  @brief      This API Sets Duty Cycle of specified PWM output
 *  @param[in]  eCpuPWM: Identifier of CPU hardware PWM 
                cDutyCycle: expecting duty cycle. Unit is %
                bReCalc: TRUE to Recalculate the Pulse specified Duty Cycle. Ex) Freq changed
 *  @param[out] None 
 *  @return     TRUE if Duty Change successfully, FALSE for others(same value previously)
 *  @note       None
 *  @attention  None
 */
BOOL dvCPUPWMDutySet(eCPU_PWM eCpuPWM, BYTE cDutyCycle, BOOL bReCalc);

/**
 *  @brief      This API Enables IWDG (write access to IWDG_PR and IWDG_RLR registers disabled).
 *  @param[in]  bEnable: TRUE to Enable, FALSE to Disable
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvCPUWatchDogEnable(BOOL bEnable);

/**
 *  @brief      This API Refresh IWDG
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvCPUWatchDogKick(void);

/**
 *  @brief      This API return Watchdog Reset status
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Watchdog Reset Occurred
 *  @note       None
 *  @attention  None
 */
BOOL dvCPUWatchDogResetOccurredGet(void);

#endif //#if !defined(_INCLUDED_DVCPUAPI_H_)


