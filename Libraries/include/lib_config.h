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

#define SUPPORT_NESTED_INTNUM_EIRQ0 0
#define SUPPORT_NESTED_INTNUM_TIMER0 0
#define SUPPORT_NESTED_INTNUM_RESV2 0 
#define SUPPORT_NESTED_INTNUM_DMA0 1
	
#define SUPPORT_NESTED_INTNUM_FRAMESYNC 0 
#define SUPPORT_NESTED_INTNUM_GPIO0 0
#define SUPPORT_NESTED_INTNUM_UART0 0 
#define SUPPORT_NESTED_INTNUM_XDMA0 1

#define SUPPORT_NESTED_INTNUM_EIRQ1 0
#define SUPPORT_NESTED_INTNUM_TIMER1 0
#define SUPPORT_NESTED_INTNUM_PMU 0
#define SUPPORT_NESTED_INTNUM_DMA1 1

#define SUPPORT_NESTED_INTNUM_ICE 0
#define SUPPORT_NESTED_INTNUM_GPIO1 0
#define SUPPORT_NESTED_INTNUM_UART1 0
#define SUPPORT_NESTED_INTNUM_XDMA1 1

#define SUPPORT_NESTED_INTNUM_RESV16 0
#define SUPPORT_NESTED_INTNUM_TIMER2 0
#define SUPPORT_NESTED_INTNUM_USBDEV 0
#define SUPPORT_NESTED_INTNUM_TWI 0

#define SUPPORT_NESTED_INTNUM_USBHOST 0
#define SUPPORT_NESTED_INTNUM_I2SRX1 0
#define SUPPORT_NESTED_INTNUM_UART2 0
#define SUPPORT_NESTED_INTNUM_SEIPRX 0

#define SUPPORT_NESTED_INTNUM_RESV24 0
#define SUPPORT_NESTED_INTNUM_WATCHDOG 0
#define SUPPORT_NESTED_INTNUM_NAND 0
#define SUPPORT_NESTED_INTNUM_DMA2 1

#define SUPPORT_NESTED_INTNUM_SDHC 0
#define SUPPORT_NESTED_INTNUM_I2STX1 0
#define SUPPORT_NESTED_INTNUM_SPI0 0
#define SUPPORT_NESTED_INTNUM_SOUNDMIXER 0

#define SUPPORT_NESTED_INTNUM_RESV32 0
#define SUPPORT_NESTED_INTNUM_GPIO2 0
#define SUPPORT_NESTED_INTNUM_SPI1 0
#define SUPPORT_NESTED_INTNUM_TIMER3 0

#define SUPPORT_NESTED_INTNUM_UART3 0
#define SUPPORT_NESTED_INTNUM_GPIO3 0
#define SUPPORT_NESTED_INTNUM_MJPEG_FULL 0 
#define SUPPORT_NESTED_INTNUM_DMA3 1

#define SUPPORT_NESTED_INTNUM_RESV40 0
#define SUPPORT_NESTED_INTNUM_XDMA2 0
#define SUPPORT_NESTED_INTNUM_GPIO4 0
#define SUPPORT_NESTED_INTNUM_MJPEG_DEC_END 0

#define SUPPORT_NESTED_INTNUM_GPIO5 0
#define SUPPORT_NESTED_INTNUM_XDMA3 0
#define SUPPORT_NESTED_INTNUM_GPIO6 0
#define SUPPORT_NESTED_INTNUM_XDMA4 1

#define SUPPORT_NESTED_INTNUM_RESV48 0
#define SUPPORT_NESTED_INTNUM_XDMA5 1
#define SUPPORT_NESTED_INTNUM_GPIO7 0
#define SUPPORT_NESTED_INTNUM_XDMA6 1

#define SUPPORT_NESTED_INTNUM_TIMER_CAP_OVER 0
#define SUPPORT_NESTED_INTNUM_GPIO8 0
#define SUPPORT_NESTED_INTNUM_RESV54 0
#define SUPPORT_NESTED_INTNUM_XDMA7 1

#define SUPPORT_NESTED_INTNUM_RESV56 0
#define SUPPORT_NESTED_INTNUM_GPIO9 0
#define SUPPORT_NESTED_INTNUM_GPIO10 0
#define SUPPORT_NESTED_INTNUM_GPIO11 0

#define SUPPORT_NESTED_INTNUM_GPIO12 0
#define SUPPORT_NESTED_INTNUM_GPIO13 0
#define SUPPORT_NESTED_INTNUM_GPIO14 0
#define SUPPORT_NESTED_INTNUM_GPIO15 0

/******************************************************************************
	INTERNAL TIME OUT
******************************************************************************/
#define TWI_RESP_TIME_OUT_COUNT (7200*100)

/******************************************************************************
	External NAND/SD-Card/USB Memory Supports
******************************************************************************/

#define CONFIG_NAND 1
#define CONFIG_SDCARD 1
#define CONFIG_USBHOST 0

/******************************************************************************
	FAT File System
******************************************************************************/
//drive number
#define DRIVE_NAND 0
#define DRIVE_SDCARD 1
#define DRIVE_USBHOST 2
#define DRIVE_MAX 3

#define CONFIG_FAT_READONLY 0
#define CONFIG_FAT_VOLUMES 3

/*
	refer "include/fatfs/ffconf.h"
	if hangul, needs 136,408 bytes 
*/
#define CONFIG_CODE_PAGE 949


/******************************************************************************
	UART Config 
******************************************************************************/
/*
	if CONFIG_UART_xx_INTERRUPT is defined, Buffer is used 
*/
#define UART_BUF_SIZE 512
#define CONFIG_UART_RX_INTERRUPT
//#define CONFIG_UART_TX_INTERRUPT

/*
	output channel number for debugstring, debugprintf , 
	otherwise, you can use set_debug_channel()
*/
#define DEBUG_CHANNEL 0



/******************************************************************************
*	SOUND Mixer
******************************************************************************/
#define WAVE_BUF_MAX (1024*1024*1) //if wave file is bigger than WAVE_BUF_MAX , interrupt handler read data (max size is WAVE_RELOAD_BUF_MAX). 

//if a wave file is bigger than WAVE_BUF_MAX, WAVE_RELOAD_BUF_MAX is used. 
//if you use MP3, must be bigger than  5Kbyte
// WAVE_BUF_MAX must be bigger than WAVE_RELOAD_BUF_MAX 
#define WAVE_RELOAD_BUF_MAX (1024*128) 
#define DEFAULT_VOLUME 255//max 255
/*
	output sample rate
*/
#define SND_OUTPUT_HZ	48000

/******************************************************************************
	SYSTEM CLOCK
******************************************************************************/
#define OSC_CLOCK 12000000

