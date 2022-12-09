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
#include "sdk.h"

#define MAX_FRAMEBUFFER 2
static U32 screen_w;
static U32 screen_h;
static U32 screen_bpp = 16;
static U32 framebuffer_base_addr = 0;
SURFACE g_FrameSurface[MAX_FRAMEBUFFER] = {{0,},};
static U32 single_frame_memsize;

static  inline void set_reg_verify8(U32 addr, U8 data)
{
	*(vU8*)addr = data;
	while ((*(vU8*)addr) != data);

}
static inline void set_reg_verify32(U32 addr, U32 data)
{
	*(vU32*)addr = data;
	while ((*(vU32*)addr) != data);
}


U32 get_screen_width()
{
	return screen_w;
}

U32 get_screen_height()
{
	return screen_h;
}

U32 get_screen_pitch()
{
	U32 bytespixel = screen_bpp / 8;

	if ( screen_w <= 512 )
		return 512 * bytespixel;
	else if ( screen_w <= 1024 )
		return 1024 * bytespixel;
	else
		return 2048 * bytespixel;
}

U32 get_screen_bpp()
{
	return screen_bpp;
}
void setscreenex( U32 framebufferAddr, U32 width, U32 height, U32 scmode, U32 ht, U32 hs, U32 ha, U32 vt, U32 vs, U32 va ,VIDEO_OUT_MODE vmode)
{
	U32 conval;
	framebuffer_base_addr = framebufferAddr;
	*R_DC_HT  = ht;
	*R_DC_HS  = hs;
	*R_DC_HA  = ha;
	*R_DC_VT  = vt;
	*R_DC_VS  = vs;
	*R_DC_VA  = va;
	*R_DC_HR = width;
	screen_w = width;
	screen_h = height;
	
	if((vmode ==  VMODE_NTSC)||(vmode ==  VMODE_PAL))
	{
		*R_DC_HR = 1024;
		conval = (1<<7);//TV-OUT
	}
	else
	{
		conval = ( vmode << 27 ); 
	}
	DEBUGPRINTF ( " CRTC %d x %d Setting Done\r\n", width, height );

	if ( scmode & SCREENMODE_RGB888 )
	{
		debugstring ( "RGB888 Mode\r\n" );
		screen_bpp = 32;
		conval |= ( 1 << 13 );
	}
	else
	{
		debugstring ( "RGB565 Mode\r\n" );
		screen_bpp = 16;
		conval |= ( 1 << 12 );
	}

	int i;

	for ( i = 3; i < 0x10; i++ )
	{
		if ( width <= ( 1 << i ) )
			break;
	}

	g_FrameSurface[0].bpp = screen_bpp;
	g_FrameSurface[0].w = width;
	g_FrameSurface[0].h = height;
	g_FrameSurface[0].pitch = ( 1 << i ) * ( screen_bpp / 8 );
	g_FrameSurface[0].TileXSize = i - 3;

	for ( i = 3; i < 0x10; i++ )
	{
		if ( height <= ( 1 << i ) )
			break;
	}

	g_FrameSurface[0].TileYSize = i;
	g_FrameSurface[0].pixels = ( void * ) framebufferAddr;

	single_frame_memsize = g_FrameSurface[0].pitch * ( 1 << g_FrameSurface[0].TileYSize );

	memcpy(&g_FrameSurface[1],&g_FrameSurface[0],sizeof(SURFACE));

	if ( scmode & SCREENMODE_DOUBLEBUF )
	{
		debugstring ( "Double Buffer Mode\r\n" );
		conval |= ( 1 << 19 );
		g_FrameSurface[1].pixels = (void*)(framebufferAddr + single_frame_memsize);
		DEBUGPRINTF("Frame-buffer(2bank)  : %dMbyte assigned\r\n",single_frame_memsize*2/(1024*1024));
	}
	else
		DEBUGPRINTF("Frame-buffer(1bank)  : %dMbyte assigned\r\n",single_frame_memsize/(1024*1024));

	*R_DC_W0BA0 = (U32)g_FrameSurface[0].pixels;
	*R_DC_W0BA1 = (U32)g_FrameSurface[1].pixels;
	*R_DC_W0BA2 = (U32)g_FrameSurface[0].pixels;
	*R_DC_W0BA3 = (U32)g_FrameSurface[1].pixels;

	ge_init();

	if ( scmode & SCREENMODE_DOUBLEBUF )
	{
		*R_DC_FESR = (1<<4)|(1<<1);//Window 0 Flip enable by Graphic engine
		*R_GE_FSA0= (U32)g_FrameSurface[1].pixels;
		*R_GE_FSA1= (U32)g_FrameSurface[0].pixels;
		*R_GE_FSA2= (U32)g_FrameSurface[1].pixels;
		*R_GE_FSA3= (U32)g_FrameSurface[0].pixels;
	}
	else
	{
		*R_GE_FSA0= (U32)g_FrameSurface[0].pixels;
		*R_GE_FSA1= (U32)g_FrameSurface[0].pixels;
		*R_GE_FSA2= (U32)g_FrameSurface[0].pixels;
		*R_GE_FSA3= (U32)g_FrameSurface[0].pixels;
	}
	*R_DC_W0_CON = conval ;
	*R_DC_W0_CON = conval | (1<<24);//crtc reset

	if((vmode != VMODE_NTSC) && (vmode != VMODE_PAL))
		*R_DC_W0WOFFSET = g_FrameSurface[0].pitch/(g_FrameSurface[0].bpp/8) - screen_w;
	else
		*R_DC_W0WOFFSET = 0;
	
	if(vmode == VMODE_NTSC)
	{
		*((volatile unsigned int *)0xf4001000) = 0x30;

		set_reg_verify8(0xf400241c, 0);
		set_reg_verify8(0xf4002440, 0x92); // 601,60Hz,FID
		set_reg_verify8(0xf4002448, 0x07); // gen hav_on,vav_on,fd_on BT601
		set_reg_verify8(0xf40026C4, 0x0); //NTSC
	}
	else if(vmode == VMODE_PAL)
	{
		*((volatile unsigned int *)0xf4001000) = 0x30;
		set_reg_verify8(0xf400241c, 0);
		set_reg_verify8(0xf4002440, 0x91);// 601,50Hz,FID
		set_reg_verify8(0xf4002448, 0x07);// gen hav_on,vav_on,fd_on BT601
		set_reg_verify8(0xf4002680, 0x02);//PAL
		set_reg_verify8(0xf4002684, 0x3E);
		set_reg_verify8(0xf4002688, 0x00);
		set_reg_verify8(0xf400268C, 0x4A);
		set_reg_verify8(0xf4002690, 0x00);
		set_reg_verify8(0xf4002694, 0x6A);
		set_reg_verify8(0xf4002698, 0x00);

		set_reg_verify8(0xf40026C4, 0x02);// PAL
		set_reg_verify8(0xf40026C8, 0x13);
	}
	if ((vmode == VMODE_NTSC) || (vmode == VMODE_PAL))
	{
		*(vU32*)0xf4000000 = 1 << 14;  //write enable  
		*(vU32*)0xf4000038 &= ~(0x9 << 12); //DAC(R) on
		*(vU32*)0xf4000000 = 0x0;  //write disable

		set_reg_verify8(0xf40026C8, 0x00); // Y/C Filter in Encoder Block (3)
		set_reg_verify8(0xf40026CC, 0x20); // Chroma Trap Filter Gain Control(30)
		set_reg_verify8(0xf40026D4, 0x00); // HUE Control(00)
		set_reg_verify8(0xf40026F0, 0x10); // C data delay control (10)

		set_reg_verify8(0xf4002548, 0x50); // Cb Gain Control (40)
		set_reg_verify8(0xf400254c, 0x40); // Cr Gain Control (40)


		//set_reg_verify8(0xf4002550 ,0x03); // LTI/CTI Enable Control

		set_reg_verify8(0xf40025B8, 0x01); // Color Tone Enhancer Enable Control(1)
		set_reg_verify8(0xf40025C4, 0x70); // Color Tone Enhancer Gain Control(40)

		set_reg_verify8(0xf40025DC, 0x11); // Fesh Tone Correction Control0 (11)
		set_reg_verify8(0xf40025DC, 0x7c); // Fesh Tone Correction Control1 (7c)

		set_reg_verify8(0xf4002624, 0x01); // Gamma Control

		set_reg_verify8(0xf4002600, 0x1F); // Horizontal Peaking Gain(1F)

	}
	else
	{
		*(vU32*)0xf4000000 = 1 << 14;  //write enable  
		*(vU32*)0xf4000038 |= (0xf << 12); //DAC off
		*(vU32*)0xf4000000 = 0x0;  //write disable
	}
	
	for(i=0;i<MAX_FRAMEBUFFER;i++)
	{
		g_FrameSurface[i].clip.x=0;
		g_FrameSurface[i].clip.y=0;
		g_FrameSurface[i].clip.endx=screen_w-1;
		g_FrameSurface[i].clip.endy=screen_h-1;
		if ( scmode & SCREENMODE_RGB888 )
			g_FrameSurface[i].pixtype = PIX_FMT_RGB888;
		else
			g_FrameSurface[i].pixtype = PIX_FMT_RGB565;
		
	}
	if(scmode & SCREENMODE_DOUBLEBUF)
		set_draw_target(&g_FrameSurface[1]);
	else
		set_draw_target(&g_FrameSurface[0]);
}

void vga_mode_on()
{
	int i;
	int y;
	int x = screen_w-1;
	int pitch = get_screen_pitch();
	if(screen_bpp==32)
	{
		for(i=0;i<MAX_FRAMEBUFFER;i++)
		{
			U32 addr = (U32)g_FrameSurface[i].pixels;
			addr += (x*4);
			for(y=0;y<screen_h;y++)
			{
				*(U32*)addr=0;
				addr += pitch;
			}
			g_FrameSurface[i].clip.endx--; // if VGA Mode, not support the last vertical line.
		}
	}
	else
	{
		for(i=0;i<MAX_FRAMEBUFFER;i++)
		{
			U32 addr = (U32)g_FrameSurface[i].pixels;
			addr += (x*2);
			for(y=0;y<screen_h;y++)
			{
				*(U16*)addr=0;
				addr += pitch;
			}
			g_FrameSurface[i].clip.endx--; // if VGA Mode, not support the last vertical line.
		}
	}
	*(vU32*)0xf4000000 = 1 << 14;  //write enable  
	*(vU32*)0xf4000038 &= ~(0xf << 12); //DAC on
	*(vU32*)0xf4000000 = 0x0;  //write disable
}
/*
1280, 720

HT:1648
HSS:72 
HSE:152
HAS:368
HAE:1648
VT:750
VSS:3
VSE:8
VAS:30
VAE:750
Dot Clock: 74.2MHz
*/
void setscreen ( U32 framebufferAddr, SCREENRES res, U32 scmode )
{
	switch ( res )
	{
	case SCREEN_480x272: //dot clock 10Mhz
		setscreenex ( framebufferAddr, 480, 272, scmode, 0x0000020D, 0x0002002B, 0x002D020D , 0x0000011E , 0x0002000C , 0x000E011E,VMODE_RGB );
		break;

	case SCREEN_640x480://dot clock 25.175Mhz
		setscreenex ( framebufferAddr, 640, 480, scmode, 0x00000320, (16<<16)|(16+96), ((16+96+41)<<16)|(640+16+96+41), 0x0000020D, (10<<16)|(10+2), ((10+2+33)<<16)|(480+10+2+33),VMODE_RGB );
		break;

	case SCREEN_800x480://dot clock 33Mhz
		//innolux A070TN94, DR070TN94,
		//Hyundai HD-800G480-70NT-6F3
		//setscreenex ( framebufferAddr, 800, 480, scmode, 1054, ( 210 << 16 ) | 212, ( 254 << 16 ) | 1054 , 524 , ( 22 << 16 ) | 23 , ( 44 << 16 ) | 524 ,VMODE_RGB);
		setscreenex ( framebufferAddr, 800, 480, scmode, 824, ( 8 << 16 ) | 14, ( 24 << 16 ) | 824 , 493 , ( 5 << 16 ) | 2 , ( 13 << 16 ) | 493 ,VMODE_RGB);
		break;

	case SCREEN_800x600://dot clock 40Mhz
		setscreenex ( framebufferAddr, 800, 600, scmode, 0x00000420, 0x002300b0, 0x01000420 , 0x00000274 , 0x0004000a , 0x001a0272 ,VMODE_RGB);
		break;

	case SCREEN_1024x600://dot clock 66Mhz
		setscreenex ( framebufferAddr, 1024, 600, scmode, 1344, (120<<16)|180, (320<<16) | 1344, 635, (15<<16)|20, (35<<16)|635,VMODE_RGB);
		break;
	case SCREEN_1024x768://dot clock 66Mhz
		setscreenex ( framebufferAddr, 1024, 768, scmode, 0x00000540, 0x002300c3, 0x01400540, 0x00000325, 0x0007000d, 0x00230323 ,VMODE_RGB);
		break;
	case SCREEN_1280x720://dot clock 66Mhz
		setscreenex ( framebufferAddr, 1280, 720, scmode, 0x00000672, 0x006e0096, 0x01720672 , 0x000002ee , 0x0006000b , 0x001e02ee,VMODE_RGB );
		break;
	case SCREEN_1280x768://dot clock 79.5Mhz
		setscreenex ( framebufferAddr, 1280, 720, scmode,1664, ((64) << 16)|(64+128), ((64+128+192)<<16)|(64+128+192+1280), 798, (3 << 16) | (3+7), ((7+20) << 16) | (768+7+20),VMODE_RGB );
		break;
	case SCREEN_TV_HD://dot clock 74Mhz
		//HD encoder
		*(vU32*)0xf4002800 = 1;
		setscreenex ( framebufferAddr, 1280, 720, scmode, 0x00000672, 0x006e0096, 0x01720672 , 0x000002ee , 0x0006000b , 0x001e02ee,VMODE_HD );
		break;

	case SCREEN_TV_NTSC://dot clock 27Mhz
		setscreenex ( framebufferAddr, 720, 480, scmode, 0x000006b4, 0x002000a0, 0x011406b4, 0x0000020d, 0x0007000d, 0x002b020b ,VMODE_NTSC);
		break;

	case SCREEN_TV_PAL://dot clock 27Mhz
		setscreenex ( framebufferAddr, 720, 572, scmode, 0x000006c0, 0x002900A8, 0x014506c0, 0x00000271, 0x00070010, 0x002f026b ,VMODE_PAL);
		break;

	default:
		debugstring ( "not supported screen size\r\n" );
		return;
	}
}

SURFACE* get_back_buffer()
{
	U8 bank;
	while ( *R_GE_FCC );
	bank = (( *R_DC_FESR) >> 8)+1;
	bank = bank % MAX_FRAMEBUFFER;
	return &g_FrameSurface[bank];
}

SURFACE* get_back_buffer2()
{
	U8 bank;
	while ( *R_GE_FCC );
	bank = (( *R_DC_FESR) >> 8)+2;
	bank = bank % MAX_FRAMEBUFFER;
	return &g_FrameSurface[bank];
}

SURFACE *get_back_buffer3()
{
	U8 bank;
	while ( *R_GE_FCC );
	bank = (( *R_DC_FESR) >> 8)+3;
	bank = bank % MAX_FRAMEBUFFER;
	return &g_FrameSurface[bank];
}

SURFACE *get_front_buffer()
{
	U8 bank;
	while ( *R_GE_FCC );
	bank = (( *R_DC_FESR) >> 8);
	bank = bank % MAX_FRAMEBUFFER;
	return &g_FrameSurface[bank];
}

SURFACE* get_ge_target_buffer()
{
	if ( ( *R_DC_FESR ) & ( 1 << 4 ) )
		return get_back_buffer();

	return get_front_buffer();
}

void enable_doublebuffer_mode(BOOL b)	
{	
	ge_wait_cmd_complete();			
	if(!b)
	{
		SURFACE* front_buf = get_front_buffer();
		set_draw_target(front_buf);								
		*R_GE_FSA0= (U32)front_buf->pixels;
		*R_GE_FSA1= (U32)front_buf->pixels;
		*R_GE_FSA2= (U32)front_buf->pixels;
		*R_GE_FSA3= (U32)front_buf->pixels;						
		*R_GE_FSA3= (U32)front_buf->pixels;						
	}
	else
	{				
		set_draw_target(get_back_buffer());			
		*R_GE_FSA0= (U32)g_FrameSurface[1].pixels;
		*R_GE_FSA1= (U32)g_FrameSurface[0].pixels;
		*R_GE_FSA2= (U32)g_FrameSurface[1].pixels;
		*R_GE_FSA3= (U32)g_FrameSurface[0].pixels;				
		*R_GE_FSA3= (U32)g_FrameSurface[0].pixels;				
	}	
}
