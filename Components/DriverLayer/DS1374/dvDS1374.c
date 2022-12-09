/**
  ******************************************************************************
  * @file    dvDS1374.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    09-05-2019
  * @brief   This file provides basic the Audio Amp Driver
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
#include "dvDS1374API.h"
#include "dvDS1374Regs.h"
#include "dvGPIOAPI.h"
#include "dvDeviceBusAPI.h"

#if ((USE_LCD_1024x600 ||  USE_LCD_1280x800) && USE_DS1374_RTC)
/** @addtogroup CWM-T10
  * @{
  */

/** @defgroup dvDS1374 
  * @brief DS1374 Driver
  * @{
  */

/** @defgroup dvDS1374_Private_Defines
  * @{
  */
#define _MOD_NAME_ "DVRTC"

// ---------------------------------------------------------------------------
// Device Bus Flags
// ---------------------------------------------------------------------------
#define DS1374_I2C_WRITE_FLAG (DEVICE_BUS_ADDR_TRANSACTION | DEVICE_BUS_FORCE_SW)
#define DS1374_I2C_READ_FLAG  (DEVICE_BUS_ADDR_TRANSACTION | DEVICE_BUS_SEND_REPEAT_AFTER_ADDR | DEVICE_BUS_FORCE_SW)  

// ---------------------------------------------------------------------------
// Device search count
// ---------------------------------------------------------------------------
#define DS1374_DEVICE_SEARCH_COUNT 5


/**
  * @}
  */

/** @defgroup dvDS1374_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @defgroup dvDS1374_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup dvDS1374_Private_Variables
  * @{
  */

// ---------------------------------------------------------------------------
// Device buses
// ---------------------------------------------------------------------------
static CROMDATA BYTE m_acDeviceBuses[DS1374_NUM_INSTANCES] =
{
/* ------------------------- BEGIN CUSTOM AREA  -------------------------------
  CONFIGURATION: Device bus for each driver instance */
  cpuTWI_SW_0,
/* ---------------------------- END CUSTOM AREA --------------------------- */
};

// ---------------------------------------------------------------------------
// Device Addresses
// ---------------------------------------------------------------------------
static CROMDATA BYTE m_acDeviceAddresses[DS1374_NUM_INSTANCES] =
{
/* ------------------------- BEGIN CUSTOM AREA  -------------------------------
  CONFIGURATION: Device bus for each driver instance */
  0x68,
/* ---------------------------- END CUSTOM AREA --------------------------- */
};

// ---------------------------------------------------------------------------
// Device Register Initial Values
// ---------------------------------------------------------------------------
static CROMDATA DS1374_REGMAP m_sDS1374InitRegs = 
{
  .dwTODCounter = 0,
  .cWD_nALMCounter0 = 0,
  .cWD_nALMCounter1 = 0,
  .cWD_nALMCounter2 = 0,
  {//.cCONTROL
    .AIE = 0,
    .RS = 0,
    .WDSTR = 0,
    .BBSQW = 0,
    .WD_nALM = 0, 
    .WACE = 0,
    .nEOSC = 0
  },
  {//. cSTATUS
    .AF = 0,
    .RSV = 0,
    .OSF = 0
  },
  #if  USE_DS1374_TRICKLECHARGER
  {//. cTRICHARGER
    .ROUT = 0x02,
    .DS = 0x02,
    .TCS = 0x0A,
  }  
  #else
  {//. cTRICHARGER, Just Marked as Disable trickle charger to touch value from first power reset value
    .ROUT = 0,
    .DS = 0x03,
    .TCS = 0x0F
  }  
  #endif
};


/* Device is present */
static BOOL m_abDS1374Presence[DS1374_NUM_INSTANCES]; // Device is present

/**
  * @}
  */

/** @defgroup dvTAS2505_Private_FunctionPrototypes
  * @{
  */

/**
  * @}
  */

/** @defgroup dvTAS2505_Private_Functions
  * @{
  */
/**
 *  @brief            Write to specified register address
 *  @param[in]    cInstance:   Device instance
                            cRegisterAddress: Register address to write
                            cData:   Data to write
 *  @param[out]  None
 *  @return         rcSUCCESS if the operation was successful.
 *  @return         rcERROR if communication to the hardware failed,  
 *  @note           None
 *  @attention     None
 */
static eRESULT DS1374RegWrite(BYTE   cInstance, BYTE cRegisterAddress, BYTE cData)
{
  ASSERT(cInstance < DS1374_NUM_INSTANCES);
 
  return(dvDeviceBusWrite(m_acDeviceBuses[cInstance],
                          m_acDeviceAddresses[cInstance],
                         (WORD)cRegisterAddress,
                         1,
                         (PCBYTE)&cData,
                         DS1374_I2C_WRITE_FLAG));
}

/**
 *  @brief            Write to specified register address
 *  @param[in]    cInstance:   Device instance
                            cRegisterAddress: Register address to write
                            cSize: BYTE size to write
                            pacBuffer:   Pointer to Data to write
 *  @param[out]  None
 *  @return         rcSUCCESS if the operation was successful.
 *  @return         rcERROR if communication to the hardware failed,  
 *  @note           None
 *  @attention     None
 */
static eRESULT DS1374RegsWrite(BYTE cInstance 
                                                                          , BYTE cRegisterAddress 
                                                                          , BYTE cSize 
                                                                          , PCBYTE pacBuffer)
{
  ASSERT(cInstance < DS1374_NUM_INSTANCES);
  
  return(dvDeviceBusWrite(m_acDeviceBuses[cInstance],
                          m_acDeviceAddresses[cInstance],
                         (WORD)cRegisterAddress,
                         cSize,
                         pacBuffer,
                         DS1374_I2C_WRITE_FLAG));
}

/**
 *  @brief            Read from the specified register address
 *  @param[in]    cInstance   Device instance
                            cRegisterAddress: Register address to Read
                            cSize: BYTE size to Read 
 *  @param[out]    pacBuffer:   pointer for return back read data
 *  @return         rcSUCCESS if the operation was successful.
 *  @return         rcERROR if communication to the hardware failed,  
 *  @note           None
 *  @attention     None
 */
static eRESULT DS1374RegsRead(BYTE   cInstance
                                                                        , BYTE cRegisterAddress
                                                                        , BYTE cSize
                                                                        , PBYTE pacBuffer)
{

  ASSERT(cInstance < DS1374_NUM_INSTANCES);
  ASSERT(pacBuffer != NULL);

  return(dvDeviceBusRead(m_acDeviceBuses[cInstance],
                         m_acDeviceAddresses[cInstance],
                        cRegisterAddress,
                        (WORD)cSize,
                        (PBYTE)pacBuffer,
                        DS1374_I2C_READ_FLAG));
}

/**
 *  @brief            Read from the specified register address
 *  @param[in]    cInstance   Device instance
 *  @param[out]    pacData   pointer for Data to read
 *  @return         rcSUCCESS if the operation was successful.
 *  @return         rcERROR if communication to the hardware failed,  
 *  @note           None
 *  @attention     None
 */
static eRESULT DS1374RegsReadAll(BYTE   cInstance, PDS1374_REGMAP psRegisterValue)
{

  ASSERT(cInstance < DS1374_NUM_INSTANCES);
  ASSERT(psRegisterValue != NULL);

  return(dvDeviceBusRead(m_acDeviceBuses[cInstance],
                         m_acDeviceAddresses[cInstance],
                        0,
                        sizeof(DS1374_REGMAP),
                        (PBYTE)psRegisterValue,
                        DS1374_I2C_READ_FLAG));
}

/**
 *  @brief Initializes static data structures that are used subsequently at runtime.
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void DS1374DriverInitialize(void)
{
  BYTE cI;

  for(cI = 0; cI < DS1374_NUM_INSTANCES; cI++)
  {
    m_abDS1374Presence[cI] = FALSE;
  }
}

/**
 *  @brief            Initializes the device by setting registers that do not change frequently at runtime.
 *  @param[in]    cInstance   Device instance
 *  @param[out]  None
 *  @return         rcSUCCESS if the operation was successful.
 *  @return         rcERROR if communication to the hardware failed,  
 *  @note           None
 *  @attention     None
 */
static eRESULT DS1374DeviceInitialize(void)
{
  eRESULT eResult = rcSUCCESS;
  BYTE cInstance;
  DS1374_REGMAP sRegisterValue;
  BYTE cRetry;

  for(cInstance = 0; cInstance < DS1374_NUM_INSTANCES; cInstance++)
  {
    cRetry = 0;

    /* Find Device. it often happen the lost device ... it looks relation with power dip phenomenon... */
    do
    {
      ///msgDbg(_MOD_NAME_"-%d Trying to find : %d...", cInstance, cRetry); 
      
      eResult = DS1374RegsReadAll(cInstance, &sRegisterValue);      
      if(rcSUCCESS == eResult)
        break;
    }while(cRetry++ <= DS1374_DEVICE_SEARCH_COUNT);
    
    if(rcSUCCESS == eResult)
    {
#if 0 //  Test purpose only
      msgDbg("Timestamp:%ld, nALMC0:0x%.2X, nALMC1:0x%.2X, nALMC2:0x%.2X, Control:0x%.2X, Status:0x%.2X, Tri:0x%.2X", 
                        sRegisterValue.dwTODCounter,
                        sRegisterValue.cWD_nALMCounter0,
                        sRegisterValue.cWD_nALMCounter1,
                        sRegisterValue.cWD_nALMCounter2,
                        *(PBYTE)&sRegisterValue.cCONTROL,
                        *(PBYTE)&sRegisterValue.cSTATUS,
                        *(PBYTE)&sRegisterValue.cTRICHARGER);
#endif

      /* Initialization. do not use sRegisterValue.cSTATUS.OSF, sRegisterValue.dwTODCounter */
      if(*(PBYTE)&sRegisterValue.cTRICHARGER != *(PBYTE)&m_sDS1374InitRegs.cTRICHARGER)
      {
        msgInfo(_MOD_NAME_"-%d is the first time power applied.", cInstance); 
        
        // Initialization
        eResult |= DS1374RegsWrite(cInstance, 0, sizeof(DS1374_REGMAP), (PCBYTE)&m_sDS1374InitRegs);        
      }      
      else
      {
        // Clear status    
        eResult |= DS1374RegWrite(cInstance, REG_STATUS, 0x00);        
      }

#if 0 // Test Purpose Only
      {
        cTRICHARGER sTriCharger;
        cSTATUS sStatus;
        DWORD dwTimeStamp;

        eResult |= DS1374RegsRead(cInstance, REG_TRICHARGER, 1, (PBYTE)&sTriCharger);
        eResult |= DS1374RegsRead(cInstance, REG_STATUS, 1, (PBYTE)&sStatus);
        eResult |= DS1374RegsRead(cInstance, REG_TODC, 4, (PBYTE)&dwTimeStamp);

        msgDbg("Tri:0x%.2X, Status:0x%.2X, Timestamp:%ld", *(PBYTE)&sTriCharger, *(PBYTE)&sStatus, dwTimeStamp);
      }
#endif

      if(eResult != rcSUCCESS)
      {
        msgWarn(_MOD_NAME_"-%d Error Ocurred during initialization !", cInstance);
      }
      else
      {
        msgInfo(_MOD_NAME_"-%d Device is Available", cInstance);
      }
      
      // Presence Set
      m_abDS1374Presence[cInstance] = TRUE;
    }
    /* rcSUCCESS != eResult */
    else
    {
      msgWarn(_MOD_NAME_"-%d COULD NOT FIND! SKIP RTC PROCESS!", cInstance);
    }
  }  

  return eResult;
}


/**
  * @}
  */

/** @defgroup dvTAS2505_Interrupt_Service_Routine
  * @{
  */

/**
  * @}
  */



/** @defgroup dvTAS2505_Public_Functions
  * @{
  */

/**
 *  @brief      This API initializes the device bus software and hardware
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
 void dvDS1374Initialize(void)
 {
   DS1374DriverInitialize();
   DS1374DeviceInitialize();
 }
 
 /**
  *  @brief      This API must be called once during system Release
  *  @param[in]  None
  *  @param[out] None
  *  @return     None
  *  @note       None
  *  @attention  None
  */
 void dvDS1374Release(void)
 {
   /* Release Code Here */
 }

/**
 *  @brief       This API perform the H/W Reset via specified I/F
 *  @param[in]   None
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None
 */
void dvDS1374HWReset(void)
{
  /* NTD */
}
 
/**
 *  @brief      This API the presence of specified DS1374 instance
 *  @param[in]  cInstance:   Device instance
 *  @param[out] None
 *  @return     TRUE if 2-wire device is presence, FALSE for others
 *  @note       None
 *  @attention  None
 */ 
 BOOL dvDS1374PresenceGet(BYTE cInstance)
 {
   ASSERT(cInstance < DS1374_NUM_INSTANCES);
   
   return m_abDS1374Presence[cInstance];
 }
 
/**
 *  @brief       This API the Get the timestamp from specified DS1374 instance
 *  @param[in]   cInstance:     Device instance
                 pdwTimestamp:  pointer to return back timestamp
 *  @param[out]  None
 *  @return      rcSUCCESS if the operation was successful.
                 rcERROR if communication to the hardware failed,  
 *  @note        None
 *  @attention   None
 */ 
 eRESULT dvDS1374TimestampGet(BYTE cInstance, PDWORD pdwTimestamp)
 {
   eRESULT eResult = rcSUCCESS;
   //DS1374_REGMAP sRegisterValue;
   DWORD dwTODCounter;
 
   ASSERT(pdwTimestamp != NULL);
 
   // eResult = DS1374RegsReadAll(cInstance, &sRegisterValue);
   eResult |= DS1374RegsRead(cInstance, REG_TODC, 4, (PBYTE)&dwTODCounter);  
 
   if(rcSUCCESS == eResult)
   {
     //*pdwTimestamp = sRegisterValue.dwTODCounter;
     *pdwTimestamp = dwTODCounter;
   }
   else
   {
     *pdwTimestamp = DS1374_TIMESTAMP_ERROR;
   }
   
   return eResult;
 }
 
/**
 *  @brief      This API the Set the timestamp from specified DS1374 instance
 *  @param[in]  cInstance:    Device instance
                pdwTimestamp: pointer to timestamp to write
 *  @param[out] None
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed,  
 *  @note       None
 *  @attention  None
 */ 
 eRESULT dvDS1374TimestampSet(BYTE cInstance, DWORD dwTimestamp)
 {
   eRESULT eResult = rcSUCCESS; 
 
   eResult = DS1374RegsWrite(cInstance, REG_TODC, 4, (PCBYTE)&dwTimestamp);
 
   return eResult;
 }


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#endif // #if ((USE_LCD_1024x600 ||  USE_LCD_1280x800) && USE_TAS2505_AUDIO_CODEC)

/******************* (C) COPYRIGHT 1997-2019 (c) MadeFactory Inc. *****END OF FILE****/  
