/**
  ******************************************************************************
  * @file    dvDeviceBus3Wire.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    03-August-2014
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
  * <h2><center>&copy; Copyright 1997-2016 (c) MadeFactory Inc.</center></h2>
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "dvDeviceBus3WireAPI.h"
#include "dvGPIOAPI.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************


//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************
#define SPI_INIT_TYPE_DEFAULT     (SPICON_SPISIZE8B | SPICON_MSBF | SPICON_MSTR)
#define SPI_INIT_FREQ_HZ_DEFAULT  1000000 // 1MHz

// ---------------------------------------------------------------------------
// Device Bus 3Wire Hardware Channel
// ---------------------------------------------------------------------------
static CROMDATA BYTE m_acDevice3WireChannel[SPI_NUM_INSTANCES] = 
{
  1 // cpuSPI1
};

//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************
static BOOL m_abDevice3WireEnabled[SPI_NUM_INSTANCES];
static SPI_INIT_STRUCT m_asDevice3WireInitStruct[SPI_NUM_INSTANCES];

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************

/**
 *  @brief      Initializes 3-wire hardware master.
 *  @param[in]   eSPId : SPI identifier for hardware
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
static void HardwareMaster3WireFreqSet(eCPU_SPI eSPId, DWORD dwFreq)
{
  PSPI_INIT_STRUCT psSPIInitStruct  = &m_asDevice3WireInitStruct[eSPId];
  BYTE cBaudRate = 0;

  if(SPI_NUM_INSTANCES > eSPId)
  {  
    if ((psSPIInitStruct->sInitType.cSPICon & SPICON_MSTR) 
        && (dwFreq > SPI_MASTER_MAXFREQ))
	    dwFreq = SPI_MASTER_MAXFREQ;
    else if (!(psSPIInitStruct->sInitType.cSPICon & SPICON_MSTR) 
            && (dwFreq > SPI_SLAVE_MAXFREQ))
      dwFreq = SPI_SLAVE_MAXFREQ;  

    cBaudRate = (get_apb_clock() / (dwFreq << 1)) - 1;
    
    switch(eSPId)
    {
      case cpuSPI1:
      	*R_SPIBAUD(m_acDevice3WireChannel[eSPId]) = cBaudRate;
        psSPIInitStruct->sInitType.dwFreq = dwFreq;
      break;

      default: ASSERT_ALWAYS(); 
      break;
    }
  }
  
}

/**
 *  @brief      Initializes 3-wire hardware master.
 *  @param[in]   eSPId : SPI identifier for hardware
                 pcsInitStructNew: pointer to SPI Init structure (readonly)
 *  @param[out]  None
 *  @return     TRUE if Init structure changed(Applied), FALSE to others
 *  @note       None
 *  @attention  None
 */ 
static BOOL Device3WireInitStructure(eCPU_SPI eSPId
                                          , PCSPI_INIT_STRUCT pcsInitStructNew)
{
  BOOL bChanged = FALSE;
  PSPI_INIT_STRUCT psSPIInitStruct  = &m_asDevice3WireInitStruct[eSPId];
  BYTE cSPICon;

  if(pcsInitStructNew != NULL
    && SPI_NUM_INSTANCES > eSPId)
  {  
    /* If moderated value is different, then init 3Wire Again */
    if(psSPIInitStruct->wFlags != pcsInitStructNew->wFlags
      || psSPIInitStruct->sInitType.cSPICon != pcsInitStructNew->sInitType.cSPICon
      || psSPIInitStruct->sInitType.dwFreq != pcsInitStructNew->sInitType.dwFreq)
    {
      switch(eSPId)
      {
        case cpuSPI1:        
          // TODO: wFlags Process
          
          /* H/W SPI re-Init */
          cSPICon = *R_SPICON(m_acDevice3WireChannel[eSPId]) & SPICON_EN;
          cSPICon |= (pcsInitStructNew->sInitType.cSPICon & ~SPICON_EN);
          *R_SPICON(m_acDevice3WireChannel[eSPId]) = cSPICon;
          
          HardwareMaster3WireFreqSet(eSPId, pcsInitStructNew->sInitType.dwFreq);

          /* Copy the new to internal */
          m_asDevice3WireInitStruct[eSPId] = *pcsInitStructNew;

          bChanged = TRUE;
        break;

        default: ASSERT_ALWAYS(); 
        break;
      }      
    }
  }

  return bChanged;
}

/**
 *  @brief      Initializes 3-wire hardware master.
 *  @param[in]   eSPId : SPI identifier for hardware
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
static void HardwareMaster3WireInit(eCPU_SPI eSPId)
{
  PSPI_INIT_STRUCT psSPIInitStruct  = NULL;
  
  ASSERT(SPI_NUM_INSTANCES > eSPId);

  psSPIInitStruct = &m_asDevice3WireInitStruct[eSPId];
  
  switch(eSPId)
  {
    case cpuSPI1:        
      /* AF Setting */
      (VUINT)*R_PAF10 &= ~(AF10_5_GP5 | AF10_6_GP6 | AF10_7_GP7);
    	(VUINT)*R_PAF10 |= (AF10_5_SPI_SCK1 | AF10_6_SPI_MISO1 | AF10_7_SPI_MOSI1);       

      /* Init Stucture */
      psSPIInitStruct->sInitType.cSPICon = SPI_INIT_TYPE_DEFAULT;
      *R_SPICON(m_acDevice3WireChannel[eSPId]) = psSPIInitStruct->sInitType.cSPICon;

      HardwareMaster3WireFreqSet(eSPId, SPI_INIT_FREQ_HZ_DEFAULT);        
    break;

    default: ASSERT_ALWAYS(); 
    break;
  }
}

/**
 *  @brief      Wait until SPI Finish
 *  @param[in]  eSPId : SPI identifier for hardware
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
static void Hardware3WireWaitFinish(eCPU_SPI eSPId)
{
  BYTE cStatus;

  do
  {
    cStatus = *R_SPISTAT(m_acDevice3WireChannel[eSPId]);
    
  }while(!(cStatus & SPISTAT_SPIF));
}

/**
 *  @brief      Wait until SPI Finish
 *  @param[in]  eSPId : SPI identifier for hardware
 *  @param[out] None
 *  @return     SPI Status Flags
 *  @note       None
 *  @attention  None
 */ 
static BYTE Hardware3WireStatusFlagGet(eCPU_SPI eSPId)
{
  return *R_SPISTAT(m_acDevice3WireChannel[eSPId]);
}

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************
/**
 *  @brief      This API initializes the device bus software and hardware
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */ 
void dvDeviceBus3WireInitialize(void)
{
  BYTE cI;

  for(cI = 0; cI < SPI_NUM_INSTANCES; cI++)
  {
    m_abDevice3WireEnabled[cI] = FALSE;
    HardwareMaster3WireInit((eCPU_SPI)cI);

    /* Enable SPI */
    dvDeviceBus3WireEnable((eCPU_SPI)cI, TRUE);
  }
}

/**
 *  @brief      This API must be called once during system Release
 *  @param[in]  None
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvDeviceBus3WireRelease(void)
{
  /* Release Code Here */
}

/**
 *  @brief      This API Enable the specified 3wire bus
 *  @param[in]  eSPId : SPI identifier for hardware
                bEnable: TRUE to Enable, FALSE to Disable
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvDeviceBus3WireEnable(eCPU_SPI eSPId, BOOL bEnable)
{
  ASSERT(SPI_NUM_INSTANCES > eSPId);

  switch(eSPId)
  {
    case cpuSPI1:   
      /* Enable/Disable the SPI */
      if(bEnable)
        *R_SPICON(m_acDevice3WireChannel[eSPId]) |= SPICON_EN;
      else
        *R_SPICON(m_acDevice3WireChannel[eSPId]) &= ~(SPICON_EN);
      
      m_abDevice3WireEnabled[eSPId] = bEnable;
    break;

    default: ASSERT_ALWAYS(); 
    break;
  }  
}

/**
 *  @brief      Reads and writes individual bits to a hardware three wire bus
 *  @param[in]  eSPId : SPI identifier for hardware
                dwWriteCount: Write Buffer Size. Can be 0
                pacWriteBuffer: Pointer to Write Buffer. Can be NULL
                dwReadCount: Read Buffer Size. Can be 0
                pcsInitStructNew: pointer to SPI Init structure (readonly. can be NULL)
 *  @param[out] pacReadBuffer: Pointer to Read Buffer. Can be NULL
 *  @return     rcSUCCESS if operation succeeds, rcERROR otherwise
 *  @note       None
 *  @attention  None
 */
eRESULT dvDeviceBus3WireTransaction(eCPU_SPI          eSPId
                                                , DWORD             dwWriteCount
                                                , PCBYTE            pacWriteBuffer
                                                , DWORD             dwReadCount
                                                , PBYTE             pacReadBuffer
                                                , PCSPI_INIT_STRUCT pcsInitStructNew)
{
  DWORD dwI;
  eRESULT eResult = rcSUCCESS;
  PCSPI_INIT_STRUCT pcsSPIInitStruct = NULL;  
  BYTE cDummyData = 0xFF;
  DWORD dwWriteCountCur = 0;
  DWORD dwReadCountCur = 0;
  DWORD dwWriteLen;
  DWORD dwReadLen;
  BYTE cFlushData;
  
  ASSERT(SPI_NUM_INSTANCES > eSPId);
  ASSERT(m_abDevice3WireEnabled[eSPId]);

  pcsSPIInitStruct = &m_asDevice3WireInitStruct[eSPId];

  /* Init 3Wire struct if different with previously */
  Device3WireInitStructure(eSPId, pcsInitStructNew);

  if(pcsSPIInitStruct->wFlags & DEVICE_3WIRE_DUMMY_00)
    cDummyData = 0x00;

  if(dwWriteCount > SPI_MAX_WFIFO)
    dwWriteLen = SPI_MAX_WFIFO;
  else
    dwWriteLen = dwWriteCount;

  if(dwReadCount > SPI_MAX_WFIFO)
    dwReadLen = SPI_MAX_RFIFO;
  else
    dwReadLen = dwReadCount;  

  /* Wait util Tx buffer empty */
  if(dwWriteCount)
  {
    while(!(Hardware3WireStatusFlagGet(eSPId) & SPISTAT_STXE));
  }

  //4 Send OPCODE or ADSI
  while(dwWriteCountCur < dwWriteCount)
  {
    for(dwI = 0; dwI < dwWriteLen; dwI++)
    {
      *R_SPIDATA(m_acDevice3WireChannel[eSPId]) = pacWriteBuffer[dwI];
      ///msgDbg("%d:0x%.2X", dwI, pacWriteBuffer[dwI]);
    }
    Hardware3WireWaitFinish(eSPId);

    // Flush Garbage
    for(dwI = 0; dwI < dwWriteLen; dwI++)
      cFlushData = *R_SPIDATA(m_acDevice3WireChannel[eSPId]);

    pacWriteBuffer += dwWriteLen;
    dwWriteCountCur += dwWriteLen;

    if((dwWriteCount - dwWriteCountCur) < SPI_MAX_WFIFO)
      dwWriteLen = dwWriteCount - dwWriteCountCur;
  }

  //4 Send Dummy and Read DATA
  while(dwReadCountCur < dwReadCount)
  {
    for(dwI = 0; dwI < dwReadLen; dwI++)
    {
      *R_SPIDATA(m_acDevice3WireChannel[eSPId]) = cDummyData;
      Hardware3WireWaitFinish(eSPId);
      cFlushData = *R_SPIDATA(m_acDevice3WireChannel[eSPId]);
      if(pacReadBuffer != NULL)
        pacReadBuffer[dwI] = cFlushData;
    }

    if(pacReadBuffer != NULL)
      pacReadBuffer += dwReadLen;
    dwReadCountCur += dwReadLen;

    if((dwReadCount - dwReadCountCur) < SPI_MAX_RFIFO)
      dwReadLen = dwReadCount - dwReadCountCur;
  }

  return eResult;
}

/**
 *  @brief      Reads and writes One byte to a hardware three wire bus
                eSPId : SPI identifier for hardware
                cWriteData: Write Data
                pcsInitStructNew: pointer to SPI Init structure (readonly. can be NULL)
 *  @return     Read 1Byte Data
 *  @note       None
 *  @attention  None
 */
BYTE dvDeviceBus3WireOneByteSend(eCPU_SPI           eSPId
                                                , BYTE              cWriteData
                                                , PCSPI_INIT_STRUCT pcsInitStructNew)
{
  BYTE cReadData;

  ASSERT(SPI_NUM_INSTANCES > eSPId);
  ASSERT(m_abDevice3WireEnabled[eSPId]);    
  
  dvDeviceBus3WireTransaction(eSPId, 1, &cWriteData, 1, &cReadData, pcsInitStructNew);

  return cReadData;  
}

/**
 *  @brief      Writes One WORD to a hardware three wire bus
                eSPId : SPI identifier for hardware
                wWriteData: Write Data
                pcsInitStructNew: pointer to SPI Init structure (readonly. can be NULL)
 *  @return     rcSUCCESS if operation succeeds, rcERROR otherwise
 *  @note       None
 *  @attention  None
 */
eRESULT dvDeviceBus3WireOneWordTxOnly(eCPU_SPI eSPId
                                                    , WORD wWriteData
                                                    , PCSPI_INIT_STRUCT pcsInitStructNew)
{
  eRESULT eResult = rcSUCCESS;
  BYTE acWriteData[2];
  
  ASSERT(SPI_NUM_INSTANCES > eSPId);
  ASSERT(m_abDevice3WireEnabled[eSPId]);

  acWriteData[0] = (wWriteData >> 8) & 0xFF;
  acWriteData[1] = (BYTE)wWriteData;

  dvDeviceBus3WireTransaction(eSPId, 2, acWriteData, 0, NULL, pcsInitStructNew);

  return eResult;
}


