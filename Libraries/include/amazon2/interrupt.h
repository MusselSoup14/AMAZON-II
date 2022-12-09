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

#define INTERRUPT_CTRL_BASE	0xf4004800

extern int critical_ref_counter;
// back up interrupt status, disable interrupt
#define CRITICAL_ENTER()	do{asm("set 8"); asm("sync"); critical_ref_counter++;} while (0)
// restore interrupt status
#define CRITICAL_EXIT() do{\
		if(critical_ref_counter>0){\
			critical_ref_counter--;\
		}\
		if(critical_ref_counter==0){\
			asm("clr 8"); asm("sync");\
		}\
	} while (0)

#define INTERRUPT_ENABLE	asm("set 13");
#define INTERRUPT_DISABLE	asm("clr 13");
#define NMI_ENABLE			asm("set 14");
#define NMI_DISABLE			asm("clr 14");
#define SETVECTORED			asm("set 12");

// Interrupt Pending Clear Register
#define R_INTPENDCLR		((volatile unsigned int*)INTERRUPT_CTRL_BASE)

// External Interrupt Mode and External PIN Level Register
#define R_EINTMOD		((volatile unsigned int*)(INTERRUPT_CTRL_BASE+4))

#define F_EINTMOD_1ST			(1<<7)
#define F_EINTMOD_1MOD_LOW		(0<<4)
#define F_EINTMOD_1MOD_HIGH		(1<<4)
#define F_EINTMOD_1MOD_FEDGE	(2<<4)
#define F_EINTMOD_1MOD_REDGE	(3<<4)
#define F_EINTMOD_1MOD_AEDGE	(4<<4)

#define F_EINTMOD_0ST			(1<<3)
#define F_EINTMOD_0MOD_LOW		(0<<0)
#define F_EINTMOD_0MOD_HIGH		(1<<0)
#define F_EINTMOD_0MOD_FEDGE	(2<<0)
#define F_EINTMOD_0MOD_REDGE	(3<<0)
#define F_EINTMOD_0MOD_AEDGE	(4<<0)

// Internal Interrupt Mode Register
#define R_IINTMOD(n)	((volatile unsigned int*)((INTERRUPT_CTRL_BASE+8) + ((n)*0x40)))

// Interrupt Pending Register
#define R_INTPEND(n)	((volatile unsigned int*)((INTERRUPT_CTRL_BASE+0xc) + ((n)*0x40)))

// Interrupt Enable Register
#define R_INTEN(n)		((volatile unsigned int*)((INTERRUPT_CTRL_BASE+0x10) + ((n)*0x40)))

// Interrupt Mask Set Register
#define R_INTMASKSET(n)	((volatile unsigned int*)((INTERRUPT_CTRL_BASE+0x14) + ((n)*0x40)))

// Interrupt Mask Status Register
#define R_INTMASK(n)	((volatile unsigned int*)((INTERRUPT_CTRL_BASE+0x14)+ ((n)*0x40)))

// Interrupt Mask Clear Register
// R_INTMASKCLR
#define R_INTMASKCLR(n)	((volatile unsigned int*)((INTERRUPT_CTRL_BASE+0x18)+ ((n)*0x40)))

// Programmable Interrupt Priority Enable Register
#define R_PIPENR	((volatile unsigned int*)((INTERRUPT_CTRL_BASE+0x1c))

#define F_PIPENR_ENABLE			(1<<0)			// Programmable Priority Enable
#define F_PIPENR_DISABLE			(0<<0)			// Programmable Priority Disable

// Interrupt Priority Vector Register
#define R_IPVR0	((volatile unsigned int*)(INTERRUPT_CTRL_BASE+0x20)
#define R_IPVR1	((volatile unsigned int*)(INTERRUPT_CTRL_BASE+0x24)
#define R_IPVR2	((volatile unsigned int*)(INTERRUPT_CTRL_BASE+0x28)
#define R_IPVR3	((volatile unsigned int*)(INTERRUPT_CTRL_BASE+0x2c)
#define R_IPVR4	((volatile unsigned int*)(INTERRUPT_CTRL_BASE+0x30)
#define R_IPVR5	((volatile unsigned int*)(INTERRUPT_CTRL_BASE+0x34)
#define R_IPVR6	((volatile unsigned int*)(INTERRUPT_CTRL_BASE+0x38)
#define R_IPVR7	((volatile unsigned int*)(INTERRUPT_CTRL_BASE+0x3c)

typedef enum 
{
	INTNUM_EIRQ0=0,
	INTNUM_TIMER0,
	INTNUM_RESV2,
	INTNUM_DMA0,

	INTNUM_FRAMESYNC,
	INTNUM_GPIO0,
	INTNUM_UART0,
	INTNUM_XDMA0,

	INTNUM_EIRQ1, //8
	INTNUM_TIMER1,
	INTNUM_PMU,
	INTNUM_DMA1,

	INTNUM_ICE, //12,0xc
	INTNUM_GPIO1,
	INTNUM_UART1,
	INTNUM_XDMA1,

	INTNUM_RESV16, //16,0x10 
	INTNUM_TIMER2,
	INTNUM_USBDEV,
	INTNUM_TWI,

	INTNUM_USBHOST,//20,0x14
	INTNUM_I2SRX1,
	INTNUM_UART2,
	INTNUM_SEIPRX,

	INTNUM_RESV24,//24
	INTNUM_WATCHDOG,
	INTNUM_NAND,
	INTNUM_DMA2,

	INTNUM_SDHC,//28
	INTNUM_I2STX1,
	INTNUM_SPI0,
	INTNUM_SOUNDMIXER,

	INTNUM_RESV32,//32
	INTNUM_GPIO2,
	INTNUM_SPI1,
	INTNUM_TIMER3,

	INTNUM_UART3,//36
	INTNUM_GPIO3,
	INTNUM_MJPEG_FULL,
	INTNUM_DMA3,

	INTNUM_RESV40,//40
	INTNUM_XDMA2,
	INTNUM_GPIO4,
	INTNUM_MJPEG_DEC_END,

	INTNUM_GPIO5,//44
	INTNUM_XDMA3,
	INTNUM_GPIO6,
	INTNUM_XDMA4,

	INTNUM_RESV48,//48
	INTNUM_XDMA5,
	INTNUM_GPIO7,
	INTNUM_XDMA6,

	INTNUM_TIMER_CAP_OVER,//52
	INTNUM_GPIO8,
	INTNUM_RESV54,
	INTNUM_XDMA7,

	INTNUM_RESV56,//56
	INTNUM_GPIO9,
	INTNUM_GPIO10,
	INTNUM_GPIO11,

	INTNUM_GPIO12,//60
	INTNUM_GPIO13,
	INTNUM_GPIO14,
	INTNUM_GPIO15,
	
	INTNUM_MAX
}INTERRUPT_TYPE;

void init_interrupt();
BOOL set_interrupt(INTERRUPT_TYPE intnum,void (*fp)());
BOOL set_interrupt_direct(int intnum,void (*fp)());//ISR must be defined as "#pragma  interrupt"
void* get_interrupt(INTERRUPT_TYPE intnum);
void enable_interrupt(INTERRUPT_TYPE num,BOOL b);

