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
#include "egl_typedef.h"
#include "egl_base.h"
#include "egl_private.h"

#ifdef HW_2D_ENGINE
void ge_wait_cmd_complete();
#endif

#include <stdlib.h>
#include <string.h>

#ifndef PIVOT_RIGHT
#define PIVOT_RIGHT (1)
#endif
#ifndef PIVOT_LEFT
#define PIVOT_LEFT (1<<1)
#endif

//#define DEFAULT_FONT_COLOR MAKE_COLORREF( 68, 70, 82 )
#define DEFAULT_FONT_COLOR MAKE_COLORREF( 0, 0, 0)
//#define FONT_24

#ifdef FONT_24
#include "ascii_24.h"
#include "kor_24.h"
static const U8 *engfont = EngFont16x24;
static const U8 *hanfont = HanFont24x24;
#define cur_fontdata->eng_font_w 16
#define ENG_FONT_H 24
#define HAN_FONT_W 24
#define HAN_FONT_W 24


#else
#include "ascii_16.h"
#include "kor_16.h"
static const U8 *engfont = EngFont8x16;
static const U8 *hanfont = HanFont16x16;
#define ENG_FONT_W 8
#define ENG_FONT_H 16
#define HAN_FONT_W 16
#define HAN_FONT_H 16
// static U8 cur_fontdata->eng_font_w=8;
// static U8 eng_font_h=16;
// static U8 han_font_w=16;
// static U8 han_font_h=16;


#endif
#include "kor_font_index.h"

static SURFACE* engchar_surf = 0;
static SURFACE* engchar_surf_vert_right = 0;
static SURFACE* hanchar_surf = 0;
static SURFACE* hanchar_surf_vert = 0;

static int bit_pivot_mode = 0;//1 is left pivot, 2 is right pivot

typedef struct _tagbitfontdata
{
	U8 eng_font_w;
	U8 eng_font_h;
	U8 han_font_w;
	U8 han_font_h;
	U8 user_eng_font_w;
	U8 user_eng_font_h;
	U8 user_han_font_w;
	U8 user_han_font_h;
	EGL_COLOR fontclr;
	EGL_COLOR bkclr;
	U8 bkmode;//0:trasnparent 1: opaque
} BITFONTDATA;

static BITFONTDATA* cur_fontdata = NULL;

static void make_char_eng( int index )
{
	int i;
	int k;
	index -= 0x20;
	if( bit_pivot_mode == PIVOT_LEFT )
	{
		memset( engchar_surf_vert_right->pixels, 0, engchar_surf_vert_right->pitch * engchar_surf_vert_right->h );
		for( i = 0; i < engchar_surf_vert_right->w; i++ )
		{
			U8 *dest = ( U8 * )engchar_surf_vert_right->pixels + ( engchar_surf_vert_right->pitch * ( engchar_surf_vert_right->h - 1 ) ) + i;
			U8 *src = ( U8* )engfont + ( index * ( cur_fontdata->eng_font_w / 8 * cur_fontdata->eng_font_h ) ) + ( i * ( cur_fontdata->eng_font_w / 8 ) );
			for( k = 0; k < engchar_surf_vert_right->h; k++ )
			{
				if( src[k / 8] & ( 1 << ( 7 - ( k % 8 ) ) ) )
				{
					*dest = 1;
				}
				dest -= engchar_surf_vert_right->pitch;
			}
		}
	}
	else if( bit_pivot_mode == PIVOT_RIGHT )
	{
		memset( engchar_surf_vert_right->pixels, 0, engchar_surf_vert_right->pitch * engchar_surf_vert_right->h );
		for( i = 0; i < engchar_surf_vert_right->w; i++ )
		{
			U8 *dest = ( U8 * )engchar_surf_vert_right->pixels + engchar_surf_vert_right->pitch - 1 - i;
			U8 *src = ( U8* )engfont + ( index * ( cur_fontdata->eng_font_w / 8 * cur_fontdata->eng_font_h ) ) + ( i * ( cur_fontdata->eng_font_w / 8 ) );
			for( k = 0; k < engchar_surf_vert_right->h; k++ )
			{
				if( src[k / 8] & ( 1 << ( 7 - ( k % 8 ) ) ) )
				{
					*dest = 1;
				}
				dest += engchar_surf_vert_right->pitch;
			}
		}
	}
	else
	{
		memset( engchar_surf->pixels, 0, engchar_surf->pitch * engchar_surf->h );
		for( i = 0; i < engchar_surf->h; i++ )
		{
			U8 *dest = ( U8 * )engchar_surf->pixels + engchar_surf->pitch * i;
			U8 *src = ( U8* )engfont + ( index * ( cur_fontdata->eng_font_w / 8 * cur_fontdata->eng_font_h ) ) + ( i * ( cur_fontdata->eng_font_w / 8 ) );
			for( k = 0; k < engchar_surf->w; k++ )
			{
				if( src[k / 8] & ( 1 << ( 7 - ( k % 8 ) ) ) )
				{
					*dest = 1;
				}
				dest++;
			}
		}
	}
}

static int _draw_char_eng( int x, int y, int index )
{
	make_char_eng( index );
	if( ( cur_fontdata->user_eng_font_w != cur_fontdata->eng_font_w ) || ( cur_fontdata->user_eng_font_h != cur_fontdata->eng_font_h ) )
	{
		if( bit_pivot_mode == 0 )
		{
			DRAW_SURFACE_SCALE_RECT( engchar_surf, x, y, cur_fontdata->user_eng_font_w, cur_fontdata->user_eng_font_h, 0, 0, engchar_surf->w, engchar_surf->h );
		}
		else
		{
			DRAW_SURFACE_SCALE_RECT( engchar_surf_vert_right, x, y, cur_fontdata->user_eng_font_h, cur_fontdata->user_eng_font_w, 0, 0, engchar_surf_vert_right->w, engchar_surf_vert_right->h );
		}
	}
	else
	{
		if( bit_pivot_mode == 0 )
		{
			DRAW_SURFACE( engchar_surf, x, y );
		}
		else
		{
			DRAW_SURFACE( engchar_surf_vert_right, x, y );
		}
	}
	return cur_fontdata->user_eng_font_w;
}

static void make_char_han( int index )
{
	int cho, jung, jong;
	int i, k;
	cho = HanIndexTbl[index * 3];
	jung = HanIndexTbl[index * 3 + 1] + 161;
	jong = HanIndexTbl[index * 3 + 2];
	if( jong )
	{
		jong += 248;
	}
	if( bit_pivot_mode == 1 )
	{
		//clear
		memset( hanchar_surf_vert->pixels, 0, hanchar_surf_vert->pitch * hanchar_surf_vert->h );
		for( i = 0; i < hanchar_surf_vert->w; i++ )
		{
			U8 *src ;
			if( cho )
			{
				U8 *dest = ( U8 * )hanchar_surf_vert->pixels + ( hanchar_surf_vert->pitch * ( hanchar_surf_vert->h - 1 ) ) + i;
				src = ( U8* )hanfont + ( cho * cur_fontdata->han_font_w / 8 * cur_fontdata->han_font_h ) + ( i * ( cur_fontdata->han_font_w / 8 ) );
				for( k = 0; k < hanchar_surf_vert->h; k++ )
				{
					if( src[k / 8] & ( 1 << ( 7 - ( k % 8 ) ) ) )
					{
						*dest = 1;
					}
					dest -= engchar_surf_vert_right->pitch;
				}
			}
			if( jung )
			{
				U8 *dest = ( U8 * )hanchar_surf_vert->pixels + ( hanchar_surf_vert->pitch * ( hanchar_surf_vert->h - 1 ) ) + i;
				src = ( U8* )hanfont + ( jung * cur_fontdata->han_font_w / 8 * cur_fontdata->han_font_h ) + ( i * ( cur_fontdata->han_font_w / 8 ) );
				for( k = 0; k < hanchar_surf_vert->h; k++ )
				{
					if( src[k / 8] & ( 1 << ( 7 - ( k % 8 ) ) ) )
					{
						*dest = 1;
					}
					dest -= engchar_surf_vert_right->pitch;
				}
			}
			if( jong )
			{
				U8 *dest = ( U8 * )hanchar_surf_vert->pixels + ( hanchar_surf_vert->pitch * ( hanchar_surf_vert->h - 1 ) ) + i;
				src = ( U8* )hanfont + ( jong * cur_fontdata->han_font_w / 8 * cur_fontdata->han_font_h ) + ( i * ( cur_fontdata->han_font_w / 8 ) );
				for( k = 0; k < hanchar_surf_vert->h; k++ )
				{
					if( src[k / 8] & ( 1 << ( 7 - ( k % 8 ) ) ) )
					{
						*dest = 1;
					}
					dest -= engchar_surf_vert_right->pitch;
				}
			}
		}
	}
	else if( bit_pivot_mode == 2 )
	{
		//clear
		memset( hanchar_surf_vert->pixels, 0, hanchar_surf_vert->pitch * hanchar_surf_vert->h );
		for( i = 0; i < hanchar_surf_vert->w; i++ )
		{
			U8 *src ;
			if( cho )
			{
				U8 *dest = ( U8 * )hanchar_surf_vert->pixels + hanchar_surf_vert->pitch - 1 - i;
				src = ( U8* )hanfont + ( cho * cur_fontdata->han_font_w / 8 * cur_fontdata->han_font_h ) + ( i * ( cur_fontdata->han_font_w / 8 ) );
				for( k = 0; k < hanchar_surf_vert->h; k++ )
				{
					if( src[k / 8] & ( 1 << ( 7 - ( k % 8 ) ) ) )
					{
						*dest = 1;
					}
					dest += engchar_surf_vert_right->pitch;
				}
			}
			if( jung )
			{
				U8 *dest = ( U8 * )hanchar_surf_vert->pixels + hanchar_surf_vert->pitch - 1 - i;
				src = ( U8* )hanfont + ( jung * cur_fontdata->han_font_w / 8 * cur_fontdata->han_font_h ) + ( i * ( cur_fontdata->han_font_w / 8 ) );
				for( k = 0; k < hanchar_surf_vert->h; k++ )
				{
					if( src[k / 8] & ( 1 << ( 7 - ( k % 8 ) ) ) )
					{
						*dest = 1;
					}
					dest += engchar_surf_vert_right->pitch;
				}
			}
			if( jong )
			{
				U8 *dest = ( U8 * )hanchar_surf_vert->pixels + hanchar_surf_vert->pitch - 1 - i;
				src = ( U8* )hanfont + ( jong * cur_fontdata->han_font_w / 8 * cur_fontdata->han_font_h ) + ( i * ( cur_fontdata->han_font_w / 8 ) );
				for( k = 0; k < hanchar_surf_vert->h; k++ )
				{
					if( src[k / 8] & ( 1 << ( 7 - ( k % 8 ) ) ) )
					{
						*dest = 1;
					}
					dest += engchar_surf_vert_right->pitch;
				}
			}
		}
	}
	else
	{
		//clear
		memset( hanchar_surf->pixels, 0, hanchar_surf->pitch * hanchar_surf->h );
		for( i = 0; i < hanchar_surf->h; i++ )
		{
			U8 *dest = ( U8 * )hanchar_surf->pixels + hanchar_surf->pitch * i;
			U8 *src ;
			if( cho )
			{
				src = ( U8* )hanfont + ( cho * cur_fontdata->han_font_w / 8 * cur_fontdata->han_font_h ) + ( i * ( cur_fontdata->han_font_w / 8 ) );
				for( k = 0; k < hanchar_surf->w; k++ )
				{
					if( src[k / 8] & ( 1 << ( 7 - ( k % 8 ) ) ) )
					{
						dest[k] = 1;
					}
				}
			}
			if( jung )
			{
				src = ( U8* )hanfont + ( jung * cur_fontdata->han_font_w / 8 * cur_fontdata->han_font_h ) + ( i * ( cur_fontdata->han_font_w / 8 ) );
				for( k = 0; k < hanchar_surf->w; k++ )
				{
					if( src[k / 8] & ( 1 << ( 7 - ( k % 8 ) ) ) )
					{
						dest[k] = 1;
					}
				}
			}
			if( jong )
			{
				src = ( U8* )hanfont + ( jong * cur_fontdata->han_font_w / 8 * cur_fontdata->han_font_h ) + ( i * ( cur_fontdata->han_font_w / 8 ) );
				for( k = 0; k < hanchar_surf->w; k++ )
				{
					if( src[k / 8] & ( 1 << ( 7 - ( k % 8 ) ) ) )
					{
						dest[k] = 1;
					}
				}
			}
		}
	}
}

static int _draw_char_han( int x, int y, int index )
{
	make_char_han( index );
	if( ( cur_fontdata->user_han_font_w != cur_fontdata->han_font_w ) || ( cur_fontdata->user_han_font_h != cur_fontdata->han_font_h ) )
	{
		if( bit_pivot_mode == 0 )
		{
			DRAW_SURFACE_SCALE_RECT( hanchar_surf, x, y, cur_fontdata->user_han_font_w, cur_fontdata->user_han_font_h, 0, 0, hanchar_surf->w, hanchar_surf->h );
		}
		else
		{
			DRAW_SURFACE_SCALE_RECT( hanchar_surf_vert, x, y, cur_fontdata->user_han_font_w, cur_fontdata->user_han_font_h, 0, 0, hanchar_surf_vert->w, hanchar_surf_vert->h );
		}
	}
	else
	{
		if( bit_pivot_mode == 0 )
		{
			DRAW_SURFACE( hanchar_surf, x, y );
		}
		else
		{
			DRAW_SURFACE( hanchar_surf_vert, x, y );
		}
	}
	return cur_fontdata->user_han_font_w ;
}


#define GET_HAN_INDEX(l, h)    (((h)-0xb0)*(0xff-0xa1)+((l)-0xa1))
static int bf_drawstring( int x, int y, char* str )
{
	int i = 0;
	U8 low, high;
	int len = 0;
	int w;
	int index;
	while( str[i] )
	{
		low = str[i];
		i++;
		if( low < 0x80 ) //eng
		{
			w = _draw_char_eng( x, y, low );
			x += w;
			len += w;
		}
		else
		{
			high = str[i];
			i++;
			index = GET_HAN_INDEX( high, low );
			if( index < 2350 ) //not support hangul
			{
				w = _draw_char_han( x, y, index );
				x += w;
				len += w;
			}
			else
			{
				w = cur_fontdata->user_han_font_w;
				x += w;
				len += w;
			}
		}
#ifdef HW_2D_ENGINE
		ge_wait_cmd_complete();
#endif

	}
	return i;
}

int bf_drawstring_vleft( int x, int y, char* str )
{
	int index;
	int i = 0;
	U8 low, high;
	int len = 0;
	int w;
	bit_pivot_mode = 1;
	while( str[i] )
	{
		low = str[i];
		i++;
		if( low < 0x80 ) //eng
		{
			w = _draw_char_eng( x, y - cur_fontdata->user_eng_font_w, low );
			y -= w;
			len += w;
		}
		else
		{
			high = str[i];
			i++;
			index = GET_HAN_INDEX( high, low );
			if( index < 2350 ) //not support hangul
			{
				w = _draw_char_han( x, y - cur_fontdata->user_han_font_w, index );
				y -= w;
				len += w;
			}
			else
			{
				w = cur_fontdata->user_han_font_w;
				y -= w;
				len += w;
			}
		}
	}
	bit_pivot_mode = 0;
	return len;
}
int bf_drawstring_vright( int x, int y, char* str )
{
	int i = 0;
	U8 low, high;
	int len = 0;
	int w;
	int index;
	bit_pivot_mode = 2;
	while( str[i] )
	{
		low = str[i];
		i++;
		if( low < 0x80 ) //eng
		{
			w = _draw_char_eng( x, y, low );
			y += w;
			len += w;
		}
		else
		{
			high = str[i];
			i++;
			index = GET_HAN_INDEX( high, low );
			if( index < 2350 ) //not support hangul
			{
				w = _draw_char_han( x, y, index );
				y += w;
				len += w;
			}
			else
			{
				w = cur_fontdata->user_han_font_w;
				y += w;
				len += w;
			}
		}
	}
	bit_pivot_mode = 0;
	return len;
}
int bf_gettextwidth( char* str )
{
	U8 low;
	int x = 0;
	int i = 0;
	while( str[i] )
	{
		low = str[i];
		i++;
		if( low < 0x80 ) //eng
		{
			x += cur_fontdata->user_eng_font_w;
		}
		else
		{
			i++;
			x += cur_fontdata->user_han_font_w;
		}
	}
	return x;
}
SURFACE* bf_makesurface( char* str )
{
	int i = 0;
	int x = 0;
	int index;
	U8 low, high;
	sARGB argb;
	int w ;
	SURFACE* surf ;
	if( engchar_surf == 0 )
	{
		return 0 ;
	}
	if( hanchar_surf == 0 )
	{
		return 0 ;
	}
	w = bf_gettextwidth( str );
	argb.a = 255;
	argb.r = 0;
	argb.g = 0;
	argb.b = 0;
	surf = CREATE_SURFACE( w, cur_fontdata->eng_font_h > cur_fontdata->han_font_h ? cur_fontdata->eng_font_h : cur_fontdata->han_font_h, 8 );
	if( surf == 0 )
	{
		return 0;
	}
	memset( surf->pixels, 0, surf->pitch * surf->h );
	surf->pal->colorkey = 0;
	surf->pal->colors[1] = engchar_surf->pal->colors[1];
	while( str[i] )
	{
		low = str[i];
		i++;
		if( low < 0x80 ) //eng
		{
			U8* dest;
			U8* src;
			int y;
			make_char_eng( low );
			dest = ( ( U8* )surf->pixels ) + x;
			src = ( U8* )engchar_surf->pixels;
			for( y = 0; y < cur_fontdata->eng_font_h; y++ )
			{
				memcpy( dest, src, cur_fontdata->eng_font_w );
				dest += surf->pitch;
				src += engchar_surf->pitch;
			}
			x += cur_fontdata->eng_font_w;
		}
		else
		{
			high = str[i];
			i++;
			index = GET_HAN_INDEX( high, low );
			if( index < 2350 ) //not support hangul
			{
				U8* dest;
				U8* src;
				int y;
				make_char_han( index );
				dest = ( ( U8* )surf->pixels ) + x;
				src = ( U8* )hanchar_surf->pixels;
				for( y = 0; y < cur_fontdata->han_font_h; y++ )
				{
					memcpy( dest, src, cur_fontdata->han_font_w );
					src += hanchar_surf->pitch;
					dest += surf->pitch;
				}
			}
			x += cur_fontdata->han_font_w;
		}
	}
	return surf;
}

static BOOL bf_init()
{
	EGL_COLOR fontclr;
	fontclr = DEFAULT_FONT_COLOR;
	engchar_surf = CREATE_SURFACE( ENG_FONT_W, ENG_FONT_H, 8 );
	if( engchar_surf == 0 )
	{
		return FALSE;
	}
	engchar_surf_vert_right = CREATE_SURFACE( ENG_FONT_W, ENG_FONT_H, 8 );
	if( engchar_surf_vert_right == 0 )
	{
		return FALSE;
	}
	engchar_surf->pal->colorkey = 0;
	engchar_surf->pal->colors[1] = fontclr;
	engchar_surf_vert_right->pal->colorkey = 0;
	engchar_surf_vert_right->pal->colors[1] = fontclr;
	hanchar_surf = CREATE_SURFACE( HAN_FONT_W, HAN_FONT_H, 8 );
	if( hanchar_surf == 0 )
	{
		return FALSE;
	}
	hanchar_surf_vert = CREATE_SURFACE( HAN_FONT_W, HAN_FONT_H, 8 );
	if( hanchar_surf_vert == 0 )
	{
		return FALSE;
	}
	hanchar_surf->pal->colorkey = 0;
	hanchar_surf->pal->colors[1] = fontclr;
	hanchar_surf_vert->pal->colorkey = 0;
	hanchar_surf_vert->pal->colors[1] = fontclr;
	return TRUE;
}

static int draw( EGL_FONT* font, int x, int y, const char* str )
{
	cur_fontdata = ( BITFONTDATA* )font->ext;
	if( cur_fontdata->bkmode == 0 )
	{
		engchar_surf->pal->colorkey = engchar_surf->pal->colorkey = hanchar_surf->pal->colorkey = hanchar_surf_vert->pal->colorkey = 0;
	}
	else
	{
		engchar_surf->pal->colorkey = engchar_surf->pal->colorkey = hanchar_surf->pal->colorkey = hanchar_surf_vert->pal->colorkey = -1;
	}
	engchar_surf->pal->colors[0]=engchar_surf_vert_right->pal->colors[0]=hanchar_surf->pal->colors[0]=hanchar_surf_vert->pal->colors[0]=cur_fontdata->bkclr;
	engchar_surf->pal->colors[1]=engchar_surf_vert_right->pal->colors[1]=hanchar_surf->pal->colors[1]=hanchar_surf_vert->pal->colors[1]=cur_fontdata->fontclr;
	return bf_drawstring( x, y, ( char* )str );
}
static void draw_len( EGL_FONT* font, int x, int y, const char* str, int len )
{
	char* buf = ( char* )malloc( len + 1 );
	if(buf==NULL)
		return ;
	strncpy( buf, str, len );
	buf[len] = 0;
	cur_fontdata = ( BITFONTDATA* )font->ext;
	if( cur_fontdata->bkmode == 0 )
	{
		engchar_surf->pal->colorkey = engchar_surf->pal->colorkey = hanchar_surf->pal->colorkey = hanchar_surf_vert->pal->colorkey = 0;
	}
	else
	{
		engchar_surf->pal->colorkey = engchar_surf->pal->colorkey = hanchar_surf->pal->colorkey = hanchar_surf_vert->pal->colorkey = -1;
	}
	engchar_surf->pal->colors[0]=engchar_surf_vert_right->pal->colors[0]=hanchar_surf->pal->colors[0]=hanchar_surf_vert->pal->colors[0]=cur_fontdata->bkclr;
	engchar_surf->pal->colors[1]=engchar_surf_vert_right->pal->colors[1]=hanchar_surf->pal->colors[1]=hanchar_surf_vert->pal->colors[1]=cur_fontdata->fontclr;
	bf_drawstring( x, y, buf );
	free( buf );
}

/*
return count in pixels
*/
static int mytext_width( EGL_FONT* font, const char* str )
{
	int i = 0;
	U8 low;
	int len = 0;
	BITFONTDATA* fontdata = ( BITFONTDATA* )font->ext;
	while( str[i] )
	{
		low = str[i];
		i++;
		if( low < 0x80 ) //eng
		{
			len += fontdata->eng_font_w;
		}
		else
		{
			i++;
			len += fontdata->han_font_w;
		}
	}
	return len;
}

static int mytext_count_in_width( EGL_FONT* font, const char* str, int w )
{
	int i = 0;
	U8 low;
	BITFONTDATA* fontdata = ( BITFONTDATA* )font->ext;
	while( str[i] )
	{
		low = str[i];
		if( low < 0x80 ) //eng
		{
			if( w < fontdata ->eng_font_w )
			{
				return i;
			}
			else
			{
				w -= fontdata ->eng_font_w;
			}
			i++;
		}
		else
		{
			if( w < fontdata ->han_font_w )
			{
				return i;
			}
			else
			{
				if(str[i+1])
				{
					i++;
					w -= fontdata ->han_font_w;
				}
				else
				{
					return i-1;
				}
			}
			i++;
		}
	}
	return i;
}


static EGL_COLOR get_color( EGL_FONT* font )
{
	BITFONTDATA* mydata = (BITFONTDATA*)font->ext;
	return mydata->fontclr;;
}

static EGL_COLOR set_color( EGL_FONT* font, EGL_COLOR color )
{
	EGL_COLOR old;
	BITFONTDATA* mydata = (BITFONTDATA*)font->ext;
	old = mydata->fontclr;
	mydata->fontclr = color;
	return old;
}
static EGL_COLOR get_bkcolor( EGL_FONT* font )
{
	BITFONTDATA* mydata = (BITFONTDATA*)font->ext;
	return mydata->bkclr;
}
static EGL_COLOR set_bkcolor( EGL_FONT* font, EGL_COLOR color )
{
	EGL_COLOR old;
	BITFONTDATA* mydata = (BITFONTDATA*)font->ext;
	old = mydata->bkclr;
	mydata->bkclr = color;
	return old;
}

static void set_bkmode( EGL_FONT* font, int mode )
{
	( ( BITFONTDATA* )font->ext )->bkmode = (U8)mode;
}


static BOOL b_init = FALSE;
void release_default_font(EGL_FONT* pFont)
{
	if(pFont)
	{
		if(pFont->ext)
			free(pFont->ext);
		free( pFont );
	}
}
EGL_FONT* egl_create_default_font()
{
	EGL_FONT* font ;
	BITFONTDATA* mydata ;
	if( b_init == FALSE )
	{
		if( bf_init() == FALSE )
		{
			return NULL;
		}
		b_init = TRUE;
	}
	mydata = ( BITFONTDATA* )malloc( sizeof( BITFONTDATA ) );
	if( mydata == NULL )
	{
		return NULL;
	}
	font = ( EGL_FONT* )malloc( sizeof( EGL_FONT ) );
	if( font == NULL )
	{
		free( mydata );
		return NULL;
	}
	mydata->eng_font_w = ENG_FONT_W;
	mydata->eng_font_h = ENG_FONT_H;
	mydata->han_font_w = HAN_FONT_W;
	mydata->han_font_h = HAN_FONT_H;
	mydata->user_eng_font_w = ENG_FONT_W;
	mydata->user_eng_font_h = ENG_FONT_H;
	mydata->user_han_font_w = HAN_FONT_W;
	mydata->user_han_font_h = HAN_FONT_H;
	mydata->bkmode = 0;
	mydata->fontclr = DEFAULT_FONT_COLOR;
	mydata->bkclr = MAKE_COLORREF(255,255,255);

	font->draw = draw;
	font->draw_len = draw_len;
	font->text_count_in_width = mytext_count_in_width;
	font->text_width = mytext_width;
	font->set_color = set_color;
	font->set_bkcolor = set_bkcolor;
	font->get_color = get_color;
	font->get_bkcolor = get_bkcolor;
	font->h = ENG_FONT_H;
	font->set_bkmode = set_bkmode;
	font->ext = mydata;
	return font;
}
