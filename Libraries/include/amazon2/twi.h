/*****************************************************************************
*
* Copyright (C) 2014      Advanced Digital Chips, Inc. All Rights Reserved.
*						http://www.adc.co.kr
*
* THIS SOFTWARE IS PROVIDED BY ADCHIPS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ADCHIPS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE
*
*****************************************************************************/
#pragma once

#define TWI_BASE_ADDR 0xf4004000

/* flags of TWI */
#define TWI_RECEIVE         0x21
#define TWI_TRANSMIT        0x22


//======================================================================================================================
// TWI Contorl Register (TWICTRL)

#define R_TWICTRL	((volatile U32*)TWI_BASE_ADDR)

#define F_TWICTRL_TWIEN				(1<<7)		// TWI Contorller Enable
#define F_TWICTRL_TWIMOD_MSTART		(1<<5)		// Master Mode Start Condition
#define F_TWICTRL_TWIMOD_SSTOP		(0<<5)		// Slave Mode Stop Condition
#define F_TWICTRL_TWITR_MTRM		(1<<4)		// Master Transmit Mode
#define F_TWICTRL_TWITR_MRCV		(0<<4)		// Master Receive Mode
#define F_TWICTRL_TWIAK_ACK			(0<<3)		// Transmit ACK Enable
#define F_TWICTRL_TWIAK_NACK		(1<<3)		// Transmit NACK Enable
#define F_TWICTRL_REPST_RSTART		(1<<2)		// Repeated Start Condition Enable
#define F_TWICTRL_TCIE_EN			(1<<1)		// Transfer Complete Interrupt Enable
#define F_TWICTRL_LSTIE_EN			(1<<0)		// Lost Arbitration Interrupt Enable
//======================================================================================================================

//======================================================================================================================
// TWI Status Register (TWISTAT)

#define R_TWISTAT	((volatile U32*)(TWI_BASE_ADDR+4))

#define F_TWISTAT_TXEMPTY			(1<<9)		// TX Buffer Empty
#define F_TWISTAT_RXFULL			(1<<8)		// RX Buffer Full
#define F_TWISTAT_TWIDT_FIN			(1<<7)		// 1Byte Data Transfer Complete
#define F_TWISTAT_TWIAS_MATCH		(1<<6)		// Slave Address Match
#define F_TWISTAT_TWIBUSY			(1<<5)		// Bus Busy
#define F_TWISTAT_TWILOST			(1<<4)		// Lost Arbitration
#define F_TWISTAT_TWISRW_RCV		(1<<3)		// Slave Receive Mode
#define F_TWISTAT_TWISRW_TRM		(0<<3)		// Slave Transmit Mode
#define F_TWISTAT_RSF				(1<<1)		// Repeated Start Condition
#define F_TWISTAT_TWIRXAK_NAK		(1<<0)		// Received NACK
#define F_TWISTAT_TWIRXAK_AK		(0<<0)		// Received ACK
//======================================================================================================================

//======================================================================================================================
// TWI Address Register (TWIADDR)
// [7-1]	Slave Address
// [0]		Not Mapped

#define R_TWIADR	((volatile U32*)(TWI_BASE_ADDR+8))
//======================================================================================================================

//======================================================================================================================
// TWI Data Register (TWIDATA)

#define R_TWIDATA	((volatile U32*)(TWI_BASE_ADDR+0xc))
//======================================================================================================================

//======================================================================================================================
// TWI Baud-Rate Registers (TWIBRx)
// [7-0]	

#define R_TWIBR0	((volatile U32*)(TWI_BASE_ADDR+0x10))
#define R_TWIBR1	((volatile U32*)(TWI_BASE_ADDR+0x14))
//======================================================================================================================

int twi_set_freq (U32 freq);
int twi_write(U8 devaddr, U8 *waddr, int len, U8 *buffer, int length);
int twi_write2(U8 devaddr, U8 waddr, U8 *buffer, int length);
int twi_read(U8 devaddr, U8 *waddr, int len, U8 *buffer,int length);
BOOL twi_trans_complete(void);
BOOL twi_wait_busy(void);


