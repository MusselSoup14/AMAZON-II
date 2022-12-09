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

#define UART_FIFO_DEPTH 16
#define UART_CHANNEL_MAX 4

/***********************************************
* UART registers
***********************************************/
#define R_UART_BASE(N) ((volatile unsigned int*)(0xf4003000+((N)*0x20)))

/*
* Register Offset
*/
// UART Channel Receiver Buffer Register (UxRB)
// Read Only - When DLAB 0
#define UART_RBR        0 // UART Channel Receiver Buffer Registers

// UART Channel Transmitter Holding Register (UxTH)
// Write Only - When DLAB 1
#define UART_THR        0 //UART Channel Transimitter Holding Registers

#define UART_DIV_LO     0 //UART Channel Divisor Latch LSB Registers
#define UART_DIV_HI     (1*4) //UART Channel Divisor Latch MSB Registers

// UART Channel Interrupt Enable Register (UxIE)
// RW - When DLAB 0
#define UART_IER        (1*4) //UART Channel Interrupt Enable Registers
#define F_UIE_RLSIEN		( 1<< 2)		// Receiver Line Status Interrupt Enable bit
#define F_UIE_THEIEN		( 1<< 1)		// Transmitter Holding Empry Interrupt Enable bit
#define F_UIE_RDAIEN		( 1<< 0)		// Received Data Available Interrupt Enable bit


#define UART_IIR        (2*4) //UART Channel Interrupt Identification Registers
#define UART_FCR        (2*4) //UART Channel FIFO Control Registers
#define UART_LCR        (3*4) //UART Channel Line Control Registers
#define UART_LSR        (5*4) //UART Channel Line Status Registers



// UART Channel Interrupt Identification Register (UxII)
// Read Only - When DLAB 0

#define F_UII_FIFOST		( 3<< 6)		// FIFOs Enable Status bit (0-not in FIFO mode | 3-FIFO mode)
#define F_UII_INTID			(15<< 0)		// UART Interrupt ID

// UART Channel FIFO Control Register (UxFC)
// Write Only - When DLAB 0
// Read  Only - When DLAB 1

#define F_UFC_RFTL_1		( 0<< 6)		// Receiver FIFO Trigger Level (0-1Byte | 1-4Byte | 2-8Byte | 3-14Byte)
#define F_UFC_RFTL_4		( 1<< 6)		// Receiver FIFO Trigger Level (0-1Byte | 1-4Byte | 2-8Byte | 3-14Byte)
#define F_UFC_RFTL_8		( 2<< 6)		// Receiver FIFO Trigger Level (0-1Byte | 1-4Byte | 2-8Byte | 3-14Byte)
#define F_UFC_RFTL_14		( 3<< 6)		// Receiver FIFO Trigger Level (0-1Byte | 1-4Byte | 2-8Byte | 3-14Byte)
#define F_UFC_XFR			( 1<< 2)		// XMIT FIFO Reset
#define F_UFC_RFR			( 1<< 1)		// RCVR FIFO Reset
#define F_UFC_FIFOEN		( 1<< 0)		// FIFO Enable bit (0-16450 UART Mode | 1-Enable FIFO)

// UART Channel Line Control Register (UxLC)
// RW

#define F_ULC_DLAB			( 1<< 7)		// Divisor Latch Access bit
#define F_ULC_SB			( 1<< 6)		// Set Break
#define F_ULC_SP			( 1<< 5)		// Stick Parity
#define F_ULC_EPS			( 1<< 4)		// Even Parity Select
#define F_ULC_PEN			( 1<< 3)		// Parity Enable bit
#define F_ULC_STB			( 1<< 2)		// Number of Stop bit
#define F_ULC_WLS_5			( 0<< 0)		// Word Length Select (0-5bit | 1-6bit | 2-7bit | 3-8bit)
#define F_ULC_WLS_6			( 1<< 0)		// Word Length Select (0-5bit | 1-6bit | 2-7bit | 3-8bit)
#define F_ULC_WLS_7			( 2<< 0)		// Word Length Select (0-5bit | 1-6bit | 2-7bit | 3-8bit)
#define F_ULC_WLS_8			( 3<< 0)		// Word Length Select (0-5bit | 1-6bit | 2-7bit | 3-8bit)

// UART Channel Line Status Register (UxLS)
// Read Only

#define F_ULS_EIRF			( 1<< 7)		// Error in RCVR FIFO
#define F_ULS_TEMP			( 1<< 6)		// Transmitter Empty
#define F_ULS_THRE			( 1<< 5)		// Transmitter Holding Register Empty
#define F_ULS_BINT			( 1<< 4)		// Break Interrupt
#define F_ULS_FERR			( 1<< 3)		// Framing Error
#define F_ULS_PERR			( 1<< 2)		// Parity Error
#define F_ULS_OERR			( 1<< 1)		// Overrun Error
#define F_ULS_DRDY			( 1<< 0)		// Data Ready

// Uart Channel Divisor Latch LSB Register (UxDLL)
// RW - When DLAB 1

#define F_UDLL				(15<< 0)		// Divisor Latch Least Significant Byte

// Uart Channel Divisor Latch MSB Register (UxDLM)
// RW - When DLAB 1

#define F_UDLM				(15<< 0)		// Divisor Latch Most Significant Byte

// Uart IrDA Mode Register (UxIRM)

#define F_UIRM_RPI			( 1<< 5)		// IrDA Rx Polarity Inversion
#define F_UIRM_RDE			( 1<< 4)		// IrDA Rx Decoding Enable
#define F_UIRM_TPI			( 1<< 1)		// IrDA Tx Polariry Inversion
#define F_UIRM_TEE			( 1<< 0)		// IrDA Tx Encoding Enable


typedef enum{
	DATABITS_5=5,
	DATABITS_6=6,
	DATABITS_7=7,
	DATABITS_8=8,
}UART_DATABITS;

typedef enum{
	STOPBITS_1=1,
	STOPBITS_2=2,
}UART_STOPBITS;

typedef enum{
	UART_PARNONE=0,
	UART_PARODD,
	UART_PAREVEN,
}UART_PARITY;

#define	UART_RX_USERBUF_EMPTY	2 /* Rx buffer is empty, no character available */
#define	UART_TX_USERBUF_FULL	3 /* Tx buffer is full, could not deposit char */
#define	UART_TX_USERBUF_EMPTY	4 /* If the Tx buffer is empty. */
#define	UART_RX_USERBUF_TIMEOUT	5 /* If a timeout occurred while waiting for a character*/
#define	UART_TX_USERBUF_TIMEOUT	6 /* If a timeout occurred while waiting to send a char.*/

BOOL uart_config(int ch,int baud,UART_DATABITS databits,UART_STOPBITS stopbit,UART_PARITY parity);
BOOL uart_getch(int n,char* ch);
int uart_getdata(int n,U8* buf, int bufmax);//return data byte length
BOOL uart_putch(int n,char ch);
BOOL uart_putdata(int n,U8* buf, int len);
BOOL uart_putstring(int n,U8* buf);
void uart_rx_flush(int ch);
void uart_tx_flush(int ch);

void set_debug_channel(int ch);
int get_debug_channel();
void r_debugprintf( const char* filename, int linenum,const char *format,	... );
void debugprintf(const char * const format, ...);
void debugstring(const char* str);

//#define DEBUGPRINTF(...)
#define DEBUGPRINTF(...)	r_debugprintf(__FILE__,__LINE__,__VA_ARGS__)
#define PRINTLINE	DEBUGPRINTF("%s , %d \r\n",__FILE__,__LINE__)
#define PRINTVAR(A)	DEBUGPRINTF("%s=0x%x(%d)\r\n",#A,A,A)

//#define ASSERT(A)	if(!(A))debugprintf("ASSERT,%s,%d:(%s)==0\r\n",__FILE__,__LINE__,#A)
///#define ASSERT(A)	// Conflict with FrameWork
