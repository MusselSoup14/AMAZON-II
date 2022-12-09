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
#include <math.h>
BOOL sw_draw_surface( SURFACE* src_surf, int dx, int dy );
BOOL sw_draw_surface_rect( SURFACE* ssrf, int dx, int dy, int sx, int sy, int w, int h );
BOOL sw_draw_surface_scale(SURFACE* src_surf,int dx, int dy,int dw,int dh);
BOOL sw_draw_surface_scalerect(SURFACE* src_surf,int dx, int dy,int dw,int dh,int sx, int sy, int sw, int sh);

BOOL ge_check_clipwindow(S32* InitDX, S32* InitDY, S32* EndX, S32* EndY,
						 S32* InitSX, S32* InitSY, S32 dxSx, S32 dxSy, S32 dySx, S32 dySy)
{
	SURFACE* targetsurf = get_ge_target_buffer();
	S32 diff;
	if ( *InitDX < targetsurf->clip.x )
	{
		diff = targetsurf->clip.x- *InitDX;
		*InitSX += diff * dxSx;
		*InitSY += diff * dxSy;
		*InitDX = targetsurf->clip.x;
	}

	if ( *InitDY < targetsurf->clip.y)
	{
		diff = targetsurf->clip.y- *InitDY;
		*InitSX += diff * dySx;
		*InitSY += diff * dySy;
		*InitDY = targetsurf->clip.y;
	}

	if ( *EndX > targetsurf->clip.endx )	
		*EndX = targetsurf->clip.endx;

	if ( *EndY > targetsurf->clip.endy )	
		*EndY = targetsurf->clip.endy;

	if ( *InitDX > *EndX )
		return FALSE;

	if ( *InitDY > *EndY ) 
		return FALSE;
	return TRUE;
}

BOOL ISPM ge_draw ( SURFACE *src, S32 InitDX, S32 InitDY, S32 EndX, S32 EndY,
			   S32 InitSX, S32 InitSY, S32 dxSx, S32 dxSy, S32 dySx, S32 dySy )
{
	U16 mode = src->drawmode | DRAWMODE_TEXTURE ;
	U16 mode2 = GROUP_E0 | GROUP_E1 | GROUP_E2 | GROUP_E6;
	static BOOL b_ge_single_cmd = FALSE;
	if (b_ge_single_cmd)
	{
		ge_wait_cmd_complete();
		b_ge_single_cmd = FALSE;
	}
	
	if ( src->pixtype == PIX_FMT_ARGB )
	{
		mode |= DRAWMODE_ALPHA;
		src->sblendmode = BLEND_SRCALPHA;
		src->dblendmode = 0x8 | BLEND_SRCALPHA;
	}
	if(ge_check_clipwindow(&InitDX,&InitDY,&EndX,&EndY,&InitSX,&InitSY,dxSx,dxSy,dySx,dySy)==FALSE)
		return FALSE;
	CRITICAL_ENTER();
	GE_WriteCmd ( 2, ( U16 ) InitDX );
	GE_WriteCmd ( 3, ( U16 ) InitDY );
	GE_WriteCmd ( 4, ( U16 ) EndX );
	GE_WriteCmd ( 5, ( U16 ) EndY );
	GE_WriteCmd ( 6, ( U16 ) ( ( U16 ) InitSX & 0xffff ) );
	GE_WriteCmd ( 7, ( U16 ) ( ( U16 ) ( InitSX >> 16 ) & 0xffff ) );
	GE_WriteCmd ( 8, ( U16 ) ( ( U16 ) InitSY & 0xffff ) );
	GE_WriteCmd ( 9, ( U16 ) ( ( U16 ) ( InitSY >> 16 ) & 0xffff ) );
	GE_WriteCmd ( 10, ( U16 ) ( ( U16 ) dxSx & 0xffff ) );
	GE_WriteCmd ( 11, ( U16 ) ( ( U16 ) ( dxSx >> 16 ) & 0xffff ) );
	GE_WriteCmd ( 12, ( U16 ) ( ( U16 ) dxSy & 0xffff ) );
	GE_WriteCmd ( 13, ( U16 ) ( ( U16 ) ( dxSy >> 16 ) & 0xffff ) );
	GE_WriteCmd ( 14, ( U16 ) ( ( U16 ) dySx & 0xffff ) );
	GE_WriteCmd ( 15, ( U16 ) ( ( U16 ) ( dySx >> 16 ) & 0xffff ) );
	GE_WriteCmd ( 16, ( U16 ) ( ( U16 ) dySy & 0xffff ) );
	GE_WriteCmd ( 17, ( U16 ) ( ( U16 ) ( dySy >> 16 ) & 0xffff ) );

	if ( ( mode & DRAWMODE_ALPHA ) == DRAWMODE_ALPHA )
	{
		mode2 |= GROUP_E3;
		GE_WriteCmd ( 18, src->alphaConstColor.g << 8 | src->alphaConstColor.b );
		GE_WriteCmd ( 19, src->alphaConstColor.a << 8 | src->alphaConstColor.r );
		GE_WriteCmd ( 20, src->dblendmode << 8 | src->sblendmode );
	}

	U32 ImageOffset  = ( U32 ) ( src->pixels ) - g_TMEM_StartAddr;
	GE_WriteCmd ( 28, ( U16 ) ( ImageOffset >> 6 )	 );
	GE_WriteCmd ( 29, ( U16 ) ( ImageOffset >> 22 ) );

	if ( src->bpp == 4 || src->bpp == 8 )
	{
		U32 paloffset = ( U32 ) ( src->pal->colors ) - g_TMEM_StartAddr;
		mode |= DRAWMODE_PALETTEINDEX;//palette update
		GE_WriteCmd ( 30, ( U32 ) ( paloffset ) >> 12 );
	}

	if ( ( src->drawmode & DRAWMODE_SHADE ) == DRAWMODE_SHADE )
	{
		mode2 |= GROUP_E4;
		GE_WriteCmd ( 21, 0 );

		if ( src->bpp == 16 )
		{
			GE_WriteCmd ( 22, ( U16 ) ( ( ( src->ShadeColor.g & 0xfc ) << 8 ) | ( src->ShadeColor.b & 0xf8 ) ) );
			GE_WriteCmd ( 23, ( U16 ) ( ( ( src->ShadeColor.a & 0xff ) << 8 ) | ( src->ShadeColor.r & 0xf8 ) ) );
		}
		else
		{
			GE_WriteCmd ( 22, ( U16 ) ( ( src->ShadeColor.g  << 8 ) | ( src->ShadeColor.b & 0xff ) ) );
			GE_WriteCmd ( 23, ( U16 ) ( ( ( src->ShadeColor.a & 0xff ) << 8 ) | ( src->ShadeColor.r & 0xf8 ) ) );
		}
	}

	if ( ( src->drawmode & DRAWMODE_TRANSPARENCY ) == DRAWMODE_TRANSPARENCY )
	{
		mode2 |= GROUP_E5;

		if ( src->bpp == 4 || src->bpp == 8 )
		{
			GE_WriteCmd ( 24, ( U16 ) ( ( ( src->transColor.g & 0xff ) << 8 ) | ( src->transColor.b & 0xff ) ) );
			GE_WriteCmd ( 25, ( U16 ) ( src->transColor.r & 0xff ) );
		}
		else if ( src->bpp == 16 )
		{
			GE_WriteCmd ( 24, ( U16 ) ( ( ( src->transColor.g & 0xfc ) << 8 ) | ( src->transColor.b & 0xf8 ) ) );
			GE_WriteCmd ( 25, ( U16 ) ( src->transColor.r & 0xf8 ) );
		}
		else
		{
			GE_WriteCmd ( 24, ( U16 ) ( ( ( src->transColor.g & 0xff ) << 8 ) | ( src->transColor.b & 0xff ) ) );
			GE_WriteCmd ( 25, ( U16 ) ( src->transColor.r & 0xff ) );
		}
	}

	U16 pd31 ;
	pd31 = ( U16 ) ( ( src->TileXSize & 0x0f ) ) | ( U16 ) ( ( src->TileYSize & 0x0f ) << 4 ) ;

	switch ( src->bpp )
	{
	case 4:
		pd31 |= ( 0 ) << 8 ;
		pd31 |= ( U16 ) ( ( src->pal->LUT4Bank & 0x0f ) << 12 ) ;
		break;

	case 8:
		pd31 |= ( 1 ) << 8 ;
		break;

	case 16:
		pd31 |= ( 2 ) << 8 ;
		break;
	case 24:
	case 32:
		pd31 |= ( 4 ) << 8 ;
		break;
	}

	if ( get_screen_bpp() == 32 )
		mode |= ( 1 ) << 14;

	GE_WriteCmd ( 31, pd31 );

	GE_WriteCmd ( 32, src->w);//real width
	
	if(src->bpp==4)
		GE_WriteCmd ( 33, src->pitch*2);
	else if(src->bpp==8)
		GE_WriteCmd ( 33, src->pitch);
	else if(src->bpp==16)
		GE_WriteCmd ( 33, src->pitch/2);
	else if(src->bpp==24)
		GE_WriteCmd ( 33, src->pitch/4);
	else if(src->bpp==32)
		GE_WriteCmd ( 33, src->pitch/4);
	GE_WriteCmd ( 34, src->h);

	GE_WriteCmd ( 0, mode );
	GE_WriteCmd ( 1, mode2 );
	ge_end_cmd();
	CRITICAL_EXIT();
	//check alpha or transparency
	U16 pd0 = mode;
	if ((pd0 & (1 << 9)) || (pd0 & (1 << 7)))
	{
		b_ge_single_cmd = TRUE;
	}
	return TRUE;
}

BOOL draw_surface_rect ( SURFACE *surf, int dx, int dy, int sx, int sy, int w, int h )
{
	return draw_surface_scalerect ( surf, dx, dy, w, h, sx, sy, w, h );
}

BOOL draw_surface ( SURFACE *src_surf, int dx, int dy )
{
	if(src_surf->bpp<=8)
	{
		if(src_surf->pal->colorkey!=-1)
		{
			src_surf->drawmode |= DRAWMODE_TRANSPARENCY;
			src_surf->transColor = *(sARGB*)&src_surf->pal->colors[src_surf->pal->colorkey];
		}
		else
		{
			src_surf->drawmode &= ~DRAWMODE_TRANSPARENCY;
		}
	}
	return draw_surface_scalerect(src_surf, dx, dy, src_surf->w, src_surf->h, 0, 0, src_surf->w, src_surf->h);
}

BOOL surface_set_alpha ( SURFACE *surf, U8 a )
{
	a = 255 - a;
	//pixel ������ �ƴ�
	surf->drawmode |= DRAWMODE_ALPHA;
	surf->alphaConstColor.a = a;
	surf->alphaConstColor.r = a;
	surf->alphaConstColor.g = a;
	surf->alphaConstColor.b = a;
	surf->sblendmode = BLEND_INVERSION | BLEND_CONSTANT;
	surf->dblendmode = BLEND_CONSTANT;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Draw surface with scale. </summary>
///
/// <remarks>	Kkh, 2013-01-18. </remarks>
///
/// <param name="surf">	The source Surface. </param>
/// <param name="dx">  	The x-coordiate, in logical units, of the upper-left of destination rectangle. </param>
/// <param name="dy">  	The y-coordiate, in logical units, of the upper-left of destination rectangle. </param>
/// <param name="dw">  	The width, in logical units, of the the destination rectangle. </param>
/// <param name="dh">  	The height, in logical units, of the the destination rectangle. </param>
/// <param name="sx">  	The x-coordiate, in logical units, of the upper-left of source rectangle. </param>
/// <param name="sy">  	The y-coordiate, in logical units, of the upper-left of source rectangle. </param>
/// <param name="sw">  	The width, in logical units, of the the source rectangle. </param>
/// <param name="sh">  	The height, in logical units, of the the source rectangle. </param>
///
/// <returns>	true if it succeeds, false if it fails. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL draw_surface_scalerect ( SURFACE *surf, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh )
{
	S32 InitDX, InitDY, EndX, EndY;
	S32 InitSX, InitSY;
	S32 dxSx, dxSy, dySx, dySy;
	InitDX = dx;
	InitDY = dy;
	EndX = dx + dw - 1;
	EndY = dy + dh - 1;
	InitSX = sx;
	InitSY = sy;
	if(surf->pixtype == PIX_FMT_A000)//not H/W
	{
		return sw_draw_surface_scalerect(surf,dx,dy,dw,dh,sx,sy,sw,sh);
	}

	if ( surf->flipmode & PIVOT_HFLIP)
	{
		InitSX	= ( sx + sw - 1 ) << 9 ;
		dxSx	= - ( sw << 9 ) / dw;
		dxSy	= 0;
	}
	else
	{
		InitSX	= ( sx << 9 );
		dxSx	= ( sw << 9 ) / dw;
		dxSy	= 0;
	}

	if ( surf->flipmode & PIVOT_VFLIP )
	{
		InitSY	= ( sy + sh - 1 ) << 9;
		dySx	= 0;
		dySy	= - ( sh << 9 ) / dh;
	}
	else
	{
		InitSY	= ( sy << 9 );
		dySx	= 0;
		dySy	= ( sh << 9 ) / dh;
	}

	return ge_draw ( surf, InitDX, InitDY, EndX, EndY, InitSX, InitSY, dxSx, dxSy, dySx, dySy );
}

BOOL draw_surface_scale ( SURFACE *surf, int dx, int dy, int dw, int dh )
{
	S32 InitDX, InitDY, EndX, EndY;
	S32 InitSX, InitSY;
	S32 dxSx, dxSy, dySx, dySy;
	
	InitDX = dx;
	InitDY = dy;
	EndX = dx + dw - 1;
	EndY = dy + dh - 1;

	if(surf->pixtype == PIX_FMT_A000)//not H/W
	{		
		return sw_draw_surface_scale(surf,dx,dy,dw,dh);
	}

	if ( surf->flipmode & PIVOT_HFLIP )
	{
		InitSX	= ( surf->w - 1 ) << 9 ;
		dxSx	= - ( surf->w << 9 ) / dw;
		dxSy	= 0;
	}
	else
	{
		InitSX	= 0;
		dxSx	= ( surf->w << 9 ) / dw;
		dxSy	= 0;
	}

	if ( surf->flipmode & PIVOT_VFLIP )
	{
		InitSY	= ( surf->h - 1 ) << 9;
		dySx	= 0;
		dySy	= - ( surf->h << 9 ) / dh;
	}
	else
	{
		InitSY	= 0;
		dySx	= 0;
		dySy	= ( surf->h << 9 ) / dh;
	}

	return ge_draw ( surf, InitDX, InitDY, EndX, EndY, InitSX, InitSY, dxSx, dxSy, dySx, dySy );
}

BOOL set_texture(SURFACE* src)
{
	U16 mode = src->drawmode | DRAWMODE_TEXTURE ;
	U16 mode2 = GROUP_E6;
	
	if ( src->pixtype == PIX_FMT_ARGB )
	{
		mode |= DRAWMODE_ALPHA;
		src->sblendmode = BLEND_SRCALPHA;
		src->dblendmode = 0x8 | BLEND_SRCALPHA;
	}

	CRITICAL_ENTER();

	if ( ( mode & DRAWMODE_ALPHA ) == DRAWMODE_ALPHA )
	{
		mode2 |= GROUP_E3;
		GE_WriteCmd ( 18, src->alphaConstColor.g << 8 | src->alphaConstColor.b );
		GE_WriteCmd ( 19, src->alphaConstColor.a << 8 | src->alphaConstColor.r );
		GE_WriteCmd ( 20, src->dblendmode << 8 | src->sblendmode );
	}

	U32 ImageOffset  = ( U32 ) ( src->pixels ) - g_TMEM_StartAddr;
	GE_WriteCmd ( 28, ( U16 ) ( ImageOffset >> 6 )	 );
	GE_WriteCmd ( 29, ( U16 ) ( ImageOffset >> 22 ) );

	if ( src->bpp == 4 || src->bpp == 8 )
	{
		U32 paloffset = ( U32 ) ( src->pal->colors ) - g_TMEM_StartAddr;
		mode |= DRAWMODE_PALETTEINDEX;//palette update
		GE_WriteCmd ( 30, ( U32 ) ( paloffset ) >> 12 );
	}

	if ( ( src->drawmode & DRAWMODE_SHADE ) == DRAWMODE_SHADE )
	{
		mode2 |= GROUP_E4;
		GE_WriteCmd ( 21, 0 );

		if ( src->bpp == 16 )
		{
			GE_WriteCmd ( 22, ( U16 ) ( ( ( src->ShadeColor.g & 0xfc ) << 8 ) | ( src->ShadeColor.b & 0xf8 ) ) );
			GE_WriteCmd ( 23, ( U16 ) ( ( ( src->ShadeColor.a & 0xff ) << 8 ) | ( src->ShadeColor.r & 0xf8 ) ) );
		}
		else
		{
			GE_WriteCmd ( 22, ( U16 ) ( ( src->ShadeColor.g  << 8 ) | ( src->ShadeColor.b & 0xff ) ) );
			GE_WriteCmd ( 23, ( U16 ) ( ( ( src->ShadeColor.a & 0xff ) << 8 ) | ( src->ShadeColor.r & 0xf8 ) ) );
		}
	}

	if ( ( src->drawmode & DRAWMODE_TRANSPARENCY ) == DRAWMODE_TRANSPARENCY )
	{
		mode2 |= GROUP_E5;

		if ( src->bpp == 4 || src->bpp == 8 )
		{
			GE_WriteCmd ( 24, ( U16 ) ( ( ( src->transColor.g & 0xff ) << 8 ) | ( src->transColor.b & 0xff ) ) );
			GE_WriteCmd ( 25, ( U16 ) ( src->transColor.r & 0xff ) );
		}
		else if ( src->bpp == 16 )
		{
			GE_WriteCmd ( 24, ( U16 ) ( ( ( src->transColor.g & 0xfc ) << 8 ) | ( src->transColor.b & 0xf8 ) ) );
			GE_WriteCmd ( 25, ( U16 ) ( src->transColor.r & 0xf8 ) );
		}
		else
		{
			GE_WriteCmd ( 24, ( U16 ) ( ( ( src->transColor.g & 0xff ) << 8 ) | ( src->transColor.b & 0xff ) ) );
			GE_WriteCmd ( 25, ( U16 ) ( src->transColor.r & 0xff ) );
		}
	}

	U16 pd31 ;
	pd31 = ( U16 ) ( ( src->TileXSize & 0x0f ) ) | ( U16 ) ( ( src->TileYSize & 0x0f ) << 4 ) ;

	switch ( src->bpp )
	{
	case 4:
		pd31 |= ( 0 ) << 8 ;
		pd31 |= ( U16 ) ( ( src->pal->LUT4Bank & 0x0f ) << 12 ) ;
		break;

	case 8:
		pd31 |= ( 1 ) << 8 ;
		break;

	case 16:
		pd31 |= ( 2 ) << 8 ;
		break;

	case 32:
		pd31 |= ( 4 ) << 8 ;
		break;
	}

	if ( get_screen_bpp() == 32 )
		mode |= ( 1 ) << 14;

	GE_WriteCmd ( 31, pd31 );

	GE_WriteCmd ( 32, src->w);//real width

	if(src->bpp==4)
		GE_WriteCmd ( 33, src->pitch*2);
	else if(src->bpp==8)
		GE_WriteCmd ( 33, src->pitch);
	else if(src->bpp==16)
		GE_WriteCmd ( 33, src->pitch/2);
	else if(src->bpp==24)
		GE_WriteCmd ( 33, src->pitch/4);
	else if(src->bpp==32)
		GE_WriteCmd ( 33, src->pitch/4);
	GE_WriteCmd ( 34, src->h);

	GE_WriteCmd ( 0, mode );
	GE_WriteCmd ( 1, mode2 );
	ge_end_cmd();
	CRITICAL_EXIT();
	return TRUE;
}

BOOL draw_texture_rect( SURFACE *surf, int dx, int dy, int sx, int sy, int w, int h )
{
	return draw_texture_scalerect(surf,dx,dy,w,h,sx,sy,w,h);
}

BOOL draw_texture(SURFACE* src, int x, int y)
{
	draw_texture_rect(src,x,y,0,0,src->w,src->h);

	return TRUE;
}

BOOL draw_texture_scalerect(SURFACE* surf,int dx, int dy, int dw, int dh,int sx, int sy ,int sw, int sh)
{
	S32 InitDX, InitDY, EndX, EndY;
	S32 InitSX, InitSY;
	S32 dxSx, dxSy, dySx, dySy;
	InitDX = dx;
	InitDY = dy;
	EndX = dx + dw - 1;
	EndY = dy + dh - 1;
	InitSX = sx;
	InitSY = sy;
	if(surf->pixtype == PIX_FMT_A000)//not H/W
	{
		return sw_draw_surface_scalerect(surf,dx,dy,dw,dh,sx,sy,sw,sh);
	}

	if ( surf->flipmode & PIVOT_HFLIP)
	{
		InitSX	= ( sx + sw - 1 ) << 9 ;
		dxSx	= - ( sw << 9 ) / dw;
		dxSy	= 0;
	}
	else
	{
		InitSX	= ( sx << 9 );
		dxSx	= ( sw << 9 ) / dw;
		dxSy	= 0;
	}

	if ( surf->flipmode & PIVOT_VFLIP )
	{
		InitSY	= ( sy + sh - 1 ) << 9;
		dySx	= 0;
		dySy	= - ( sh << 9 ) / dh;
	}
	else
	{
		InitSY	= ( sy << 9 );
		dySx	= 0;
		dySy	= ( sh << 9 ) / dh;
	}
	U16 mode = surf->drawmode | DRAWMODE_TEXTURE ;
	U16 mode2 = GROUP_E0 | GROUP_E1 | GROUP_E2 ;
	if(ge_check_clipwindow(&InitDX,&InitDY,&EndX,&EndY,&InitSX,&InitSY,dxSx,dxSy,dySx,dySy)==FALSE)
		return FALSE;
	CRITICAL_ENTER();
	GE_WriteCmd ( 2, ( U16 ) InitDX );
	GE_WriteCmd ( 3, ( U16 ) InitDY );
	GE_WriteCmd ( 4, ( U16 ) EndX );
	GE_WriteCmd ( 5, ( U16 ) EndY );
	GE_WriteCmd ( 6, ( U16 ) ( ( U16 ) InitSX & 0xffff ) );
	GE_WriteCmd ( 7, ( U16 ) ( ( U16 ) ( InitSX >> 16 ) & 0xffff ) );
	GE_WriteCmd ( 8, ( U16 ) ( ( U16 ) InitSY & 0xffff ) );
	GE_WriteCmd ( 9, ( U16 ) ( ( U16 ) ( InitSY >> 16 ) & 0xffff ) );
	GE_WriteCmd ( 10, ( U16 ) ( ( U16 ) dxSx & 0xffff ) );
	GE_WriteCmd ( 11, ( U16 ) ( ( U16 ) ( dxSx >> 16 ) & 0xffff ) );
	GE_WriteCmd ( 12, ( U16 ) ( ( U16 ) dxSy & 0xffff ) );
	GE_WriteCmd ( 13, ( U16 ) ( ( U16 ) ( dxSy >> 16 ) & 0xffff ) );
	GE_WriteCmd ( 14, ( U16 ) ( ( U16 ) dySx & 0xffff ) );
	GE_WriteCmd ( 15, ( U16 ) ( ( U16 ) ( dySx >> 16 ) & 0xffff ) );
	GE_WriteCmd ( 16, ( U16 ) ( ( U16 ) dySy & 0xffff ) );
	GE_WriteCmd ( 17, ( U16 ) ( ( U16 ) ( dySy >> 16 ) & 0xffff ) );

	GE_WriteCmd ( 0, mode );
	GE_WriteCmd ( 1, mode2 );
	ge_end_cmd();
	CRITICAL_EXIT();
	return TRUE;
}

