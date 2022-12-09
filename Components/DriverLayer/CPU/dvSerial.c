/**
  ******************************************************************************
  * @file    dvSerial.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    07-Feb-2018
  * @brief   This file provides basic the CPU driver functions.
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
#include "dvSerialAPI.h"
#include "dvCPUAPI.h"
#include "utilExecutiveAPI.h"

/** @addtogroup CWM-T10
  * @{
  */

/** @defgroup dvSerial 
  * @brief CPU UART Driver
  * @{
  */

/** @defgroup dvSerial_Private_TypesDefinitions
  * @{
  */
  
// ---------------------------------------------------------------------------
// Structure for SERIAL(UART) Configurations
// ---------------------------------------------------------------------------
typedef struct tagUARTCONFIG
{
  BYTE            cInstance;
  INTERRUPT_TYPE  eIntType; // Interrupt Type
  int             nBaudRate;
  UART_DATABITS   eDataBits;
  UART_STOPBITS   eStopBits;
  UART_PARITY     eParity;
  eSIGNAL_GROUPS  eSignal;
}UART_CONFIG, *PUART_CONFIG;
typedef CROMDATA UART_CONFIG *PCUART_CONFIG;

/**
  * @}
  */

/** @defgroup dvSerial_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup dvSerial_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup dvUSerial_Private_Variables
  * @{
  */

// ---------------------------------------------------------------------------
// SERIAL(UART) Default Configuration
// ---------------------------------------------------------------------------
static CROMDATA UART_CONFIG m_sSerialInitDefault[SERIAL_NUM_INSTANCES] = 
{
  {// cpuUART_SETUP
    .cInstance  = 0, 
    .eIntType   = INTNUM_UART0,
    .nBaudRate  = 115200,
    .eDataBits  = DATABITS_8,
    .eStopBits  = STOPBITS_1,
    .eParity    = UART_PARNONE,
    .eSignal    = sgSERIAL_IO0    
  },
  {// cpuUART_GUI
    .cInstance  = 2, 
    .eIntType   = INTNUM_UART2,
    .nBaudRate  = 115200,
    .eDataBits  = DATABITS_8,
    .eStopBits  = STOPBITS_1,
    .eParity    = UART_PARNONE,
    .eSignal    = sgSERIAL_IO2    
  },  
  {// cpuUART_TOUCH
    .cInstance  = 3, 
    .eIntType   = INTNUM_UART3,
    .nBaudRate  = 115200,
    .eDataBits  = DATABITS_8,
    .eStopBits  = STOPBITS_1,
    .eParity    = UART_PARNONE,
    .eSignal    = sgSERIAL_IO3    
  },  
};

// Pointer to receive buffer from SERIAL_PROTOCOL_TASK
///static PBYTE            m_pacRxBuffer[SERIAL_NUM_INSTANCES];

// Current count of bytes received
static WORD             m_awBytesReceived[SERIAL_NUM_INSTANCES];

// Number of bytes than can be received before sending sgSERIAL_READ_THRESHOLD
// to SERIAL_PROTOCOL_TASK.
static WORD             m_awRxThreshold[SERIAL_NUM_INSTANCES];

// Number of bytes available in receive buffer
static WORD             m_awReceiveCountMax[SERIAL_NUM_INSTANCES];

#if 0
// Pointer to transmit buffer
static PCBYTE           m_pacTxBuffer[SERIAL_NUM_INSTANCES];

// Index into transmit buffer
static WORD             m_awTransmitIndex[SERIAL_NUM_INSTANCES];

// Actual number of bytes transmited
static WORD             m_awBytesTransmitted[SERIAL_NUM_INSTANCES];

// Number of bytes to transmit
static WORD             m_awTransmitCount[SERIAL_NUM_INSTANCES];
#endif

// Pointer to Serial Rx Queue
static PSERIAL_QUE m_pasSerialQueRx[SERIAL_NUM_INSTANCES];

// State of serial driver
static eSERIAL_STATUS   m_aeSerialState[SERIAL_NUM_INSTANCES];

/**
  * @}
  */

/** @defgroup dvSerial_Private_FunctionPrototypes
  * @{
  */

/**
  * @}
  */

/** @defgroup dvSerial_Private_Functions
  * @{
  */

/**
 *  @brief      Copies RX FIFO entries into SW RX buffer pointed to by m_apacRxBuffer
 *  @param      eSerialId : Identifier of Hardware USART
                cChar : BYTE to Copy
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void CopyToReceiveBuffer(eCPU_SERIAL eSerialId, BYTE cChar)
{
  ASSERT(eSerialId < SERIAL_NUM_INSTANCES);

  if(m_pasSerialQueRx[eSerialId] != NULL)
  {
    if(m_awBytesReceived[eSerialId] < m_awReceiveCountMax[eSerialId])
    {
      PSERIAL_QUE psQue = m_pasSerialQueRx[eSerialId];      

      psQue->pacBuffer[psQue->wHeader++] = cChar;

      psQue->wHeader %= m_awReceiveCountMax[eSerialId];
      psQue->wAvailableRxBytes++;      
    }
  }
  else
  {
    // Disable RX interrupt, otherwise RX interrupt will continuously
    // occur
    ///EnableRxInterrupt0(FALSE);
    dvSerialInterruptEnable(eSerialId, FALSE);
    utilExecSignalSet(m_sSerialInitDefault[eSerialId].eSignal, SI_SIGNAL_READ_FULL);    
  }        

  //SetRxInterruptTrigger0();*/
}

/** @defgroup dvSerial_Interrupt_Service_Routine
  * @{
  */

/**
 * @brief   This function handles USART1 global interrupt request.
 * @param   None
 * @retval  None
*/
static void USART0_IRQHandler(void)
{
  if(*((volatile U32*)0xF4003014) & F_ULS_DRDY)
  {
    CopyToReceiveBuffer(cpuUART_SETUP 
                        , *(volatile U32*)R_UART_BASE(m_sSerialInitDefault[cpuUART_SETUP].cInstance));
  }
}

/**
 * @brief   This function handles USART1 global interrupt request.
 * @param   None
 * @retval  None
*/
static void USART2_IRQHandler(void)
{
  if(*((volatile U32*)0xF4003054) & F_ULS_DRDY)
  {
    CopyToReceiveBuffer(cpuUART_GUI 
                        , *(volatile U32*)R_UART_BASE(m_sSerialInitDefault[cpuUART_GUI].cInstance));
  }
}

/**
 * @brief   This function handles USART1 global interrupt request.
 * @param   None
 * @retval  None
*/
static void USART3_IRQHandler(void)
{
  if(*((volatile U32*)0xF4003074) & F_ULS_DRDY)
  {
    CopyToReceiveBuffer(cpuUART_TOUCH 
                        , *(volatile U32*)R_UART_BASE(m_sSerialInitDefault[cpuUART_TOUCH].cInstance));
  }
}



/**
  * @}
  */



/**
 *  @brief      Initialize the USART Driver
 *  @param      eSerialId : Identifier of Hardware USART
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void SerialDriverInitialize(eCPU_SERIAL eSerialId)
{
  ASSERT(eSerialId < SERIAL_NUM_INSTANCES);

  m_pasSerialQueRx[eSerialId] = NULL;
  m_awBytesReceived[eSerialId] = 0;
  m_awRxThreshold[eSerialId] = 0;
  m_awReceiveCountMax[eSerialId] = 0;
  
  m_aeSerialState[eSerialId] = ssOK;  
}


/**
 *  @brief      Configures the USART 
 *  @param      eSerialId : Identifier of Hardware USART
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void SerialDeviceInitialize(eCPU_SERIAL eSerialId)
{
  ASSERT(eSerialId < SERIAL_NUM_INSTANCES);

  switch(eSerialId)
  {
    case cpuUART_SETUP:      
      /* USART0 configuration ------------------------------------------------------*/
      /* USART0 configured as follow:
            - BaudRate = 115200 baud  
            - Word Length = 8 Bits
            - One Stop Bit
            - No parity
            - Hardware flow control disabled (RTS and CTS signals)
            - Receive and transmit enabled
      */

      /* GPIO AF Setting */
      // GP1.0:Tx0, GP1.1:Rx0
      // Done in dvCPUPeriphInitialize() in dvCPU.c

      /* UART Configuration */
      uart_config(m_sSerialInitDefault[eSerialId].cInstance
                            , m_sSerialInitDefault[eSerialId].nBaudRate
                            , m_sSerialInitDefault[eSerialId].eDataBits
                            , m_sSerialInitDefault[eSerialId].eStopBits
                            , m_sSerialInitDefault[eSerialId].eParity);

      /* Interrupt Setting */
      set_interrupt(m_sSerialInitDefault[eSerialId].eIntType, USART0_IRQHandler);
      enable_interrupt(m_sSerialInitDefault[eSerialId].eIntType, FALSE);
    break;

    case cpuUART_GUI:      
      /* USART2 configuration ------------------------------------------------------*/
      /* USART2 configured as follow:
            - BaudRate = 115200 baud  
            - Word Length = 8 Bits
            - One Stop Bit
            - No parity
            - Hardware flow control disabled (RTS and CTS signals)
            - Receive and transmit enabled
      */

      /* GPIO AF Setting */
      // GP10.0:Tx2, GP10.1:Rx2
      // Done in dvCPUPeriphInitialize() in dvCPU.c

      /* UART Configuration */
      uart_config(m_sSerialInitDefault[eSerialId].cInstance
                            , m_sSerialInitDefault[eSerialId].nBaudRate
                            , m_sSerialInitDefault[eSerialId].eDataBits
                            , m_sSerialInitDefault[eSerialId].eStopBits
                            , m_sSerialInitDefault[eSerialId].eParity);

      /* Interrupt Setting */
      set_interrupt(m_sSerialInitDefault[eSerialId].eIntType, USART2_IRQHandler);
      enable_interrupt(m_sSerialInitDefault[eSerialId].eIntType, FALSE);
    break;    

    case cpuUART_TOUCH:      
      /* USART3 configuration ------------------------------------------------------*/
      /* USART3 configured as follow:
            - BaudRate = 115200 baud  
            - Word Length = 8 Bits
            - One Stop Bit
            - No parity
            - Hardware flow control disabled (RTS and CTS signals)
            - Receive and transmit enabled
      */

      /* GPIO AF Setting */
      // GP5.0:Tx3, GP5.1:Rx3
      // Done in dvCPUPeriphInitialize() in dvCPU.c

      /* UART Configuration */
      uart_config(m_sSerialInitDefault[eSerialId].cInstance
                            , m_sSerialInitDefault[eSerialId].nBaudRate
                            , m_sSerialInitDefault[eSerialId].eDataBits
                            , m_sSerialInitDefault[eSerialId].eStopBits
                            , m_sSerialInitDefault[eSerialId].eParity);

      /* Interrupt Setting */
      set_interrupt(m_sSerialInitDefault[eSerialId].eIntType, USART3_IRQHandler);
      enable_interrupt(m_sSerialInitDefault[eSerialId].eIntType, FALSE);
    break;
    
    default: ASSERT_ALWAYS(); break;
  }

}

/**
  * @}
  */


/** @defgroup dvSerial_Public_Functions
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
void dvSerialInitialize(void)
{
  BYTE cI;
  
  for(cI = 0; (eCPU_SERIAL)cI < SERIAL_NUM_INSTANCES; cI++)
  {
    SerialDriverInitialize((eCPU_SERIAL)cI);
    SerialDeviceInitialize((eCPU_SERIAL)cI);
  }
}

/**
 *  @brief        This API Release the CPU UART Device
 *  @param[in]    None
 *  @param[out]   None
 *  @return       None
 *  @note         None
 *  @attention    None
 */
void dvSerialRelease(void)
{ 

}

/**
 *  @brief      This API Configures the Vector Table base location.
 *  @param      None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvSerialNVICInitialize(void)
{
  /* Perform in SerialInitialize() */
}
  
/**
 *  @brief        This API Enables or Disable the specified USART Interrupt
 *  @param[in]    eSerialId : CPU USART Device Id
 *  @param[in]    bEnable : TRUE for Enable Interrupt, FALSE for Disable
 *  @param[out]   None
 *  @return       None
 *  @note         None
 *  @attention    None
 */
void dvSerialInterruptEnable(eCPU_SERIAL eSerialId, BOOL bEnable)
{
 
  ASSERT(eSerialId < SERIAL_NUM_INSTANCES);

  switch(eSerialId)
  {
    case cpuUART_SETUP:
    case cpuUART_GUI:      
    case cpuUART_TOUCH:      
      /* Enable or Disable USARTx Receive interrupts */
      enable_interrupt(m_sSerialInitDefault[eSerialId].eIntType, bEnable);
    break;  

    default: ASSERT_ALWAYS(); break;
  }    
}

/**
 *  @brief         This API reads data from the RS-232 Serial Port.
 *  @param[in]     eSerialId : CPU USART Device Id
                   wCount     : Number of bytes to available for storage in the pacBuffer parameter
                   wThreshold : Number of bytes to receive before signaling the protocol
                                task.  Must be less than or equal to amount of bytes
                                specified in the wCount parameter.
                    psQue : Pointer to Serial Que   
 *  @param[out]     None
 *  @return         None
 *  @note           None
 *  @attention      None 
 */  
void dvSerialReadSet(eCPU_SERIAL eSerialId
                                            , WORD  wCount
                                            , WORD  wThreshold
                                            , PSERIAL_QUE psQue)
{
  ASSERT(eSerialId < SERIAL_NUM_INSTANCES);
  
  // Disable interrupts so we do not corrupt data pointers
  ///EnableRxInterrupt0(FALSE);
  dvSerialInterruptEnable(eSerialId, FALSE);
  m_awBytesReceived[eSerialId] = 0;
  m_aeSerialState[eSerialId] = ssOK;
  m_awReceiveCountMax[eSerialId] = wCount;
  m_awRxThreshold[eSerialId] = wThreshold;

  m_pasSerialQueRx[eSerialId] = psQue;

  // Enable interrupts to copy data out of the FIFO or respond to errors
  ///EnableRxInterrupt0(TRUE);
  dvSerialInterruptEnable(eSerialId, TRUE);
}

/**
 *  @brief          This API reads data from the RS-232 Serial Port.
 *  @param[in]      eSerialId : CPU USART Device Id
                    wCount     : Number of bytes to Read
                    pacBuffer  : Pointer to an array of at least wCount bytes to receive data into
 *  @param[out]     None
 *  @return         None
 *  @note           None
 *  @attention      None 
 */  
WORD dvSerialRead(eCPU_SERIAL eSerialId
                                            , WORD  wCount
                                            , PBYTE pacBuffer)
{
  WORD wIndex = 0;
  WORD wReadSize = 0;
  PSERIAL_QUE psQue = m_pasSerialQueRx[eSerialId];
  
  ASSERT(eSerialId < SERIAL_NUM_INSTANCES);

  if(psQue != NULL
    && (psQue->wHeader != psQue->wTail))
  {
    // Disable interrupts so we do not corrupt data pointers
    ///EnableRxInterrupt0(FALSE);
    dvSerialInterruptEnable(eSerialId, FALSE);

    wReadSize = MIN(wCount, psQue->wAvailableRxBytes);    

    for(wIndex = 0; wIndex < wReadSize; wIndex++)
    {
      pacBuffer[wIndex] = psQue->pacBuffer[psQue->wTail++];
      psQue->wTail %= m_awReceiveCountMax[eSerialId];
      psQue->wAvailableRxBytes--; 

      m_awBytesReceived[eSerialId]++;

      if(m_awBytesReceived[eSerialId] == m_awRxThreshold[eSerialId])
      {
        utilExecSignalSet(m_sSerialInitDefault[eSerialId].eSignal, SI_SIGNAL_READ_THRESHOLD);
      }
      
      // Enable interrupts to copy data out of the FIFO or respond to errors
      ///EnableRxInterrupt0(TRUE);
      // If we have received the same number of bytes in the read buffer
      // disable interrupts and send a sgSERIAL_READ_FULL message to the
      // protocol task
      if (m_awBytesReceived[eSerialId] >= m_awReceiveCountMax[eSerialId])
      {
        //EnableRxErrorInterrupt0(FALSE);
        ///EnableRxInterrupt0(FALSE);
        dvSerialInterruptEnable(eSerialId, FALSE);
        utilExecSignalSet(m_sSerialInitDefault[eSerialId].eSignal,  SI_SIGNAL_READ_FULL);
      }                
    }    
    
    if(!utilExecSignalGet(m_sSerialInitDefault[eSerialId].eSignal,  SI_SIGNAL_READ_FULL))
      dvSerialInterruptEnable(eSerialId, TRUE);
    
  }

  return wReadSize;
}

/**
 *  @brief          This API Transmit the buffer via specified UART instance
 *  @param[in]      eSerialId : CPU USART Device Id
                    wCount: Number of bytes to transmit
                    pacBuffer: Pointer to an array of at least wCount bytes to transmit
 *  @param[out]     None
 *  @return         None
 *  @note           None
 *  @attention      None 
 */  
void dvSerialWrite(eCPU_SERIAL eSerialId
                                                    , WORD wCount
                                                    , PCBYTE pacBuffer)
{  
  ASSERT(eSerialId < SERIAL_NUM_INSTANCES);
  ASSERT(pacBuffer != NULL);  

  uart_putdata(m_sSerialInitDefault[eSerialId].cInstance, (U8 *)pacBuffer, (int)wCount);

  utilExecSignalSet(m_sSerialInitDefault[eSerialId].eSignal, SI_SIGNAL_WRITE_COMPLETE);
}

/**
 *  @brief        This API changes the threshold for signaling the protocol task when
                  receiving bytes on the Serial Port.
 *  @param[in]    eSerialId : CPU USART Device Id
                  wThreshold : Number of bytes to receive before signaling the protocol
                               task.  Must be less than or equal to amount of bytes
                               specified in the wCount parameter of the last call to
                               dvSerialRead()
                                                 
 *  @param[out]   None
 *  @return       None
 *  @note         None
 *  @attention    None 
 */ 
void dvSerialSetThreshold(eCPU_SERIAL eSerialId, WORD wThreshold)
{
  ASSERT(eSerialId < SERIAL_NUM_INSTANCES);
  ASSERT(wThreshold <= m_awReceiveCountMax[eSerialId]);

    m_awRxThreshold[eSerialId] = wThreshold;

    // We have already received wThreshold bytes. Immediatly signal the
    // protocol task
    if (wThreshold <= m_awBytesReceived[eSerialId])
    {
        utilExecSignalSet(m_sSerialInitDefault[eSerialId].eSignal, SI_SIGNAL_READ_THRESHOLD);
    }

    dvSerialInterruptEnable(eSerialId, TRUE);
}

/**
 *  @brief          This API acquires the current status of the Serial Port hardware.
 *  @param[in]      eSerialId : CPU USART Device Id
 *  @param[out]     pwBytesRead    : Number of bytes read since the last call to dvSerialRead() 
                    pwBytesAvail : Number of bytes read available from Queue 
 *  @return         Current status of serial interface (ok, read overflow, read error, or read overflow and error)
 *  @note           None
 *  @attention      None 
 */  
eSERIAL_STATUS dvSerialStatus(eCPU_SERIAL eSerialId
                                                                  , PWORD pwBytesRead
                                                                  , PWORD pwBytesAvail)
{
  ASSERT(eSerialId < SERIAL_NUM_INSTANCES);
  
  eSERIAL_STATUS      eTempSerialStatus = m_aeSerialState[eSerialId];

  if (ssREAD_ERROR == m_aeSerialState[eSerialId])
  {
    m_aeSerialState[eSerialId] = ssOK;
  }
  else if (ssREAD_OVERFLOW_ERROR == m_aeSerialState[eSerialId])
  {
    m_aeSerialState[eSerialId] = ssREAD_OVERFLOW;
  }

  if(pwBytesRead != NULL)
    *pwBytesRead = m_awBytesReceived[eSerialId];
  if(pwBytesAvail != NULL)
    *pwBytesAvail = m_pasSerialQueRx[eSerialId]->wAvailableRxBytes;
  
  return eTempSerialStatus;
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

/******************* (C) COPYRIGHT 1997-2018 (c) MadeFactory Inc. *****END OF FILE****/  
