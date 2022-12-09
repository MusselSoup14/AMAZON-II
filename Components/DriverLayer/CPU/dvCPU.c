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
/* Includes ------------------------------------------------------------------*/
#include "dvCPUAPI.h"
#include "dvGPIO.h"
#include "dvGPIOAPI.h"
#include "dvCPUPeriphAPI.h"
#include "dvDeviceBusAPI.h"
#include "dvDeviceBus3WireAPI.h"
#include "dvSerialAPI.h"
#include "utilHostIfAPI.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
// ---------------------------------------------------------------------------
// CPU Watchdog Kick(Counter reset) Value
// ---------------------------------------------------------------------------
#define CPU_WATCHDOG_CNT   41625000 // About 1000ms @ APB 41625000Hz, WDTCNT = ( Set_time(s) * APB Clk(Hz) )

// ---------------------------------------------------------------------------
// Structure for AMAZON-II PWM Configurations
// ---------------------------------------------------------------------------
typedef struct tagCPUPWMCONFIG
{
  DWORD     dwPWMFrequency; // unit is Hz
  BYTE      cPWMDutyCycle; // unit is %
  BOOL      bPWMEnabled;
}CPU_PWM_CONFIG, *PCPU_PWM_CONFIG;
typedef CROMDATA CPU_PWM_CONFIG *PCCPUT_PWM_CONFIG;

typedef struct tagCPUPWMDEFAULT
{
  BYTE      cPrescaleFactor; //  F_TMCTRL_PFSEL_2 ~ F_TMCTRL_PFSEL_32768
  DWORD     dwFreqHz; // Unit is Hz
  BYTE      cDutyRatio; // Unit %
  BYTE      cChannel;
}CPU_PWM_DEFAULT, *PCPU_PWM_DEFAULT;

//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************
// ---------------------------------------------------------------------------
// PWM Default Configuration
// ---------------------------------------------------------------------------
static CROMDATA CPU_PWM_DEFAULT m_sPWMInitDefault[cpuPWM_INVALID] =
{
/* ------------------------ BEGIN CUSTOM AREA ------------------------------*/
#if USE_AMAZON_STK
  {F_TMCTRL_PFSEL_8, 200, 0, 1},    // cpuPWM_LCD_DIMMING
#elif USE_LCD_1024x600
  {F_TMCTRL_PFSEL_2, 40000, 0, 1}, // cpuPWM_LCD_DIMMING
#elif USE_LCD_1280x800
  {F_TMCTRL_PFSEL_2, 30000, 0, 1}, // cpuPWM_LCD_DIMMING
#endif
/* -------------------------- END CUSTOM AREA ------------------------------*/  
};


//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************
// ---------------------------------------------------------------------------
// Watchdog Reset Flag
// ---------------------------------------------------------------------------
static BOOL m_bWatchdogResetOccurred = FALSE; // TRUE Watchdog reset occurred

// ---------------------------------------------------------------------------
// Structure for ADStar PWM Configurations
// ---------------------------------------------------------------------------
static CPU_PWM_CONFIG m_sPWMConfig[cpuPWM_INVALID];

// ***************************************************************************
// **************** START OF ISR PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************

/**
 *  @brief      Configures Vector Table base location.
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void CPUInterruptInitialize(void)
{
  init_interrupt();
}


/**
 *  @brief      Configures Timer
 *  @param[in]  eCpuTimer : Identifier of Hardware Timer
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void CPUTimerInitialize(eCPU_TIMER eCpuTimer)
{  
  BYTE cChannel;
  
  ASSERT(eCpuTimer < TIMER_NUM_INSTANCES);

  switch(eCpuTimer)
  {
    case cpuTIMER_SYSTEM:
      cChannel = 3;
       // in APBClock is 41625000Hz and prescale is 32768, 1270.294189Hz, 787.2192192us
       // Max: 51.59041153
      *R_TPCTRL(cChannel)  = F_TPCTRL_CNTCLR;       // pre-scale Counter and Timer Counter Reset
      *R_TPCTRL(cChannel)  = F_TPCTRL_CLKSEL_CAP0;  // "timer.h" has wrong definition between CAP0 and SCLK.
      *R_TMCNT(cChannel)   = 0xFFFFFFFF;
      *R_TMCTRL(cChannel)  = F_TMCTRL_PFSEL_32768;
    break;    

    default: ASSERT_ALWAYS(); break;
  }

}

/**
 *  @brief      Configures the PWM
 *  @param[in]  eCpuPWM : Identifier of Hardware PWM
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void CPUPWMInitialize(eCPU_PWM eCpuPWM)
{
  BYTE    cChannel;
  DWORD   dwPulsePeriod;
  WORD    wPrescale;  
  
  ASSERT(eCpuPWM < PWM_NUM_INSTANCES);

  m_sPWMConfig[eCpuPWM].dwPWMFrequency  = m_sPWMInitDefault[eCpuPWM].dwFreqHz;
  m_sPWMConfig[eCpuPWM].cPWMDutyCycle   = m_sPWMInitDefault[eCpuPWM].cDutyRatio;
  m_sPWMConfig[eCpuPWM].bPWMEnabled     = FALSE;  

  switch(m_sPWMInitDefault[eCpuPWM].cPrescaleFactor)
  {
    case F_TMCTRL_PFSEL_2:      wPrescale = 2; break;
    case F_TMCTRL_PFSEL_8:      wPrescale = 8; break;
    case F_TMCTRL_PFSEL_32:     wPrescale = 32; break;
    case F_TMCTRL_PFSEL_128:    wPrescale = 128; break;
    case F_TMCTRL_PFSEL_512:    wPrescale = 512; break;
    case F_TMCTRL_PFSEL_2048:   wPrescale = 2048; break;
    case F_TMCTRL_PFSEL_8192:   wPrescale = 8192; break;
    case F_TMCTRL_PFSEL_32768:  wPrescale = 32768; break;
    
    default: ASSERT_ALWAYS(); break;
  }  

  switch(eCpuPWM)
  {
    case cpuPWM_LCD_DIMMING: 
      cChannel = m_sPWMInitDefault[eCpuPWM].cChannel;
      /* Init the LCD BLU Dimming Timer */
      *R_TPCTRL(cChannel) = F_TPCTRL_CNTCLR;        // pre-scale Counter and Timer Counter Reset
      *R_TPCTRL(cChannel) = F_TPCTRL_CLKSEL_CAP0;  // "timer.h" has wrong definition between CAP0 and SCLK.
      
      // TMCNT = ( Set_time(s) * APB Clk(Hz) / (1/Pre-scaler Factor) )  // APB clock about 50MHz.
      dwPulsePeriod = ((DWORD)get_apb_clock()/m_sPWMInitDefault[eCpuPWM].dwFreqHz)/wPrescale + 1;
      
      *R_TMCNT(cChannel)  = dwPulsePeriod;
      *R_TMDUT(cChannel)  = ((dwPulsePeriod)*m_sPWMInitDefault[eCpuPWM].cDutyRatio)/100;
      *R_TMPUL(cChannel)  = 0xFFFFFFFF; // set to 32bit max value
      *R_TMCTRL(cChannel) = F_TMCTRL_TMOD_PWM 
                            | F_TMCTRL_PWMO
#if (USE_AMAZON_STK ||  USE_LCD_1024x600 || USE_LCD_1280x800)
                            | F_TMCTRL_PWML_HIGH 
#endif                            
                            | m_sPWMInitDefault[eCpuPWM].cPrescaleFactor;
    break;

    default: ASSERT_ALWAYS(); break;
  }
  
}

/**
 *  @brief      Configures the External Interrupt
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void CPUExtIntInitialize(void)
{

}

static void CPUWatchDogInitialize(void)
{
  /* Read Watchdog timer status bit */
  m_bWatchdogResetOccurred = (*R_WDTCTRL & (1 << 4)) ?  TRUE : FALSE;
}

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
void dvCPUInitialize(void)
{
  BYTE cI;

  CPUInterruptInitialize();
  dvCPUPeriphInitialize();
  dvGPIOInitialize();  
  
  for(cI = 0; (eCPU_TIMER)cI < TIMER_NUM_INSTANCES; cI++)
  {
    CPUTimerInitialize((eCPU_TIMER)cI);
  }  

  for(cI = 0; cI < PWM_NUM_INSTANCES; cI++)
  {
    CPUPWMInitialize((eCPU_PWM)cI);
  }
  
  dvSerialInitialize(); 

  dvDeviceBusInitialize(); 
  dvDeviceBus3WireInitialize();  

  /* Interrupt */
  CPUExtIntInitialize();  

  CPUWatchDogInitialize();

  /* Timer Enable */  
  dvCPUTimerEnable(cpuTIMER_SYSTEM, TRUE); // internal counter  
}

/**
 *  @brief      This API must be called once during system Release
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvCPURelease(void)
{
  dvGPIORelease();
}

/**
 *  @brief      This API Enable/Disable the specified timer.
 *  @param[in]  eCpuTimer : Timer get the time remaining for
 *  @param[in]  bEnable : TRUE for Enable, FALSE for Disable
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvCPUTimerEnable(eCPU_TIMER eCpuTimer, BOOL bEnable)
{  
  ASSERT(TIMER_NUM_INSTANCES > eCpuTimer);

  switch(eCpuTimer)
  {
#if 0  
    case cpuTIMER0:
      
      if(bEnable)
        *R_TM0CTRL |= F_TMCTRL_TMEN;
      else
        *R_TM0CTRL &= ~F_TMCTRL_TMEN;
    break;

    case cpuTIMER1:
      if(bEnable)
        *R_TM1CTRL |= F_TMCTRL_TMEN;
      else
        *R_TM1CTRL &= ~F_TMCTRL_TMEN;

    break;

    case cpuTIMER2:
      if(bEnable)
        *R_TM2CTRL |= F_TMCTRL_TMEN;
      else
        *R_TM2CTRL &= ~F_TMCTRL_TMEN;      
    break;
#endif      

    case cpuTIMER_SYSTEM:
      if(bEnable)
        *R_TMCTRL(3) |= F_TMCTRL_TMEN;
      else
        *R_TMCTRL(3) &= ~F_TMCTRL_TMEN;
    break;

    default: break;
  }
}

/**
 *  @brief      This API Enable/Disable interrupt of the specified timer.
 *  @param[in]  eCpuTimer : Timer get the time remaining for
 *  @param[in]  bEnable : TRUE for Enable, FALSE for Disable
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvCPUTimerINTEnableSet(eCPU_TIMER eCpuTimer, BOOL bEnable)
{
  ASSERT(TIMER_NUM_INSTANCES > eCpuTimer);

  switch(eCpuTimer)
  {
#if 0
    case cpuTIMER0:
      enable_interrupt(INTNUM_TIMER0, bEnable);
      break;

    case cpuTIMER1:
      enable_interrupt(INTNUM_TIMER1, bEnable);
      break;

    case cpuTIMER2:
      enable_interrupt(INTNUM_TIMER2, bEnable);
      break;
#endif

    case cpuTIMER_SYSTEM:
      enable_interrupt(INTNUM_TIMER3, bEnable);
      break;

    default: break;
  }
}

/**
 *  @brief      This API Set the Timer Period of the specified timer.
 *  @param[in]  eCpuTimer : Timer Identifier
 *  @param[in]  wCounter : Period value to set
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvCPUTimerPeriodSet(eCPU_TIMER eCpuTimer, WORD wPeriod)
{
  ASSERT(TIMER_NUM_INSTANCES > eCpuTimer);
  ASSERT(wPeriod != 0);

  switch(eCpuTimer)
  {
#if 0
    case cpuTIMER0:
    case cpuTIMER1:
    case cpuTIMER2:      
#endif
    case cpuTIMER_SYSTEM:      
    break;

    default: break;
  }
}


/**
 *  @brief      This API gets the number of timer ticks remaining on the specified timer.
 *  @param[in]  eCpuTimer : Timer get the time remaining for
 *  @param[in]  dwCounter : specifies the Counter register new value
 *  @param[out] None
 *  @return     Number of ticks cycles remaining for the timer
 *  @note       None
 *  @attention  None
 */
void dvCPUTimerSet(eCPU_TIMER eCpuTimer, DWORD dwCounter)
{
  // Verify that a valid timer is being gotten
  ASSERT(TIMER_NUM_INSTANCES > eCpuTimer);

  switch(eCpuTimer)
  {
#if 0  
    case  cpuTIMER0:
      *R_TM0CNT = wCounter;
    break;

    case  cpuTIMER1:
      *R_TM1CNT = wCounter;
    break;

    case  cpuTIMER2:
      *R_TM2CNT = wCounter;
    break;
#endif    

    case  cpuTIMER_SYSTEM:
      *R_TMCNT(3) = dwCounter;
    break;
      
    default: break;
  }
}


/**
 *  @brief      This API gets the number of timer ticks remaining on the specified timer.
 *  @param[in]  eCpuTimer : Timer get the time remaining for
 *  @param[out] None
 *  @return     Number of ticks cycles remaining for the timer
 *  @note       None
 *  @attention  None
 */
DWORD dvCPUTimerGet(eCPU_TIMER eCpuTimer)
{
  DWORD dwReturn = 0; // Return value for timer

  // Verify that a valid timer is being gotten
  ASSERT(TIMER_NUM_INSTANCES > eCpuTimer);

  switch(eCpuTimer)
  {
#if 0  
    case  cpuTIMER0:
      wReturn = *R_TM0CNT;
    break;

    case  cpuTIMER1:
      wReturn = *R_TM1CNT;
    break;

    case  cpuTIMER2:
      wReturn = *R_TM2CNT;
    break;
#endif      

    case  cpuTIMER_SYSTEM:
      dwReturn = *R_TMCNT(3);
    break;
    
    default: break;
  }

  // Return no time left if the timer is disabled or invalid
  return dwReturn;
}

/**
 *  @brief      This API Enable/Disable the specified PWM.
 *  @param[in]  bEnable : TRUE for Enable, FALSE for Disable
                eCpuPWM : Identifier of CPU hardware PWM
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvCPUPWMEnableSet(eCPU_PWM eCpuPWM, BOOL bEnable)
{
  BYTE cChannel;
  ASSERT(eCpuPWM < PWM_NUM_INSTANCES);

  if(m_sPWMConfig[eCpuPWM].bPWMEnabled != bEnable)
  {
    switch(eCpuPWM)
    {
      case cpuPWM_LCD_DIMMING:
        cChannel = m_sPWMInitDefault[eCpuPWM].cChannel;        
        if(bEnable)
          *R_TMCTRL(cChannel) |= F_TMCTRL_TMEN;
        else
          *R_TMCTRL(cChannel) &= ~F_TMCTRL_TMEN;

        m_sPWMConfig[eCpuPWM].bPWMEnabled = bEnable;
      break;

      default: break;
    }  
  }
}

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
BOOL dvCPUPWMDutySet(eCPU_PWM eCpuPWM, BYTE cDutyCycle, BOOL bReCalc)
{
  BOOL              bReturn = FALSE;  
  PCPU_PWM_CONFIG   psCpuPWMConfig;
  DWORD             dwPulsePeriod;
  BYTE              cChannel;
  WORD              wPrescale;

  ASSERT(eCpuPWM < PWM_NUM_INSTANCES);

  psCpuPWMConfig = &m_sPWMConfig[eCpuPWM];

  if((cDutyCycle != psCpuPWMConfig->cPWMDutyCycle || bReCalc)
    && eCpuPWM < PWM_NUM_INSTANCES)
  {    

    switch(m_sPWMInitDefault[eCpuPWM].cPrescaleFactor)
    {
      case F_TMCTRL_PFSEL_2:      wPrescale = 2; break;
      case F_TMCTRL_PFSEL_8:      wPrescale = 8; break;
      case F_TMCTRL_PFSEL_32:     wPrescale = 32; break;
      case F_TMCTRL_PFSEL_128:    wPrescale = 128; break;
      case F_TMCTRL_PFSEL_512:    wPrescale = 512; break;
      case F_TMCTRL_PFSEL_2048:   wPrescale = 2048; break;
      case F_TMCTRL_PFSEL_8192:   wPrescale = 8192; break;
      case F_TMCTRL_PFSEL_32768:  wPrescale = 32768; break;
      
      default: ASSERT_ALWAYS(); break;
    }  

    dwPulsePeriod = (get_apb_clock()/m_sPWMInitDefault[eCpuPWM].dwFreqHz)/wPrescale + 1;

    switch(eCpuPWM)
    {
      case cpuPWM_LCD_DIMMING:        
        cChannel            = m_sPWMInitDefault[eCpuPWM].cChannel;
        ///*R_TMDUT(cChannel)  = (*R_TMCNT(cChannel)*cDutyCycle)/100; // *R_TMCNT(x) has shadow Register in PWM Mode?
        *R_TMDUT(cChannel)  = (dwPulsePeriod * cDutyCycle)/100;
      
        psCpuPWMConfig->cPWMDutyCycle = cDutyCycle;
      break;

      default: break;
    }

    bReturn = TRUE;
  }

  return bReturn;
}


/**
 *  @brief      This API Enables IWDG (write access to IWDG_PR and IWDG_RLR registers disabled).
 *  @param[in]  bEnable: TRUE to Enable, FALSE to Disable
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvCPUWatchDogEnable(BOOL bEnable)
{
#if USE_WATCHDOG    
  *R_WDTCNT = CPU_WATCHDOG_CNT;	

  if(bEnable)
    *R_WDTCTRL = 0 << 1 // : Watchdog timer RESET mode
                | 1 << 0; // Watchdog timer enable 
  else
    *R_WDTCTRL = 0;  
#endif
}

/**
 *  @brief      This API Refresh IWDG
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvCPUWatchDogKick(void)
{
#if USE_WATCHDOG  
  *R_WDTCNT = CPU_WATCHDOG_CNT;	
#endif
}

/**
 *  @brief      This API return Watchdog Reset status
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Watchdog Reset Occurred
 *  @note       None
 *  @attention  None
 */
BOOL dvCPUWatchDogResetOccurredGet(void)
{
  return m_bWatchdogResetOccurred;
}



