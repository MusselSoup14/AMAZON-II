/**
  ******************************************************************************
  * @file    dvMCP25625.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    27-03-2019
  * @brief   This file provides basic the CAN Controller
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
#include "dvMCP25625API.h"
#include "dvGPIOAPI.h"
#include "dvDeviceBus3WireAPI.h"
#include "utilExecutiveAPI.h"

#if USE_CAN_BUS
/** @addtogroup CWM-T10
  * @{
  */

/** @defgroup dvMCP25625 
  * @brief MCP25625 Driver
  * @{
  */

/** @defgroup dvMCP25625_Private_Defines
  * @{
  */

///#define _DEF_USE_CANSPI_RXB1_

/* CAN SPI Interrupt Input GP Channel, Pin(Mask) and Interrupt Type */
#define CANSPI_INT_GP_CH          GPIO_7
#define CANSPI_INT_GP_PIN_MASK    GPIO_Pin_7
#define CANSPI_INT_GP_AF          AF7_7_GP7
#define CANSPI_INT_NUM            INTNUM_GPIO7
#define CANSPI_INT_IRQHANDLER     CANSPI_INT_IRQHandler

// ---------------------------------------------------------------------------
// SPI Instruction Set
// ---------------------------------------------------------------------------
#define CANSPI_CMD_RESET       0xC0  /* Resets the internal registers to the default state, sets Configuration mode. */
#define CANSPI_CMD_READ        0x03  /* Reads data from the register beginning at the selected address. */
#define CANSPI_CMD_WRITE       0x02  /* Writes data to the register beginning at the selected address. */
#define CANSPI_CMD_RTS         0x80  // Request to Send
#define CANSPI_CMD_RTS_TXB0    0x81  // RTS TX buffer 0
#define CANSPI_CMD_RTS_TXB1    0x82  // RTS TX buffer 1
#define CANSPI_CMD_RTS_TXB2    0x84  // RTS TX buffer 2
#define CANSPI_CMD_RD_STATUS   0xA0  /* Quick polling command that reads several Status bits 
                                     for transmit and receive functions. */
#define CANSPI_CMD_BIT_MODIFY  0x05  /* Allows the user to set or clear individual bits in a particular register. */
#define CANSPI_CMD_RX_STATUS   0xB0  /* Quick polling command that indicates a filter match and message type
                                     (standard, extended and/or remote) of the received message. */

// ---------------------------------------------------------------------------
// Register Addresses
// ---------------------------------------------------------------------------
#define cREGADDR_CANCTRL	  0x0F
#define cREGADDR_CANSTAT	  0x0E

#define cREGADDR_CNF1   	  0x2A
#define cREGADDR_CNF2   	  0x29
#define cREGADDR_CNF3    	  0x28

#define cREGADDR_TXB0CTRL	  0x30 /* TRANSMIT BUFFER 0 CONTROL REGISTER */
#define cREGADDR_TXB0ID 	  0x31
#define cREGADDR_TXB0DATA 	0x36

#define cREGADDR_TXB1CTRL	  0x40  /* TRANSMIT BUFFER 0 CONTROL REGISTER */
#define cREGADDR_TXB1ID  	  0x41
#define cREGADDR_TXB1DATA 	0x46

#define cREGADDR_TXB2CTRL	  0x50  /* TRANSMIT BUFFER 0 CONTROL REGISTER */
#define cREGADDR_TXB2ID 	  0x51
#define cREGADDR_TXB2DATA 	0x56

#define cREGADDR_RXF0       0x00
#define cREGADDR_RXF1       0x04
#define cREGADDR_RXF2       0x08
#define cREGADDR_RXF3       0x10
#define cREGADDR_RXF4       0x14
#define cREGADDR_RXF5       0x18

#define cREGADDR_RXM0       0x20
#define cREGADDR_RXM1       0x24

#define cREGADDR_RXB0CTRL	  0x60
#define cREGADDR_RXB0ID 	  0x61
#define cREGADDR_RXB0DATA 	0x66

#define cREGADDR_RXB1CTRL	  0x70
#define cREGADDR_RXB1ID 	  0x71
#define cREGADDR_RXB1DATA 	0x76

#define cREGADDR_CANINTE	  0x2B
#define cREGADDR_CANINTF	  0x2C

#define cREGADDR_ERRFLAG    0x2D
#define cREGADDR_TEC        0x1C
#define cREGADDR_REC        0x1D


//! CAN Module Event (Interrupts); flags can be or'ed
#define CANSPI_EVENT_NO            (0x00)
#define CANSPI_EVENT_RXB0          (0x01)
#define CANSPI_EVENT_RXB1          (0x02)
#define CANSPI_EVENT_TXB0          (0x04)
#define CANSPI_EVENT_TXB1          (0x08)
#define CANSPI_EVENT_TXB2          (0x10)
#define CANSPI_EVENT_ERROR         (0x20)
#define CANSPI_EVENT_WAKEUP        (0x40)
#define CANSPI_EVENT_MESSAGE_ERROR (0x80)
#define CANSPI_EVENT_ALL           (0xFF)

// Maximum number of data bytes in message
#define CANSPI_MAX_DATA_BYTES       8

// Maximum number of data bytes in Tx Buffer
#define CANSPI_TX_BUFFER_SIZE       13

// Filter byte size
#define CANSPI_FILTER_SIZE          4

// Mask Byte Size
#define CANSPI_MASK_SIZE            4

// Maximum number of data bytes in Rx Buffer
#define CANSPI_RX_BUFFER_SIZE       13

// Mximum number of count to transimit retry
#define CANSPI_TRANMIT_WAIT_MAX     1000


/**
  * @}
  */

/** @defgroup dvMCP25625_Private_TypesDefinitions
  * @{
  */

// ---------------------------------------------------------------------------
// Register Structures
// ---------------------------------------------------------------------------

// Message Transmit Registers ---------------------------------------------- //

//! TXBNCTRL: TRANSMIT BUFFER x CONTROL REGISTER (ADDRESS: 30h, 40h, 50h)
typedef union 
{
  struct 
  {
    BYTE TxPriority       : 2; /* Transmit Buffer Priority bits
                                    11 = Highest message priority
                                    10 = High intermediate message priority
                                    01 = Low intermediate message priority
                                    00 = Lowest message priority (*) */
    BYTE Unimplemented0   : 1;
    BYTE TxRequest        : 1; /* Message Transmit Request bit
                                    1 = Buffer is currently pending transmission 
                                        (MCU sets this bit to request message be transmitted –
                                        bit is automatically cleared when the message is sent)
                                    0 = Buffer is not currently pending transmission (*)
                                        (MCU can clear this bit to request a message abort) */
    BYTE TxError          : 1; /* (R)Transmission Error Detected bit
                                    1 = A bus error occurred while the message was being transmitted
                                    0 = No bus error occurred while the message was being transmitted */
    BYTE ArbitrationLost  : 1; /* (R)Message Lost Arbitration bit
                                    1 = Message lost arbitration while being sent
                                    0 = Message did not lose arbitration while being sent */
    BYTE MessageAborted   : 1; /* (R)ABTF: Message Aborted Flag bit
                                    1 = Message was aborted
                                    0 = Message completed transmission successfully */
    BYTE Unimplemented1   : 1;
  };
  BYTE cData;
}CANSPI_REG_TXBNCTRL;

//! TXRTSCTRL: TxnRTS PIN CONTROL AND STATUS REGISTER (ADDRESS: 0Dh)
typedef union 
{
  struct 
  {
    BYTE B0RTSM         : 1; /* Tx0RTS Pin mode bit
                                  1 = Pin is used to request message transmission of TXB0 buffer (on falling edge)
                                  0 = Digital input (*) */
    BYTE B1RTSM         : 1; /* Tx1RTS Pin mode bit
                                  1 = Pin is used to request message transmission of TXB1 buffer (on falling edge)
                                  0 = Digital input (*) */
    BYTE B2RTSM         : 1; /* Tx2RTS Pin mode bit
                                  1 = Pin is used to request message transmission of TXB2 buffer (on falling edge)
                                  0 = Digital input (*) */
    BYTE B0RTS          : 1; /* (R)Tx0RTS Pin State bit
                                  - Reads state of Tx0RTS pin when in Digital Input mode
                                  - Reads as ‘0’ when pin is in ‘Request-to-Send’ mode */
    BYTE B1RTS          : 1; /* (R)Tx1RTX Pin State bit
                                  - Reads state of Tx1RTS pin when in Digital Input mode
                                  - Reads as ‘0’ when pin is in ‘Request-to-Send’ mode */
    BYTE B2RTS          : 1; /* (R)Tx2RTS Pin State bit
                                  - Reads state of Tx2RTS pin when in Digital Input mode
                                  - Reads as ‘0’ when pin is in ‘Request-to-Send’ mode */
    BYTE Unimplemented0 : 2;
  };
  BYTE cData;
}CANSPI_REG_TXRTSCTRL;

//! TXBnSIDH: TRANSMIT BUFFER x STANDARD IDENTIFIER HIGH REGISTER (ADDRESS: 31h, 41h, 51h)
typedef union 
{
  struct 
  {
    BYTE SIDH; /* Standard Identifier bits [10:3] */
  };
  BYTE cData;
}CANSPI_REG_TXBnSIDH;

//! TXBnSIDL: TRANSMIT BUFFER x STANDARD IDENTIFIER LOW REGISTER (ADDRESS: 32h, 42h, 52h)
typedef union 
{
  struct 
  {
    BYTE EIDHH          : 2; /* Extended Identifier bits [17:16] */
    BYTE Unimplemented0 : 1;
    BYTE IDE            : 1; /* Extended Identifier Enable bit
                                  1 = Message will transmit the Extended Identifier
                                  0 = Message will transmit the Standard Identifier */
    BYTE Unimplemented1 : 1;
    BYTE SIDL           : 3; /* Standard Identifier bits [2:0] */
  };
  BYTE cData;
}CANSPI_REG_TXBnSIDL;

//! TXBnEIDH: TRANSMIT BUFFER x EXTENDED IDENTIFIER HIGH REGISTER (ADDRESS: 33h, 43h, 53h)
typedef union 
{
  struct 
  {
    BYTE EIDH; /* Extended Identifier bits [15:8] */
  };
  BYTE cData;
}CANSPI_REG_TXBnEIDH;

//! TXBnEIDL: TRANSMIT BUFFER x EXTENDED IDENTIFIER LOW REGISTER (ADDRESS: 34h, 44h, 54h)
typedef union 
{
  struct 
  {
    BYTE EIDL; /* Extended Identifier bits [7:0] */
  };
  BYTE cData;
}CANSPI_REG_TXBnEIDL;

//! TXBnDLC: TRANSMIT BUFFER x DATA LENGTH CODE REGISTER (ADDRESS: 35h, 45h, 55h)
typedef union 
{
  struct 
  {
    BYTE DLC            : 4; /* Data Length Code bits.
                                  Sets the number of data bytes to be transmitted (0 to 8 bytes). */    
    BYTE Unimplemented0 : 2;
    BYTE RTR            : 1; /* Remote Transmission Request bit
                                  1 = Transmitted message will be a Remote Transmit Request
                                  0 = Transmitted message will be a data frame */
    BYTE Unimplemented1 : 1;
  };
  BYTE cData;
}CANSPI_REG_TXBnDLC;

//! TXBnDn: TRANSMIT BUFFER x DATA BYTE n REGISTER (ADDRESS: 36h-3Dh, 46h-4Dh, 56h-5Dh)
typedef union 
{
  struct 
  {
    BYTE TXBnDn; /* Transmit Buffer x Data Field Bytes n bits [7:0] */
  };
  BYTE cData;
}CANSPI_REG_TXBnDn;



// Message Receive Registers ---------------------------------------------- //

//! RXB0CTRL: RECEIVE BUFFER 0 CONTROL REGISTER (ADDRESS: 60h)
typedef union
{
  struct 
  {
    BYTE FILTHIT        : 1; /* (R)Filter Hit bit.
                                Indicates which acceptance filter enabled the reception of a message.
                                  1 = Acceptance Filter 1 (RXF1)
                                  0 = Acceptance Filter 0 (RXF0) */
    BYTE Unimplemented1 : 1;
    BYTE BUKT           : 1; /* (R)Rollover Enable bit
                                  1 = RXB0 message will roll over and be written to RXB1 if RXB0 is full
                                  0 = Rollover is disabled */
    BYTE RXRTR          : 1; /* (R)Received Remote Transfer Request bit
                                  1 = Remote Transfer Request received
                                  0 = No Remote Transfer Request received */
    BYTE Unimplemented2 : 1;
    BYTE RXM            : 2; /* Receive Buffer Operating mode bits
                                  11 = Turns mask/filters off; receives any message
                                  10 = Reserved
                                  01 = Reserved
                                  00 = Receives all valid messages using either Standard 
                                       or Extended Identifiers that meet filter criteria; (*)
                                       Extended ID Filter registers, RXFxEID8 and RXFxEID0, 
                                       are applied to the first two bytes of data in the messages with standard IDs. */
    BYTE Unimplemented3 : 1;
  };
  BYTE cData;
}CANSPI_REG_RXB0CTRL;

//! RXB1CTRL: RECEIVE BUFFER 1 CONTROL REGISTER (ADDRESS: 70h)
typedef union 
{
  struct 
  {
    BYTE FILTHIT        : 3; /* (R)Filter Hit bits
                                Indicates which acceptance filter enabled the reception of a message.
                                  101 = Acceptance Filter 5 (RXF5)
                                  100 = Acceptance Filter 4 (RXF4)
                                  011 = Acceptance Filter 3 (RXF3)
                                  010 = Acceptance Filter 2 (RXF2)
                                  001 = Acceptance Filter 1 (RXF1) (only if the BUKT bit is set in RXB0CTRL)
                                  000 = Acceptance Filter 0 (RXF0) (only if the BUKT bit is set in RXB0CTRL) */
    BYTE RXRTR          : 1; /* (R)Received Remote Transfer Request bit
                                  1 = Remote Transfer Request received
                                  0 = No Remote Transfer Request received */
    BYTE Unimplemented1 : 1;
    BYTE RXM            : 2; /* Receive Buffer Operating mode bits
                                  11 = Turns mask/filters off; receives any message
                                  10 = Reserved
                                  01 = Reserved
                                  00(*) = Receives all valid messages using either Standard 
                                          or Extended Identifiers that meet filter criteria */
    BYTE Unimplemented3 : 1;
  };
  BYTE cData;
}CANSPI_REG_RXB1CTRL;

//! BFPCTRL: RxnBF PIN CONTROL AND STATUS REGISTER (ADDRESS: 0Ch)
typedef union 
{
  struct 
  {
    BYTE B0BFM        : 1; /* Rx0BF Pin Operation mode bit
                                1 = Pin is used as an interrupt when a valid message is loaded into RXB0
                                0(*) = Digital Output mode */
    BYTE B1BFM        : 1; /* Rx1BF Pin Operation mode bit
                                1 = Pin is used as an interrupt when a valid message is loaded into RXB1
                                0(*) = Digital Output mode */
    BYTE B0BFE        : 1; /* Rx0BF Pin Function Enable bit
                                1 = Pin function is enabled, operation mode is determined by the B0BFM bit
                                0(*) = Pin function is disabled, pin goes to the high-impedance state */
    BYTE B1BFE        : 1; /* Rx1BF Pin Function Enable bit
                                1 = Pin function is enabled, operation mode is determined by the B1BFM bit
                                0(*) = Pin function is disabled, pin goes to the high-impedance state */
    BYTE B0BFS        : 1; /* Rx0BF Pin State bit (Digital Output mode only)
                                - Reads as ‘0’ when Rx0BF is configured as an interrupt pin */
    BYTE B1BFS        : 1; /* Rx1BF Pin State bit (Digital Output mode only)
                                - Reads as ‘0’ when Rx1BF is configured as an interrupt pin */
    BYTE Unimplemented: 2;
  };
  BYTE cData;
}CANSPI_REG_BFPCTRL;

//! RXBnSIDH: RECEIVE BUFFER x STANDARD IDENTIFIER HIGH REGISTER (ADDRESS: 61h, 71h)
typedef union
{
  struct
  {
    BYTE SIDH; /* (R)Standard Identifier bits [10:3]
                  These bits contain the eight Most Significant bits of the Standard Identifier 
                  for the received message. */
  };
  BYTE cData;
}CANSPI_REG_RXBnSIDH;

//! RXBnSIDL: RECEIVE BUFFER x STANDARD IDENTIFIER LOW REGISTER (ADDRESS: 62h, 72h)
typedef union 
{
  struct 
  {
    BYTE EIDHH          : 2; /* (R)Extended Identifier bits [17:16]
                                These bits contain the two Most Significant bits 
                                of the Extended Identifier for the received message. */
    BYTE Unimplemented0 : 1;
    BYTE IDE            : 1; /* (R)Extended Identifier Flag bit
                                This bit indicates whether the received message was a standard or an extended frame.
                                  1 = Received message was an extended frame
                                  0 = Received message was a standard frame */
    BYTE SRR            : 1; /* (R)Standard Frame Remote Transmit Request bit (valid only if the IDE bit = 0)
                                  1 = Standard frame Remote Transmit Request received
                                  0 = Standard data frame received */
    BYTE SIDL           : 3; /* (R)Standard Identifier bits [2:0]
                                These bits contain the three Least Significant bits 
                                of the Standard Identifier for the received message. */
  };
  BYTE cData;
}CANSPI_REG_RXBnSIDL;

//! RXBnEIDH: RECEIVE BUFFER x EXTENDED IDENTIFIER HIGH REGISTER (ADDRESS: 63h, 73h)
typedef union
{
  struct
  {
    BYTE EIDHL; /* (R)Extended Identifier bits [15:8]
                   These bits hold bits 15 through 8 of the Extended Identifier for the received message. */
  };
  BYTE cData;
}CANSPI_REG_RXBnEIDH;

//! RXBnEIDH: RECEIVE BUFFER x EXTENDED IDENTIFIER LOW REGISTER (ADDRESS: 64h, 74h)
typedef union
{
  struct
  {
    BYTE EIDL; /* (R)Extended Identifier bits [7:0]
                  These bits hold the Least Significant eight bits of the Extended Identifier for the received message. */
  };
  BYTE cData;
}CANSPI_REG_RXBnEIDL;

//! RXBnDLC: RECEIVE BUFFER x DATA LENGTH CODE REGISTER (ADDRESS: 65h, 75h)
typedef union 
{
  struct 
  {
    BYTE DLC            : 4; /* (R)Data Length Code bits
                                Indicates the number of data bytes that were received. */
    BYTE RB0            : 1; /* (R)Reserved Bit 0 */
    BYTE RB1            : 1; /* (R)Reserved Bit 1 */
    BYTE RTR            : 1; /* (R)Extended Frame Remote Transmission Request bit
                                (valid only when the IDE bit in the RXBxSIDL register is ‘1’)
                                  1 = Extended frame Remote Transmit Request received
                                  0 = Extended data frame received */
    BYTE Unimplemented1 : 1;
  };
  BYTE cData;
}CANSPI_REG_RXBnDLC;

//! RXBnDn: RECEIVE BUFFER x DATA BYTE n REGISTER (ADDRESS: 66h-6Dh, 76h-7Dh)
typedef union
{
  struct
  {
    BYTE RBnD;  /* (R)Receive Buffer x Data Field Bytes n bits
                   Eight bytes containing the data bytes for the received message. */
  };
  BYTE cData;
}CANSPI_REG_RXBnDn;

//! RXFxSIDH: FILTER x STANDARD IDENTIFIER HIGH REGISTER (ADDRESS: 00h, 04h, 08h, 10h, 14h, 18h)
typedef union
{
  struct
  {
    BYTE SIDH;  /* Standard Identifier Filter bits [10:3]
                    These bits hold the filter bits to be applied to bits[10:3] 
                    of the Standard Identifier portion of a received message. */
  };
  BYTE cData;
}CANSPI_REG_RXFnSIDH;

//! RXFxSIDL: FILTER x STANDARD IDENTIFIER LOW REGISTER (ADDRESS: 01h, 05h, 09h, 11h, 15h, 19h)
typedef union
{
  struct
  {
    BYTE EIDHH          : 2; /* Extended Identifier Filter bits [17:16] 
                                These bits hold the filter bits to be applied to bits[17:16] 
                                of the Extended Identifier portion of a received message. */
    BYTE Unimplemented0 : 1;
    BYTE EXIDE          : 1; /* Extended Identifier Enable bit
                                1 = Filter is applied only to extended frames
                                0 = Filter is applied only to standard frames */
    BYTE Unimplemented1 : 1; 
    BYTE SIDL           : 3;  /* Standard Identifier Filter bits [2:0] 
                                  These bits hold the filter bits to be applied to bits[2:0] 
                                  of the Standard Identifier portion of a received message. */
  };
  BYTE cData;
}CANSPI_REG_RXFnSIDL;

//! RXFnEIDH: FILTER x EXTENDED IDENTIFIER HIGH REGISTER (ADDRESS: 02h, 06h, 0Ah, 12h, 16h, 1Ah)
typedef union
{
  struct
  {
    BYTE EIDHL; /* Extended Identifier bits [15:8]
                   These bits hold the filter bits to be applied to bits[15:8] of the Extended Identifier portion of a received
                   message or to Byte 0 in received data if corresponding with RXM[1:0] = 00 and EXIDE = 0. */
  };
  BYTE cData;
}CANSPI_REG_RXFnEIDH;

//! RXFnEIDL: FILTER x EXTENDED IDENTIFIER LOW REGISTER (ADDRESS: 03h, 07h, 0Bh, 13h, 17h, 1Bh)
typedef union
{
  struct
  {
    BYTE EIDL; /* Extended Identifier bits [7:0]
                  These bits hold the filter bits to be applied to bits[7:0] of the Extended Identifier portion of a received
                  message or to Byte 1 in received data if corresponding with RXM[1:0] = 00 and EXIDE = 0. */
  };
  BYTE cData;
}CANSPI_REG_RXFnEIDL;

//! RXMnSIDH: MASK x STANDARD IDENTIFIER HIGH REGISTER (ADDRESS: 20h, 24h)
typedef union
{
  struct
  {
    BYTE SIDH; /* Standard Identifier Mask bits [10:3] 
                  These bits hold the mask bits to be applied to bits[10:3] 
                  of the Standard Identifier portion of a received message. */
  };
  BYTE cData;
}CANSPI_REG_RXMnSIDH;

//! RXMnSIDL: MASK x STANDARD IDENTIFIER LOW REGISTER (ADDRESS: 21h, 25h)
typedef union
{
  struct
  {
    BYTE EIDHH          : 2; /* Extended Identifier Mask bits [17:16] 
                                These bits hold the mask bits to be applied to bits[17:16] 
                                of the Extended Identifier portion of a received message. */
    BYTE Unimplemented0 : 3;
    BYTE SIDL           : 3;  /* Standard Identifier Mask bits [2:0] 
                                 These bits hold the mask bits to be applied to bits[2:0] 
                                 of the Standard Identifier portion of a received message. */
  };
  BYTE cData;
}CANSPI_REG_RXMnSIDL;

//! RXMxEIDH: MASK x EXTENDED IDENTIFIER HIGH REGISTER (ADDRESS: 22h, 26h)
typedef union
{
  struct
  {
    BYTE EIDHL; /* Extended Identifier bits [15:8] 
                   These bits hold the filter bits to be applied to bits[15:8] 
                   of the Extended Identifier portion of a received message. 
                   If corresponding with RXM[1:0] = 00 and EXIDE = 0, these bits are applied to Byte 0 in
                   received data. */
  };
  BYTE cData;
}CANSPI_REG_RXMnEIDH;


//! RXMxEID0: MASK x EXTENDED IDENTIFIER LOW REGISTER (ADDRESS: 23h, 27h)
typedef union
{
  struct
  {
    BYTE EIDL; /* Extended Identifier Mask bits [7:0]
                  These bits hold the filter bits to be applied to bits[7:0] 
                  of the Extended Identifier portion of a received message. 
                  If corresponding with RXM[1:0] = 00 and EXIDE = 0, these bits are applied to Byte 1 in
                  received data. */
  };
  BYTE cData;
}CANSPI_REG_RXMnEIDL;



// Bit Time Configuration Registers ---------------------------------------------- //

//! CNF1: CONFIGURATION 1 REGISTER (ADDRESS: 2Ah)
typedef union 
{
  struct 
  {
    BYTE BRP : 6; /* Baud Rate Prescaler bits
                      TQ = 2 x (BRP[5:0] + 1)/FOSC */
    BYTE SJW : 2; /* Synchronization Jump Width Length bits
                        11 = Length = 4 x TQ
                        10 = Length = 3 x TQ
                        01 = Length = 2 x TQ
                        00(*) = Length = 1 x TQ */
  };
  BYTE cData;
}CANSPI_REG_CNF1;

//! CNF2: CONFIGURATION 2 REGISTER (ADDRESS: 29h)
typedef union 
{
  struct 
  {
    BYTE PropSeg      : 3; /* Propagation Segment Length bits
                              (PRSEG[2:0] + 1) x TQ */
    BYTE PhaseSeg1    : 3; /* PS1 Length bits
                              (PHSEG1[2:0] + 1) x TQ */
    BYTE BusSampling  : 1; /* Sample Point Configuration bit
                                1 = Bus line is sampled three times at the sample point
                                0(*) = Bus line is sampled once at the sample point */
    BYTE BTLMode      : 1; /* PS2 Bit Time Length bit
                                1 = Length of PS2 is determined by the PHSEG2[2:0] bits of CNF3
                                0(*) = Length of PS2 is the greater of PS1 and IPT (2 TQ) */
  };
  BYTE cData;
}CANSPI_REG_CNF2;

//! CNF3: CONFIGURATION 3 REGISTER (ADDRESS: 28h)
typedef union 
{
  struct 
  {
    BYTE PhaseSeg2          : 3; /* PS2 Length bits
                                      (PHSEG2[2:0] + 1) x TQ
                                      Minimum valid setting for PS2 is 2 TQ. */
    BYTE Unimplemented0     : 3;
    BYTE WakeUpFilterEnable : 1; /* Wake-up Filter bit
                                      1 = Wake-up filter is enabled
                                      0(*) = Wake-up filter is disabled */
    BYTE SOF                : 1; /* Start-of-Frame Signal bit
                                    If CLKEN (CANCTRL[2]) = 1:
                                    1 = CLKOUT pin is enabled for SOF signal
                                    0(*) = CLKOUT pin is enabled for clock out function
                                    If CLKEN (CANCTRL[2]) = 0:
                                    Bit is don’t care. */
  };
  BYTE cData;
}CANSPI_REG_CNF3;



// Error Detection Registers ---------------------------------------------- //

//! TEC: TRANSMIT ERROR COUNTER REGISTER (ADDRESS: 1Ch)
typedef union
{
  struct
  {
    BYTE TEC; /* Transmit Error Count bits */
  };
  BYTE cData;
}CANSPI_REG_TEC;

//! REC: RECEIVER ERROR COUNTER REGISTER (ADDRESS: 1Dh)
typedef union
{
  struct
  {
    BYTE REC; /* Receive Error Count bits */
  };
  BYTE cData;
}CANSPI_REG_REC;


//! EFLG: ERROR FLAG REGISTER (ADDRESS: 2Dh)
typedef union 
{
  struct 
  {
    BYTE ERR_WARN       : 1; /* (R)Error Warning Flag bit
                                  - Sets when TEC or REC is equal to or greater than 96 (TXWAR or RXWAR = 1)
                                  - Resets when both REC and TEC are less than 96 */
    BYTE RXERR_WARN     : 1; /* (R)Receive Error Warning Flag bit
                                  - Sets when REC is equal to or greater than 96
                                  - Resets when REC is less than 96 */
    BYTE TXERR_WARN     : 1; /* (R)Transmit Error Warning Flag bit
                                  - Sets when TEC is equal to or greater than 96
                                  - Resets when TEC is less than 96 */
    BYTE RXERR_PASSIVE  : 1; /* (R)Receive Error-Passive Flag bit
                                  - Sets when REC is equal to or greater than 128
                                  - Resets when REC is less than 128 */
    BYTE TXERR_PASSIVE  : 1; /* (R)Transmit Error-Passive Flag bit
                                  - Sets when TEC is equal to or greater than 128
                                  - Resets when TEC is less than 128 */
    BYTE TX_BUS_OFF     : 1; /* (R)Bus-Off Error Flag bit
                                  - Bit sets when TEC reaches 255
                                  - Resets after a successful bus recovery sequence */
    BYTE RXB0_OVR       : 1; /* Receive Buffer 0 Overflow Flag bit
                                  - Sets when a valid message is received for RXB0 
                                    and the RX0IF bit in the CANINTF register is ‘1’
                                  - Must be reset by MCU */
    BYTE RXB1_OVR       : 1; /* Receive Buffer 1 Overflow Flag bit
                                - Sets when a valid message is received for RXB1 
                                  and the RX1IF bit in the CANINTF register is ‘1’
                                - Must be reset by MCU */
  };
  BYTE cData;
}CANSPI_REG_ERRFLAG;



// Interrupt Registers ---------------------------------------------- //

//! CANINTE: CAN INTERRUPT ENABLE REGISTER (ADDRESS: 2Bh)
typedef union
{
  struct
  {
    BYTE RX0IE    : 1; /* Receive Buffer 0 Full Interrupt Enable bit
                            1 = Interrupt when message is received in RXB0
                            0(*) = Disabled */
    BYTE RX1IE    : 1; /* Receive Buffer 1 Full Interrupt Enable bit
                            1 = Interrupt when message is received in RXB1
                            0(*) = Disabled */
    BYTE TX0IE    : 1; /* Transmit Buffer 0 Empty Interrupt Enable bit
                            1 = Interrupt on TXB0 becoming empty
                            0(*) = Disabled */
    BYTE TX1IE    : 1; /* Transmit Buffer 1 Empty Interrupt Enable bit
                            1 = Interrupt on TXB1 becoming empty
                            0(*) = Disabled */
    BYTE TX2IE    : 1; /* Transmit Buffer 2 Empty Interrupt Enable bit
                            1 = Interrupt on TXB2 becoming empty
                            0(*) = Disabled */
    BYTE ERRIE    : 1; /* Error Interrupt Enable bit (multiple sources in the EFLG register)
                            1 = Interrupt on EFLG error condition change
                            0(*) = Disabled */
    BYTE WAKIE    : 1; /* Wake-up Interrupt Enable bit
                            1 = Interrupt on CAN bus activity
                            0(*) = Disabled */
    BYTE MERRE    : 1; /* Message Error Interrupt Enable bit
                            1 = Interrupt on error during message reception or transmission
                            0(*) = Disabled */
  };
  BYTE cData;
}CANSPI_REG_CANINTE;

//! CANINTF: CAN INTERRUPT FLAG REGISTER (ADDRESS: 2Ch)
typedef union
{
  struct
  {
    BYTE RX0IF    : 1; /* Receive Buffer 0 Full Interrupt Flag bit
                            1 = Interrupt pending (must be cleared by MCU to reset interrupt condition)
                            0(*) = No interrupt pending */
    BYTE RX1IF    : 1; /* Receive Buffer 1 Full Interrupt Flag bit
                            1 = Interrupt pending (must be cleared by MCU to reset interrupt condition)
                            0(*) = No interrupt pending */
    BYTE TX0IF    : 1; /* Transmit Buffer 0 Empty Interrupt Flag bit
                            1 = Interrupt pending (must be cleared by MCU to reset interrupt condition)
                            0(*) = No interrupt pending */
    BYTE TX1IF    : 1; /* Transmit Buffer 1 Empty Interrupt Flag bit
                            1 = Interrupt pending (must be cleared by MCU to reset interrupt condition)
                            0(*) = No interrupt pending */
    BYTE TX2IF    : 1; /* Transmit Buffer 2 Empty Interrupt Flag bit
                            1 = Interrupt pending (must be cleared by MCU to reset interrupt condition)
                            0(*) = No interrupt pending */
    BYTE ERRIF    : 1; /* Error Interrupt Flag bit (multiple sources in the EFLG register)
                            1 = Interrupt pending (must be cleared by MCU to reset interrupt condition)
                            0(*) = No interrupt pending */
    BYTE WAKIF    : 1; /* Wake-up Interrupt Flag bit
                            1 = Interrupt pending (must be cleared by MCU to reset interrupt condition)
                            0(*) = No interrupt pending */
    BYTE MERRF    : 1; /* Message Error Interrupt Flag bit
                            1 = Interrupt pending (must be cleared by MCU to reset interrupt condition)
                            0(*) = No interrupt pending */
  };
  BYTE cData;
}CANSPI_REG_CANINTF;



// CAN Control Registers ---------------------------------------------- //


//! CANCTRL: CAN CONTROL REGISTER (ADDRESS: XFh)
typedef union 
{
  struct 
  {
    BYTE ClockOutPrescaler  : 2; /* CLKOUT Pin Prescaler bits
                                      00 = FCLKOUT = System Clock/1
                                      01 = FCLKOUT = System Clock/2
                                      10 = FCLKOUT = System Clock/4
                                      11(*) = FCLKOUT = System Clock/8 */
    BYTE ClockOutEnable     : 1; /* CLKOUT Pin Enable bit
                                      1(*) = CLKOUT pin is enabled
                                      0 = CLKOUT pin is disabled (pin is in a high-impedance state) */
    BYTE OneShotMode        : 1; /* One-Shot Mode bit
                                      1 = Enabled: Message will only attempt to transmit one time
                                      0(*) = Disabled: Messages will reattempt transmission if required */
    BYTE TxAbortAll         : 1; /* Abort All Pending Transmissions bit
                                      1 = Requests abort of all pending transmit buffers
                                      0(*) = Terminates request to abort all transmissions */
    BYTE OpModeRequest      : 3; /* Request Operation mode bits
                                      000 = Sets Normal Operation mode
                                      001 = Sets Sleep mode
                                      010 = Sets Loopback mode
                                      011 = Sets Listen-Only mode
                                      100(*) = Sets Configuration mode
                                      All other values for the REQOPx bits are invalid and should not be used. 
                                      On power-up, REQOP = b’100. */
  };
  BYTE cData;
}CANSPI_REG_CANCTRL;

//! CANSTAT: CAN STATUS REGISTER (ADDRESS: XEh)
typedef union 
{
  struct 
  {
    BYTE Unimplemented0 : 1;
    BYTE InterruptCode  : 3; /* (R)Interrupt Flag Code bits
                                  000 = No Interrupt
                                  001 = Error interrupt
                                  010 = Wake-up interrupt
                                  011 = TXB0 interrupt
                                  100 = TXB1 interrupt
                                  101 = TXB2 interrupt
                                  110 = RXB0 interrupt
                                  111 = RXB1 interrupt */
    BYTE Unimplemented1 : 1;
    BYTE OpMode         : 3;  /* (R)Operation Mode bits
                                  000 = Device is in the Normal Operation mode
                                  001 = Device is in Sleep mode
                                  010 = Device is in Loopback mode
                                  011 = Device is in Listen-Only mode
                                  100 = Device is in Configuration mode */
  };
  BYTE cData;
}CANSPI_REG_CANSTAT;


// ---------------------------------------------------------------------------
// CAN SPI Object definitions
// ---------------------------------------------------------------------------
//! Propagation Segment Time Quanta
typedef enum 
{
  canspiPROP_1TQ,
  canspiPROP_2TQ,
  canspiPROP_3TQ,
  canspiPROP_4TQ,
  canspiPROP_5TQ,
  canspiPROP_6TQ,
  canspiPROP_7TQ,
  canspiPROP_8TQ,

  canspiPROP_INVALID
}eCANSPI_PROPSEG_TQ;

//! Phase Segment1 Time Quanta
typedef enum 
{
  canspiPHSEG1_1TQ,
  canspiPHSEG1_2TQ,
  canspiPHSEG1_3TQ,
  canspiPHSEG1_4TQ,
  canspiPHSEG1_5TQ,
  canspiPHSEG1_6TQ,
  canspiPHSEG1_7TQ,
  canspiPHSEG1_8TQ,

  canspiPHSEG1_INVALID
}eCANSPI_PHSEG1_TQ;

//! Phase Segment2 Time Quanta
typedef enum 
{
  ///canspiPHSEG2_1TQ,
  canspiPHSEG2_2TQ = 1,
  canspiPHSEG2_3TQ,
  canspiPHSEG2_4TQ,
  canspiPHSEG2_5TQ,
  canspiPHSEG2_6TQ,
  canspiPHSEG2_7TQ,
  canspiPHSEG2_8TQ,

  canspiPHSEG2_INVALID
}eCANSPI_PHSEG2_TQ;

//! Synchronization Jump Width
typedef enum 
{
  canspiSJW_1TQ,
  canspiSJW_2TQ,
  canspiSJW_3TQ,
  canspiSJW_4TQ,
  
  canspiSJW_INVALID
}eCANSPI_SJW_TQ;

//! Sample Point Configuration
typedef enum 
{
  canspiSAMPLE_ONCE,
  canspiSAMPLE_TRICE,
  
  canspiSAMPLE_INVALID
}eCANSPI_SAM_CONFIG;

//! Phase Seg2 Bit Time Length
typedef enum 
{
  canspiPHSEG2_BTL_MIN,
  canspiPHSEG2_BTL_CNF3,
  
  canspiPHSEG2_BTL_INVALID
}eCANSPI_PHSEG2_CONFIG;

//! Start of Frame Output
typedef enum 
{
  canspiPIN_CLKO,
  canspiPIN_SOF,
  
  canspiPIN_INVALID
}eCANSPI_CLKO_SOF_CONFIG;

//! Wake Up Filter Enable
typedef enum 
{
  canspiWAKFIL_DISABLE,
  canspiWAKFIL_ENABLE
}eCANSPI_WAKFIL_CONFIG;


// ---------------------------------------------------------------------------
// CAN SPI Message Objects definitions
// ---------------------------------------------------------------------------
//! Read Status
typedef union tagDRV_CANSPI_REG_BUFFER_STATUS 
{
  struct 
  {
    BYTE RX0IF    : 1; /* Receive Buffer 0 Full Interrupt Flag bit
                            1 = Interrupt pending (must be cleared by MCU to reset interrupt condition)
                            0(*) = No interrupt pending */
    BYTE RX1IF    : 1; /* Receive Buffer 1 Full Interrupt Flag bit
                            1 = Interrupt pending (must be cleared by MCU to reset interrupt condition)
                            0(*) = No interrupt pending */
    BYTE TX0REQ   : 1; /* Message Transmit Request bit
                            1 = Buffer is currently pending transmission 
                                (MCU sets this bit to request message be transmitted –
                                bit is automatically cleared when the message is sent)
                            0 = Buffer is not currently pending transmission (*)
                                (MCU can clear this bit to request a message abort) */
    BYTE TX0IF    : 1; /* Transmit Buffer 0 Empty Interrupt Flag bit
                            1 = Interrupt pending (must be cleared by MCU to reset interrupt condition)
                            0(*) = No interrupt pending */
    BYTE TX1REQ   : 1; /* Message Transmit Request bit
                            1 = Buffer is currently pending transmission 
                                (MCU sets this bit to request message be transmitted –
                                bit is automatically cleared when the message is sent)
                            0 = Buffer is not currently pending transmission (*)
                                (MCU can clear this bit to request a message abort) */
    BYTE TX1IF    : 1; /* Transmit Buffer 1 Empty Interrupt Flag bit
                            1 = Interrupt pending (must be cleared by MCU to reset interrupt condition)
                            0(*) = No interrupt pending */
    BYTE TX2REQ   : 1; /* Message Transmit Request bit
                            1 = Buffer is currently pending transmission 
                                (MCU sets this bit to request message be transmitted –
                                bit is automatically cleared when the message is sent)
                            0 = Buffer is not currently pending transmission (*)
                                (MCU can clear this bit to request a message abort) */ 
    BYTE TX3IF    : 1; /* Transmit Buffer 2 Empty Interrupt Flag bit
                            1 = Interrupt pending (must be cleared by MCU to reset interrupt condition)
                            0(*) = No interrupt pending */
  };
  BYTE cData;
}CANSPI_REG_BUFFER_STATUS;

//! Receive Modes
typedef enum 
{
  canspiRXM_USE_FILTER  = 0x0,
  canspiRXM_RECEIVE_ALL = 0x3
}eCANSPI_RXM_MODE;

//! Rollover to RXB1 if RXB0 is full
typedef enum 
{
  canspiRXB_ROLLOVER_DISABLED,
  canspiRXB_ROLLOVER_ENABLED
}eCANSPI_RXB_ROLLOVER;

//! Receive Mode configuration
typedef struct tagCANSPI_RX_CONFIG 
{
  unsigned RXB0_ReceiveMode : 2;
  unsigned RXB1_ReceiveMode : 2;
  unsigned RXB_RollOver : 1;
}CANSPI_RX_CONFIG, *PCANSPI_RX_CONFIG;
typedef CROMDATA CANSPI_RX_CONFIG *PCCANSPI_RX_CONFIG;

//! CAN Bit time Configure
typedef struct tagCANSPI_BIT_TIME_CONFIG 
{
  DWORD BRP                : 6;
  DWORD PropSeg            : 3;
  DWORD PhaseSeg1          : 3;
  DWORD PhaseSeg2          : 3;
  DWORD SJW                : 2;
  DWORD BusSampling        : 1;
  DWORD PhaseSeg2Config    : 1;
  DWORD SOFPinConfig       : 1;
  DWORD WakeUpFilterEnable : 1;
}CANSPI_BIT_TIME_CONFIG, *PCANSPI_BIT_TIME_CONFIG;
typedef CROMDATA CANSPI_BIT_TIME_CONFIG *PCCANSPI_BIT_TIME_CONFIG;


/**
  * @}
  */

/** @defgroup dvMCP25625_Private_Macros
  * @{
  */
#define CANSPI_CHIP_SEL()  dvGPIOFieldSet(gfGPO_CAN_nCS, LOW)
#define CANSPI_CHIP_REL()  dvGPIOFieldSet(gfGPO_CAN_nCS, HIGH)

// Code anchor for break points
#define NOP() ///asm("nop")


/**
  * @}
  */

/** @defgroup dvMCP25625_Private_Variables
  * @{
  */

static CROMDATA BYTE m_acDeviceBuses[MCP25625_NUM_INSTANCES] =
{
/* ------------------------- BEGIN CUSTOM AREA  -------------------------------
  CONFIGURATION: Device bus for each driver instance */
  cpuSPI1
/* ---------------------------- END CUSTOM AREA --------------------------- */
};

static CROMDATA SPI_INIT_STRUCT m_sSPIMCP25625InitStruct = 
{
/* ------------------------- BEGIN CUSTOM AREA  -------------------------------  
  CONFIGURATION: Device Bus 3Wire init structure */
  .wFlags = (DEVICE_3WIRE_DUMMY_00),
  .sInitType = 
  {
    .cSPICon  = (SPICON_SPISIZE8B | SPICON_MSBF | SPICON_MSTR /*| SPICON_CPOL | SPICON_CPHA*/),
    .dwFreq   = 8000000, // support up to 10MHz
  }
/* ---------------------------- END CUSTOM AREA --------------------------- */  
};


/* CAN Buffer Status */
static CANSPI_REG_BUFFER_STATUS m_sCANSPICANStatus;

/* CAN Operation Mode */
///static eCANSPI_OPERATION_MODE   m_eCANSPIOPMode;

/* Filter Objects */
static CANSPI_FILTEROBJ_ID      m_asCANSPIFilterObj[canspiFILTER_INVALID];

/* Mask Objects */
static CANSPI_MASKOBJ_ID        m_asCANSPIMaskObj[canspiMASK_INVALID];

// structure to CAN Tx Message 
static CAN_TX_MSG               m_sCanTxMsg = {0};

// Current count of message received
static WORD                   m_wRxCount;

// Number of message available in receive buffer
static WORD                   m_wRxCountMax;

// State of serial driver
static eCAN_STATUS            m_eCANStatus;

// pointer to CAN received frame Que
static PCAN_FRAME_QUE_RX        m_psCanFrameQueRx;



/**
  * @}
  */

/** @defgroup dvMCP25625_Private_FunctionPrototypes
  * @{
  */

/**
  * @}
  */

/** @defgroup dvMCP25625_Private_Functions
  * @{
  */

/**
 *  @brief      Writes and Read One byte to a hardware three wire bus                
                cWriteData: Write Data
 *  @return     1Byte Data that read
 *  @note       None
 *  @attention  None
 */
static BYTE CANSPIOneByteSend(BYTE cWriteData)
{
  BYTE cReadData;
  
  CANSPI_CHIP_SEL();
  cReadData = dvDeviceBus3WireOneByteSend((eCPU_SPI)m_acDeviceBuses[0]
                                      , cWriteData 
                                      , (PCSPI_INIT_STRUCT)&m_sSPIMCP25625InitStruct);
  CANSPI_CHIP_REL();
  
  return cReadData;                                      
}

/**
 *  @brief      Reads and writes individual bits to a hardware three wire bus
 *  @param[in]  dwWriteCount: Write Buffer Size. Can be 0
                pacWriteBuffer: Pointer to Write Buffer. Can be NULL
                dwReadCount: Read Buffer Size. Can be 0                
 *  @param[out] pacReadBuffer: Pointer to Read Buffer. Can be NULL
 *  @return     rcSUCCESS if operation succeeds, rcERROR otherwise
 *  @note       None
 *  @attention  None
 */
static eRESULT CANSPITransaction(DWORD             dwWriteCount
                                        , PCBYTE            pacWriteBuffer
                                        , DWORD             dwReadCount
                                        , PBYTE             pacReadBuffer) 
{
  eRESULT eResult = rcSUCCESS;

  CANSPI_CHIP_SEL();      
  eResult = dvDeviceBus3WireTransaction((eCPU_SPI)m_acDeviceBuses[0]
                              , dwWriteCount
                              , pacWriteBuffer 
                              , dwReadCount
                              , pacReadBuffer
                              , (PCSPI_INIT_STRUCT)&m_sSPIMCP25625InitStruct);
  CANSPI_CHIP_REL();

  return eResult;
}

/**
 *  @brief      Read 1 Register value as specified address
 *  @param[in]  cAddress: Register address to read from
 *  @return     1Byte Register value that read
 *  @note       None
 *  @attention  None
 */
static BYTE CANSPIRegRead(BYTE cAddress)
{
  BYTE cData       = 0;
  BYTE acBuffer[2] = {CANSPI_CMD_READ, cAddress};

  CANSPITransaction(sizeof(acBuffer), acBuffer, 1, &cData);

  return cData;
}

/**
 *  @brief      Write 1 Register value as specified address
 *  @param[in]  cAddress: Register address to Write to
                cData: Register value to write to
 *  @return     rcSUCCESS= No error occurred and returned result is complete
                rcERROR= An error occurred during the operation
 *  @note       None
 *  @attention  None
 */
static eRESULT CANSPIRegWrite(BYTE cAddress, BYTE cData)
{
  BYTE acBuffer[3] = {CANSPI_CMD_WRITE, cAddress, cData};

  return CANSPITransaction(sizeof(acBuffer), acBuffer, 0, NULL);
}

/**
 *  @brief      Mofity 1 Register Data as specified address and Mask
 *  @param[in]  cAddress: Register address to mofity to
                cMask: Mask to modity to
                cData: Register value to mofity to
 *  @return     rcSUCCESS= No error occurred and returned result is complete
                rcERROR= An error occurred during the operation
 *  @note       None
 *  @attention  None
 */
static eRESULT CANSPIRegBitModify(BYTE cAddress, BYTE cMask, BYTE cData)
{

  BYTE acBuffer[4] = {CANSPI_CMD_BIT_MODIFY, cAddress, cMask, cData};  

  return CANSPITransaction(sizeof(acBuffer), acBuffer, 0, NULL);
}

/**
 *  @brief      Write the register values as specified register address
 *  @param[in]  cAddress: Register address to write to
                pacWriteBuffer: pointer to write buffer
                cWriteCount: byte size to write to
 *  @return     rcSUCCESS = No error occurred and returned result is complete
                rcERROR   = An error occurred during the operation
 *  @note       None
 *  @attention  None
 */
static eRESULT CANSPIWrites(BYTE cAddress, PCBYTE pacWriteBuffer, BYTE cWriteCount)
{
  eRESULT   eResult       = rcSUCCESS;  
  BYTE      acBuffer[257] = {CANSPI_CMD_WRITE, cAddress, };

  memcpy(&acBuffer[2], pacWriteBuffer, cWriteCount);  
  eResult = CANSPITransaction(cWriteCount+2, (PCBYTE)acBuffer, 0, NULL); 

  return eResult;    
}

/**
 *  @brief      Read register values from register address as specified read size
 *  @param[in]  cAddress: Register address to read to
                cReadCount: byte size to read
 *  @param[out] pacReadBuffer: pointer to return back read value                
 *  @return     rcSUCCESS = No error occurred and returned result is complete
                rcERROR   = An error occurred during the operation
 *  @note       None
 *  @attention  None
 */
static eRESULT CANSPIReads(BYTE cAddress, PBYTE pacReadBuffer, BYTE cReadCount)
{
  eRESULT eResult = rcSUCCESS;
  BYTE    acBuffer[2] = {CANSPI_CMD_READ, cAddress};  

  eResult = CANSPITransaction(sizeof(acBuffer), (PCBYTE)acBuffer, cReadCount, pacReadBuffer);   

  return eResult;    
}

#if !defined(NDEBUG) // avoid compiler warn
/**
 *  @brief      Get the CAN Operation mode
 *  @param[in]  None
 *  @return     CAN Operation Mode                
 *  @note       None
 *  @attention  None
 */
static eCANSPI_OPERATION_MODE CANSPIOperationModeGet(void)
{
  CANSPI_REG_CANSTAT sCANStatus;

  NOP();
  sCANStatus.cData = CANSPIRegRead(cREGADDR_CANSTAT);

  return sCANStatus.OpMode;
}
#endif

/**
 *  @brief      Set the CAN Operation mode
 *  @param[in]  eOPMode: CAN Operation Mode to set
 *  @return     rcSUCCESS = No error occurred and returned result is complete
                rcERROR   = An error occurred during the operation               
 *  @note       None
 *  @attention  None
 */
static eRESULT CANSPIOperationModeSet(eCANSPI_OPERATION_MODE eOPMode)
{
  eRESULT eResult = rcSUCCESS;  
  CANSPI_REG_CANCTRL sCtrl;
  CANSPI_REG_CANSTAT sCANStatus;

  /* Read */
  NOP();
  sCtrl.cData = CANSPIRegRead(cREGADDR_CANCTRL);

  /* Modify */
  sCtrl.OpModeRequest     = eOPMode;
  sCtrl.ClockOutPrescaler = 0;
#if !USE_CAN_AUTORETRANMISSION  
  sCtrl.OneShotMode       = 1; /* Make One Shot Mode */
#endif

  /* Write */
  eResult |= CANSPIRegWrite(cREGADDR_CANCTRL, sCtrl.cData);

  /* Wait till mode has changed */
  sCANStatus.cData = CANSPIRegRead(cREGADDR_CANSTAT);

  while (sCANStatus.OpMode != sCtrl.OpModeRequest) 
  {
    sCANStatus.cData = CANSPIRegRead(cREGADDR_CANSTAT);
    NOP();
    NOP();

    if(sCtrl.OpModeRequest == canspiMODE_SLEEP 
        && sCANStatus.OpMode == canspiMODE_LISTEN_ONLY) 
    {
      // Device already woke up while in sleep mode
      NOP();
      NOP();
      NOP();
      break;
    }
  }

  return eResult;
}

/**
 *  @brief      Configure the CANSPI Bit Time and so on
 *  @param[in]  psConfig: CANSPI Configurations
 *  @return     rcSUCCESS = No error occurred and returned result is complete
                rcERROR   = An error occurred during the operation               
 *  @note       None
 *  @attention  None
 */
static eRESULT CANSPIConfigurationSet(PCCANSPI_BIT_TIME_CONFIG pcsConfig)
{
  eRESULT eResult = rcSUCCESS;  
  CANSPI_REG_CNF1 sCNF1;
  CANSPI_REG_CNF2 sCNF2;
  CANSPI_REG_CNF3 sCNF3;

  sCNF1.BRP = pcsConfig->BRP;
  sCNF1.SJW = pcsConfig->SJW;

  sCNF2.PropSeg     = pcsConfig->PropSeg;
  sCNF2.PhaseSeg1   = pcsConfig->PhaseSeg1;
  sCNF2.BusSampling = pcsConfig->BusSampling;
  sCNF2.BTLMode     = pcsConfig->PhaseSeg2Config;

  sCNF3.PhaseSeg2           = pcsConfig->PhaseSeg2;
  sCNF3.WakeUpFilterEnable  = pcsConfig->WakeUpFilterEnable;
  sCNF3.SOF                 = pcsConfig->SOFPinConfig;

  eResult |= CANSPIRegWrite(cREGADDR_CNF1, sCNF1.cData);
  eResult |= CANSPIRegWrite(cREGADDR_CNF2, sCNF2.cData);
  eResult |= CANSPIRegWrite(cREGADDR_CNF3, sCNF3.cData);

  return eResult;
}

/**
 *  @brief      Load to transmit message to specified TX Channel
 *  @param[in]  eChannel: TX Channel to transmit
                pcsCanTxMsg: Pointer to CAN Tx Frame to transmit
 *  @return     rcSUCCESS = No error occurred and returned result is complete
                rcERROR   = An error occurred during the operation               
 *  @note       need call CANSPITransmitChannelFlush() after this
 *  @attention  None
 */
static eRESULT CANSPITransmitChannelLoad(eCANSPI_TX_CHANNEL eChannel
                                                  , PCCAN_TX_MSG pcsCanTxMsg)
{
  eRESULT eResult = rcSUCCESS;
  BYTE    acBuffer[CANSPI_TX_BUFFER_SIZE];
  BYTE    cAddress;  
  DWORD   dwReg       = 0;
  BYTE    cWriteSize  = 0;
  BYTE    cI;
  BYTE    cMaxSize;
  CANSPI_REG_TXBnSIDL sSIDL;
  CANSPI_REG_TXBnDLC  sDLC;

  /* TXBnSIDH */
  NOP();
  NOP();
  dwReg = pcsCanTxMsg->StdId >> 3;
  acBuffer[cWriteSize++] = (dwReg & 0xFF);

  /* TXBnSIDL */
  sSIDL.cData   = 0;
  sSIDL.SIDL    = pcsCanTxMsg->StdId & 0x7;
  sSIDL.IDE     = pcsCanTxMsg->IDE;
  dwReg         = pcsCanTxMsg->ExtId >> 16;
  sSIDL.EIDHH   = dwReg & 0x3;
  acBuffer[cWriteSize++] = sSIDL.cData;

  /* TXBnEID8 */
  dwReg = pcsCanTxMsg->ExtId >> 8;
  acBuffer[cWriteSize++] = dwReg & 0xFF;

  /* TXBnEID0 */
  acBuffer[cWriteSize++] = pcsCanTxMsg->ExtId & 0xFF;

  /* TXBnDLC */
  sDLC.cData  = 0;
  sDLC.DLC    = pcsCanTxMsg->DLC;
  sDLC.RTR    = pcsCanTxMsg->RTR;
  acBuffer[cWriteSize++] = sDLC.cData;

  /* Data Bytes */
  if(cMaxSize > 8) 
    cMaxSize = 8;  

  /* Copy data */
  for(cI = 0; cI < cMaxSize; cI++)
  {
    acBuffer[cWriteSize++] = pcsCanTxMsg->Data[cI];
  }

  /* Tx Buffer Address */
  switch (eChannel) 
  {
    case canspiTX_CH0:  cAddress = cREGADDR_TXB0ID; break;
    case canspiTX_CH1:  cAddress = cREGADDR_TXB1ID; break;
    case canspiTX_CH2:  cAddress = cREGADDR_TXB2ID; break;
    default:            cAddress = cREGADDR_TXB0ID; break;
  }

  /* Write buffer */
  eResult |= CANSPIWrites(cAddress, (PCBYTE)acBuffer, cWriteSize);

  return eResult;    
}

/**
 *  @brief      Flush the loaded message in specified TX Channel
 *  @param[in]  eChannel: TX Channel to Flush
 *  @return     rcSUCCESS = No error occurred and returned result is complete
                rcERROR   = An error occurred during the operation               
 *  @note       None
 *  @attention  None
 */
static eRESULT CANSPITransmitChannelFlush(eCANSPI_TX_CHANNEL eChannel)
{
  eRESULT eResult = rcSUCCESS;
  BYTE    cCmd = 0;
  
  NOP();
  NOP();

  /* Get command */
  switch(eChannel)
  {
    case canspiTX_CH0:  cCmd = CANSPI_CMD_RTS_TXB0; break;
    case canspiTX_CH1:  cCmd = CANSPI_CMD_RTS_TXB1; break;
    case canspiTX_CH2:  cCmd = CANSPI_CMD_RTS_TXB2; break;
    default:            cCmd = CANSPI_CMD_RTS_TXB0; break;
  }

  CANSPIOneByteSend(cCmd);
  NOP();
  NOP();

  return eResult;
}

/**
 *  @brief      Configure the CAN Filter as specified Filter
 *  @param[in]  eFilter: Filter Id to configure
                pcsFilterObj: Pointer to Filter Object to configure
 *  @return     rcSUCCESS = No error occurred and returned result is complete
                rcERROR   = An error occurred during the operation               
 *  @note       None
 *  @attention  None
 */
static eRESULT CANSPIFilterConfigure(eCANSPI_FILTER eFilter, PCCANSPI_FILTEROBJ_ID pcsFilterObj)
{
  eRESULT eResult = rcSUCCESS;

  BYTE    acBuffer[CANSPI_FILTER_SIZE];
  DWORD   dwReg       = 0;
  BYTE    cWriteSize  = 0;
  BYTE    cAddress;
  CANSPI_REG_TXBnSIDL sSIDL;

  /* RXFnSIDH */
  NOP();
  NOP();
  dwReg = pcsFilterObj->SID >> 3;
  acBuffer[cWriteSize++] = dwReg & 0xff;

  /* RXFnSIDL */
  sSIDL.cData  = 0;
  sSIDL.SIDL  = pcsFilterObj->SID & 0x7;
  sSIDL.IDE   = pcsFilterObj->EXIDE;
  dwReg       = (pcsFilterObj->EID >> 16);
  sSIDL.EIDHH = dwReg & 0x3;
  acBuffer[cWriteSize++] = sSIDL.cData;

  /* RXFnEID8 */
  dwReg = (pcsFilterObj->EID >> 8);
  acBuffer[cWriteSize++] = dwReg & 0xff;

  /* RXFnEID0 */
  acBuffer[cWriteSize++] = pcsFilterObj->EID & 0xff;

  /* Register Address */
  switch(eFilter) 
  {
    case canspiFILTER_0:  cAddress = cREGADDR_RXF0; break;
    case canspiFILTER_1:  cAddress = cREGADDR_RXF1; break;
    case canspiFILTER_2:  cAddress = cREGADDR_RXF2; break;
    case canspiFILTER_3:  cAddress = cREGADDR_RXF3; break;
    case canspiFILTER_4:  cAddress = cREGADDR_RXF4; break;
    case canspiFILTER_5:  cAddress = cREGADDR_RXF5; break;
    default:              cAddress = cREGADDR_RXF0; break;
  }

  /* Write buffer */
  eResult |= CANSPIWrites(cAddress, acBuffer, CANSPI_FILTER_SIZE);

  return eResult;
}

/**
 *  @brief      Configure the CAN Mask as specified Mask
 *  @param[in]  eMask: Mask Id to configure
                pcsMaskObj: Pointer to Mask Object to configure
 *  @return     rcSUCCESS = No error occurred and returned result is complete
                rcERROR   = An error occurred during the operation               
 *  @note       None
 *  @attention  None
 */
static eRESULT CANSPIMaskConfigure(eCANSPI_MASK eMask, PCCANSPI_MASKOBJ_ID pcsMaskObj)
{
  eRESULT eResult = rcSUCCESS;
  
  BYTE    acBuffer[CANSPI_MASK_SIZE];
  DWORD   dwReg       = 0;
  BYTE    cWriteSize  = 0;
  BYTE    cAddress;
  CANSPI_REG_TXBnSIDL sSIDL;

  /* RXMnSIDH */
  NOP();
  NOP();
  dwReg = pcsMaskObj->MSID >> 3;
  acBuffer[cWriteSize++] = dwReg & 0xff;

  /* RXMnSIDL */
  sSIDL.cData = 0;
  sSIDL.SIDL  = pcsMaskObj->MSID & 0x7;
  sSIDL.IDE   = 0; // Not implemented
  dwReg       = (pcsMaskObj->MEID >> 16);
  sSIDL.EIDHH = dwReg & 0x3;
  acBuffer[cWriteSize++] = sSIDL.cData;

  /* RXMnEID8 */
  dwReg = (pcsMaskObj->MEID >> 8);
  acBuffer[cWriteSize++] = dwReg & 0xff;

  /* RXMnEID0 */
  acBuffer[cWriteSize++] = pcsMaskObj->MEID & 0xff;

  /* Register Address */
  switch(eMask)
  {
    case canspiMASK_0:  cAddress = cREGADDR_RXM0; break;
    case canspiMASK_1:  cAddress = cREGADDR_RXM1; break;
    default:            cAddress = cREGADDR_RXM0; break;
  }

  /* Write buffer */
  eResult |= CANSPIWrites(cAddress, acBuffer, CANSPI_MASK_SIZE);

  return eResult;
}

/**
 *  @brief      CAN Message read from specified RX Channel
 *  @param[in]  eChannel: RX Channel to read
 *  @param[out] psCanRxMsg: Pointer to return back CAN Rx Message
 *  @return     rcSUCCESS = No error occurred and returned result is complete
                rcERROR   = An error occurred during the operation               
 *  @note       Read total 13bytes (Header 5bytes + data 8bytes)
 *  @attention  None
 */
static eRESULT CANSPIReceiveMessageGet(eCANSPI_RX_CHANNEL eChannel
                                                , PCAN_FRAME_RX psCanRxMsg)
{
  eRESULT eResult = rcSUCCESS;
  
  BYTE    acBuffer[CANSPI_RX_BUFFER_SIZE];
  BYTE    cReadSize = 0;
  BYTE    cAddress;
  WORD    wI;

  CANSPI_REG_RXBnSIDL sSIDL;
  CANSPI_REG_RXBnDLC  sDLC;

  /* Get address */
  switch(eChannel)
  {
    case canspiRX_CH0:  cAddress = cREGADDR_RXB0ID; break;
    case canspiRX_CH1:  cAddress = cREGADDR_RXB1ID; break;
    default:            cAddress = cREGADDR_RXB0ID; break;
  }

  /* Read buffer */
  cReadSize = CANSPI_MAX_DATA_BYTES + 5; // Read All
  eResult |= CANSPIReads(cAddress, acBuffer, cReadSize);
  NOP();
  NOP();

  /* SID   */
  sSIDL.cData = acBuffer[1];
  psCanRxMsg->StdId = sSIDL.SIDL;
  psCanRxMsg->StdId += (WORD)acBuffer[0] << 3;

  /* EID */
  psCanRxMsg->ExtId = acBuffer[3];
  psCanRxMsg->ExtId += (DWORD)acBuffer[2] << 8;  
  psCanRxMsg->ExtId += (DWORD)sSIDL.EIDHH << 16;

  /* Control     */
  sDLC.cData = acBuffer[4];
  psCanRxMsg->IDE = sSIDL.IDE;
  if(CAN_Id_Standard == sSIDL.IDE)
    psCanRxMsg->RTR = sSIDL.SRR;
  else
    psCanRxMsg->RTR = sDLC.RTR; 
  psCanRxMsg->DLC = sDLC.DLC;
  
  /*psRxObj->ctrl.RB0 = sDLC.RB0;
  psRxObj->ctrl.RB1 = sDLC.RB1;*/

  /* Data */
  for (wI = 0; wI < psCanRxMsg->DLC; wI++) 
  {
    psCanRxMsg->Data[wI] = acBuffer[wI + 5];
  }
  NOP();
  NOP();

  return eResult;
}

/**
 *  @brief      Configure the RX Buffer
 *  @param[in]  pcsConfig: pointer to RX buffer configuration
 *  @param[out] None
 *  @return     rcSUCCESS = No error occurred and returned result is complete
                rcERROR   = An error occurred during the operation               
 *  @note       Read total 13bytes (Header 5bytes + data 8bytes)
 *  @attention  None
 */
static eRESULT CANSPIReceiveConfigurationSet(PCCANSPI_RX_CONFIG pcsConfig)
{
  eRESULT eResult = rcSUCCESS;
  
  CANSPI_REG_RXB0CTRL sRXB0Ctrl;  
  CANSPI_REG_RXB1CTRL sRXB1Ctrl;  

  /* Configure RB0 */
  sRXB0Ctrl.cData = 0;
  sRXB0Ctrl.RXM = pcsConfig->RXB0_ReceiveMode;
  sRXB0Ctrl.BUKT = pcsConfig->RXB_RollOver;
  eResult |= CANSPIRegWrite(cREGADDR_RXB0CTRL, sRXB0Ctrl.cData);

  /* Configure RB1 */
  sRXB1Ctrl.cData = 0;
  sRXB1Ctrl.RXM = pcsConfig->RXB1_ReceiveMode;
  eResult |= CANSPIRegWrite(cREGADDR_RXB1CTRL, sRXB1Ctrl.cData);

  return eResult;    
}                                                

/**
 *  @brief      Read Interrupt Register(Module Event)
 *  @param[in]  None
 *  @param[out] None
 *  @return     Interrupt Flags (bit or'ed)
 *  @note       None
 *  @attention  None
 */
static BYTE CANSPIModuleEventGet(void)
{
  BYTE cFlags;

  /* Read Interrupt register */
  cFlags = CANSPIRegRead(cREGADDR_CANINTF);

  return cFlags;
}

/**
 *  @brief      Enable/Disable the Interrupt (Module Event)
 *  @param[in]  cFlags: Module Event to Enable/Disable (bitwised)
                bEnable: TRUE to Enable, FALSE to Disable
 *  @param[out] None
 *  @return     rcSUCCESS = No error occurred and returned result is complete
                rcERROR   = An error occurred during the operation   
 *  @note       None
 *  @attention  None
 */
static eRESULT CANSPIModuleEventEnable(BYTE cFlags, BOOL bEnable)
{
  eRESULT eResult = rcSUCCESS;
  BYTE    cData   = (bEnable) ? 0xFF : 0x00;
  
  // Write Interrupt Enable Register
  eResult |= CANSPIRegBitModify(cREGADDR_CANINTE, cFlags, cData);

  return eResult;
}

/**
 *  @brief      Clear the Interrupt (Module Event)
 *  @param[in]  cFlags: Module Event to Clear (bitwised)
 *  @param[out] None
 *  @return     rcSUCCESS = No error occurred and returned result is complete
                rcERROR   = An error occurred during the operation   
 *  @note       None
 *  @attention  None
 */
static eRESULT CANSPIModuleEventClear(BYTE cFlags)
{
  eRESULT eResult = rcSUCCESS;
  
  // Write Interrupt Flag Register
  eResult |= CANSPIRegBitModify(cREGADDR_CANINTF, cFlags, 0x00);

  return eResult;    
}

/**
 *  @brief      Get the Tx/Rx Buffer status (interrupt flags and transmit request)
 *  @param[in]  None
 *  @param[out] None
 *  @return     Tx/Rx Buffer status (interrupt flags and transmit request)
 *  @note       None
 *  @attention  None
 */
static CANSPI_REG_BUFFER_STATUS CANSPIBufferStatusGet(void)
{
  CANSPI_REG_BUFFER_STATUS sStatus;
  BYTE acTxBuffer[1] = {CANSPI_CMD_RD_STATUS};
  BYTE acRxBuffer[2] = {0,};

  CANSPITransaction(sizeof(acTxBuffer), acTxBuffer, sizeof(acRxBuffer), acRxBuffer);

  sStatus.cData = acRxBuffer[0];

  return sStatus;
}

/**
 *  @brief      Perform HW Reset 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void CANSPIHWReset(void)
{
  dvGPIOFieldSet(gfGPO_CAN_nRESET, LOW);
  commDelayMS(50);
  dvGPIOFieldSet(gfGPO_CAN_nRESET, HIGH);
  commDelayMS(50);
}

/**
 *  @brief      Perform SW Reset 
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void CANSPISWReset(void)
{
  
  CANSPIOneByteSend(CANSPI_CMD_RESET);
  
  commDelayMS(50);
}

/** @defgroup dvMCP25625_Interrupt_Service_Routine
  * @{
  */

/**
 *  @brief      Interrupt Handler to CANSPI nINT (Edge Detect)
                None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void CANSPI_INT_IRQHandler(void)
{
  /* CANSPI Interrupt Level is LOW */
  if(!(*R_GPILEV(CANSPI_INT_GP_CH) & CANSPI_INT_GP_PIN_MASK))
  {
    PCAN_FRAME_QUE_RX   psQue = m_psCanFrameQueRx;
    WORD                wReadSize = 0;  

    if(psQue != NULL
      && psQue->wAvailFrameRx < m_wRxCountMax)
    {
      wReadSize = dvCANSPIReadFrame(1, (PCAN_RX_MSG)&psQue->pasFrameRx[psQue->wHeader++]);
      psQue->wHeader %= m_wRxCountMax;
      psQue->wAvailFrameRx++;
    }      
  }

  /* Clear Status */
  *R_GPEDS(CANSPI_INT_GP_CH) |= CANSPI_INT_GP_PIN_MASK; 
}


/**
  * @}
  */


/**
 *  @brief      Initialize the Driver
 *  @param      None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void CANSPIDriverInitialize(void)
{
  m_wRxCount        = 0;
  m_wRxCountMax     = 0;

  m_psCanFrameQueRx = NULL;

  m_eCANStatus      = canSTATUS_OK;

}


/**
 *  @brief      Configures the Device 
 *  @param      None
 *  @return     rcSUCCESS if the operation was successful.
 *  @note       None
 *  @attention  None
 */
static eRESULT CANSPIDeviceInitialize(void)
{
  eRESULT                 eResult = rcSUCCESS;
  CANSPI_RX_CONFIG        sRxConfig;
  CANSPI_BIT_TIME_CONFIG  sCANSPIBTConfig; 

  CANSPIHWReset();
  CANSPISWReset();

#if (USE_CAN_LOW_SPEED == 0)
  // Configure device and bit time (1Mbps@20MHz)
  sCANSPIBTConfig.BRP         = 0;
  sCANSPIBTConfig.SJW         = canspiSJW_1TQ;  
  sCANSPIBTConfig.PropSeg     = canspiPROP_2TQ;
  sCANSPIBTConfig.PhaseSeg1   = canspiPHSEG1_2TQ;
  sCANSPIBTConfig.PhaseSeg2   = canspiPHSEG2_5TQ;
#else
  // Configure device and bit time (500Kbps@20MHz)
  sCANSPIBTConfig.BRP         = 0;
  sCANSPIBTConfig.PropSeg     = canspiPROP_7TQ;
  sCANSPIBTConfig.PhaseSeg1   = canspiPHSEG1_6TQ;
  sCANSPIBTConfig.PhaseSeg2   = canspiPHSEG2_6TQ;
  sCANSPIBTConfig.SJW         = canspiSJW_4TQ;
#endif

  sCANSPIBTConfig.BusSampling         = canspiSAMPLE_TRICE;
  sCANSPIBTConfig.PhaseSeg2Config     = canspiPHSEG2_BTL_CNF3;
  sCANSPIBTConfig.SOFPinConfig        = canspiPIN_CLKO;
  sCANSPIBTConfig.WakeUpFilterEnable  = canspiWAKFIL_ENABLE;

  eResult |= CANSPIConfigurationSet((PCCANSPI_BIT_TIME_CONFIG)&sCANSPIBTConfig);

  /* Receive Buffer 0: Configure Filter and Mask objects */
  // Filter 0-1
  // Allow CAN Base Frame with ID 0x200
  m_asCANSPIFilterObj[canspiFILTER_0].SID    = CANTP_NODE_ID_T10;
  m_asCANSPIFilterObj[canspiFILTER_0].EID    = 0x0;
  m_asCANSPIFilterObj[canspiFILTER_0].EXIDE  = 0;

  // FILTER_1 == FILTER_0
  m_asCANSPIFilterObj[canspiFILTER_1] = m_asCANSPIFilterObj[canspiFILTER_0];

  eResult |= CANSPIFilterConfigure(canspiFILTER_0, (PCCANSPI_FILTEROBJ_ID)&m_asCANSPIFilterObj[canspiFILTER_0]);
  eResult |= CANSPIFilterConfigure(canspiFILTER_1, (PCCANSPI_FILTEROBJ_ID)&m_asCANSPIFilterObj[canspiFILTER_1]);

  // Mask 0: Allow all messages
  m_asCANSPIMaskObj[canspiMASK_0].MSID = 0x000;
  m_asCANSPIMaskObj[canspiMASK_0].MEID = 0x0;
  eResult |= CANSPIMaskConfigure(canspiMASK_0, (PCCANSPI_MASKOBJ_ID)&m_asCANSPIMaskObj[canspiMASK_0]);

#if defined(_DEF_USE_CANSPI_RXB1_)
  /* Receive Buffer 1: Configure Filter and Mask objects */
  // Filter 2-5
  // Disable RXB1: Allow only message were all ID bits are set
  m_asCANSPIFilterObj[canspiFILTER_2].SID    = 0x7ff;
  m_asCANSPIFilterObj[canspiFILTER_2].EID    = 0x3ffff;
  m_asCANSPIFilterObj[canspiFILTER_2].EXIDE  = 1;

  // FILTER_5 == FILTER_4 == FILTER_3 == FILTER_2
  m_asCANSPIFilterObj[canspiFILTER_5] 
    = m_asCANSPIFilterObj[canspiFILTER_4] 
    = m_asCANSPIFilterObj[canspiFILTER_3]
    = m_asCANSPIFilterObj[canspiFILTER_2];

  eResult |= CANSPIFilterConfigure(canspiFILTER_2, (PCCANSPI_FILTEROBJ_ID)&m_asCANSPIFilterObj[canspiFILTER_2]);
  eResult |= CANSPIFilterConfigure(canspiFILTER_3, (PCCANSPI_FILTEROBJ_ID)&m_asCANSPIFilterObj[canspiFILTER_3]);
  eResult |= CANSPIFilterConfigure(canspiFILTER_4, (PCCANSPI_FILTEROBJ_ID)&m_asCANSPIFilterObj[canspiFILTER_4]);
  eResult |= CANSPIFilterConfigure(canspiFILTER_5, (PCCANSPI_FILTEROBJ_ID)&m_asCANSPIFilterObj[canspiFILTER_5]);

  // Mask 1: Check all bits
  m_asCANSPIMaskObj[canspiMASK_1].MSID = 0x7ff;
  m_asCANSPIMaskObj[canspiMASK_1].MEID = 0x3ffff;
  eResult |= CANSPIMaskConfigure(canspiMASK_1, (PCCANSPI_MASKOBJ_ID)&m_asCANSPIMaskObj[canspiMASK_1]);
#endif

  /* Configure Receive Mode */
  sRxConfig.RXB0_ReceiveMode  = canspiRXM_USE_FILTER;
  sRxConfig.RXB1_ReceiveMode  = canspiRXM_USE_FILTER;
  sRxConfig.RXB_RollOver      = canspiRXB_ROLLOVER_ENABLED;
  eResult |= CANSPIReceiveConfigurationSet(&sRxConfig);

  /* Clear all flags */
  eResult |= CANSPIModuleEventClear(CANSPI_EVENT_ALL);



  /* Set NormaL mode */
  eResult |= CANSPIOperationModeSet(canspiMODE_NORMAL);

  /* CANSPI Interrupt Set */
  *R_PAF(CANSPI_INT_GP_CH)     |= CANSPI_INT_GP_AF;           // AF to GPIO
  ///*R_GPPDEN(CANSPI_INT_GP_CH)   = CANSPI_INT_GP_PIN_MASK;     // Pull-Up Enable
  ///*R_GPPUDIS(CANSPI_INT_GP_CH)  = CANSPI_INT_GP_PIN_MASK;  // Pull-Up Clear
  ///*R_GPPDDIS(CANSPI_INT_GP_CH)  = CANSPI_INT_GP_PIN_MASK;  // Pull-Down Clear
  ///*R_GPODM(CANSPI_INT_GP_CH)    = CANSPI_INT_GP_PIN_MASK;  // Open Drain
  *R_GPIDIR(CANSPI_INT_GP_CH)   = CANSPI_INT_GP_PIN_MASK;   // Input Mode
  
  *R_GPFED(CANSPI_INT_GP_CH)   |= (VUINT)CANSPI_INT_GP_PIN_MASK;   // Set Falling Edge
  *R_GPRED(CANSPI_INT_GP_CH)   &= ~CANSPI_INT_GP_PIN_MASK;  // Clear Rising Edge

  set_interrupt(CANSPI_INT_NUM, CANSPI_INT_IRQHandler);

  /* Start from Interrupt Disabled status */
  enable_interrupt(CANSPI_INT_NUM, FALSE);

  return eResult;
}

/**
  * @}
  */


/** @defgroup dvMCP25625_Public_Functions
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
void dvDeviceBusCANInitialize(void)
{
  CANSPIDriverInitialize();
  if(rcSUCCESS != CANSPIDeviceInitialize())
  {
    /* Error Process Here */
  }
  else
  {
    msgDbg("CAN Initialized as \"%s\" Operation Mode"
              , (0 == CANSPIOperationModeGet()) ? "Normal" : "Abnormal");
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
void dvDeviceBusCANRelease(void)
{ 

}

/**
 *  @brief       This API reads data from the CAN Bus
 *  @param[in]   wCount     : Number of bytes to Read
                 pasCanFrame  : Pointer to an array of at least wCount frame to receive data into
 *  @param[out]  None
 *  @return      Count of Read frame
 *  @note        None
 *  @attention   None 
 */  
WORD dvCANSPIReadFrame(WORD   wCount, PCAN_FRAME_RX pasCanFrame)
{
  ///WORD wIndex;
  WORD wReadSize        = 0;
  BYTE cEventFlags      = 0;

  cEventFlags = CANSPIModuleEventGet();

  if(cEventFlags & CANSPI_EVENT_RXB0)
  {
    // Get message
    if(rcSUCCESS == CANSPIReceiveMessageGet(canspiRX_CH0, pasCanFrame))
      wReadSize = pasCanFrame->DLC;
    
    // Clear flag
    CANSPIModuleEventClear(CANSPI_EVENT_RXB0);
  }
  else if(cEventFlags & CANSPI_EVENT_RXB1)
  {
    // Get message
    if(rcSUCCESS == CANSPIReceiveMessageGet(canspiRX_CH0, pasCanFrame))
      wReadSize = pasCanFrame->DLC;
    
    // Clear flag
    CANSPIModuleEventClear(CANSPI_EVENT_RXB1);
  }

  return wReadSize;
}


/**
 *  @brief      This API Configures the CAN Bus Filter
 *  @param      eFilter: Filter Id to Set to
                sFilterObj: CAN Filter to Set
 *  @return     rcSUCCESS = No error occurred and returned result is complete
                rcERROR   = An error occurred during the operation 
 *  @note       None
 *  @attention  None
 */
eRESULT dvDeviceBusCANFilterSet(eCANSPI_FILTER eFilter, CANSPI_FILTEROBJ_ID sFilterObj)
{ 
  return CANSPIFilterConfigure(eFilter, (PCCANSPI_FILTEROBJ_ID)&sFilterObj);
}

/**
 *  @brief      This API return the CAN Bus Filter settings as specified Filter Id
 *  @param      eFilter: Filter Id to Return Back
 *  @return     CAN filter Setting
 *  @note       None
 *  @attention  None
 */
CANSPI_FILTEROBJ_ID dvDeviceBusCANFilterGet(eCANSPI_FILTER eFilter)
{
  ASSERT(eFilter < canspiFILTER_INVALID);
  
  return m_asCANSPIFilterObj[eFilter];
}

/**
 *  @brief      This API Configures the CAN Bus Mask
 *  @param      eMask: Mask Id to Set to
                sMaskObj: CAN Mask to Set
 *  @return     rcSUCCESS = No error occurred and returned result is complete
                rcERROR   = An error occurred during the operation 
 *  @note       None
 *  @attention  None
 */
eRESULT dvDeviceBusCANMaskSet(eCANSPI_MASK eMask, CANSPI_MASKOBJ_ID sMaskObj)
{ 
  return CANSPIMaskConfigure(eMask, (PCCANSPI_MASKOBJ_ID)&sMaskObj);
}

/**
 *  @brief      This API return the CAN Bus Filter settings as specified Filter Id
 *  @param      eFilter: Filter Id to Return Back
 *  @return     CAN filter Setting
 *  @note       None
 *  @attention  None
 */
CANSPI_MASKOBJ_ID dvDeviceBusCANMaskGet(eCANSPI_MASK eMask)
{
  ASSERT(eMask < canspiMASK_INVALID);
  
  return m_asCANSPIMaskObj[eMask];
}


/**
 *  @brief      This API Enables or Disable the specified CAN Interrupt
 *  @param[in]  bEnable : TRUE for Enable Interrupt, FALSE for Disable
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void dvDeviceBusCANInterruptEnable(BOOL bEnable)
{
  /* Enable receive interrupts */
#if defined(_DEF_USE_CANSPI_RXB1_)
  CANSPIModuleEventEnable((CANSPI_EVENT_RXB0 | CANSPI_EVENT_RXB1), bEnable);
#else
  CANSPIModuleEventEnable(CANSPI_EVENT_RXB0, bEnable);
#endif

  /* Interrupt Start from Disable */
  enable_interrupt(CANSPI_INT_NUM, bEnable);
}

/**
 *  @brief       This API reads data from the CAN Bus.
                 wCount     : Number of message to available for storage in the pacBuffer parameter
                 psQueRx : Pointer to CAN Rx Que   
 *  @param[out]  None
 *  @return      None
 *  @note        None
 *  @attention   None 
 */  
void dvDeviceBusCANReadSet(WORD  wCount, PCAN_FRAME_QUE_RX psQueRx)
{
  ASSERT(NULL != psQueRx );
  
  // Disable interrupts so we do not corrupt data pointers
  dvDeviceBusCANInterruptEnable(FALSE);

  m_wRxCount    = 0;
  m_wRxCountMax  = wCount;  
  m_psCanFrameQueRx   = psQueRx;
  
  m_eCANStatus        = canSTATUS_OK;

  // Enable interrupts to copy data out of the FIFO or respond to errors
  dvDeviceBusCANInterruptEnable(TRUE);  
}

/**
 *  @brief       This API reads data from the CAN Bus
 *  @param[in]   wCount     : Number of bytes to Read
                 pasCanFrame  : Pointer to an array of at least wCount frame to receive data into
 *  @param[out]  None
 *  @return      Count of Read frame
 *  @note        None
 *  @attention   None 
 */  
WORD dvDeviceBusCANRead(WORD wCount, PCAN_FRAME_RX pasCanFrame)
{
  WORD wIndex;
  WORD wReadSize        = 0;
  PCAN_FRAME_QUE_RX psQue = m_psCanFrameQueRx;

  if(psQue != NULL
    && (psQue->wHeader != psQue->wTail))
  {
    // Disable interrupts so we do not corrupt data pointers
    dvDeviceBusCANInterruptEnable(FALSE);

    wReadSize = MIN(wCount, psQue->wAvailFrameRx);

    for(wIndex = 0; wIndex < wReadSize; wIndex++)
    {
      pasCanFrame[wIndex] = psQue->pasFrameRx[psQue->wTail++];
      psQue->wTail %= m_wRxCountMax;
      psQue->wAvailFrameRx--;

      m_wRxCount++;

      // Enable interrupts to copy data out of the FIFO or respond to errors
      // If we have received the same number of bytes in the read buffer
      // disable interrupts and send a sgSERIAL_READ_FULL message to the
      // protocol task
      if (m_wRxCount >= m_wRxCountMax)
      {
        // Interrupt Disable
        dvDeviceBusCANInterruptEnable(FALSE);
        utilExecSignalSet(sgCANSPI,  CANIO_SIGNAL_READ_FULL);
      }        
    }

    // Interrupt Enable
    if(!utilExecSignalGet(sgCANSPI,  CANIO_SIGNAL_READ_FULL))
      dvDeviceBusCANInterruptEnable(TRUE);  
    
  }


  return wReadSize;
}



/**
 *  @brief       This API Transmit the buffer via CAN Bus
 *  @param[in]   dwMsgId: CAN Message Id to write
                 cCount: Number of bytes to transmit. Can not be exceed 8
                 pacBuffer: Pointer to an array of at least wCount bytes to transmit
                 wFlags: Flags for the CAN transaction (see Definitions)
 *  @param[out]  None
 *  @return      rcSUCCESS= No error occurred and returned result is complete
                 rcERROR= An error occurred during the operation
 *  @note        cCount must be lower than 8bytes
 *  @attention   None 
 */  
eRESULT dvDeviceBusCANWrite(DWORD dwMsgId, BYTE cCount, PCBYTE pacBuffer, WORD wFlags)
{
  eRESULT eResult = rcSUCCESS; 

  DWORD dwRetry = 0;

  ASSERT(cCount <= BUS_CAN_BUFFER_SIZE);

  if(cCount <= BUS_CAN_BUFFER_SIZE)
  {
    if(wFlags & BUS_CAN_20B)
    {
      m_sCanTxMsg.ExtId = dwMsgId & 0x1FFFFFFF;
      m_sCanTxMsg.IDE = CAN_Id_Extended;    
    }
    else
    {
      m_sCanTxMsg.StdId = dwMsgId & 0x7FF;
      m_sCanTxMsg.IDE = CAN_Id_Standard;    
    }

    m_sCanTxMsg.RTR = (wFlags & BUS_CAN_RTR) ? CAN_RTR_REMOTE : CAN_RTR_DATA;
    m_sCanTxMsg.DLC = cCount;

    memcpy(m_sCanTxMsg.Data, pacBuffer, cCount);

    // Check if FIFO is not full
    do
    {// This Loop can be excute over 621 at 2Kbytes transmit
    
      m_sCANSPICANStatus = CANSPIBufferStatusGet();
      
    }while(m_sCANSPICANStatus.TX0REQ && dwRetry++ < CANSPI_TRANMIT_WAIT_MAX);

    if(dwRetry < CANSPI_TRANMIT_WAIT_MAX)
    {
      // Load message and transmit
      CANSPITransmitChannelLoad(canspiTX_CH0, &m_sCanTxMsg);      
      CANSPITransmitChannelFlush(canspiTX_CH0);    
    }
    else
    {
      eResult = rcERROR;
    }
  }
  else
  {
    eResult = rcERROR;
  }    

  return eResult;
}

/**
 *  @brief       This API acquires the current status of the CAN Bus hardware.
 *  @param[in]   None
 *  @param[out]  pwCountRead  : Number of message read since the last call to dvDeviceBusCANRead() 
                 pwCountAvail : Number of message read available from Queue
 *  @return      Current status of serial interface (ok, read overflow, read error, or read overflow and error)
 *  @note        None
 *  @attention   None 
 */  
eCAN_STATUS dvDeviceBusCANStatus(PWORD pwCountRead, PWORD pwCountAvail)
{
  eCAN_STATUS eCANStatus = m_eCANStatus;  

  if(pwCountRead != NULL)
    *pwCountRead = m_wRxCount;
  if(pwCountAvail != NULL)
    *pwCountAvail = m_psCanFrameQueRx->wAvailFrameRx;

  return eCANStatus;
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
#endif // #if USE_CAN_BUS  

/******************* (C) COPYRIGHT 1997-2019 (c) MadeFactory Inc. *****END OF FILE****/  
