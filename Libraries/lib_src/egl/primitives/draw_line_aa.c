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
#include "surface.h"
#include "primitives/draw_line.h"
#include "egl_private.h"

#ifdef HW_2D_ENGINE
void ge_wait_cmd_complete();
void ge_draw_rectfill(int x, int y, int w, int h,EGL_COLOR c);
#endif

INLINE static void _putpixel16_a(SURFACE* dst, int x, int y, U8 sR,U8 sG,U8 sB,U8 alpha,U16 rgb565)
{
	U8 dR,dG,dB;
	U16 dc;
	U16* pixel;
	pixel = GETPOINT16(dst,x,y);
	if(alpha==0xff)
	{
		*pixel = rgb565;
		return;
	}
	dc = *pixel;
	EXTRACT_RGB565(dc,dR,dG,dB);
	dR = dR + ((sR - dR) * alpha >> 8);
	dG = dG + ((sG - dG) * alpha >> 8);
	dB = dB + ((sB - dB) * alpha >> 8);
	*pixel = MAKE_RGB565(dR,dG,dB);
}
INLINE static void _putpixel32_a(SURFACE* dst, int x, int y, U8 sR,U8 sG,U8 sB,U8 alpha,EGL_COLOR c)
{
	U32* pixel ;
	U8 dR,dG,dB;
	EGL_COLOR dc;
	pixel = GETPOINT32(dst,x,y);
	if(alpha==0xff)
	{
		*pixel = c;
		return;
	}
	dc = *pixel;
	EXTRACT_RGB(dc,dR,dG,dB);
	dR = dR + ((sR - dR) * alpha >> 8);
	dG = dG + ((sG - dG) * alpha >> 8);
	dB = dB + ((sB - dB) * alpha >> 8);
	*pixel = MAKE_COLORREF(dR,dG,dB);
}

#define AAlevels 256
#define AAbits 8

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Draw antialised line . </summary>
///
/// <remarks>	2012-12-20. </remarks>
///
/// <param name="x1">   	The first x value. </param>
/// <param name="y1">   	The first y value. </param>
/// <param name="x2">   	The second x value. </param>
/// <param name="y2">   	The second y value. </param>
/// <param name="color">	The color. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void r_sw_draw_line_aa(SURFACE* dst,int x1,int y1,int x2,int y2,EGL_COLOR color)
{
	int xx0, yy0, xx1, yy1;
	U16 rgb565;
	U32 intshift, erracc, erradj;
	U32 erracctmp, wgt, wgtcompmask;
	int dx, dy, tmp, xdir, y0p1, x0pxdir;
	U8 sR,sG,sB;
	if(x1==x2)
		return ;
	if(x2<x1)
	{
		int t = x1;
		x1 = x2;
		x2 = t;
	}
	if((y1<dst->clip.y)||(y1>=dst->clip.endy))
	{
		return ;
	}
	if(x1<dst->clip.x)
	{
		x1= dst->clip.x;
	}
	if(x2>=dst->clip.endx)
	{
		x2 = dst->clip.endx;
	}
	rgb565 = MAKE_RGB565FROM888(color);
	if(dst->bpp==16)
	{
		EXTRACT_RGB565(color,sR,sG,sB);
	}
	else if(dst->bpp==32)
	{
		EXTRACT_RGB(color,sR,sG,sB);
	}
	else 
		return ;


	/*
	* Keep on working with 32bit numbers 
	*/
	xx0 = x1;
	yy0 = y1;
	xx1 = x2;
	yy1 = y2;

	/*
	* Reorder points if required 
	*/
	if (yy0 > yy1) {
		tmp = yy0;
		yy0 = yy1;
		yy1 = tmp;
		tmp = xx0;
		xx0 = xx1;
		xx1 = tmp;
	}

	/*
	* Calculate distance 
	*/
	dx = xx1 - xx0;
	dy = yy1 - yy0;

	/*
	* Check for special cases 
	*/
	if (dx == 0) {
		/*
		* Vertical line 
		*/
		r_draw_vline(x1, y1, y2, color);
		return;
	} else if (dy == 0) {
		/*
		* Horizontal line 
		*/
		r_draw_hline(x1, y1,x2, color);
		return;
	}
	/*
	* Adjust for negative dx and set xdir 
	*/
	if (dx >= 0) {
		xdir = 1;
	} else {
		xdir = -1;
		dx = (-dx);
	}

	/*
	* Zero accumulator 
	*/
	erracc = 0;

	/*
	* # of bits by which to shift erracc to get intensity level 
	*/
	intshift = 32 - AAbits;

	/*
	* Mask used to flip all bits in an intensity weighting 
	*/
	wgtcompmask = AAlevels - 1;


	/*
	* Draw the initial pixel in the foreground color 
	*/
	if(dst->bpp==16)
	{
		*GETPOINT16(dst,x1,y1) = rgb565;
	}
	else
	{
		*GETPOINT32(dst,x1,y1) = color;
	}
	

	/*
	* x-major or y-major? 
	*/
	if (dy > dx) {

		/*
		* y-major.  Calculate 16-bit fixed point fractional part of a pixel that
		* X advances every time Y advances 1 pixel, truncating the result so that
		* we won't overrun the endpoint along the X axis 
		*/
		/*
		* Not-so-portable version: erradj = ((Uint64)dx << 32) / (Uint64)dy; 
		*/
		erradj = ((dx << 16) / dy) << 16;

		/*
		* draw all pixels other than the first and last 
		*/
		if(dst->bpp==16)
		{
			x0pxdir = xx0 + xdir;
			while (--dy) {
				erracctmp = erracc;
				erracc += erradj;
				if (erracc <= erracctmp) {
					/*
					* rollover in error accumulator, x coord advances 
					*/
					xx0 = x0pxdir;
					x0pxdir += xdir;
				}
				yy0++;		/* y-major so always advance Y */

				/*
				* the AAbits most significant bits of erracc give us the intensity
				* weighting for this pixel, and the complement of the weighting for
				* the paired pixel. 
				*/
				wgt = (erracc >> intshift) & 255;
				_putpixel16_a(dst, xx0, yy0, sR,sG,sB,255 - wgt,rgb565);
				_putpixel16_a(dst, x0pxdir, yy0, sR,sG,sB, wgt,rgb565);
			}
		}
		else if(dst->bpp==32)
		{
			x0pxdir = xx0 + xdir;
			while (--dy) {
				erracctmp = erracc;
				erracc += erradj;
				if (erracc <= erracctmp) {
					/*
					* rollover in error accumulator, x coord advances 
					*/
					xx0 = x0pxdir;
					x0pxdir += xdir;
				}
				yy0++;		/* y-major so always advance Y */

				/*
				* the AAbits most significant bits of erracc give us the intensity
				* weighting for this pixel, and the complement of the weighting for
				* the paired pixel. 
				*/
				wgt = (erracc >> intshift) & 255;
				_putpixel32_a(dst, xx0, yy0, sR,sG,sB, 255 - wgt,color);
				_putpixel32_a(dst, x0pxdir, yy0, sR,sG,sB, wgt,color);
			}
		}

	} else {

		/*
		* x-major line.  Calculate 16-bit fixed-point fractional part of a pixel
		* that Y advances each time X advances 1 pixel, truncating the result so
		* that we won't overrun the endpoint along the X axis. 
		*/
		/*
		* Not-so-portable version: erradj = ((Uint64)dy << 32) / (Uint64)dx; 
		*/
		erradj = ((dy << 16) / dx) << 16;

		/*
		* draw all pixels other than the first and last 
		*/
		y0p1 = yy0 + 1;
		while (--dx) {

			erracctmp = erracc;
			erracc += erradj;
			if (erracc <= erracctmp) {
				/*
				* Accumulator turned over, advance y 
				*/
				yy0 = y0p1;
				y0p1++;
			}
			xx0 += xdir;	/* x-major so always advance X */
			/*
			* the AAbits most significant bits of erracc give us the intensity
			* weighting for this pixel, and the complement of the weighting for
			* the paired pixel. 
			*/
			wgt = (erracc >> intshift) & 255;
			_putpixel32_a (dst, xx0, yy0, sR,sG,sB, 255 - wgt,color);
			_putpixel32_a (dst, xx0, y0p1, sR,sG,sB, wgt,color);
		}
	}
}

void r_draw_line_aa(int x1,int y1,int x2,int y2,EGL_COLOR color)
{
	r_sw_draw_line_aa(EGL_GET_TARGET_SURFACE(),x1,y1,x2,y2,color);
}

void draw_line_aa(int x1,int y1,int x2,int y2,EGL_COLOR color)
{
#ifdef HW_2D_ENGINE
	ge_wait_cmd_complete();
#endif
	r_sw_draw_line_aa(EGL_GET_TARGET_SURFACE(),x1,y1,x2,y2,color);
	SURFACE_FLUSH();
}

void sw_draw_line_aa(SURFACE* dst,int x1,int y1,int x2,int y2,EGL_COLOR color)
{
#ifdef HW_2D_ENGINE
	ge_wait_cmd_complete();
#endif
	r_sw_draw_line_aa(dst,x1,y1,x2,y2,color);
	SURFACE_FLUSH();
}
