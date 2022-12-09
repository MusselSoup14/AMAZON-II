/**
  ******************************************************************************
  * @file    dvDeviceBus.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    25-07-2018
  * @brief   This file provides the APIs to other components
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
#include "dvDeviceBusAPI.h"
#include "utilExecutiveAPI.h"
#include "dvGPIOAPI.h"

#define _MOD_NAME_ "\"TWI\""
// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
#define HW_I2C_FLAG_TIMEOUT         (7200*10) // About 10ms@AHB101MHz
#define HW_I2C_LONG_TIMEOUT         (TWI_RESP_TIME_OUT_COUNT) // About 100ms@AHB101MHz

// ------------------------------------------------------------------
// enum for TWI Bus type
// ------------------------------------------------------------------
typedef enum
{
  dbtSW,
  dbtHW,
  
  dbt_INVALID, // Sentinel
}eTWI_BUS_TYPE;

// ------------------------------------------------------------------
// Structure for software TWI Configuration
// ------------------------------------------------------------------
typedef struct tagTWISWConfig
{
  eGPIO_CHANNEL ePort;
  WORD          wPinSrcSCL;
  WORD          wPinSrcSDA;
}TWI_PINMUX, *PTWI_PINMUX;

typedef struct tagI2CINITSTRUCT
{
  DWORD dwSpeed;  
}I2C_INIT_STRUCT, *PI2C_INIT_STRUCT;
typedef CROMDATA I2C_INIT_STRUCT *PCI2C_INIT_STRUCT;

//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************
// DATA for TWI Bus Type. must be start HW to init HW I2C
static eTWI_BUS_TYPE m_aeTWIBusType[TWI_NUM_INSTANCES] =
{
  /* ----------------------- BEGIN CUSTOM AREA  -----------------------------*/    
  dbtHW, // cpuTWI_HW
  dbtSW, // cpuTWI_SW_0
  /* ------------------------ END CUSTOM AREA  ------------------------------*/  
};

// TWI Pinmux Configuration
static CROMDATA TWI_PINMUX m_asTWIPinMux[TWI_NUM_INSTANCES] =
{
  /* ----------------------- BEGIN CUSTOM AREA  -----------------------------*/    
  {GPIO_INVALID, 0, 0},                       // cpuTWI_HW
  {GPIO_8, GPIO_PinSource3, GPIO_PinSource5}, // cpuTWI_SW_0  
  /* ------------------------ END CUSTOM AREA  ------------------------------*/  
};

// SW Delay between Clock
#define TWISWDelay()			commDelayUS(1)

// SW Pin Control
#define TWISWSCLHigh(HI)   TWISWPinLowHigh(HI, TRUE);
#define TWISWSDAHigh(HI)   TWISWPinLowHigh(HI, FALSE);
#define TWISWSDAGet()      TWISWSDARead()

//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************
static TWI_PINMUX       m_sTWISWPinMux;

static I2C_INIT_STRUCT  m_asHWI2C_InitStructure[TWI_NUM_INSTANCES];
static WORD             m_awHWI2C_Flags[TWI_NUM_INSTANCES]          = {0,};
// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************

/**
 *  @brief      Initializes 2-wire hardware master structure
 *  @param[in]  eTWId : TWI identifier for hardware TWI
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
static void HardwareSturctureInit(PCI2C_INIT_STRUCT pcsI2CInitStruct)
{
  U64   tmp;
  DWORD dwTWIBR0_CNT, dwTWIBR1_CNT;
  
  ASSERT(NULL != pcsI2CInitStruct && 0 != pcsI2CInitStruct->dwSpeed);

  /* Clock Settings */
  tmp = (U64)get_apb_clock()*700/1000000000;
  dwTWIBR0_CNT = tmp + 3;
  dwTWIBR1_CNT = (get_apb_clock()/(2*pcsI2CInitStruct->dwSpeed)) 
                  - ((dwTWIBR0_CNT+7) >> 1);

  *R_TWIBR0 = dwTWIBR0_CNT;
	*R_TWIBR1 = dwTWIBR1_CNT;
  *R_TWICTRL = F_TWICTRL_TWIEN;
}

/**
 *  @brief      Initializes 2-wire hardware master.
 *  @param[in]  eTWId : TWI identifier for hardware TWI
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
static void HardwareMasterInit(eCPU_TWI eTWId)
{
  ASSERT(TWI_NUM_INSTANCES > eTWId);

  /* AF Setting */
  *R_PAF5 &= ~(AF5_4_GP4 | AF5_5_GP5);
	*R_PAF5 |= (AF5_4_TWI_SCL | AF5_5_TWI_SDA); 
}

/**
 *  @brief      Initializes 2-wire hardware master.
 *  @param[in]  eTWId : TWI identifier for hardware TWI
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
static void HardwareMasterConfig(eCPU_TWI eTWId, WORD wFlags)
{  
  ASSERT(TWI_NUM_INSTANCES > eTWId);
  
  /* I2C configuration */  
  if(wFlags & DEVICE_BUS_FAST_MODE)    
    m_asHWI2C_InitStructure[eTWId].dwSpeed = 400000; // 400Khz to fast mode
  else if(wFlags & DEVICE_BUS_SLOW_MODE)
    m_asHWI2C_InitStructure[eTWId].dwSpeed = 5000; // 5Khz to slow mode
  else 
    m_asHWI2C_InitStructure[eTWId].dwSpeed = 100000; // 100Khz to standard mode

  m_awHWI2C_Flags[eTWId] = wFlags;

  //twi_set_freq(m_asHWI2C_InitStructure[eTWId].dwSpeed);
  HardwareSturctureInit(&m_asHWI2C_InitStructure[eTWId]);
}

/**
 *  @brief      Generates I2Cx communication START condition.
 *  @param      bMasterTrasmitMode: TRUE to Master Transmit Mode, FALSE to Master Receive Mode
                bRestartCondition: TRUE to Restart Condition
 *  @return     None.
 *  @note       None
 *  @attention  None  
 */
static void I2C_GenerateSTART(BOOL bMasterTrasmitMode, BOOL bRestartCondition)
{
  BYTE cCtrl = F_TWICTRL_TWIEN | F_TWICTRL_TWIMOD_MSTART;

  if(bMasterTrasmitMode) cCtrl |= F_TWICTRL_TWITR_MTRM;

  if(bRestartCondition) cCtrl |= F_TWICTRL_REPST_RSTART;

  *R_TWICTRL = cCtrl;
}

/**
 *  @brief      Generates I2Cx communication STOP condition.
 *  @param      None
 *  @return     None.
 *  @note       None
 *  @attention  None  
 */
static void I2C_GenerateSTOP(void)
{
  //*R_TWICTRL = 0; /* Stop */
  *R_TWICTRL = F_TWICTRL_TWIEN;
}

/**
 *  @brief      Transmits the address byte to select the slave device.
 *  @param      cAddress: specifies the slave address which will be transmitted
                bWriteMode: TRUE I2C device will be a write mode, false to read mode
 *  @return     None.
 *  @note       None
 *  @attention  None  
 */
static void I2C_Send7bitAddress(BYTE cAddress, BOOL bWriteMode)
{
  cAddress = (bWriteMode) ? (cAddress & 0xFE) : (cAddress | 0x01);
  
  *R_TWIDATA = cAddress;
}

/**
 *  @brief      Checks whether the specified I2C flag is set or not.
 *  @param      wI2CFlag:  specifies the event to be checked.
 *              This parameter can be any combination of the following values: 
                  @arg F_TWISTAT_TXEMPTY: TX Buffer Empty
                  @arg F_TWISTAT_RXFULL: RX Buffer Full
                  @arg F_TWISTAT_TWIDT_FIN: 1Byte Data Transfer Complete
                  @arg F_TWISTAT_TWIAS_MATCH: Slave Address Match
                  @arg F_TWISTAT_TWIBUSY: Bus Busy
                  @arg F_TWISTAT_TWILOST: Lost Arbitration
                  @arg F_TWISTAT_TWISRW_RCV: Slave Receive Mode
                  @arg F_TWISTAT_TWISRW_TRM: Slave Transmit Mode
                  @arg F_TWISTAT_RSF: Repeated Start Condition
                  @arg F_TWISTAT_TWIRXAK_NAK: Received NACK
                  @arg F_TWISTAT_TWIRXAK_AK: Received ACK
 *  @return     None
 *  @note       None
 *  @attention  None  
 */
static BOOL I2C_GetFlagStatus(WORD wI2CFlag)
{
  BOOL bSuccess = FALSE;
  
  WORD wStatus;
	register volatile U32* pStatus asm("%r14") = R_TWISTAT;

  do
  {
    wStatus = *pStatus & 0x3FF;

    /* Do Not Arbitration Check */   

    /* Compare Flags */
    if((wStatus & wI2CFlag) == wI2CFlag)
			bSuccess = TRUE;
    
  }while(0);  

  return bSuccess;
}

/**
 *  @brief      Sends a data byte through the I2Cx peripheral.
 *  @param      cData: Byte to be transmitted..
 *  @return     None.
 *  @note       None
 *  @attention  None  
 */
static void I2C_SendData(BYTE cData)
{
  *R_TWIDATA = cData;
}

/**
 *  @brief      Returns the most recent received data by the I2Cx peripheral.
 *  @param      None
 *  @return     The value of the received data.
 *  @note       None
 *  @attention  None  
 */
static BYTE I2C_ReceiveData(void)
{
  return *R_TWIDATA;
}

/**
 *  @brief      Enables or disables the specified I2C acknowledge feature.
 *  @param[in]  bNackEnable : TRUE to state of the I2C Acknowledgement to NACK.
                              FALSE to ACK
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
static void I2C_AcknowledgeConfig(BOOL bNackEnable) 
{
  //BYTE cCtrl = F_TWICTRL_TWIEN | F_TWICTRL_TWIMOD_MSTART;
  BYTE cCtrl = *R_TWICTRL;

  if(bNackEnable) cCtrl |= F_TWICTRL_TWIAK_NACK;
  else cCtrl &= ~(F_TWICTRL_TWIAK_NACK);

  *R_TWICTRL = cCtrl;
}

/**
 *  @brief      Wait Transfer
 *  @param[in]  dwWaitTime : TWI Wait time to Transfer
 *  @param[out] None
 *  @return     Error Number (0: None, 1: Lost Arbitration, 0xFF: Time out)
 *  @note       None
 *  @attention  None
 */ 
static BYTE I2C_WaitTransfer(DWORD dwWaitTime)
{
  DWORD dwStatus;
	register volatile U32*  pStatus asm("%r14") = R_TWISTAT;
  BOOL                    bLostArbitration = FALSE;
  BYTE cErrorNum = 0;
	
	while(dwWaitTime--)
	{
		dwStatus = *pStatus;

    /* 1 Byte Transfer Complete */
		if((dwStatus & F_TWISTAT_TWIDT_FIN) == F_TWISTAT_TWIDT_FIN)
			return 0;

    /* Arbitration Check */
		if((dwStatus & F_TWISTAT_TWILOST) == F_TWISTAT_TWILOST)
		{
			///*R_TWICTRL = 0;
			*R_TWISTAT |= F_TWISTAT_TWILOST;
      bLostArbitration = TRUE;

			break;
		}		
	}	  

  if(bLostArbitration)
    cErrorNum = 1;
  else
    cErrorNum = 0xFF;

  /* Stop */
	///*R_TWICTRL = 0;
  
	return cErrorNum;
}

/**
 *  @brief      Wait BUSY Clear
 *  @param[in]  dwWaitTime : TWI Wait time to BUSY Clear
 *  @param[out] None
 *  @return     Error Number (0: None, 1: Lost Arbitration, 0xFF: Time out)
 *  @note       None
 *  @attention  None
 */ 
static BYTE I2C_WaitBusy(DWORD dwWaitTime)
{
  DWORD                   dwStatus;	
	register volatile U32*  pStatus asm("%r14") = R_TWISTAT;
  BOOL                    bLostArbitration = FALSE;
  BYTE cErrorNum = 0;
	
  while(dwWaitTime--)
	{
		dwStatus = *pStatus ;
    
		if((dwStatus & F_TWISTAT_TWIBUSY) == 0)
			return 0;

    /* Arbitration Check */
		if((dwStatus & F_TWISTAT_TWILOST) == F_TWISTAT_TWILOST)
		{
			///*R_TWICTRL = 0;
			*R_TWISTAT |= F_TWISTAT_TWILOST;
      bLostArbitration = TRUE;
      
			break;
		}
	}    

  if(bLostArbitration)
    cErrorNum = 1;
  else
    cErrorNum = 0xFF;
  
  /* Clear BUSY status */
	*R_TWISTAT &= ~(F_TWISTAT_TWIBUSY);
	
	/* Stop */
	///*R_TWICTRL = 0;
	
	return cErrorNum;
}

/**
 *  @brief      Performs a block 2-wire read transaction using HW.
 *  @param[in]  cDeviceAddress   : Device address to read
 *  @param[in]  wRegisterAddress : Internal device register address to read
 *  @param[in]  wCount           : Number of bytes to read from the device
 *  @param[out]  pacBuffer        : Buffer to store data read
 *  @param[in]  wFlags           : Flags for the 2 wire transaction (see Definitions) 
 *  @return     rcSUCCESS if read operation succeeds, rcERROR otherwise
 *  @note       None
 *  @attention  None
 */ 
static eRESULT HardwareMasterRead( WORD wDeviceAddress,
                                              WORD wRegisterAddress,
                                              WORD wCount,
                                              PBYTE pacBuffer,
                                              WORD wFlags)
{
  eRESULT eResult = rcERROR; 
  WORD wI = 0;
  WORD wCountTmp;
  BYTE acBuffer[2];
  DWORD dwTimeOut = HW_I2C_LONG_TIMEOUT;
  BYTE cErrorNum;

  /* Check Bus Busy */
  cErrorNum = I2C_WaitBusy(HW_I2C_LONG_TIMEOUT);
  if(cErrorNum)
  {
    #if USE_DEBUG_TWI
    msgErr("[0x%X] WAITBUSY(%s)", wDeviceAddress, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
    #endif
    goto HW_STOP_CONDITION;
  }

  if(wFlags & DEVICE_BUS_ADDR_TRANSACTION)
  {
    /* Send Device address for write */
    I2C_Send7bitAddress((BYTE)(wDeviceAddress << 1), TRUE);

    /* SLA+W Condition */
    I2C_GenerateSTART(TRUE, FALSE);    

    /* Transfer Compelete Wait */
    cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
    if(cErrorNum)
    {
      #if USE_DEBUG_TWI
      msgErr("[0x%X] SLA+W(%s)", wDeviceAddress, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
      #endif    
      goto HW_STOP_CONDITION;
    }

    ///msgDbg("SLW+W");

    /* SUBADDR Write */
    if(wFlags & DEVICE_BUS_WORD_ADDR)
    {
      wCountTmp = 2;
      acBuffer[0] = (unsigned char)((wRegisterAddress >> 8) & 0xFF);
      acBuffer[1] = (unsigned char)(wRegisterAddress & 0xFF) ;

      /* Send the register address to read from: MSB of the address first */
      I2C_SendData(acBuffer[0]);

      /* Transfer Compelete Wait */
      cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
      if(cErrorNum)
      {
        #if USE_DEBUG_TWI
        msgErr("[0x%X] SUBADDR_MSB(%s)", wDeviceAddress, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
        #endif
        goto HW_STOP_CONDITION;
      }

      /* Send the register address to read from: MSB of the address first */
      I2C_SendData(acBuffer[1]);

      /* Transfer Compelete Wait */
      cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
      if(cErrorNum)
      {
        #if USE_DEBUG_TWI
        msgErr("[0x%X] SUBADDR_LSB(%s)", wDeviceAddress, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
        #endif
        goto HW_STOP_CONDITION;
      }

      ///msgDbg("DEVICE_BUS_WORD_ADDR");
    }
    else
    {
      wCountTmp = 1;
      acBuffer[0] = (unsigned char)(wRegisterAddress & 0xFF);

      /* Send the register address to read from: MSB of the address first */
      I2C_SendData(acBuffer[0]);

      /* Transfer Compelete Wait */
      cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
      if(cErrorNum)
      {
        #if USE_DEBUG_TWI
        msgErr("[0x%X] SUBADDR(%s)", wDeviceAddress, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
        #endif
        goto HW_STOP_CONDITION;    
      }

      ///msgDbg("DEVICE_BUS_BYTE_ADDR");
    }

    wCountTmp = wCount;

    /* RESTART Condition */
    if(wFlags & DEVICE_BUS_SEND_REPEAT_AFTER_ADDR)
    {
      BYTE cDummyByte;
      
      /* Send RESTRAT condition a second time */  
      I2C_GenerateSTART(FALSE, TRUE);

      dwTimeOut = HW_I2C_FLAG_TIMEOUT;
      while(I2C_GetFlagStatus(F_TWISTAT_RSF))
      {
        if((dwTimeOut--) == 0) 
        {
          #if USE_DEBUG_TWI
          msgErr("[0x%X] RESTART(Timeout)", wDeviceAddress);
          #endif
          goto HW_STOP_CONDITION;
        }
      }

      /* Send Device Address for read */
      I2C_Send7bitAddress((BYTE)(wDeviceAddress << 1), FALSE); 

      /* Transfer Compelete Wait */
      cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
      if(cErrorNum)
      {
        #if USE_DEBUG_TWI
        msgErr("[0x%X] RE:SLA+R(%s)", wDeviceAddress, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
        #endif
        goto HW_STOP_CONDITION;
      }

      /* Dummy Byte Read to SCL Generate.
         It make one more data read but reading value is OK.
         Without below, reading data always 0 on one byte read
        */
      cDummyByte = I2C_ReceiveData();
      /* Transfer Compelete Wait */
      cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
      if(cErrorNum)
      {
        #if USE_DEBUG_TWI
        msgErr("[0x%X] RE:DUMMYR(%s)", wDeviceAddress, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
        #endif
        goto HW_STOP_CONDITION;
      }

      ///msgDbg("DEVICE_BUS_SEND_REPEAT_AFTER_ADDR");
    }

    /* BYTEDATA Read */
    if(wFlags & DEVICE_BUS_WORD_DATA
      && !(wCountTmp % 2))
    {
      // TODO: I2C HW  "DEVICE_BUS_WORD_DATA"
    }
    else
    {
      //BYTE cDummyByte;
      
      for(wI = 0; wI < wCountTmp; wI++)
      {      
        if(wI == wCountTmp - 1)
          I2C_AcknowledgeConfig(TRUE);  /* Disable Acknowledgement */

        /* Transfer Compelete Wait */ 
        cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
        if(cErrorNum)
        {
          #if USE_DEBUG_TWI
          msgErr("[0x%X] DATA[%d](%s)", wDeviceAddress, wI, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
          #endif
          wI = 0;
          goto HW_STOP_CONDITION;   
        }
        
        /* Read the byte received from Slave */
        acBuffer[0] = I2C_ReceiveData();
        
        /* Copy to Buffer */
        *(pacBuffer++) = acBuffer[0];

        //msgDbg("[%d] DATA: 0x%.2X", wI, acBuffer[0]);        
      }    

      /* Dummy Byte Read to SCL Generate */
      //cDummyByte = I2C_ReceiveData();

      goto HW_I2C_END;
    }
  }
  else
  {    
    /* Send Device address for read */
    I2C_Send7bitAddress((BYTE)(wDeviceAddress << 1), FALSE);
   
    /* SLA+W Condition */
    I2C_GenerateSTART(FALSE, FALSE);

    /* Transfer Compelete Wait */
    cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
    if(cErrorNum)
    {
      #if USE_DEBUG_TWI
      msgErr("[0x%X] SLA+R(%s)", wDeviceAddress, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
      #endif
      goto HW_STOP_CONDITION;
    }

    /* Dummy Byte Read to SCL Generate */
    I2C_ReceiveData();
    /* Transfer Compelete Wait */
    cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
    if(cErrorNum)
    {
      #if USE_DEBUG_TWI
      msgErr("[0x%X] DUMMYR(%s)", wDeviceAddress, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
      #endif
      goto HW_STOP_CONDITION;
    }

    wCountTmp = wCount;

    for(wI = 0; wI < wCountTmp; wI++)
    {        
      if(wI == wCountTmp - 1)
        I2C_AcknowledgeConfig(TRUE);  /* Disable Acknowledgement */ 

      /* Transfer Compelete Wait */   
      cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
      if(cErrorNum)
      {
        #if USE_DEBUG_TWI
        msgErr("[0x%X] DATA[%d](%s)", wDeviceAddress, wI, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
        #endif
        wI = 0;
        goto HW_STOP_CONDITION;
      }
      
      /* Read the byte received from the Slave */
      acBuffer[0] = I2C_ReceiveData(); // coundn't generate clock ?
        
      /* Copy to Buffer */
      *(pacBuffer++) = acBuffer[0];
    }                

    goto HW_I2C_END;
  }

HW_STOP_CONDITION:
  
  /* Send STOP Condition */
  I2C_GenerateSTOP();

HW_I2C_END:  
  
  if(wI > 0) eResult = rcSUCCESS;

  return eResult;
}

/**
 *  @brief     Performs a block 2-wire write transaction using HW.
 *  @param[in]  wDeviceAddress   : Device address to read
 *  @param[in]  wRegisterAddress : Internal device register address to read
 *  @param[in]  wCount           : Number of bytes to read from the device
 *  @param[in]  pacBuffer        :Buffer to stored data write
 *  @param[in]  wFlags           : Flags for the 2 wire transaction (see Definitions) 
 *  @return     rcSUCCESS if read operation succeeds, rcERROR otherwise
 *  @note       None
 *  @attention  None
 */ 
static eRESULT HardwareMasterWrite(WORD  wDeviceAddress,
                                             WORD wRegisterAddress,
                                             WORD wCount,
                                             PCBYTE pacBuffer,
                                             WORD wFlags)
{
  eRESULT eResult = rcERROR;
  WORD wI = 0;
  WORD wCountTmp;
  BYTE acBuffer[2];
  ///DWORD dwTimeOut = HW_I2C_LONG_TIMEOUT;
  BYTE cErrorNum;

  /* Check Bus Busy */
  cErrorNum = I2C_WaitBusy(HW_I2C_LONG_TIMEOUT);
  if(cErrorNum)
  {
    #if USE_DEBUG_TWI
    msgErr("[0x%X] WAITBUSY(%s)", wDeviceAddress, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
    #endif
    goto HW_STOP_CONDITION;
  }  

  if(wFlags & DEVICE_BUS_ADDR_TRANSACTION)
  {
    /* Send Device address for write */
    I2C_Send7bitAddress((BYTE)(wDeviceAddress << 1), TRUE);
    
    /* SLA+W Condition */
    I2C_GenerateSTART(TRUE, FALSE);

    /* Transfer Compelete Wait */
    cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
    if(cErrorNum)
    {
      #if USE_DEBUG_TWI
      msgErr("[0x%X] SLA+W(%s)", wDeviceAddress, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
      #endif
      goto HW_STOP_CONDITION;
    }

    /* SUBADDR Write */
    if(wFlags & DEVICE_BUS_WORD_ADDR)
    {
      wCountTmp = 2;
      acBuffer[0] = (unsigned char)((wRegisterAddress >> 8) & 0xFF);
      acBuffer[1] = (unsigned char)(wRegisterAddress & 0xFF) ;

      /* Send the register address to read from: MSB of the address first */
      I2C_SendData(acBuffer[0]);

      /* Transfer Compelete Wait */
      cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
      if(cErrorNum)
      {
        #if USE_DEBUG_TWI
        msgErr("[0x%X] SUBADDR_MSB(%s)", wDeviceAddress, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
        #endif
        goto HW_STOP_CONDITION;     
      }

      /* Send the register address to read from: MSB of the address first */
      I2C_SendData(acBuffer[1]);

      /* Transfer Compelete Wait */
      cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
      if(cErrorNum)
      {
        #if USE_DEBUG_TWI
        msgErr("[0x%X] SUBADDR_MSB(%s)", wDeviceAddress, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
        #endif
        goto HW_STOP_CONDITION; 
      }
    }
    else
    {
      wCountTmp = 1;
      acBuffer[0] = (unsigned char)(wRegisterAddress & 0xFF);

      /* Send the register address to read from: LSB of the address first */
      I2C_SendData(acBuffer[0]);

      /* Transfer Compelete Wait */
      cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
      if(cErrorNum)
      {
        #if USE_DEBUG_TWI
        msgErr("[0x%X] SUBADDR(0x%.2X:%s)", wDeviceAddress, acBuffer[0], (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
        #endif
        goto HW_STOP_CONDITION;    
      }
    }

    wCountTmp = wCount;

    /* RESTART Condition */
    if(wFlags & DEVICE_BUS_SEND_REPEAT_AFTER_ADDR)
    { 
      /* Send STRAT condition a second time */  
      I2C_GenerateSTART(TRUE, TRUE);

      /* Send Device Address for write */
      I2C_Send7bitAddress((BYTE)(wDeviceAddress << 1), TRUE);

      /* Transfer Compelete Wait */
      cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
      if(cErrorNum)
      {
        #if USE_DEBUG_TWI
        msgErr("[0x%X] RE:SLA+W(%s)", wDeviceAddress, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
        #endif
        goto HW_STOP_CONDITION;    
      }
    }

    /* BYTEDATA Write */
    if(wFlags & DEVICE_BUS_WORD_DATA
      && !(wCountTmp % 2))
    {   
      // TODO: I2C HW  "DEVICE_BUS_WORD_DATA"
    }
    else
    {
      if(wCountTmp)
      {
        for(wI = 0; wI < wCountTmp; wI++)
        {        
          /* Write the byte */
          acBuffer[0] = *(pacBuffer++);
          I2C_SendData(acBuffer[0]);
          
          /* Transfer Compelete Wait */
          cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
          if(cErrorNum)
          {
            #if USE_DEBUG_TWI
            msgErr("[0x%X] DATA[%d](%s)", wDeviceAddress, wI, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
            #endif
            wI = 0;
            goto HW_STOP_CONDITION;         
          }
        }
      }
      else
      {
        /* To case of "DEVICE_BUS_ADDR_TRANSACTION" but NO DATA */
        eResult = rcSUCCESS;
      }

      //goto HW_I2C_END;
    }    
  }
  else
  {
    wCountTmp = wCount;

    /* Send Device address for write */
    I2C_Send7bitAddress((BYTE)(wDeviceAddress << 1), TRUE);

    /* Generate START condition */
    I2C_GenerateSTART(TRUE, FALSE);

    /* Transfer Compelete Wait */
    cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
    if(cErrorNum)
    {
      #if USE_DEBUG_TWI
      msgErr("[0x%X] SLA+R(%s)", wDeviceAddress, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
      #endif
      goto HW_STOP_CONDITION; 
    }
    
    for(wI = 0; wI < wCountTmp; wI++)
    {
      /* Write the byte */
      acBuffer[0] = *(pacBuffer++);
      I2C_SendData(acBuffer[0]);
      
      /* Transfer Compelete Wait */
      cErrorNum = I2C_WaitTransfer(HW_I2C_FLAG_TIMEOUT);
      if(cErrorNum)
      {
        #if USE_DEBUG_TWI
        msgErr("[0x%X] DATA[%d](%s)", wDeviceAddress, wI, (cErrorNum == 1) ? "ARBIT" : "TIMEOUT");
        #endif
        wI = 0;
        goto HW_STOP_CONDITION; 
      }
    }    

    //goto HW_I2C_END;
  }

HW_STOP_CONDITION:
  /* Send STOP Condition */
  I2C_GenerateSTOP();

//HW_I2C_END:  
  if(wI > 0) eResult = rcSUCCESS;
  
  return eResult;
}

/**
 *  @brief      Initializes 2-wire spftware master.
 *  @param[in]   eTWId : TWI identifier for software TWI
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
static void SoftwareMasterInit(eCPU_TWI eTWId)
{ 
  VUINT         vunAF      = 0;
  eGPIO_CHANNEL ePort     = m_asTWIPinMux[eTWId].ePort;
  WORD          dwMaskSCL = (DWORD)1 << m_asTWIPinMux[eTWId].wPinSrcSCL;
  WORD          dwMaskSDA = (DWORD)1 << m_asTWIPinMux[eTWId].wPinSrcSDA;
  
  ASSERT(TWI_NUM_INSTANCES > eTWId
        && dbtSW == m_aeTWIBusType[eTWId]
        && GPIO_INVALID > ePort);

  if(ePort < GPIO_13)
  {
    /* Port Alternate Function Register Set	*/  
    vunAF = (VUINT)*R_PAF(ePort);
    // No need to clear when AF is GP
    vunAF |= ((0x03 << (m_asTWIPinMux[eTWId].wPinSrcSCL << 1)) 
              | (0x03 << (m_asTWIPinMux[eTWId].wPinSrcSDA << 1)));
    *R_PAF(ePort) = vunAF;    
  }
  
  /* Port to Open Drain */
#if 0
  *R_GPPUDIS(ePort) = (dwMaskSCL | dwMaskSDA);  
  *R_GPODM(ePort) = (dwMaskSCL | dwMaskSDA);
#else  
  *R_GPPUEN(ePort) = (dwMaskSCL | dwMaskSDA); // Internal Pull-Up needed even if has external Pull-Up?
#endif  

  /* Port Direction Output Mode */ 
  *R_GPODIR(ePort) = (dwMaskSCL | dwMaskSDA);

  /* Port to High (Bus Release) */
  *R_GPOHIGH(ePort) = (dwMaskSCL | dwMaskSDA);
}

/**
 *  @brief      Asign 2-wire software master GPIO port and pin
 *  @param[in]  eTWId : TWI identifier for software TWI
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
static void TWISWPortSelect(eCPU_TWI eTWId)
{
  ASSERT(TWI_NUM_INSTANCES > eTWId);
  ASSERT(dbtSW == m_aeTWIBusType[eTWId]);

  m_sTWISWPinMux = m_asTWIPinMux[eTWId];
}

/**
 *  @brief      Change 2-wire software master SDA mode
 *  @param[in]  bInput : TRUE for input mode, FALSE for output mode
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
static void TWISWSDAOut(BOOL bOutput)
{
  eGPIO_CHANNEL ePort     = m_sTWISWPinMux.ePort;
  DWORD         dwMaskSDA = 1 << m_sTWISWPinMux.wPinSrcSDA;

  if(bOutput)
    *R_GPODIR(ePort) = dwMaskSDA;
  else
    *R_GPIDIR(ePort) = dwMaskSDA;
  
  ///msgDbg("0x%X", *R_GPDIR(ePort));
}

/**
 *  @brief      Change 2-wire software master SDA mode
 *  @param[in]  bInput : TRUE for input mode, FALSE for output mode
 *  @param[out] None
 *  @return     SDA read Data
 *  @note       None
 *  @attention  None
 */ 
static BYTE TWISWSDARead(void)
{
  DWORD         dwData    = 0;  

  dwData = *R_GPILEV(m_sTWISWPinMux.ePort);
  dwData &= (1 << m_sTWISWPinMux.wPinSrcSDA);
  dwData >>= (m_sTWISWPinMux.wPinSrcSDA);

  return (BYTE)dwData;
}

/**
 *  @brief      Change 2-wire software master SCL/SDA Pin Status (H/L)
 *  @param[in]  bHigh : TRUE for HIGH, FALSE to LOW
                bPinSCL: TRUE to SCL Pin, FALSE to SDA Pin
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
static void TWISWPinLowHigh(BOOL bHigh, BOOL bPinSCL)
{
  eGPIO_CHANNEL ePort             = m_sTWISWPinMux.ePort;
  DWORD         dwMask            = (bPinSCL) ? (1 << m_sTWISWPinMux.wPinSrcSCL) : (1 << m_sTWISWPinMux.wPinSrcSDA);

  if(bHigh) *R_GPOHIGH(ePort) = dwMask;
  else  *R_GPOLOW(ePort) = dwMask;
}

/**
 *  @brief      Generate 2-wire software START Condition
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
static void TWISWStart(void)
{
  TWISWDelay();

  TWISWSDAOut(OUTPUT);		// SDA GPIO as Output	
  TWISWDelay();

  TWISWSDAHigh(HIGH);
  TWISWDelay();

  TWISWSCLHigh(HIGH);
  TWISWDelay();

  TWISWSDAHigh(LOW);
  TWISWDelay();

  TWISWSCLHigh(LOW);
  TWISWDelay();
}

/**
 *  @brief      Generate 2-wire software STOP Condition
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
static void TWISWStop(void)
{
  TWISWDelay();

  TWISWSDAOut(OUTPUT);		// SDA GPIO as Output
  TWISWDelay();

  TWISWSDAHigh(LOW);
  TWISWDelay();

  TWISWSCLHigh(HIGH);
  TWISWDelay();

  TWISWSDAHigh(HIGH);
  TWISWDelay();
}

/**
 *  @brief      Receive 2-wire software ACK Condition
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if ACK Received, FALSE if NACK received
 *  @note       None
 *  @attention  None
 */ 
static BOOL TWISWAckRx(void)
{
  register BYTE _t1_;
  BOOL bACK = 1;

  TWISWSDAOut(INPUT);   // SDA GPIO as Input

  TWISWDelay();
  TWISWSCLHigh(HIGH);
  TWISWDelay();
  TWISWDelay();
  for(_t1_=0;_t1_<10;_t1_++);
  TWISWDelay();
  TWISWDelay();  
  bACK = (TWISWSDAGet() > 0) ? 0/*NACK*/ : 1/*ACK*/;
  TWISWSCLHigh(LOW);
  TWISWDelay(); 
//  Delay(1);

  return bACK;
}

/**
 *  @brief      Send 2-wire software ACK Condition
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
void TWISWAckTx(void)
{
  register BYTE _t1_;
  
  TWISWSDAOut(OUTPUT);    // SDA GPIO as Output
//  I2C_Delay(); //
//  I2C_Delay(); //
  TWISWSDAHigh(LOW);
  TWISWDelay();//
  TWISWDelay();//    
  TWISWSCLHigh(HIGH);
  TWISWDelay();
  TWISWDelay();
  for(_t1_=0;_t1_<10;_t1_++);
  TWISWDelay();
  TWISWDelay();
  TWISWSCLHigh(LOW);
  TWISWDelay();
  
//  Delay(1);
}


/**
 *  @brief      Send 2-wire software NACK Condition
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
void TWISWNackTx(void)
{
  register BYTE _t1_;
  
  TWISWSDAOut(OUTPUT);    // SDA GPIO as Output 
  //  I2C_Delay();
  //  I2C_Delay();  //
  TWISWSDAHigh(HIGH);
  TWISWDelay();//
  TWISWDelay();//    
  TWISWSCLHigh(HIGH);
  TWISWDelay();
  TWISWDelay();
  for(_t1_=0;_t1_<10;_t1_++);
  TWISWDelay();
  TWISWDelay();
  TWISWSCLHigh(LOW);
  TWISWDelay();
  
//  Delay(1);
}


/**
 *  @brief      Send Data to slave via 2-wire software
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
static void TWISWDataTx(BYTE cData8bits)
{
  BYTE cI;

  TWISWDelay();

  // Data Write Mode
  TWISWSDAOut(OUTPUT);		// SDA GPIO as Output
  TWISWDelay();

  // Data Write Start
  for( cI = 0; cI < 8; cI++)
  {
    TWISWDelay();
    
    if (cData8bits & (0x80))
    {
      TWISWSDAHigh(HIGH);
    }
    else
    {
      TWISWSDAHigh(LOW);
    }
    
    TWISWDelay();
    //		for(_t1_=0;_t1_<3;_t1_++);
    TWISWSCLHigh(HIGH);
    TWISWDelay();
    cData8bits <<= 1;
    TWISWDelay();
    TWISWSCLHigh(LOW);
    //	delay(1);
  }
}

/**
 *  @brief      Receive Data from slave via 2-wire software
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
static BYTE TWISWDataRx(void)
{
  BYTE cData8bits = 0;
  BYTE cI;
  	
  TWISWDelay();
  TWISWSDAOut(INPUT);		// SDA GPIO as Input
  TWISWDelay();
  for(cI = 1; cI <= 8; cI++)
  {
  //	delay(1);
  //		for(_t1_=0;_t1_<10;_t1_++);
  	TWISWDelay();
  	TWISWSCLHigh(HIGH);;
  	TWISWDelay();
        if(TWISWSDAGet()) cData8bits |= 0x01;
  	TWISWDelay();
  	TWISWSCLHigh(LOW);
  	//	delay(1);
  	
  	if(cI == 8) continue;	
    
  	cData8bits<<=1;
  	//	delay(1);
  }
  return cData8bits;

}

/**
 *  @brief      Performs a block 2-wire read transaction using HW.
 *  @param[in]  wDeviceAddress   : Device address to read
                wRegisterAddress : Internal device register address to read
                wCount           : Number of bytes to read from the device
                wFlags           : Flags for the 2 wire transaction (see Definitions) 
 *  @param[out] pacBuffer        : Buffer to store data read
 *  @return     rcSUCCESS if read operation succeeds, rcERROR otherwise
 *  @note       None
 *  @attention  None
 */ 
static eRESULT SoftwareMasterRead( WORD              wDeviceAddress,
                                  WORD              wRegisterAddress,
                                  WORD              wCount,
                                  PBYTE             pacBuffer,
                                  WORD              wFlags)
{
  eRESULT eResult = rcERROR;  
  WORD wI = 0;
  WORD wCountTmp;
  BYTE acBuffer[2];

  if(wFlags & DEVICE_BUS_ADDR_TRANSACTION)
  {
    /* SLA+W Condition */
    TWISWStart();
    TWISWDataTx((wDeviceAddress << 1) & 0xFE); // W
    if(!TWISWAckRx())
    {
      wI = 0;
#if USE_DEBUG_TWI      
      msgErr("I2C 0x%.2X READ: NACK AFTER SLA+W", wDeviceAddress);
#endif        
      
      goto STOP_CONDITION;
    }
    
    if(wFlags & DEVICE_BUS_WORD_ADDR)
    {
      wCountTmp = 2;
      acBuffer[0] = (unsigned char)((wRegisterAddress >> 8) & 0xFF);
      acBuffer[1] = (unsigned char)(wRegisterAddress & 0xFF) ;
    }
    else
    {
      wCountTmp = 1;
      acBuffer[0] = (unsigned char)(wRegisterAddress & 0xFF);
    }

    /* SUBADDR Write */
    for(wI = 0; wI < wCountTmp; wI++)
    {
      TWISWDataTx(acBuffer[wI]);
      if(!TWISWAckRx())
      {
        wI =0;
#if USE_DEBUG_TWI        
        msgErr("I2C 0x%.2X READ: NACK AFTER SUBADDR", wDeviceAddress);
#endif
        goto STOP_CONDITION;
      }
    }

    wCountTmp = wCount;

    /* RESTART Condition */
    if(wFlags & DEVICE_BUS_SEND_REPEAT_AFTER_ADDR)
    {
      TWISWStart();
      TWISWDataTx((wDeviceAddress << 1) | 0x01); // R
      if(!TWISWAckRx())
      {
        wI = 0;
#if USE_DEBUG_TWI        
        msgErr("I2C 0x%.2X READ: NACK AFTER RESTART", wDeviceAddress);
#endif
        goto STOP_CONDITION;
      }
    }

    /* BYTEDATA Read */
    if(wFlags & DEVICE_BUS_WORD_DATA
      && !(wCountTmp % 2))
    {
      for(wI = 0; wI < wCountTmp; wI+=2)
      {
        acBuffer[0] = TWISWDataRx();
        //msgDbg("##### [K] I2CRead DATA%d: 0x%.2X\n",wI, acBuffer[0]);
        TWISWAckTx();        
        acBuffer[1] = TWISWDataRx();
        //msgDbg("##### [K] I2CRead DATA%d: 0x%.2X\n",wI+1, acBuffer[1]);
        if(wI+1 < wCountTmp-1) TWISWAckTx();
        else TWISWNackTx();
        *(pacBuffer++) = acBuffer[1];
        *(pacBuffer++) = acBuffer[0];        
      }
    }
    else
    {
      for(wI = 0; wI < wCountTmp; wI++)
      {
        acBuffer[0] = TWISWDataRx();
        *(pacBuffer++) = acBuffer[0];
        //msgDbg("##### [K] I2CRead DATA%d: 0x%X\n",wI, acBuffer[0]);
        if(wI < wCountTmp -1)
          TWISWAckTx();
        else
          TWISWNackTx();
      }                
    }
  }
  else
  {
    TWISWStart();
    TWISWDataTx((wDeviceAddress << 1) | 0x1); // R
    if(!TWISWAckRx())
    {
      wI = 0;
#if USE_DEBUG_TWI      
      msgErr("I2C 0x%.2X READ: NACK AFTER SLA+R", wDeviceAddress);
#endif
      goto STOP_CONDITION;
    }

    wCountTmp = wCount;

    for(wI = 0; wI < wCountTmp; wI++)
    {
      acBuffer[0] = TWISWDataRx();
      *(pacBuffer++) = acBuffer[0];
      //msgDbg("##### [K] I2CRead DATA%d: 0x%.2X\n",wI, acBuffer[0]);
      if(wI < (wCountTmp - 1))
        TWISWAckTx();
      else
        TWISWNackTx();    
    }
  }

STOP_CONDITION:
  /* STOP Condition */
  TWISWStop();
  
  if(wI > 0) eResult = rcSUCCESS;

  return eResult;
}


/**
 *  @brief      Performs a block 2-wire write transaction using HW.
 *  @param[in]  wDeviceAddress   : Device address to read
                wRegisterAddress : Internal device register address to read
                wCount           : Number of bytes to read from the device
                pacBuffer        :Buffer to stored data write
                wFlags           : Flags for the 2 wire transaction (see Definitions) 
 *  @return     rcSUCCESS if read operation succeeds, rcERROR otherwise
 *  @note       None
 *  @attention  None
 */ 
static eRESULT SoftwareMasterWrite(WORD              wDeviceAddress,
                                   WORD              wRegisterAddress,
                                   WORD              wCount,
                                   PCBYTE            pacBuffer,
                                   WORD              wFlags)
{
  eRESULT eResult = rcERROR;
  WORD wI;
  WORD wCountTmp;
  BYTE acBuffer[2];

  if(wFlags & DEVICE_BUS_ADDR_TRANSACTION)
  {
    TWISWStart();
    TWISWDataTx((wDeviceAddress << 1) & 0xFE); // W    
    if(!TWISWAckRx())
    {
      wI =0;
#if USE_DEBUG_TWI      
      msgErr("I2C 0x%.2X WRITE: NACK AFTER SLA+W", wDeviceAddress);
#endif
      goto STOP_CONDITION;
    }

    if(wFlags & DEVICE_BUS_WORD_ADDR)
    {
      wCountTmp = 2;
      acBuffer[0] = (BYTE)((wRegisterAddress >> 8) & 0xFF);
      acBuffer[1] = (BYTE)(wRegisterAddress & 0xFF);
    }
    else
    {
      wCountTmp = 1;
      acBuffer[0] = (unsigned char)(wRegisterAddress & 0xFF);
    }

    /* SUBADDR Write */
    for(wI = 0; wI < wCountTmp; wI++)
    {
      TWISWDataTx(acBuffer[wI]);
      
      if(!TWISWAckRx())
      {
        wI =0;
#if USE_DEBUG_TWI        
        msgErr("I2C 0x%.2X WRITE: NACK AFTER SUBADDR", wDeviceAddress);
#endif
        goto STOP_CONDITION;
      }      
    }

    wCountTmp = wCount;

    /* RESTART Condition */
    if(wFlags & DEVICE_BUS_SEND_REPEAT_AFTER_ADDR)
    {
      TWISWStart();
      TWISWDataTx((wDeviceAddress << 1) & 0xFE); // W
      
      if(!TWISWAckRx())
      {
        wI =0;
#if USE_DEBUG_TWI        
        msgErr("I2C 0x%.2X WRITE: NACK AFTER RESTART", wDeviceAddress);
#endif
        goto STOP_CONDITION;
      }      
    }

    /* BYTEDATA Write */
    if(wFlags & DEVICE_BUS_WORD_DATA
      && !(wCountTmp % 2))
    {   
      for(wI = 0; wI < wCountTmp; wI+=2)
      {
        acBuffer[1] = *(pacBuffer++);
        acBuffer[0] = *(pacBuffer++);
        //msgDbg("##### [K] I2CRead DATA%d: 0x%.2X\n",wI, acBuffer[1]);
        //msgDbg("##### [K] I2CRead DATA%d: 0x%.2X\n",wI+1, acBuffer[0]);
        TWISWDataTx(acBuffer[0]);
        if(!TWISWAckRx())
        {
          wI =0;
#if USE_DEBUG_TWI          
          msgErr("I2C 0x%.2X WRITE: NACK AFTER BYTEDATA0 WRITE", wDeviceAddress);
#endif
          goto STOP_CONDITION;
        }      
        TWISWDataTx(acBuffer[1]);
        if(!TWISWAckRx())
        {
          wI =0;
#if USE_DEBUG_TWI          
          msgErr("I2C 0x%.2X WRITE: NACK AFTER BYTEDATA1 WRITE", wDeviceAddress);
#endif
          goto STOP_CONDITION;
        }      
      }
    }
    else
    {
      for(wI = 0; wI < wCountTmp; wI++)
      {
        acBuffer[0] = *(pacBuffer++);
        TWISWDataTx(acBuffer[0]);
        if(!TWISWAckRx())
        {
          wI =0;
#if USE_DEBUG_TWI          
          msgErr("I2C 0x%.2X WRITE: NACK AFTER BYTEDATA WRITE", wDeviceAddress);
#endif
          goto STOP_CONDITION;
        }      
      }
    }    
  }
  else
  {
    wCountTmp = wCount;
    TWISWStart();
    TWISWDataTx((wDeviceAddress << 1) & 0xFE); // W
    if(!TWISWAckRx())
    {
      wI =0;
#if USE_DEBUG_TWI      
      msgErr("I2C 0x%.2X WRITE: NACK AFTER SLA+W", wDeviceAddress);
#endif
      goto STOP_CONDITION;
    }      

    for(wI = 0; wI < wCountTmp; wI++)
    {
      acBuffer[0] = *(pacBuffer++);
      //msgDbg("##### [K] I2CRead DATA%d: 0x%.2X\n",wI, acBuffer[0]);
      TWISWDataTx(acBuffer[0]);
      if(!TWISWAckRx())
      {
        wI =0;
#if USE_DEBUG_TWI        
        msgErr("I2C 0x%.2X WRITE: NACK AFTER BYTEDATA0 WRITE", wDeviceAddress);
#endif
        goto STOP_CONDITION;
      }      
    }    
  }

STOP_CONDITION:
  /* STOP Condition */
  TWISWStop();

  if(wI > 0) eResult = rcSUCCESS;
  
  return eResult;

}

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************
/**
 *  @brief      This API initializes the device bus software and hardware
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
void dvDeviceBusInitialize(void)
{
  BYTE cI;

  for(cI = 0; cI < TWI_NUM_INSTANCES; cI++)
  {
    if(m_aeTWIBusType[cI] == dbtHW)
    {
      HardwareMasterInit((eCPU_TWI)cI);
      HardwareMasterConfig((eCPU_TWI)cI, 0);
    }
    else if(m_aeTWIBusType[cI] == dbtSW)    
      SoftwareMasterInit((eCPU_TWI)cI);
    else 
      ASSERT_ALWAYS();
  }
}

/**
 *  @brief      This API must be called once during system Release
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvDeviceBusRelease(void)
{
}

/**
 *  @brief      Reads data from a device connected to an external 2-Wire device bus 
 *  @param[in]  cBusId         : Bus ID to execute the read / write on
                wDeviceAddress   : Device address to read / write
                wRegisterAddress : Internal device register address to read / write. 
                     Optional parameter, unused if the transaction is not an
                     addressed transaction.  Only the lower 8 bits are used
                     for addressed transactions unless word address is
                     specified.
                wCount           : Number of bytes to read / write from / to the external device
                wFlags           : Flags for the 2 wire transaction (see Definitions)
 *  @param[out] pacBuffer        : Pointer to an array of bytes for storing read data or data to be written 
 *  @return     rcSUCCESS if operation succeeds, rcERROR otherwise
 *  @note       None
 *  @attention  None
 */
eRESULT dvDeviceBusRead(BYTE cBusId,
                                  WORD  wDeviceAddress,
                                  WORD  wRegisterAddress,
                                  WORD  wCount,
                                  PBYTE pacBuffer,
                                  WORD  wFlags)
{
  eRESULT eResult = rcSUCCESS;

  ASSERT(cBusId < TWI_NUM_INSTANCES);

  if((wFlags & DEVICE_BUS_FORCE_SW) == DEVICE_BUS_FORCE_SW)
  {
    if(m_aeTWIBusType[cBusId] != dbtSW)
    {
      SoftwareMasterInit((eCPU_TWI)cBusId);
      m_aeTWIBusType[cBusId] = dbtSW;        
    }
    TWISWPortSelect((eCPU_TWI) cBusId);
    eResult |= SoftwareMasterRead(wDeviceAddress, 
                                              wRegisterAddress, 
                                              wCount, 
                                              pacBuffer, 
                                              wFlags);    
  }
  else
  {
    WORD wFlagPrev, wFlagNew;
    
    if(m_aeTWIBusType[cBusId] != dbtHW)
    {
      HardwareMasterInit((eCPU_TWI)cBusId);
      m_aeTWIBusType[cBusId] = dbtHW;              
    }  

    wFlagPrev = m_awHWI2C_Flags[cBusId] & (DEVICE_BUS_FAST_MODE | DEVICE_BUS_SLOW_MODE);
    wFlagNew = wFlags & (DEVICE_BUS_FAST_MODE | DEVICE_BUS_SLOW_MODE);
    
    if(wFlagPrev != wFlagNew)      
      HardwareMasterConfig((eCPU_TWI)cBusId, wFlags);

    eResult |= HardwareMasterRead(wDeviceAddress, 
                                              wRegisterAddress, 
                                              wCount, 
                                              pacBuffer, 
                                              wFlags);    
  } 

  return eResult;  
}

/**
 *  @brief      Writes data from a device connected to an external 2-Wire device bus
 *  @param[in]  cBusId         : Bus ID to execute the read / write on
                wDeviceAddress   : Device address to read / write
                wRegisterAddress : Internal device register address to read / write. 
                     Optional parameter, unused if the transaction is not an
                     addressed transaction.  Only the lower 8 bits are used
                     for addressed transactions unless word address is specified.
                wCount           : Number of bytes to read / write from / to the external device
                wFlags           : Flags for the 2 wire transaction (see Definitions)
 *  @param[out] pacBuffer        : Pointer to an array of bytes for storing read data or data to be written 
 *  @return     rcSUCCESS if operation succeeds, rcERROR otherwise
 *  @note       None
 *  @attention  None
 */
eRESULT dvDeviceBusWrite(BYTE cBusId,
                                 WORD   wDeviceAddress,
                                 WORD   wRegisterAddress,
                                 WORD   wCount,
                                 PCBYTE pacBuffer,
                                 WORD   wFlags)
{
  eRESULT eResult = rcSUCCESS;

  ASSERT(cBusId < TWI_NUM_INSTANCES);

  if((wFlags & DEVICE_BUS_FORCE_SW) == DEVICE_BUS_FORCE_SW)
  {
    if(m_aeTWIBusType[cBusId] != dbtSW)
    {
      SoftwareMasterInit((eCPU_TWI)cBusId);      
      m_aeTWIBusType[cBusId] = dbtSW;              
    }  
    TWISWPortSelect((eCPU_TWI) cBusId);
    eResult |= SoftwareMasterWrite(wDeviceAddress, 
                                              wRegisterAddress, 
                                              wCount, 
                                              pacBuffer, 
                                              wFlags);
  }
  else
  {
    WORD wFlagPrev, wFlagNew;
    
    if(m_aeTWIBusType[cBusId] != dbtHW)
    {
      HardwareMasterInit((eCPU_TWI)cBusId);
      m_aeTWIBusType[cBusId] = dbtHW;              
    }  

    wFlagPrev = m_awHWI2C_Flags[cBusId] & (DEVICE_BUS_FAST_MODE | DEVICE_BUS_SLOW_MODE);
    wFlagNew = wFlags & (DEVICE_BUS_FAST_MODE | DEVICE_BUS_SLOW_MODE);
    
    if(wFlagPrev != wFlagNew)
      HardwareMasterConfig((eCPU_TWI)cBusId, wFlags);            
    

    eResult |= HardwareMasterWrite(wDeviceAddress
                                                              , wRegisterAddress
                                                              , wCount
                                                              , pacBuffer
                                                              , wFlags);
  } 

  return eResult;
}


