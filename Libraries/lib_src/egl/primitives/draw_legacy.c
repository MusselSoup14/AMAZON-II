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
#include "egl_private.h"
#include "primitives/draw_line.h"
#include "primitives/draw_rect.h"
#include "primitives/draw_poly.h"
#include "primitives/draw_arc.h"
#include "primitives/draw_thickline.h"
#include "surface.h"
#include "primitives/draw_roundrect.h"

#ifndef _MSC_VER
void dcache_invalidate_way();
#endif


#define DRAWUTIL_SUPPORT_CLIPWINDOW
#define DRAWUTIL_SUPPORT_CHECK_RECT

static U8 r = 0;
static U8 g = 0;
static U8 b = 0;
void drawsetrgb( U8 sr, U8 sg, U8 sb )
{
	r = sr;
	g = sg;
	b = sb;
}
void drawgetrgb( U8* dr, U8* dg, U8* db )
{
	*dr = r;
	*dg = g;
	*db = b;
}

INLINE static void putpixel32( int x, int y, int color )
{
#ifdef DRAWUTIL_SUPPORT_CLIPWINDOW
	if( x < EGL_GET_TARGET_SURFACE()->clip.x )
		return;
	if( x > EGL_GET_TARGET_SURFACE()->clip.endx)
		return;
	if( y < EGL_GET_TARGET_SURFACE()->clip.y )
		return;
	if( y > EGL_GET_TARGET_SURFACE()->clip.endy )
		return;
#endif
#ifdef DRAWUTIL_SUPPORT_CHECK_RECT
	if( x < 0 )
		return;
	if( y < 0 )
		return;
	if( x >= EGL_GET_TARGET_SURFACE()->w )
		return;
	if( y >= EGL_GET_TARGET_SURFACE()->h )
		return;
#endif
	*( U32* )( ( ( U32 )EGL_GET_TARGET_SURFACE()->pixels )  + ( x * 4 ) + ( EGL_GET_TARGET_SURFACE()->pitch * y ) ) = color;
}
INLINE static void putpixel16( int x, int y, int color )
{
#ifdef DRAWUTIL_SUPPORT_CLIPWINDOW
	if( x < EGL_GET_TARGET_SURFACE()->clip.x )
		return;
	if( x > EGL_GET_TARGET_SURFACE()->clip.endx)
		return;
	if( y < EGL_GET_TARGET_SURFACE()->clip.y )
		return;
	if( y > EGL_GET_TARGET_SURFACE()->clip.endy )
		return;
#endif
#ifdef DRAWUTIL_SUPPORT_CHECK_RECT
	if( x < 0 )
		return;
	if( y < 0 )
		return;
	if( x >= EGL_GET_TARGET_SURFACE()->w )
		return;
	if( y >= EGL_GET_TARGET_SURFACE()->h )
		return;
#endif
	*( U16* )( ( ( U32 )EGL_GET_TARGET_SURFACE()->pixels ) + ( x * 2 ) + ( EGL_GET_TARGET_SURFACE()->pitch * y ) ) = color;
}
void drawputpixel( int x, int y, U8 r, U8 g, U8 b )
{
	if( EGL_GET_TARGET_SURFACE()->bpp == 32 )
		putpixel32( x, y, MAKE_RGB888( r, g, b ) );
	else
		putpixel16( x, y, MAKE_RGB565( r, g, b ) );
}

void drawline( int x1, int y1, int x2, int y2 )
{
	r_draw_line( x1, y1, x2, y2, MAKE_COLORREF( r, g, b ) );
}
void line2( int x, int y, int x1, int y2 )
{
	drawline( x, y, x1, y );
	drawline( x1, y, x1, y2 );
}

void xyline3( int x, int y, int x1, int y2, int x3 )
{
	drawline( x, y, x1, y );
	drawline( x1, y, x1, y2 );
	drawline( x1, y2, x3, y2 );
}

void yxline( int x, int y, int endy )
{
	if ( endy < y )
	{
		endy--;
		drawline( x, endy, x, y );
	}
	else
	{
		endy++;
		drawline( x, y, x, endy );
	}
}

void yxline2( int x, int y, int y1, int x2, int y3 )
{
	drawline( x, y, x, y1 );
	drawline( x, y1, x2, y1 );
	drawline( x2, y1, x2, y3 );
}

void drawrect( int x, int y, int w, int h )
{
	draw_rect( x, y, w, h, MAKE_COLORREF( r, g, b ) );
}
void drawrectfill( int x, int y, int w, int h )
{
	draw_rectfill( x, y, w, h, MAKE_COLORREF( r, g, b ) );
}
void drawcircle ( int x, int y, int rad )
{
	draw_circle(x,y,rad,MAKE_COLORREF(r,g,b));
}
void drawcirclefill( int x, int y, int rad )
{
	draw_circlefill(x,y,rad,MAKE_COLORREF(r,g,b));
}
void drawellipse( int x, int y, int Xradius, int Yradius )
{
	draw_ellipse(x,y,Xradius,Yradius,MAKE_COLORREF(r,g,b));
}
void drawellipsefill( int x, int y, int Xradius, int Yradius )
{
	draw_ellipsefill(x,y,Xradius,Yradius,MAKE_COLORREF(r,g,b));
}

void drawround(int x0, int y0, int w, int h, int corner)
{
	draw_roundrect(x0, y0, w, h, corner ,MAKE_COLORREF(r,g,b));
}

void drawroundfill(int x0, int y0, int w, int h, int corner)
{
	draw_roundrectfill(x0, y0, w, h, corner ,MAKE_COLORREF(r,g,b));
}
