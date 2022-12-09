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
#include "primitives/draw_rect.h"
#include "primitives/draw_line.h"
#include "egl_private.h"
#include "primitives/draw_roundrect.h"

#define DRAWUTIL_SUPPORT_CLIPWINDOW
#define DRAWUTIL_SUPPORT_CHECK_RECT

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
void r_draw_roundrect(int x0, int y0, int w, int h, int corner ,EGL_COLOR c)
{
	int startaddr;
	int bpl;
	register int i;
	int dx, dy;
	int Xcenter, Ycenter, X2center, Y2center;
	int x = 0;
	int rightInc = 6;
	int d, diagonalInc;
	int k;

	SURFACE* surf = EGL_GET_TARGET_SURFACE();
	bpl = surf->pitch;
	startaddr = ( U32 )surf->pixels;
	if ( w == 0 || h == 0 )
		return;

	if ( corner != 0 )
	{
		d = w < h ? w : h;
		--corner;
		if ( corner != 0 && corner + 2 >= d )
		{
			if ( corner + 2 == d )  --corner;
			else corner = 0;
		}
	}
	d = 3 - ( corner << 1 );
	diagonalInc = 10 - ( corner << 2 );
	/*Rectangles*/
	dx = w - ( corner << 1 );
	Xcenter = x0 + corner;
	dy = h - ( corner << 1 );
	Ycenter = y0 + corner;
	/*Centers*/
	X2center = Xcenter + dx - 1;
	Y2center = Ycenter + dy - 1;
	i = dx;
	if( surf->bpp == 16 )
	{
		int color ;
		U8 r,g,b;
		EXTRACT_RGB(c,r,g,b);
		color = MAKE_RGB565(r,g,b);
		k = 0;
		switch( i % 4 )
		{
			do
			{
		case 0:
			putpixel16( Xcenter + k, y0, color );
			putpixel16( Xcenter + k, y0 + h - 1, color );
			k++;
		case 3:
			putpixel16( Xcenter + k, y0, color );
			putpixel16( Xcenter + k, y0 + h - 1, color );
			k++;
		case 2:
			putpixel16( Xcenter + k, y0, color );
			putpixel16( Xcenter + k, y0 + h - 1, color );
			k++;
		case 1:
			putpixel16( Xcenter + k, y0, color );
			putpixel16( Xcenter + k, y0 + h - 1, color );
			k++;
			}
			while( ( i -= 4 ) > 0 );
		}
		if ( h < 3 )  return;
		k = 0;
		i = dy;
		switch( i % 4 )
		{
			do
			{
		case 0:
			putpixel16( x0, Ycenter + k, color );
			putpixel16( x0 + w - 1, Ycenter + k, color );
			k++;
		case 3:
			putpixel16( x0, Ycenter + k, color );
			putpixel16( x0 + w - 1, Ycenter + k, color );
			k++;
		case 2:
			putpixel16( x0, Ycenter + k, color );
			putpixel16( x0 + w - 1, Ycenter + k, color );
			k++;
		case 1:
			putpixel16( x0, Ycenter + k, color );
			putpixel16( x0 + w - 1, Ycenter + k, color );
			k++;
			}
			while( ( i -= 4 ) > 0 );
		}
		while ( x < corner )
		{
			putpixel16( Xcenter - corner, Ycenter - x, color );
			putpixel16( Xcenter - x, Ycenter - corner, color );
			putpixel16( X2center + x, Ycenter - corner, color );
			putpixel16( X2center + corner, Ycenter - x, color );
			putpixel16( X2center + x, Y2center + corner, color );
			putpixel16( X2center + corner, Y2center + x, color );
			putpixel16( Xcenter - x, Y2center + corner, color );
			putpixel16( Xcenter - corner, Y2center + x, color );
			if ( d >= 0 )
			{
				d += diagonalInc;
				diagonalInc += 8;
				--corner;
			}
			else
			{
				d += rightInc;
				diagonalInc += 4;
			}
			rightInc += 4;
			++x;
		}/*while*/
	}
	else//32
	{
		int color ;
		U8 r,g,b;
		EXTRACT_RGB(c,r,g,b);
		color = MAKE_RGB888(r,g,b);
		k = 0;
		switch( i % 4 )
		{
			do
			{
		case 0:
			putpixel32( Xcenter + k, y0, color );
			putpixel32( Xcenter + k, y0 + h - 1, color );
			k++;
		case 3:
			putpixel32( Xcenter + k, y0, color );
			putpixel32( Xcenter + k, y0 + h - 1, color );
			k++;
		case 2:
			putpixel32( Xcenter + k, y0, color );
			putpixel32( Xcenter + k, y0 + h - 1, color );
			k++;
		case 1:
			putpixel32( Xcenter + k, y0, color );
			putpixel32( Xcenter + k, y0 + h - 1, color );
			k++;
			}
			while( ( i -= 4 ) > 0 );
		}
		if ( h < 3 )  return;
		i = dy;
		k = 0;
		switch( i % 4 )
		{
			do
			{
		case 0:
			putpixel32( x0, Ycenter + k, color );
			putpixel32( x0 + w - 1, Ycenter + k, color );
			k++;
		case 3:
			putpixel32( x0, Ycenter + k, color );
			putpixel32( x0 + w - 1, Ycenter + k, color );
			k++;
		case 2:
			putpixel32( x0, Ycenter + k, color );
			putpixel32( x0 + w - 1, Ycenter + k, color );
			k++;
		case 1:
			putpixel32( x0, Ycenter + k, color );
			putpixel32( x0 + w - 1, Ycenter + k, color );
			k++;
			}
			while( ( i -= 4 ) > 0 );
		}
		while ( x < corner )
		{
			putpixel32( Xcenter - corner, Ycenter - x, color );
			putpixel32( Xcenter - x, Ycenter - corner, color );
			putpixel32( X2center + x, Ycenter - corner, color );
			putpixel32( X2center + corner, Ycenter - x, color );
			putpixel32( X2center + x, Y2center + corner, color );
			putpixel32( X2center + corner, Y2center + x, color );
			putpixel32( Xcenter - x, Y2center + corner, color );
			putpixel32( Xcenter - corner, Y2center + x, color );
			if ( d >= 0 )
			{
				d += diagonalInc;
				diagonalInc += 8;
				--corner;
			}
			else
			{
				d += rightInc;
				diagonalInc += 4;
			}
			rightInc += 4;
			++x;
		}/*while*/
	}
}
void draw_roundrect(int x0, int y0, int w, int h, int corner ,EGL_COLOR c)
{
	r_draw_roundrect(x0,y0,w,h,corner,c);
	SURFACE_FLUSH();
}

void r_draw_roundrectfill(int x0, int y0, int w, int h, int corner ,EGL_COLOR c)
{
	int startaddr;
	int bpl;
	SURFACE* surf;
	register int i;
	int dx, dy;
	int Xcenter, Ycenter, X2center, Y2center;
	int x = 0;
	int rightInc = 6;
	int d, diagonalInc;
	int k;

	surf = EGL_GET_TARGET_SURFACE();
	bpl = surf->pitch;
	startaddr = ( U32 )surf->pixels;
	if ( w == 0 || h == 0 )
		return;
	/*TODO: We can do better :-)*/
	if ( corner != 0 )
	{
		d = w < h ? w : h;
		--corner;
		if ( corner != 0 && corner + 2 >= d )
		{
			if ( corner + 2 == d )  --corner;
			else corner = 0;
		}
	}
	d = 3 - ( corner << 1 );
	diagonalInc = 10 - ( corner << 2 );
	/*Rectangles*/
	dx = w - ( corner << 1 );
	Xcenter = x0 + corner;
	dy = h - ( corner << 1 );
	Ycenter = y0 + corner;
	/*Centers*/
	X2center = Xcenter + dx - 1;
	Y2center = Ycenter + dy - 1;
	r_draw_rectfill( x0, Ycenter, w, dy,c );
	if( surf->bpp == 16 )
	{
		U8 r,g,b;
		int color ;
		EXTRACT_RGB(c,r,g,b);
		color = MAKE_RGB565( r, g, b );
		while ( x <= corner )
		{
			k = 0;
			i = X2center - Xcenter + 2 * corner + 1;
			switch( i % 4 )
			{
				do
				{
			case 0:
				putpixel16( Xcenter - corner + k, Ycenter - x, color );
				putpixel16( Xcenter - corner + k, Y2center + x, color );
				k++;
			case 3:
				putpixel16( Xcenter - corner + k, Ycenter - x, color );
				putpixel16( Xcenter - corner + k, Y2center + x, color );
				k++;
			case 2:
				putpixel16( Xcenter - corner + k, Ycenter - x, color );
				putpixel16( Xcenter - corner + k, Y2center + x, color );
				k++;
			case 1:
				putpixel16( Xcenter - corner + k, Ycenter - x, color );
				putpixel16( Xcenter - corner + k, Y2center + x, color );
				k++;
				}
				while( ( i -= 4 ) > 0 );
			}
			k = 0;
			i = X2center - Xcenter + 2 * x + 1;
			switch( i % 4 )
			{
				do
				{
			case 0:
				putpixel16( Xcenter - x + k, Y2center + corner, color );
				putpixel16( Xcenter - x + k, Ycenter - corner, color );
				k++;
			case 3:
				putpixel16( Xcenter - x + k, Y2center + corner, color );
				putpixel16( Xcenter - x + k, Ycenter - corner, color );
				k++;
			case 2:
				putpixel16( Xcenter - x + k, Y2center + corner, color );
				putpixel16( Xcenter - x + k, Ycenter - corner, color );
				k++;
			case 1:
				putpixel16( Xcenter - x + k, Y2center + corner, color );
				putpixel16( Xcenter - x + k, Ycenter - corner, color );
				k++;
				}
				while( ( i -= 4 ) > 0 );
			}
			if ( d >= 0 )
			{
				d += diagonalInc;
				diagonalInc += 8;
				--corner;
			}
			else
			{
				d += rightInc;
				diagonalInc += 4;
			}
			rightInc += 4;
			++x;
		}/*while*/
	}
	else//32
	{
		U8 r,g,b;
		int color ;
		EXTRACT_RGB(c,r,g,b);
		color = MAKE_RGB888( r, g, b );
		while ( x <= corner )
		{
			k = 0;
			i = X2center - Xcenter + 2 * corner + 1;
			switch( i % 4 )
			{
				do
				{
			case 0:
				putpixel32( Xcenter - corner + k, Ycenter - x, color );
				putpixel32( Xcenter - corner + k, Y2center + x, color );
				k++;
			case 3:
				putpixel32( Xcenter - corner + k, Ycenter - x, color );
				putpixel32( Xcenter - corner + k, Y2center + x, color );
				k++;
			case 2:
				putpixel32( Xcenter - corner + k, Ycenter - x, color );
				putpixel32( Xcenter - corner + k, Y2center + x, color );
				k++;
			case 1:
				putpixel32( Xcenter - corner + k, Ycenter - x, color );
				putpixel32( Xcenter - corner + k, Y2center + x, color );
				k++;
				}
				while( ( i -= 4 ) > 0 );
			}
			k = 0;
			i = X2center - Xcenter + 2 * x + 1;
			switch( i % 4 )
			{
				do
				{
			case 0:
				putpixel32( Xcenter - x + k, Y2center + corner, color );
				putpixel32( Xcenter - x + k, Ycenter - corner, color );
				k++;
			case 3:
				putpixel32( Xcenter - x + k, Y2center + corner, color );
				putpixel32( Xcenter - x + k, Ycenter - corner, color );
				k++;
			case 2:
				putpixel32( Xcenter - x + k, Y2center + corner, color );
				putpixel32( Xcenter - x + k, Ycenter - corner, color );
				k++;
			case 1:
				putpixel32( Xcenter - x + k, Y2center + corner, color );
				putpixel32( Xcenter - x + k, Ycenter - corner, color );
				k++;
				}
				while( ( i -= 4 ) > 0 );
			}
			if ( d >= 0 )
			{
				d += diagonalInc;
				diagonalInc += 8;
				--corner;
			}
			else
			{
				d += rightInc;
				diagonalInc += 4;
			}
			rightInc += 4;
			++x;
		}/*while*/
	}
}
void draw_roundrectfill(int x0, int y0, int w, int h, int corner ,EGL_COLOR c)
{
	r_draw_roundrectfill(x0,y0,w,h,corner,c);
	SURFACE_FLUSH();
}
