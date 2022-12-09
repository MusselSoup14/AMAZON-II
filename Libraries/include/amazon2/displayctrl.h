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
#define DC_BASE   0xF4001000

#define R_DC_HT       ((volatile unsigned int *)(DC_BASE + 0x04))
#define R_DC_HS       ((volatile unsigned int *)(DC_BASE + 0x08))
#define R_DC_HA       ((volatile unsigned int *)(DC_BASE + 0x0C))
#define R_DC_VT       ((volatile unsigned int *)(DC_BASE + 0x10))
#define R_DC_VS       ((volatile unsigned int *)(DC_BASE + 0x14))
#define R_DC_VA       ((volatile unsigned int *)(DC_BASE + 0x18))
#define R_DC_STAT       ((volatile unsigned int *)(DC_BASE + 0x20))
#define R_DC_W0_CON      ((volatile unsigned int *)(DC_BASE + 0x24))
#define R_DC_FYC       ((volatile unsigned int *)(DC_BASE + 0x38))
#define R_DC_HR       ((volatile unsigned int*)(DC_BASE + 0x3c))

#define R_DC_W0BA0       ((volatile unsigned int*)(DC_BASE + 0xa8))
#define R_DC_W0BA1       ((volatile unsigned int*)(DC_BASE + 0xac))
#define R_DC_W0BA2       ((volatile unsigned int*)(DC_BASE + 0xb0))
#define R_DC_W0BA3		((volatile unsigned int*)(DC_BASE + 0xb4))

#define R_DC_FESR		((volatile unsigned int*)(DC_BASE + 0xb8))

#define R_DC_W0WOFFSET	((volatile unsigned int*)(DC_BASE + 0xd0))
#define R_DC_W1WOFFSET	((volatile unsigned int*)(DC_BASE + 0xd4))

//for OSD
#define R_OSDBA0		((volatile unsigned int*)(DC_BASE + 0x70))
#define R_OSDBA1		((volatile unsigned int*)(DC_BASE + 0x74))
#define R_OSDIMGW		((volatile unsigned int*)(DC_BASE + 0x78))
#define R_OSDIMGH		((volatile unsigned int*)(DC_BASE + 0x7c))
#define R_OSDTARGETX		((volatile unsigned int*)(DC_BASE + 0x80))
#define R_OSDTARGETY		((volatile unsigned int*)(DC_BASE + 0x84))
#define R_OSDPAL		((volatile unsigned int*)(DC_BASE + 0x88))
#define R_OSDCNTL		((volatile unsigned int*)(DC_BASE + 0x8c))
		  
#define R_OSDALPHA0		((volatile unsigned int*)(DC_BASE + 0x90))
#define R_OSDALPHA1		((volatile unsigned int*)(DC_BASE + 0xC4))
		  
#define R_OSDCOLORKEY		((volatile unsigned int*)(DC_BASE + 0x94))
		  
#define R_OSDTARGETWIDTH		((volatile unsigned int*)(DC_BASE + 0xA0))
#define R_OSDTARGETHEIGHT		((volatile unsigned int*)(DC_BASE + 0xA4))


typedef enum {
	SCREEN_480x272=0,
	SCREEN_640x480,
	SCREEN_800x480,
	SCREEN_800x600,
	SCREEN_1024x600,
	SCREEN_1024x768,
	SCREEN_1280x720,
	SCREEN_1280x768,
	//for TV OUT
	SCREEN_TV_HD, //1280x720
	SCREEN_TV_NTSC,
	SCREEN_TV_PAL,
}SCREENRES;
typedef enum {
	VMODE_NTSC=0,
	VMODE_PAL=1,
	VMODE_SD=2,
	VMODE_RGB=3,
	VMODE_HD=4
}VIDEO_OUT_MODE;

//screen mode
#define SCREENMODE_RGB565		(0<<0)
#define SCREENMODE_RGB888		(1<<0)
#define SCREENMODE_SINGLEBUF	(0<<1)
#define SCREENMODE_DOUBLEBUF	(1<<1)

U32 get_screen_width();
U32 get_screen_height();
U32 get_screen_pitch();
U32 get_screen_bpp();

void setscreenex(U32 framebufferAddr,U32 width,U32 height,U32 scmode,U32 ht,U32 hs, U32 ha, U32 vt, U32 vs, U32 va,VIDEO_OUT_MODE outmode);
void setscreen(U32 framebufferAddr,SCREENRES size,U32 scmode);
void vga_mode_on();//if VGA Output mode, must be set

//for double buffering
void flip();
SURFACE* get_front_buffer();
SURFACE* get_back_buffer();
SURFACE* get_back_buffer2();
SURFACE* get_back_buffer3();
SURFACE* get_ge_target_buffer();//return current target buffer for Graphic engine, when using GUI, don't use this.
void enable_doublebuffer_mode(BOOL b);
