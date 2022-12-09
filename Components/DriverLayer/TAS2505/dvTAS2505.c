/**
  ******************************************************************************
  * @file    dvTAS2505.c
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
#include "dvTAS2505API.h"
#include "dvTAS2505Regs.h"
#include "dvGPIOAPI.h"
#include "dvDeviceBusAPI.h"

#if ((USE_LCD_1024x600 ||  USE_LCD_1280x800) && USE_TAS2505_AUDIO_CODEC)
/** @addtogroup CWM-T10
  * @{
  */

/** @defgroup dvTAS2505 
  * @brief TAS2505 Driver
  * @{
  */

/** @defgroup dvTAS2505_Private_Defines
  * @{
  */
#define _MOD_NAME_ "DRV_AUCODEC"

// ---------------------------------------------------------------------------
// Device Bus Flags
// ---------------------------------------------------------------------------
#define TAS_FLAG_WRITE (DEVICE_BUS_ADDR_TRANSACTION | DEVICE_BUS_FAST_MODE)
#define TAS_FLAG_READ  (DEVICE_BUS_ADDR_TRANSACTION | DEVICE_BUS_SEND_REPEAT_AFTER_ADDR | DEVICE_BUS_FAST_MODE)  

/**
  * @}
  */

/** @defgroup dvTAS2505_Private_TypesDefinitions
  * @{
  */
// ---------------------------------------------------------------------------
// Structure for Register Init
// ---------------------------------------------------------------------------
typedef struct tagTASREGINIT
{
  WORD    wPRAddress; /* (Page | Register) Address */
  BYTE    cData;      /* Data to Init */
}GCC_PACKED TAS_REG_INIT;  
typedef CROMDATA TAS_REG_INIT *PCTAS_REG_INIT; 

/**
  * @}
  */

/** @defgroup dvTAS2505_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup dvTAS2505_Private_Variables
  * @{
  */

// ---------------------------------------------------------------------------
// Device buses
// ---------------------------------------------------------------------------
static CROMDATA BYTE m_acDeviceBuses[TAS2505_NUM_INSTANCES] =
{
/* ------------------------- BEGIN CUSTOM AREA  -------------------------------
  CONFIGURATION: Device bus for each driver instance */
  cpuTWI_HW,
/* ---------------------------- END CUSTOM AREA --------------------------- */
};

// ---------------------------------------------------------------------------
// Device Addresses
// ---------------------------------------------------------------------------
static CROMDATA BYTE m_acDeviceAddresses[TAS2505_NUM_INSTANCES] =
{
/* ------------------------- BEGIN CUSTOM AREA  -------------------------------
  CONFIGURATION: Device bus for each driver instance */
  0x18,
/* ---------------------------- END CUSTOM AREA --------------------------- */
};

// ---------------------------------------------------------------------------
// TAS2505 Register Initial Values (Constant ROM data)
// ---------------------------------------------------------------------------
static CROMDATA TAS_REG_INIT m_asTASRegsInit[] = 
{
/* -------------------------- BEGIN CUSTOM AREA  ---------------------------- */
  {REG_TAS_AIF1,            0x00}, // WORD = 16bits, BLCK&WCLK inputs, I2S mode
  {REG_TAS_AIF2,            0x00}, // Data slot offset = 0
  {REG_TAS_DAC_INSTRUCT,    0x02}, // Dac Instruction programming PRB #2 for Mono routing
  {REG_TAS_REFPWR_CTRL,     0x10}, // Master Refernce Powered On
  {REG_TAS_COMM_MODE,       0x00},  // Output common mode for DAC set to 0.9V
  //{REG_TAS_HP_ROUTE,        0x04}, // Mixer P output is connected to HP Out Mixer
  //{REG_TAS_HP_VOLUME,       0x00}, // HP Voulme, 0dB Gain
  //{REG_TAS_AINL_VOLUME,     0x00}, // No need to enable Mixer M and Mixer P, AINL Voulme, 0dB Gain
  //{REG_TAS_HPOUT_AIN,       0x20}, // Power up HP
  //{REG_TAS_HP_GAIN,         0x00}, // Unmute HP with 0dB gain
  {REG_TAS_SPK_VOLUME,      0x00}, // SPK attn.Gain = 0dB (to 1V RMS in 1KHz Wave)
  //{REG_TAS_SPK_VOLUME,      0x75}, // SPK attn.Gain = Mute
  //{REG_TAS_SPK_VOLUME,      0x0E}, // SPK attn.Gain = -7.0dB (to 0.5V RMS in 1KHz)
  //{REG_TAS_SPK_AMPVOLUME,   0x10}, // SPK driver.Gain = 6.0dB 
  {REG_TAS_SPK_AMPVOLUME,   0x30}, // SPK driver.Gain = 18dB  (to Saturated 3V RMS on 0dB in 1KHz)
  {REG_TAS_SPK_AMP,         0x02}, // SPK Powered up

  {REG_TAS_DAC_CHANNEL1,    0xB0}, // DAC Data path to L/R Mixed. DAC Powered up, Soft step 1 per Fs.
  {REG_TAS_DAC_CHANNEL_VOL, 0x00},  // DAC digital gain 0dB
  //{REG_TAS_DAC_CHANNEL_VOL, 0x07},  // DAC digital gain +3.5dB (1V RMS on 0dB input to 1.75V RMS)
  {REG_TAS_DAC_CHANNEL2,    0x04},  // DAC volume not muted
/* ---------------------------- END CUSTOM AREA --------------------------- */
};

/* Currently Page No. */
static BYTE               m_acTASPageNo[TAS2505_NUM_INSTANCES]; 
/* Currently Input */
static eTAS2505_INPUT_ID  m_aeTASInputId[TAS2505_NUM_INSTANCES];

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
 *  @brief       Write to specified register address
 *  @param[in]   cInstance:         Device instance
 *  @param[in]   cRegisterAddress:  Register Address to Write to
 *  @param[in]   cSize:             Number of bytes to write
 *  @param[in]   pacData:           pointer for Data to write
 *  @param[out]  None
 *  @return      rcSUCCESS if the operation was successful.
 *  @return      rcERROR if communication to the hardware failed,  
 *  @note        None
 *  @attention   None
 */
static eRESULT TAS2505RegsWrite(BYTE   cInstance,
                                     BYTE   cRegisterAddress,
                                     BYTE   cSize,
                                     PCBYTE pacData)
{    
  ASSERT(cInstance < TAS2505_NUM_INSTANCES);  
  
  return(dvDeviceBusWrite(m_acDeviceBuses[cInstance],
                          m_acDeviceAddresses[cInstance],
                         (WORD)cRegisterAddress,
                         (WORD)cSize,
                         pacData,
                         TAS_FLAG_WRITE));
}

/**
 *  @brief        Read from the specified register address
 *  @param[in]    cInstance:        Device instance
 *  @param[in]    cRegisterAddress: Register Address to read to
 *  @param[in]    cSize:            Number of bytes to read
 *  @param[out]   pacData:          pointer for Data to read
 *  @return       rcSUCCESS if the operation was successful.
 *  @return       rcERROR if communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
static eRESULT TAS2505RegsRead(BYTE   cInstance,
                                    BYTE   cRegisterAddress,
                                    BYTE   cSize,
                                    PBYTE  pacData)
{
  ASSERT(cInstance < TAS2505_NUM_INSTANCES);

  return(dvDeviceBusRead(m_acDeviceBuses[cInstance],
                         m_acDeviceAddresses[cInstance],
                        (WORD)cRegisterAddress,
                        (WORD)cSize,
                        pacData,
                        TAS_FLAG_READ));
}

/**
 *  @brief        Write one data to specified register address
 *  @param[in]    cInstance:        Device instance
 *  @param[in]    cRegisterAddress: Register Address to Write to
 *  @param[in]    cValue:           Data to write
 *  @return       rcSUCCESS if the operation was successful.
 *  @return       rcERROR if communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
static eRESULT TAS2505OneRegWrite(BYTE   cInstance,
                                     BYTE   cRegisterAddress,
                                     BYTE   cValue)
{
  return TAS2505RegsWrite(cInstance,
                           cRegisterAddress,
                           1,
                           (PCBYTE)&cValue);
}

/**
 *  @brief        Read one data from the specified register address
 *  @param[in]    cInstance:          Device instance
 *  @param[in]    cRegisterAddress:   Register Address to read to
 *  @param[out]   pcValue:            pointer for Data to read
 *  @return       rcSUCCESS if the operation was successful.
 *  @return       rcERROR if communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
static eRESULT TAS2505OneRegRead(BYTE   cInstance,
                                    BYTE   cRegisterAddress,
                                    PBYTE pcValue)
{

  return TAS2505RegsRead(cInstance,
                          cRegisterAddress,
                          1,
                          pcValue);
}

/**
 *  @brief        Write to specified Register Field and specified register address
 *  @param[in]    cInstance:          Device instance
 *  @param[in]    cRegisterAddress:   Register Address to Write to
 *  @param[in]    cBitPosition:       Bit Position
 *  @param[in]    cBitOffset:         Bit Size
 *  @param[out]   cData:              Data to write
 *  @return       rcSUCCESS if the operation was successful.
 *  @return       rcERROR if communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
static eRESULT TAS2505OneRegFieldWrite(BYTE   cInstance,
                                     BYTE   cRegisterAddress,
                                     BYTE cBitPosition,
                                     BYTE   cBitOffset,
                                     BYTE cData)
{
  BYTE cValue;
  
  ASSERT(cInstance < TAS2505_NUM_INSTANCES);
  ASSERT(cBitPosition < 8);
  ASSERT(cBitOffset+cBitPosition <= 8);

  if(rcSUCCESS != TAS2505OneRegRead(cInstance, cRegisterAddress, &cValue))
  {
    return rcERROR; // >>>>>>>>>
  }

  cBitOffset = ~(0xFF << cBitOffset);
  cValue &= ~(cBitOffset << cBitPosition);
  cValue |= (cData & cBitOffset) << cBitPosition;  


  return(TAS2505OneRegWrite(cInstance, cRegisterAddress, cValue));  
}

/**
 *  @brief       Set Register Page No
 *  @param[in]   cInstance: Device instance
 *  @param[in]   cPageNo:   Number to Page set
 *  @param[out]  None
 *  @return      rcSUCCESS if the operation was successful.
 *  @return      rcERROR if communication to the hardware failed,  
 *  @note        None
 *  @attention   None
 */
static eRESULT TAS2505PageSet(BYTE cInstance, BYTE cPageNo)
{
  eRESULT eResult = rcSUCCESS;

  if(m_acTASPageNo[cInstance] != cPageNo)
  {
    eResult |= TAS2505OneRegWrite(cInstance, REG_TAS_PAGE, cPageNo);
    if(rcSUCCESS == eResult)
      m_acTASPageNo[cInstance] = cPageNo;
  }

  return eResult;
}

/**
 *  @brief        Create the device instance as specified initializing value
 *  @param[in]    cInstance:    Device instance
                  psInitValue:  Pointer to initial value
                  wSize:        size to init to
 *  @param[out]   None
 *  @return       rcSUCCESS if the operation was successful.
                  rcERROR if communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
static eRESULT TAS2505CreateInstance(BYTE cInstance, PCTAS_REG_INIT psInitValue, WORD wSize)
{
  eRESULT eResult = rcSUCCESS;
  WORD    wI;  
  WORD    wPBRAddress;
  BYTE    cData;

  ASSERT(wSize > 0);
  ASSERT(psInitValue != NULL);

  for(wI = 0; wI < wSize; wI++)
  {
    wPBRAddress = psInitValue->wPRAddress;
    cData = psInitValue->cData;

    eResult = dvTAS2505OneWrite(cInstance, wPBRAddress, cData);

    if(eResult != rcSUCCESS)
    {
      msgErr(_MOD_NAME_"-%d INIT FAILED [PAGE:%d, ADDR:0x%.2X, VALUE:0x%.2X]", 
                    cInstance, 
                    (BYTE)((wPBRAddress & TAS_PAGE_MASK) >> TAS_PAGE_SHIFT),
                    (BYTE)wPBRAddress,
                    cData);
      return rcERROR; //>>>>>>>>>>>>>>>
    }    

    psInitValue++;
  }

  return eResult;
}

/**
 *  @brief      Initialize the Driver
 *  @param      cInstance: Device instance
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void TAS2505DriverInitialize(BYTE cInstance)
{
  ASSERT(cInstance < TAS2505_NUM_INSTANCES);

  m_acTASPageNo[cInstance] = 0xFF; 
  m_aeTASInputId[cInstance] = aiINVALID;
}


/**
 *  @brief      Configures the Device 
 *  @param      cInstance: Device instance
 *  @return     rcSUCCESS if the operation was successful.
 *  @note       None
 *  @attention  None
 */
static eRESULT TAS2505DeviceInitialize(BYTE cInstance)
{
  eRESULT eResult = rcERROR;

  ASSERT(cInstance < TAS2505_NUM_INSTANCES);

  do
  {
    TAS_LDO_CTRL    uLDOCtrl  = {.cData   = 0};
    TAS_CLOCK_MUX1  uMux1     = {.cData   = 0};
    TAS_CLOCK_PLL   uPLL      = {.dwData  = 0};
    TAS_CLOCK_NDAC  uNDAC     = {.cData   = 0};
    TAS_CLOCK_MDAC  uMDAC     = {.cData   = 0};
    TAS_DAC_OSR     uDOSR     = {.wData   = 0};

    /* LDO output Init */ 
    // LDO output as 1.8V and Level Shifter Power Up
    uLDOCtrl.MUST_BE_1 = 1;
    if(rcSUCCESS != dvTAS2505OneWrite(cInstance, REG_TAS_LDO_CTRL, uLDOCtrl.cData)) break; //---------->    
    
    /* Clock Init */   
    uMux1.CODEC_CLKIN = 3; // CODEC_CLKIN = PLL_CLK
    uMux1.PLL_CLKIN   = 1; // PLL_CLKIN = BCLK (3.072MHz)
    if(rcSUCCESS != dvTAS2505OneWrite(cInstance, REG_TAS_CLOCK_MUX1, uMux1.cData)) break; //---------->

    uPLL.PLL_R    = 1; // P=1
    uPLL.PLL_P    = 4; // R=1
    uPLL.PLL_PWR  = 1; // Power Up PLL
    uPLL.PLL_J    = 7; // J=4
    uPLL.PLL_DH   = 0; // PLL Divider D[13:8] = 0
    uPLL.PLL_DL   = 0; // PLL Divider D[7:0] = 0
    if(rcSUCCESS != dvTAS2505Writes(cInstance, REG_TAS_CLOCK_PLL, sizeof(TAS_CLOCK_PLL), (PBYTE)&uPLL.dwData)) 
      break; //---------->

    // DAC NDAC Powered up, NADC=4
    uNDAC.NDAC          = 2;
    uNDAC.NDAC_DIV_PWR  = 1;
    if(rcSUCCESS != dvTAS2505OneWrite(cInstance, REG_TAS_CLOCK_NDAC, uNDAC.cData)) break; //---------->

    // DAC MDAC Powered up, MDAC=2
    uMDAC.MDAC          = 7; 
    uMDAC.MDAC_DIV_PWR  = 1;
    if(rcSUCCESS != dvTAS2505OneWrite(cInstance, REG_TAS_CLOCK_MDAC, uMDAC.cData)) break; //---------->

    commDelayMS(50); // Delay of 15ms at least for PLL to Lock

    // DOSR=128
    uDOSR.DOSR_H = 0;
    uDOSR.DOSR_L = 80; 
    if(rcSUCCESS != dvTAS2505Writes(cInstance, REG_TAS_DAC_OSR, sizeof(TAS_DAC_OSR), (PBYTE)&uDOSR.wData)) 
      break; //---------->    


    /* Create Instance */
    if(rcSUCCESS != TAS2505CreateInstance(cInstance, 
                      (PCTAS_REG_INIT)&m_asTASRegsInit, 
                      (sizeof(m_asTASRegsInit) / sizeof(TAS_REG_INIT)))) break; //---------->

    eResult = rcSUCCESS;
  }while(0);

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
 *  @brief      This API must be called once during system startup, 
                before calls are made to any other APIs in the Chip Driver component.
 *  @param      None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvTAS2505Initialize(void)
{
  BYTE    cInstance;
  

  /* H/W Reset */
  dvTAS2505HWReset();

  for(cInstance = 0; cInstance < TAS2505_NUM_INSTANCES; cInstance++)
  {
    eRESULT eResult     = rcSUCCESS;
    
    TAS2505DriverInitialize(cInstance);
    eResult |= TAS2505DeviceInitialize(cInstance);

    if(rcSUCCESS == eResult)
    {
      msgInfo(_MOD_NAME_"-%d Device is Available", cInstance);    
    }  
    else
    {
      /* Error Process Here */
      msgErr(_MOD_NAME_"-%d DEVICE INITIALIZED FAILED !", cInstance);      
    }
  }
  
  
}

/**
 *  @brief       This API Release the Device
 *  @param[in]   None
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None
 */
void dvTAS2505Release(void)
{ 

}

/**
 *  @brief       This API perform the H/W Reset via specified I/F
 *  @param[in]   None
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None
 */
void dvTAS2505HWReset(void)
{
  /* RESET hold: at least 10nS */
  dvGPIOFieldSet(gfGPO_AMP_nRESET, LOW);
  commDelayMS(10); 
  
  /* RESET Release: at least 1ms to stabilize after RESET release */
  dvGPIOFieldSet(gfGPO_AMP_nRESET, HIGH);
  commDelayMS(10); 
}

/**
 *  @brief            This API perform the S/W Reset via specified I/F
 *  @param[in]    cInstance   Device instance
 *  @param[out]  None
 *  @return         rcSUCCESS if the operation was successful.
 *  @return         rcERROR if communication to the hardware failed,  
 *  @note           None
 *  @attention     None
 */
eRESULT dvTAS2505SWReset(BYTE cInstance)
{
  eRESULT eResult = rcSUCCESS;

  eResult |= dvTAS2505OneWrite(cInstance, REG_TAS_SWRESET, 0x01);

  commDelayMS(20); // wait 10ms

  return eResult;
}

/**
 *  @brief        This API sets the registers in the TAS2505 to configure the specified
                  input as the source for the specified output.
 *  @param[in]    eAudioInputId  : Identifier of audio input source
                  eAudioOutputId : Identifier of audio output
 *  @param[out]   None
 *  @return       rcSUCCESS if the operation was successful.
 *  @return       rcERROR if communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
eRESULT dvTAS2505Configure(BYTE cInstance,
                                 eTAS2505_INPUT_ID eAudioInputId)                                                
{
  eRESULT         eResult     = rcSUCCESS;

  /* NTD in This Product */

  return eResult;
}

/**
 *  @brief       This API Control GPIO pin as bit bang output
 *  @param[in]   cInstance: Device instance
 *  @param[in]   bHigh:     TRUE to HIGH, FALSE to Low
 *  @param[out]  None
 *  @return      rcSUCCESS if the operation was successful.
 *  @return      rcERROR if communication to the hardware failed,  
 *  @note        None
 *  @attention   None
*/
eRESULT dvTAS2505GPIOBitBangOutSet(BYTE cInstance, BOOL bHigh)
{ 
  eRESULT eResult = rcSUCCESS;

  /* NTD in This Product */ 

  return eResult;
}                                                

/**
 *  @brief       This API is Wrapper of TAS2505RegsWrite()
 *  @param[in]   cInstance:     Device instance
 *  @param[in]   wPBRAddress:  Register Address(included Book/Page) to Write to
 *  @param[in]   cSize:         Number of bytes to write
 *  @param[in]   pacData:       pointer for Data to write
 *  @param[out]  None
 *  @return      rcSUCCESS if the operation was successful.
 *  @return      rcERROR if communication to the hardware failed,  
 *  @note        None
 *  @attention   None
 */
eRESULT dvTAS2505Writes(BYTE cInstance, 
                              WORD wPBRAddress, 
                              BYTE cSize, 
                              PCBYTE pacData)
{
  eRESULT eResult = rcSUCCESS;

  eResult |= TAS2505PageSet(cInstance, 
                          (BYTE)((wPBRAddress & TAS_PAGE_MASK) >> TAS_PAGE_SHIFT));

  eResult |= TAS2505RegsWrite(cInstance, 
                                (BYTE)wPBRAddress,
                                cSize,
                                pacData);

  return rcSUCCESS;
}

/**
 *  @brief        This API Wrapper of TAS2505PageSet(), TAS2505RegsRead()
 *  @param[in]    cInstance:    Device instance
 *  @param[in]    wPBRAddress: Register Address(included Book/Page) to Read
 *  @param[in]    cSize:        Number of bytes to read
 *  @param[out]   pacData:      pointer for Data to read
 *  @return       rcSUCCESS if the operation was successful.
 *  @return       rcERROR if communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
eRESULT dvTAS2505Reads(BYTE cInstance, 
                            WORD wPBRAddress, 
                            BYTE cSize, 
                            PBYTE pacData)
{
  eRESULT eResult = rcSUCCESS;

  eResult |= TAS2505PageSet(cInstance, 
                            (BYTE)((wPBRAddress & TAS_PAGE_MASK) >> TAS_PAGE_SHIFT));

  eResult |= TAS2505RegsRead(cInstance, 
                              (BYTE)wPBRAddress,
                              cSize,
                              pacData);

  return eResult;
}

/**
 *  @brief       This API is Wrapper of TAS2505PageSet(), TAS2505OneRegWrite()
 *  @param[in]   cInstance:     Device instance
 *  @param[in]   wPBRAddress:  Register Address(included Book/Page) to Write to
 *  @param[in]   cData:         Data to write
 *  @param[out]  None
 *  @return      rcSUCCESS if the operation was successful.
 *  @return      rcERROR if communication to the hardware failed,  
 *  @note        None
 *  @attention   None
 */
eRESULT dvTAS2505OneWrite(BYTE cInstance, 
                                WORD wPBRAddress, 
                                BYTE cData)
{
  eRESULT eResult = rcSUCCESS;

  eResult |= TAS2505PageSet(cInstance, 
                            (BYTE)((wPBRAddress & TAS_PAGE_MASK) >> TAS_PAGE_SHIFT));

  eResult |= TAS2505OneRegWrite(cInstance, 
                                (BYTE)wPBRAddress,
                                cData);

  return rcSUCCESS;
}

/**
 *  @brief        This API is Wrapper of TAS2505PageSet(), TAS2505OneRegRead()
 *  @param[in]    cInstance:    Device instance
 *  @param[in]    wPBRAddress: Register Address(included Book/Page) to Read to
 *  @param[out]   pacData:      pointer for Data to read
 *  @return       rcSUCCESS if the operation was successful.
 *  @return       rcERROR if communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
eRESULT dvTAS2505OneRead(BYTE cInstance, 
                              WORD wPBRAddress, 
                              PBYTE pacData)
{
  eRESULT eResult = rcSUCCESS;

  eResult |= TAS2505PageSet(cInstance, 
                            (BYTE)((wPBRAddress & TAS_PAGE_MASK) >> TAS_PAGE_SHIFT));

  eResult |= TAS2505OneRegRead(cInstance, 
                                (BYTE)wPBRAddress,
                                pacData);

  return eResult;  
}


/**
 *  @brief        This API is Wrapper of TAS2505PageSet(), TAS2505OneRegFieldWrite()
 *  @param[in]    cInstance:    Device instance
 *  @param[in]    wPBRAddress: Register Address(included Book/Page) to Write to
 *  @param[in]    cBitPosition: Bit Position
 *  @param[in]    cBitOffset:   Bit Size
 *  @param[out]   cData:        Data to write
 *  @return       rcSUCCESS if the operation was successful.
 *  @return       rcERROR if communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
eRESULT dvTAS2505OneFieldWrite(BYTE cInstance,
                                        WORD wPBRAddress,
                                        BYTE cBitPosition,
                                        BYTE cBitOffset,
                                        BYTE cData)
{
  eRESULT eResult = rcSUCCESS;

  eResult |= TAS2505PageSet(cInstance, 
                            (BYTE)((wPBRAddress & TAS_PAGE_MASK) >> TAS_PAGE_SHIFT));
  eResult |= TAS2505OneRegFieldWrite(cInstance,
                                      (BYTE)wPBRAddress,
                                      cBitPosition,
                                      cBitOffset,
                                      cData);
  
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
