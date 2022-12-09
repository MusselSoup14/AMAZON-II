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
//======================================================================================================================
// PMU Write Enable Register (PMUWREN)

#define R_PMUWREN  ((volatile unsigned int*)0xF4000000)

#define F_PMUWREN_BOD		(1 <<14)		// BOD Control Register Write Enable
#define F_PMUWREN_USB		(1 <<13)		// USB PHY Control Register Write Enable
#define F_PMUWREN_PCLK	(1 <<12)		// PCLK Control Register Write Enable
#define F_PMUWREN_HCLK	(1 <<11)		// HCLK Control Register Write Enable
#define F_PMUWREN_SOUND	(1 <<10)		// Sound Clock Control Register Write Enable
#define F_PMUWREN_PLL		(1 << 9)		// PLL Control Register Write Enable
#define F_PMUWREN_CLOCK	(1 << 8)		// Clock Control Register Write Enable
#define F_PMUWREN_HALT	(3 << 1)		// Halt Control Register Write Enable

#define PMU_WRITE_ENABLE	(*(volatile unsigned int*)0xF4000000 = 0x7fff)//BOD is at 14bit
#define PMU_WRITE_DISABLE	(*(volatile unsigned int*)0xF4000000 = 0)
//======================================================================================================================

//======================================================================================================================
// Clock Control Register (CLKCON)

#define R_CLKCON  ((volatile unsigned int*)0xF4000030)

#define F_CLKCON_USBCLK_PLL2			(0 <<20)		// USB clock select - PLL2.
#define F_CLKCON_USBCLK_PLL1			(1 <<20)		// USB clock select - PLL1.

#define F_CLKCON_DACCLK_DCLK			(0 <<18)		// DAC clock select - dotclock.
#define F_CLKCON_DACCLK_NDCLK			(1 <<18)		// DAC clock select - negative dotclock.

#define F_CLKCON_DCLK_SCLK1				(0 <<16)		// Dotclock - Dotclock / 1	
#define F_CLKCON_DCLK_SCLK2				(1 <<16)		// Dotclock - Dotclock / 2	
#define F_CLKCON_DCLK_SCLK4				(2 <<16)		// Dotclock - Dotclock / 4	
#define F_CLKCON_DCLK_SCLK8				(3 <<16)		// Dotclock - Dotclock / 8	

#define F_CLKCON_DCLK_PLL3				(0 <<14)		// Dotclock select - PLL 3
#define F_CLKCON_DCLK_CRTC				(1 <<14)		// Dotclock select - CRTC clock (GP11/LCD_VCLKIN)
#define F_CLKCON_DCLK_ICE				(2 <<14)		// Dotclock select - ICE clock (GP3/VIN_CLK)

#define F_CLKCON_I2S_PLL1				(0 <<12)		// I2S clock select - PLL1
#define F_CLKCON_I2S_PLL2				(1 <<12)		// I2S clock select - PLL2
#define F_CLKCON_I2S_SOUND				(2 <<12)		// I2S clock select - sound clock

#define F_CLKCON_AHB_PLL1				(0 <<11)		// AHB clock select - PLL1
#define F_CLKCON_AHB_PLL2				(1 <<11)		// AHB clock select - PLL2

#define F_CLKCON_AHB_SCLK1				(0 <<8)			// AHB Clock - Src Clock / 1
#define F_CLKCON_AHB_SCLK2				(1 <<8)			// AHB Clock - Src Clock / 2
#define F_CLKCON_AHB_SCLK4				(2 <<8)			// AHB Clock - Src Clock / 4
#define F_CLKCON_AHB_SCLK8				(3 <<8)			// AHB Clock - Src Clock / 8
#define F_CLKCON_AHB_SCLK16				(4 <<8)			// AHB Clock - Src Clock / 16
#define F_CLKCON_AHB_SCLK1024			(5 <<8)			// AHB Clock - Src Clock / 1024

#define F_CLKCON_APB_SCLK4				(0 <<7)			// APB Clock - System Clock / 4
#define F_CLKCON_APB_SCLK2				(1 <<7)			// APB Clock - System Clock / 2

#define F_CLKCON_MCLK_SCLK1				(0 <<4)			// MCLK - Src Clock / 1
#define F_CLKCON_MCLK_SCLK2				(1 <<4)			// MCLK - Src Clock / 2
#define F_CLKCON_MCLK_SCLK4				(2 <<4)			// MCLK - Src Clock / 4
#define F_CLKCON_MCLK_SCLK8				(3 <<4)			// MCLK - Src Clock / 8
#define F_CLKCON_MCLK_SCLK16			(4 <<4)			// MCLK - Src Clock / 16
#define F_CLKCON_MCLK_SCLK1024			(5 <<4)			// MCLK - Src Clock / 1024

#define F_CLKCON_PLL3_OSC				(0 <<3)			// PLL3 SEL select - OSC clock
#define F_CLKCON_PLL3_PLL3				(1 <<3)			// PLL3 SEL select - PLL3 output clock

#define F_CLKCON_PLL2_OSC				(0 <<2)			// PLL2 SEL select - OSC clock
#define F_CLKCON_PLL2_PLL2				(1 <<2)			// PLL2 SEL select - PLL2 output clock

#define F_CLKCON_PLL1_OSC				(0 <<1)			// PLL1 SEL select - OSC clock
#define F_CLKCON_PLL1_PLL1				(1 <<1)			// PLL1 SEL select - PLL1 output clock

#define F_CLKCON_PLL0_OSC				(0 <<0)			// PLL0 SEL select - OSC clock
#define F_CLKCON_PLL0_PLL0				(1 <<0)			// PLL0 SEL select - PLL0 output clock
//======================================================================================================================

//======================================================================================================================
// Clock Enable Register 

#define R_CLKEN	((volatile unsigned int*)0xF4000034)

#define F_CLKEN_AHB_ENABLE				(1<<30)			// AHB Bus Clock Enable
#define F_CLKEN_APB_ENABLE				(1<<29)			// APB Bus Clock Enable
#define F_CLKEN_AXI_ENABLE				(1<<28)			// AXI Bus Clock Enable
#define F_CLKEN_USBHOST_ENABLE			(1<<25)			// USB Host Clock Enable
#define F_CLKEN_USBDEVICE_ENABLE		(1<<24)			// USB Device Clock Enable
#define F_CLKEN_MJPEG_ENABLE			(1<<23)			// MJPEG Clock Enable
#define F_CLKEN_AHBDMA_ENABLE			(1<<22)			// AHB DMA Clock Enable
#define F_CLKEN_SDHC_ENABLE				(1<<21)			// SDHC Clock Enable
#define F_CLKEN_NAND_ENABLE				(1<<20)			// NAND Clock Enable
#define F_CLKEN_EXSRAM_ENABLE			(1<<18)			// External Sram Clock Enable
#define F_CLKEN_SERIAL_ENABLE			(1<<17)			// Serial Flash Clock Enable
#define F_CLKEN_GPIO_ENABLE				(1<<16)			// GPIO Clock Enable
#define F_CLKEN_INTCON_ENABLE			(1<<14)			// Interrupt Clock Enable
#define F_CLKEN_I2S_ENABLE				(1<<13)			// I2S Clock Enable
#define F_CLKEN_SPI_ENABLE				(1<<12)			// SPI Clock Enable
#define F_CLKEN_TWI_ENABLE				(1<<11)			// TWI Clock Enable
#define F_CLKEN_TIMER_ENABLE			(1<<10)			// Timer Clock Enable
#define F_CLKEN_WDT_ENABLE				(1<<9)			// Watch Dog Timer Clock Enable
#define F_CLKEN_UART_ENABLE				(1<<8)			// UART Clock Enable
#define F_CLKEN_DDR_ENABLE				(1<<7)			// DDR Clock Enable
#define F_CLKEN_HDVIDEO_ENABLE			(1<<6)			// HD Video Clock Enable
#define F_CLKEN_VIDEOEN_ENABLE			(1<<5)			// Video Encoder Clock Enable
#define F_CLKEN_BT656DEC_ENABLE			(1<<4)			// BT656 Decoder Clock Enable
#define F_CLKEN_GRAPHIC_ENABLE			(1<<3)			// Graphic Engine Clock Enable
#define F_CLKEN_DEBUG_ENABLE			(1<<2)			// Debugger Clock Enable
#define F_CLKEN_CRTC_ENABLE				(1<<1)			// CRTC Clock Enable
#define F_CLKEN_AXIDMA_ENABLE			(1<<0)			// AXI DMA Clock Enable

//======================================================================================================================

//======================================================================================================================
// System Control Register 

#define R_SYSCON	((volatile unsigned int*)0xF4000038)

#define F_SYSCON_DACR_PDOWN				(1 <<15)		// DAC R channel power down
#define F_SYSCON_DACB_PDOWN				(1 <<14)		// DAC B channel power down
#define F_SYSCON_DACG_PDOWN				(1 <<13)		// DAC G channel power down
#define F_SYSCON_DAC_PDOWN				(1 <<12)		// DAC power down

#define F_SYSCON_USBDEVICE				(0 <<11)		// USB phy select - USB Device
#define F_SYSCON_USBHOST				(1 <<11)		// USB phy select - USB HOST

#define F_SYSCON_DPDNPULDOWN			(1 << 6)		// Dp, Dn pull-down Enable bit - 0 Disable / 1 Enable
#define F_SYSCON_DNPULDOWN				(1 << 5)		// Dn pull-down Enable bit - 0 Disable / 1 Enable
#define F_SYSCON_DPPULDOWN				(1 << 4)		// Dp pull-down Enable bit - 0 Disable / 1 Enable
#define F_SYSCON_DNPULUP_IDLE			(1 << 3)		// Dn IDLE pull-up Enable bit - 0 Disable / 1 Enable
#define F_SYSCON_DNPULUP_ALWAYS			(1 << 2)		// Dn ALWAYS pull-up Enable bit - 0 Disable / 1 Enable
#define F_SYSCON_DPPULUP_IDLE			(1 << 1)		// Dp IDLE pull-up Enable bit - 0 Disable / 1 Enable
#define F_SYSCON_DPPULUP_ALWAYS			(1 << 0)		// Dp ALWAYS pull-up Enable bit - 0 Disable / 1 Enable

U32 get_axi_clock();
U32 get_ahb_clock();
U32 get_apb_clock();

U32 get_pll(int num);
void usb_clock_init();
void crtc_clock_init();

