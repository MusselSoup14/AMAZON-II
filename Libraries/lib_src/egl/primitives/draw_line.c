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

void sw_draw_hline(SURFACE* dst, int x, int y, int x2,EGL_COLOR c)
{
	
	if(x==x2)
		return ;
	if(x2<x)
	{
		int t = x;
		x = x2;
		x2 = t;
	}
	if((y<dst->clip.y)||(y>=dst->clip.endy))
	{
		return ;
	}
	if(x<dst->clip.x)
	{
		x = dst->clip.x;
	}
	if(x2>=dst->clip.endx)
	{
		x2 = dst->clip.endx;
	}
	if(dst->bpp==16)
	{
		U8 r,g,b;
		register U16* pixel ;
		U16 color565;
		EXTRACT_RGB(c,r,g,b);
		color565 = MAKE_RGB565(r,g,b);
		pixel = GETPOINT16(dst,x,y);
		for(;x<x2;x++)
		{
			*pixel=color565;
			pixel++;
		}
	}
	else if(dst->bpp==32)
	{
		register U32* pixel = GETPOINT32(dst,x,y);
		for(;x<x2;x++)
		{
			*pixel = c;
			pixel++;
		}
	}
}
void r_draw_hline(int x, int y, int x2,EGL_COLOR c)
{
	
#ifdef HW_2D_ENGINE
	if(x==x2)
		return ;
	if(x2<x)
	{
		int t = x;
		x = x2;
		x2 = t;
	}
	ge_draw_rectfill(x,y,x2-x,1,c);
	return;
#else
	sw_draw_hline(EGL_GET_TARGET_SURFACE(),x,y,x2,c);
#endif
}
void draw_hline(int x, int y, int x2,EGL_COLOR c)
{
#ifdef HW_2D_ENGINE
	ge_draw_rectfill(x,y,x2-x,1,c);
#else
	r_draw_hline(x,y,x2,c);
	SURFACE_FLUSH();
#endif // HW_2D_ENGINE
}
void sw_draw_vline(SURFACE* dst, int x, int y, int y2,EGL_COLOR c)
{
	if(y==y2)
		return ;
	if(y2<y)
	{
		int t = y;
		y = y2;
		y2 = t;
	}
	if((x<dst->clip.x)||(x>=dst->clip.endx))
	{
		return ;
	}
	if(y<dst->clip.y)
	{
		y = dst->clip.y;
	}
	if(y2>=dst->clip.endy)
	{
		y2 = dst->clip.endy;
	}
	if(dst->bpp==16)
	{
		U8 r,g,b;
		U16 color565;
		register U16* pixel ;
		EXTRACT_RGB(c,r,g,b);
		color565 = MAKE_RGB565(r,g,b);
		pixel = GETPOINT16(dst,x,y);
		for(;y<y2;y++)
		{
			*pixel=color565;
			pixel += (dst->pitch/2);
		}
	}
	else if(dst->bpp==32)
	{
		U32* pixel = GETPOINT32(dst,x,y);
		for(;y<y2;y++)
		{
			*pixel=c;
			pixel += (dst->pitch/4);
		}
	}
	else //if(dst->bpp==8)
	{
		U8* pixel = GETPOINT8(dst,x,y);
		for(;y<y2;y++)
		{
			*pixel=(U8)c;
			pixel += dst->pitch;
		}
	}
}
void r_draw_vline(int x, int y, int y2,EGL_COLOR c)
{
#ifdef HW_2D_ENGINE
	if(y==y2)
		return ;
	if(y2<y)
	{
		int t = y;
		y = y2;
		y2 = t;
	}
	ge_draw_rectfill(x,y,1,y2-y,c);
#else
	SURFACE* dst = EGL_GET_TARGET_SURFACE();
	sw_draw_hline(dst,x,y,y2,c);
	
#endif//HW_2D_ENGINE
}
void draw_vline(int x, int y, int y2,EGL_COLOR c)
{
#ifdef HW_2D_ENGINE
	ge_draw_rectfill(x,y,1,y2-y,c);
#else
	r_draw_vline(x,y,y2,c);
	SURFACE_FLUSH();
#endif //HW_2D_ENGINE
}

void r_draw_line(int x1, int y1, int x2, int y2,EGL_COLOR color)
{
	int pixx, pixy;
	int x, y;
	int dx, dy;
	int sx, sy;
	int swaptmp;
	U8* pixel;
	U16 rgb565;
	int leftx,rightx,topy,bottomy;
	SURFACE* dst = EGL_GET_TARGET_SURFACE();
	if(x1==x2)
	{
		r_draw_vline(x1,y1,y2,color);
		return;
	}
	else if(y1==y2)
	{
		r_draw_hline(x1,y1,x2,color);
		return;
	}
#ifdef HW_2D_ENGINE
	ge_wait_cmd_complete();
#endif
	if(x1>x2)
	{
		leftx = x2;
		rightx = x1;
	}
	else
	{
		leftx = x1;
		rightx = x2;
	}
	
	if(y1>y2)
	{
		topy = y2;
		bottomy = y1;
	}
	else
	{
		topy = y1;
		bottomy = y2;
	}

	//check clipping
	if(leftx<dst->clip.x)
	{
		x1 = dst->clip.x;
	}
	if(rightx>dst->clip.endx)
	{
		x2 = dst->clip.endx;
	}
	if(topy<dst->clip.y)
	{
		y1 = dst->clip.y;
	}
	if(bottomy>dst->clip.endy)
	{
		y2 = dst->clip.endy;
	}
	
	dx = x2 - x1;
	dy = y2 - y1;
	sx = (dx >= 0) ? 1 : -1;
	sy = (dy >= 0) ? 1 : -1;


	/*
	* More variable setup 
	*/
	dx = sx * dx + 1;
	dy = sy * dy + 1;
	pixx = dst->bpp/8;
	pixy = dst->pitch;
	pixel = ((U8 *) dst->pixels) + pixx * (int) x1 + pixy * (int) y1;
	pixx *= sx;
	pixy *= sy;
	if (dx < dy) {
		swaptmp = dx;
		dx = dy;
		dy = swaptmp;
		swaptmp = pixx;
		pixx = pixy;
		pixy = swaptmp;
	}
	/*
	* Draw 
	*/
	x = 0;
	y = 0;
	switch (dst->bpp/8) {
	case 2:
		{
			U8 r,g,b;
			EXTRACT_RGB(color,r,g,b);
			rgb565 = MAKE_RGB565(r,g,b);
		}
		for (; x < dx; x++, pixel += pixx) {
			*(U16 *) pixel = rgb565;
			y += dy;
			if (y >= dx) {
				y -= dx;
				pixel += pixy;
			}
		}
		break;
	case 4:	
		for (; x < dx; x++, pixel += pixx) {
			*(U32 *) pixel = color;
			y += dy;
			if (y >= dx) {
				y -= dx;
				pixel += pixy;
			}
		}
		break;
	}
}

void draw_line(int x1, int y1, int x2, int y2,EGL_COLOR color)
{
	r_draw_line(x1, y1,x2, y2, color);
	SURFACE_FLUSH();

}
void sw_draw_line(SURFACE* dst,int x1, int y1, int x2, int y2,EGL_COLOR color)
{
	int pixx, pixy;
	int x, y;
	int dx, dy;
	int sx, sy;
	int swaptmp;
	U8* pixel;
	U16 rgb565;
	int leftx,rightx,topy,bottomy;
	if(x1==x2)
	{
		sw_draw_vline(dst,x1,y1,y2,color);
		return;
	}
	else if(y1==y2)
	{
		sw_draw_hline(dst,x1,y1,x2,color);
		return;
	}

	if(x1>x2)
	{
		leftx = x2;
		rightx = x1;
	}
	else
	{
		leftx = x1;
		rightx = x2;
	}
	
	if(y1>y2)
	{
		topy = y2;
		bottomy = y1;
	}
	else
	{
		topy = y1;
		bottomy = y2;
	}

	//check clipping
	if(leftx<dst->clip.x)
	{
		x1 = dst->clip.x;
	}
	if(rightx>dst->clip.endx)
	{
		x2 = dst->clip.endx;
	}
	if(topy<dst->clip.y)
	{
		y1 = dst->clip.y;
	}
	if(bottomy>dst->clip.endy)
	{
		y2 = dst->clip.endy;
	}
	
	dx = x2 - x1;
	dy = y2 - y1;
	sx = (dx >= 0) ? 1 : -1;
	sy = (dy >= 0) ? 1 : -1;


	/*
	* More variable setup 
	*/
	dx = sx * dx + 1;
	dy = sy * dy + 1;
	pixx = dst->bpp/8;
	pixy = dst->pitch;
	pixel = ((U8 *) dst->pixels) + pixx * (int) x1 + pixy * (int) y1;
	pixx *= sx;
	pixy *= sy;
	if (dx < dy) {
		swaptmp = dx;
		dx = dy;
		dy = swaptmp;
		swaptmp = pixx;
		pixx = pixy;
		pixy = swaptmp;
	}
	/*
	* Draw 
	*/
	x = 0;
	y = 0;
	switch (dst->bpp/8) {
	case 2:
		{
			U8 r,g,b;
			EXTRACT_RGB(color,r,g,b);
			rgb565 = MAKE_RGB565(r,g,b);
		}
		for (; x < dx; x++, pixel += pixx) {
			*(U16 *) pixel = rgb565;
			y += dy;
			if (y >= dx) {
				y -= dx;
				pixel += pixy;
			}
		}
		break;
	case 4:	
		for (; x < dx; x++, pixel += pixx) {
			*(U32 *) pixel = color;
			y += dy;
			if (y >= dx) {
				y -= dx;
				pixel += pixy;
			}
		}
		break;
	}
}
