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

/* USB Controller */
#define __USBBASE           0xF0001000 
#define __USBFA_REG			(__USBBASE + 0x00)
#define __USBPM_REG			(__USBBASE + 0x04)
#define __USBEI_REG			(__USBBASE + 0x08)
#define __USBUI_REG			(__USBBASE + 0x10)
#define __USBEIE_REG		(__USBBASE + 0x14)
#define __USBUIE_REG		(__USBBASE + 0x18)
#define __USBLBFN_REG		(__USBBASE + 0x1C)
#define __USBHBFN_REG		(__USBBASE + 0x20)
#define __USBIND_REG		(__USBBASE + 0x24)
#define __USBMAXP_REG		(__USBBASE + 0x28)
#define __USBE0C_REG		(__USBBASE + 0x2C)
#define __USBIC1_REG		(__USBBASE + 0x2C)
#define __USBIC2_REG		(__USBBASE + 0x30)
#define __USBOC1_REG		(__USBBASE + 0x38)
#define __USBOC2_REG		(__USBBASE + 0x3C)
#define __USBLBOWC_REG		(__USBBASE + 0x40)
#define __USBHBOWC_REG		(__USBBASE + 0x44)
#define __USBEP0D_REG		(__USBBASE + 0x48)
#define __USBEP1D_REG		(__USBBASE + 0x4C)
#define __USBEP2D_REG		(__USBBASE + 0x50)
#define __USBEP3D_REG		(__USBBASE + 0x54)
#define __USBEP4D_REG		(__USBBASE + 0x58)

//----------------------------------
// Interrupt Register Bit Masks
//----------------------------------
#define EP0_IRQ			    0x01
#define EP1_IRQ			    0x02
#define EP2_IRQ			    0x04
#define EP3_IRQ			    0x08
#define EP4_IRQ			    0x10

#define DMA_DONE			0x10
#define SOF_DONE			0x20

#define USB_SUSPEND_IRQ     0x01
#define USB_RESUME_IRQ      0x02
#define USB_RESET_IRQ       0x04

#define USB_RESET_DONE		0x40


#define EP0_LEN 			16		// EP0 MaxPktSize 
#define EP1_LEN 		    64	    // EP1 MaxPktSize 
#define EP2_LEN 			64		// EP2 MaxPktSize 
#define EP3_LEN 			16	    // EP3 MaxPktSize 
#define EP4_LEN 			16	    // EP4 MaxPktSize 
#define MAXIFCNUM			8		// Maximum of 8 interfaces

#define DEV_LEN		18			// Device Descriptor Length
#define CFG_LEN	    39		    // Configuration Descriptor Length

void ep0_isr();
void usb_reset(void);
void usb_resume(void);
void usb_suspend(void);

int write_usb(BYTE *data, int len); 
int read_usb(BYTE *xmit_data);//buf size must be bigger than 64

void usb_set_description(BYTE* cfg,BYTE* dev);
//if -1,not return until data received
// return -1, timeout
void usb_init();


