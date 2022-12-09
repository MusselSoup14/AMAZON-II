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

#define GE_CONTROL_BASE				0x80000000UL

#define R_GE_PWP	((volatile unsigned int*)0x80000000UL) ///< Packet Write Pointer register
#define R_GE_PRP	((volatile unsigned int*)0x80000004UL) ///< Packet Read Pointer register
#define R_GE_RC		((volatile unsigned int*)0x80000008UL) ///< Rendering Control Register
#define R_GE_CDB	((volatile unsigned int*)0x8000000CUL) ///< Current Display Bank Register
#define R_GE_FCC	((volatile unsigned char*)0x80000010UL) ///< Flip Command Count Register

#define R_TMASA		((volatile unsigned int*)0x80000014UL) //Texture Memory Area Start Address
#define R_GE_FSA0		((volatile unsigned int*)0x80000018UL) //Frame Memory Area Start Address
#define R_GE_FSA1		((volatile unsigned int*)0x8000001cUL) //Frame Memory Area Start Address
#define R_GE_FSA2		((volatile unsigned int*)0x80000020UL) //Frame Memory Area Start Address
#define R_GE_FSA3		((volatile unsigned int*)0x80000024UL) //Frame Memory Area Start Address


typedef enum tagBlendMode 
{ 
	BLEND_ZERO            = 0,
	BLEND_CONSTANT = 1,
	BLEND_SRCALPHA = 2,
	BLEND_SRCPIXEL = 3,
	BLEND_DSTALPHA = 4,
	BLEND_DSTPIXEL = 5,
	BLEND_INVERSION = 8,
} BlendMode;

/***********************************************
	Draw Mode
************************************************/
#define DRAWMODE_TEXTURE      (1<<4)
#define DRAWMODE_TILE		  (1<<5)
#define DRAWMODE_NOREPEAT	  (1<<6)
#define DRAWMODE_TRANSPARENCY (1<<7)
#define DRAWMODE_SHADE        (1<<8)
#define DRAWMODE_ALPHA        (1<<9)
#define DRAWMODE_MOSAIC_2x2	  (1<<10)
#define DRAWMODE_MOSAIC_4x4	  (2<<10)
#define DRAWMODE_MOSAIC_8x8	  (3<<10)
#define DRAWMODE_MOSAIC_16x16 (4<<10)
#define DRAWMODE_MOSAIC_32x32 (5<<10)
#define DRAWMODE_MOSAIC_32x16 (6<<10)
#define DRAWMODE_MOSAIC_16x32 (7<<10)
#define DRAWMODE_PALETTEINDEX (1<<13)


#define GROUP_E0	(1<<0)
#define GROUP_E1	(1<<1)
#define GROUP_E2	(1<<2)
#define GROUP_E3	(1<<3)
#define GROUP_E4	(1<<4)
#define GROUP_E5	(1<<5)
#define GROUP_E6	(1<<6)

extern U32 _ge_CurCommandNum;///< 현재 Packet의 넘버 0-2047
extern U32 g_TMEM_StartAddr;///< Texture memory 시작주소
extern U32 g_TMEM_Size ;


#define GE_WriteCmd( A, D )	*(vU16*)( g_TMEM_StartAddr + (_ge_CurCommandNum<<7) + ((U32)(A)<<1)) = D

void ge_init();
void ge_end_cmd( void );
void ge_wait_empty_cmd_q( void );
void ge_wait_cmd_complete();

void ge_set_texture_mem(U32 addr, U32 size);
void flip( void );
void flip_wait( void );
void async_flip( void );
void async_flip_wait( void );

void ge_draw_rectfill(int x, int y,int w,int h,EGL_COLOR c);
void ge_draw_rectfillalpha(int x, int y,int w,int h,EGL_COLOR acolor);
BOOL surface_set_alpha ( SURFACE *surf, U8 a );

