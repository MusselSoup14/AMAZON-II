/**
  ******************************************************************************
  * @file    dvAK4183.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    09-Feb-2018
  * @brief   This file contains all the functions prototypes for the CPU Driver
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
  * <h2><center>&copy; Copyright 1997-2018 (c) MadeFactory Inc.</center></h2>
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/  
#include "dvAK4183API.h"
#include "dvGPIOAPI.h"
#include "dvDeviceBusAPI.h"

#if USE_TOUCH_AK4183
// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

// ---------------------------------------------------------------------------
// Device Constant
// ---------------------------------------------------------------------------
#if USE_IMPROVED_TOUCH_SPEED
#define AK4183_SAMPLE_COUNT               1// 5
#define AK4183_SAMPLE_MIN                 0// (AK4183_SAMPLE_COUNT - 2)
#define AK4183_SAMPLE_SKIP_DIF            0// 30
#else
#define AK4183_SAMPLE_COUNT               5
#define AK4183_SAMPLE_MIN                 (AK4183_SAMPLE_COUNT - 2)
#define AK4183_SAMPLE_SKIP_DIF            30
#endif

// ---------------------------------------------------------------------------
// Device Bus Flags
// ---------------------------------------------------------------------------
#define AK4183_FLAG_WRITE (0 /*DEVICE_BUS_FAST_MODE*/)
#define AK4183_FLAG_READ  (0 /*DEVICE_BUS_FAST_MODE*/)

//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************
static CROMDATA BYTE m_acDeviceBuses[AK4183_NUM_INSTANCES] =
{
/* ------------------------- BEGIN CUSTOM AREA  -------------------------------
  CONFIGURATION: Device bus for each driver instance */
  cpuTWI_HW,
/* ---------------------------- END CUSTOM AREA --------------------------- */
};

static CROMDATA BYTE m_acDeviceAddress[AK4183_NUM_INSTANCES] =
{
/* ------------------------- BEGIN CUSTOM AREA  -------------------------------
  CONFIGURATION: Device bus for each driver instance */
  0x49,
/* ---------------------------- END CUSTOM AREA --------------------------- */
};

// ---------------------------------------------------------------------------
// AK4183 Control Register Map
// ---------------------------------------------------------------------------
/*  BIT Name  Function 
    7   S     Start Bit. ¡°1¡± Accelerate and Axis Command, ¡°0¡±: Sleep mode Command 
    6-4 A2-A0 Channel Selection Bits. 
              Analog inputs to the A/D converter and the activated driver switches 
              are selected. Please see the following table for the detail. 
    3   X1    Don't care 
    2   PD0   Power down bit (refer to power-down control) 
    1   MODE  Resolution of A/D converter. ¡°0¡±: 12 bit output ¡°1¡±: 8 bit output 
    0   X2    Don't care 
*/    
typedef struct tagAK4183CTRLREG
{
  BYTE 
    RSV0:           1,
    MODE_8BIT:      1,
    PWRDN:          1,
    RSV1:           1,
    CHANNEL:        3,
    START:          1;
}AK4183_CTRL_REG, *PAK4183_CTRL_REG;
typedef CROMDATA AK4183_CTRL_REG *PCAK4183_CTRL_REG;

/* Instruction bits*/
#define	AK4183_CMD_SLEEP		      0x70
#define AK4183_CMD_DRIVER_Y	      0x90
#define AK4183_CMD_DRIVER_X	      0x80
#define AK4183_CMD_DRIVER_YP	    0xA0
#define AK4183_CMD_DRIVER_XM	    0xB0
#define	AK4183_CMD_READ_X		      0xC0
#define	AK4183_CMD_READ_Y		      0xD0
#define AK4183_CMD_PRESS_Z1			  0xE0
#define AK4183_CMD_PRESS_Z2			  0xF0

#define AK4183_MEASURE_X		      (0xC << 4)
#define AK4183_MEASURE_Y		      (0xD << 4)
#define AK4183_MEASURE_Z1		      (0xE << 4)
#define AK4183_MEASURE_Z2		      (0xF << 4)

#define AK4183_AUTO_POWER_IRQ_EN	(0x0 << 2)
#define AK4183_ADC_ON_IRQ_DIS		  (0x1 << 2)

#define AK4183_12BIT			        (0x0 << 1)
#define AK4183_8BIT			          (0x1 << 1)

#define	AK4183_VAL_12BIT			    (1 << 12)
#define	AK4183_MAX_12BIT			    (AK4183_VAL_12BIT - 1)

#define AK4183_ADC_ON_12BIT	      (AK4183_12BIT | AK4183_AUTO_POWER_IRQ_EN)

#define AK4183_READ_X				      (AK4183_ADC_ON_12BIT | AK4183_MEASURE_X)
#define AK4183_READ_Y				      (AK4183_ADC_ON_12BIT | AK4183_MEASURE_Y)
#define AK4183_READ_Z1				    (AK4183_ADC_ON_12BIT | AK4183_MEASURE_Z1)
#define AK4183_READ_Z2				    (AK4183_ADC_ON_12BIT | AK4183_MEASURE_Z2)

//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************
/* Internal Device Status */
static AK4183_STATUS m_asAK4183Status[AK4183_NUM_INSTANCES];

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************

/**
 *  @brief      Calculate the Touch ADC Average Value
 *  @param      cInstance: Device instance
                panBuffer: Pointer to Read buffer to average
                wBufferSize: byte size to read buffer
                pnAdcAverage: Pointer to return back ADC Average
                pcsConfig: Pointer to touch config
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static inline void AK4183AdcAverage(const PINT panBuffer
                                            , WORD wBufferSize
                                            , PINT pnAdcAverage
                                            , PCAK4183_CONFIG pcsConfig)
{
	WORD wI;	
  UINT nSum         = 0;
	UINT nAverage = -1;

  if(panBuffer != NULL 
    && wBufferSize
    && pnAdcAverage != NULL)
  {  
  	for(wI = 0; wI < wBufferSize; wI++) 
    {
  		nSum += panBuffer[wI];
  	}

    nAverage = nSum / wBufferSize;

    // Recalulate average again exclude GAP (+-SKIP_DIF)
  	nSum = 0;

#if !USE_IMPROVED_TOUCH_SPEED
  {
    WORD wValidCount  = 0;
    
    // Recalulate average again exclude GAP (+-SKIP_DIF)
  	nSum = 0;
    
  	for (wI = 0; wI < wBufferSize; wI++) 
  	{
  		if(((nAverage + AK4183_SAMPLE_SKIP_DIF) > panBuffer[wI]) 
          && ((nAverage - AK4183_SAMPLE_SKIP_DIF) < panBuffer[wI]))
  		{
  			nSum += panBuffer[wI];
  			wValidCount++;
  		}
  	}
    
  	if(wValidCount < wBufferSize-2)
  	  nAverage = -1;
    else  
    	nAverage = nSum/wValidCount;
  }
#endif

    *pnAdcAverage = nAverage;
  }  
}

/**
 *  @brief      Convert the X/Y ADC to Screen Axis
 *  @param      cInstance: Device instance
                nAdcX: Pointer to X-ADC Value
                nAdcY: Pointer to Y-ADC Value
                psPoint: Pointer to return back screen position
                pcsConfig: Pointer to touch config
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed. 
 *  @note       None
 *  @attention  None
 */
static inline eRESULT AK4183AdcToScreenAxis(INT nAdcX
                                                    , INT nAdcY
                                                    , PPOINT psPoint
                                                    , PCAK4183_CONFIG pcsConfig)
{
  eRESULT eResult = rcSUCCESS;
  
  nAdcX = MAX(nAdcX, pcsConfig->wAdcXMin);
  nAdcX = MIN(nAdcX, pcsConfig->wAdcXMax);


	psPoint->wX = (nAdcX-pcsConfig->wAdcXMin)*PANE_WIDTH_MAX 
                / (pcsConfig->wAdcXMax - pcsConfig->wAdcXMin);

  nAdcY = MAX(nAdcY, pcsConfig->wAdcYMin);
  nAdcY = MIN(nAdcY, pcsConfig->wAdcYMax);	

	psPoint->wY = PANE_HEIGHT_MAX - (nAdcY-pcsConfig->wAdcYMin)*PANE_HEIGHT_MAX
                                  /(pcsConfig->wAdcYMax-pcsConfig->wAdcYMin);  

  return eResult;
}

/**
 *  @brief      Write the Data to Device
 *  @param      cInstance: Device instance
                cCmdByte: Control Command Byte to Write
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed. 
 *  @note       None
 *  @attention  None
 */
static eRESULT AK4183WriteCtrlCmd(BYTE cInstance, BYTE cCmdByte)
{
  eRESULT eResult;

  eResult = dvDeviceBusWrite(m_acDeviceBuses[cInstance]
                          , m_acDeviceAddress[cInstance]
                          , 0
                          , 1
                          , (PCBYTE)&cCmdByte
                          , AK4183_FLAG_WRITE);

  return eResult;
}

/**
 *  @brief      Read the Data from Device
 *  @param      cInstance: Device instance
                pnADCValue: pointer to return back ADC Value
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed. 
 *  @note       None
 *  @attention  None
 */
static eRESULT AK4183ReadAdcSingle(BYTE cInstance, PINT pnADCValue)
{
  eRESULT eResult         = rcERROR;
  WORD    wADCValue       = 0;
  BYTE    acADCValue[2];

  ASSERT(pnADCValue != NULL);

  eResult = dvDeviceBusRead(m_acDeviceBuses[cInstance]
                        , m_acDeviceAddress[cInstance]
                        , 0
                        , 2
                        , acADCValue
                        , AK4183_FLAG_READ);


  if(rcSUCCESS == eResult)
  { 
    wADCValue = ((WORD)acADCValue[0] << 8) | acADCValue[1];
    
    /* Shift LSB 4bit by 12bit max */
    wADCValue = (wADCValue >> 4) & 0xFFF;
    *pnADCValue = (INT)wADCValue;
    //msgDbg("X:%d", wADCValue);
  }
  else
  {
    *pnADCValue = -1; 
  }

  return eResult;
}

/**
 *  @brief      Read the specified Axis ADC value from Device
 *  @param      cInstance: Device instance                
                pnReturnADC: Pointer to return ADC-X Value
                bAxisX: TRUE to Axis-X, FALSE to Axis-Y
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed. 
 *  @note       None
 *  @attention  None
 */
static inline eRESULT AK4183ReadAdcAxis(BYTE cInstance, PINT pnReturnADC, BOOL bAxisX)
{
  eRESULT eResult           = rcERROR;
  
  BYTE cCmdByte             = (bAxisX) ? AK4183_CMD_DRIVER_X : AK4183_CMD_DRIVER_Y;
  INT anADCValue[AK4183_SAMPLE_COUNT];
  WORD wI, wJ;
  INT nADCValueTemp;
  PCAK4183_CONFIG pcsConfig = &m_asAK4183Status[cInstance].sConfig;

  ASSERT(cInstance < AK4183_NUM_INSTANCES && pnReturnADC != NULL);

  memset(anADCValue, 0, sizeof(INT)*AK4183_SAMPLE_COUNT);

  do
  {
    /* Looks New AK4183 Fast mode is too faster than ADC sampling speed 
       so we move the sampling command into each of read */
    //if(rcSUCCESS != AK4183WriteCtrlCmd(cInstance, cCmdByte))
      //break;
    
    for(wI = 0, wJ = 0; wI < AK4183_SAMPLE_COUNT; wI++)
    {
      if(rcSUCCESS == AK4183WriteCtrlCmd(cInstance, cCmdByte)
        && rcSUCCESS == AK4183ReadAdcSingle(cInstance, &nADCValueTemp)
        && nADCValueTemp <= pcsConfig->wAdcXMax
        && nADCValueTemp >= pcsConfig->wAdcXMin)
      {
        anADCValue[wJ++] = nADCValueTemp;
      }
    }

    if(wJ > AK4183_SAMPLE_MIN)
    {
      AK4183AdcAverage(anADCValue, wJ, pnReturnADC, pcsConfig);
      eResult = rcSUCCESS;
    } 
    else
      *pnReturnADC = -1;

  }while(0);

  return eResult;
}

/**
 *  @brief      Read the ADC-PRESS value from Device
 *  @param      cInstance: Device instance
                pwAdcPress: Pointer to return ADC-PRESS Value                
                bZ1: TRUE to Z1 Press, FALSE to Z2 Press
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed. 
 *  @note       None
 *  @attention  None
 */
static inline eRESULT AK4183ReadAdcPress(BYTE cInstance, PINT pnReturnADC, BOOL bZ1)
{
  eRESULT eResult     = rcERROR;

  BYTE cCmdByte       = (bZ1) ? AK4183_CMD_PRESS_Z1 : AK4183_CMD_PRESS_Z2;
  ///PCAK4183_CONFIG pcsConfig = &m_asAK4183Status[cInstance].sConfig;
  INT nADCValueTemp;

  ASSERT(cInstance < AK4183_NUM_INSTANCES && pnReturnADC != NULL);

  do
  {
    if(rcSUCCESS != AK4183WriteCtrlCmd(cInstance, cCmdByte))
      break;

    if(rcSUCCESS != AK4183ReadAdcSingle(cInstance, &nADCValueTemp))
      break;

    eResult = rcSUCCESS;
  }while(0);


  if(rcSUCCESS == eResult)
    *pnReturnADC = nADCValueTemp;
  else
    *pnReturnADC = -1;
  

  return eResult;
}

/**
 *  @brief Initializes static data structures that are used subsequently at runtime.
 *  @param[in]  cInstance : Device instance
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void AK4183DriverInitialize(BYTE cInstance)
{
  ASSERT(cInstance < AK4183_NUM_INSTANCES);

  memset(&m_asAK4183Status, 0, sizeof(AK4183_STATUS)*AK4183_NUM_INSTANCES);

  /* Configuration to Default Value */
  m_asAK4183Status[cInstance].sConfig.wAdcXMin        = AK4183_CONFIG_ADC_X_MIN_DEF;
  m_asAK4183Status[cInstance].sConfig.wAdcXMax        = AK4183_CONFIG_ADC_X_MAX_DEF;
  m_asAK4183Status[cInstance].sConfig.wAdcYMin        = AK4183_CONFIG_ADC_Y_MIN_DEF;
  m_asAK4183Status[cInstance].sConfig.wAdcYMax        = AK4183_CONFIG_ADC_Y_MAX_DEF;
  m_asAK4183Status[cInstance].sConfig.wAdcPressMin    = AK4183_CONFIG_ADC_PRESS_MIN_DEF;
  m_asAK4183Status[cInstance].sConfig.wAdcAVGTol      = AK4183_CONFIG_ADC_AVG_TOL_DEF;
  m_asAK4183Status[cInstance].bConfigured             = TRUE;
}

/**
 *  @brief       Initializes the device by setting registers that do not change frequently at runtime.
 *  @param[in]   cInstance: Device instance
 *  @param[out]  None
 *  @return      rcSUCCESS if the operation was successful.
 *  @return      rcERROR if communication to the hardware failed,  
 *  @note        None
 *  @attention   None
 */
static eRESULT AK4183DeviceInitialize(BYTE cInstance)
{
  eRESULT eResult = rcSUCCESS;

  ASSERT(cInstance < AK4183_NUM_INSTANCES);

  /* GPIO AF Set to EIRQ1 */
  *R_PAF11 &= ~(AF11_6_GP6);
	*R_PAF11 |= AF11_6_EIRQ1;

  return eResult;
}

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *  @brief       This API initializes Device and Driver that do not change frequently at runtime.
 *  @param[in]   None
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None
 */
void dvAK4183Initialize(void)
{
  BYTE cInstance;

 dvAK4183HWReset();  
  
  for(cInstance = 0; cInstance < AK4183_NUM_INSTANCES; cInstance++)
  {
    AK4183DriverInitialize(cInstance);
    if(rcSUCCESS != AK4183DeviceInitialize(cInstance))
    {
      /* Error Process Here */
      msgErr("AK4183-%d INIT ERROR !", cInstance);
    }
  }  
}

/**
 *  @brief        This API perform that Free Resources that Used on this component
 *  @param[in]    None
 *  @param[out]   None
 *  @return       None
 *  @note         None
 *  @attention    None
 */
void dvAK4183Release(void)
{
  /* Release Code Here */
}

/**
 *  @brief         This API Process the internal state of component..
 
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvAK4183Process(void)
{
  dvAK4183TouchGet(0, NULL);
}

/**
 *  @brief        This API perform the H/W Reset via GPO
 *  @param[in]    None
 *  @param[out]   None
 *  @return       None
 *  @note         None
 *  @attention    one
 */
void dvAK4183HWReset(void)
{
  /* No HW reset function */
}

/**
 *  @brief        This API perform the S/W Reset via specified I/F
 *  @param[in]    cInstance: Device instance
 *  @param[out]   None
 *  @return       rcSUCCESS if the operation was successful.
                  rcERROR if communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
eRESULT dvAK4183SWReset(BYTE cInstance)
{
  eRESULT eResult = rcSUCCESS;

  /* No SW reset function */
    
  return eResult;
}

/**
 *  @brief        This API perform Reads a Touch Point
 *  @param[in]    cInstance: Device instance
                  psPoint: Pointer to read point return back
 *  @param[out]   None
 *  @return       rcSUCCESS if Event occurred and the operation was successful
                  rcERROR if Event is not occurred or communication to the hardware failed,  
 *  @note         None
 *  @attention    None
 */
eRESULT dvAK4183TouchGet(BYTE cInstance, PPOINT psPoint)
{
  eRESULT eResult = rcERROR;
  BYTE cPenIRQLevel = (BYTE)*R_EINTMOD; 
  PAK4183_READ_STATUS psReadStatus = &m_asAK4183Status[cInstance].sReadStatus;
  PCAK4183_CONFIG psConfig = &m_asAK4183Status[cInstance].sConfig;
  
  if((cPenIRQLevel & F_EINTMOD_1ST) == 0) // Check Level is LOW (Event Occurred)
  {    
    do
    {
      if(rcSUCCESS != AK4183ReadAdcAxis(cInstance, &psReadStatus->nAdcX, TRUE))
        break; //--------->    

      ///msgDbg("ADC-X: %d", psReadStatus->nAdcX);
      
      if(rcSUCCESS != AK4183ReadAdcAxis(cInstance, &psReadStatus->nAdcY, FALSE))
        break; //--------->

      ///msgDbg("ADC-Y: %d", psReadStatus->nAdcY);

      if(rcSUCCESS == AK4183ReadAdcPress(cInstance, &psReadStatus->nAdcPress, TRUE))
      {
        if(psReadStatus->nAdcPress > psConfig->wAdcPressMin)
        {
          if(rcSUCCESS == AK4183AdcToScreenAxis(psReadStatus->nAdcX
                                                , psReadStatus->nAdcY
                                                , &psReadStatus->sPoint
                                                , psConfig))
          {
            eResult = rcSUCCESS;

            /*msgDbg("X:%d, Y:%d, P:%d"
                  , psReadStatus->sPoint.wX
                  , psReadStatus->sPoint.wY
                  , psReadStatus->nAdcPress);*/
          }          
        }
      }
    }while(0);
    
  }

  if(rcSUCCESS != eResult)
  {
    ZERO_STRUCTURE(m_asAK4183Status[cInstance].sReadStatus);
  }
  else
  {
    psReadStatus->bEventOccurred = TRUE;

    if(NULL != psPoint)
    {
      *psPoint = psReadStatus->sPoint;
    }
  }

  return eResult;  
}

/**
 *  @brief      This API Set Device Status as specified
 *  @param      cInstance: Device instance
                sConfigNew: Device Status to set
 *  @return     rcSUCCESS if the operation was successful.
                rcERROR if communication to the hardware failed.
 *  @note       None
 *  @attention  None
 */
eRESULT dvAK4183ConfigSet(BYTE cInstance, AK4183_CONFIG sConfigNew)
{
  eRESULT eResult = rcSUCCESS;  

  ASSERT(cInstance < AK4183_NUM_INSTANCES);

  m_asAK4183Status[cInstance].sConfig = sConfigNew;
  
  return eResult;
}

/**
 *  @brief      This API return the Device Read Status
 *  @param      cInstance: Device instance
 *  @return     Device Status
 *  @note       None
 *  @attention  None
 */
AK4183_READ_STATUS dvAK4183ReadStatusGet(BYTE cInstance)
{
  ASSERT(cInstance < AK4183_NUM_INSTANCES);
  
  return m_asAK4183Status[cInstance].sReadStatus;
}

#endif // #if USE_TOUCH_AK4183

