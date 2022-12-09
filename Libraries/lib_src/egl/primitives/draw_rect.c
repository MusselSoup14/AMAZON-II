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

#ifdef HW_2D_ENGINE
	void ge_draw_rectfill(int x, int y, int w, int h,EGL_COLOR c);
#endif

void r_draw_rect(int x, int y, int w, int h,EGL_COLOR c)
{
#ifdef HW_2D_ENGINE
	ge_draw_rectfill(x,y,w,1,c);
	ge_draw_rectfill(x+w-1,y,1,h,c);
	ge_draw_rectfill(x,y+h-1,w,1,c);
	ge_draw_rectfill(x,y,1,h,c);
#else
	r_draw_hline(x,y,x+w,c);
	r_draw_vline(x+w-1,y,y+h,c);
	r_draw_hline(x,y+h-1,x+w,c);
	r_draw_vline(x,y+1,y+h,c);
#endif // HW_2D_ENGINE
}
void draw_rect(int x, int y, int w, int h,EGL_COLOR c)
{
#ifdef HW_2D_ENGINE
	ge_draw_rectfill(x,y,w,1,c);
	ge_draw_rectfill(x+w-1,y,1,h,c);
	ge_draw_rectfill(x,y+h-1,w,1,c);
	ge_draw_rectfill(x,y,1,h,c);
#else
	r_draw_rect(x, y, w, h,c);
	SURFACE_FLUSH();
#endif//HW_2D_ENGINE
}

void r_draw_rectfill(int x, int y, int w, int h,EGL_COLOR c)
{
#ifdef HW_2D_ENGINE
	ge_draw_rectfill(x,y,w,h,c);
#else
	int i;
	for(i=0;i<h;i++)
	{
		r_draw_hline(x,y+i,x+w,c);
	}
#endif // HW_2D_ENGINE
}
void draw_rectfill(int x, int y, int w, int h,EGL_COLOR c)
{
#ifdef HW_2D_ENGINE
	ge_draw_rectfill(x,y,w,h,c);
#else
	r_draw_rectfill(x, y, w, h,c);
	SURFACE_FLUSH();
#endif//HW_2D_ENGINE
}

static void _r_rectfill_gradient(int x, int y, int w, int h,EGL_COLOR startcolor,EGL_COLOR endcolor,BOOL bVertical)
{
	int step_r,step_g,step_b;
	int add_r,add_g,add_b;
	unsigned char sr,sg,sb;
	unsigned char er,eg,eb;
	EGL_COLOR col;
	int i;
	EXTRACT_RGB(startcolor,sr,sg,sb);
	EXTRACT_RGB(endcolor,er,eg,eb);
	///24.8 fixed point
	if(bVertical)
	{
		step_r = ((int)(sr-er)*1024)/h;
		step_g = ((int)(sg-eg)*1024)/h;
		step_b = ((int)(sb-eb)*1024)/h;
		add_r=add_g=add_b = 0 ;
		for(i=0;i<h;i++)
		{
			col = startcolor - ((add_r/1024)<<16) - ((add_g/1024)<<8) - (add_b/1024);
#ifdef HW_2D_ENGINE
			ge_draw_rectfill(x,y+i,w,1,col);
#else
			r_draw_hline(x,y+i,x+w,col);
#endif //HW_2D_ENGINE
			add_r += step_r;
			add_g += step_g;
			add_b += step_b;
		}
	}
	else
	{
		step_r = ((int)(sr-er)*1024)/w;
		step_g = ((int)(sg-eg)*1024)/w;
		step_b = ((int)(sb-eb)*1024)/w;
		add_r=add_g=add_b = 0 ;
		for(i=0;i<w;i++)
		{
			col = startcolor - ((add_r/1024)<<16) - ((add_g/1024)<<8) - (add_b/1024);
#ifdef HW_2D_ENGINE
			ge_draw_rectfill(x+i,y,1,h,col);
#else
			r_draw_vline(x+i,y,y+h,col);
#endif // HW_2D_ENGINE
			add_r += step_r;
			add_g += step_g;
			add_b += step_b;
		}
	}
}

void r_draw_rectfill_v_gradient(int x, int y, int w, int h,EGL_COLOR startcolor,EGL_COLOR endcolor)
{
	_r_rectfill_gradient(x,y, w, h,startcolor,endcolor,TRUE);
}
void draw_rectfill_v_gradient(int x, int y, int w, int h,EGL_COLOR startcolor,EGL_COLOR endcolor)
{
#ifdef HW_2D_ENGINE
	_r_rectfill_gradient(x,y,w,h,startcolor,endcolor,TRUE);
#else
	r_draw_rectfill_v_gradient(x, y, w, h,startcolor,endcolor);
	SURFACE_FLUSH();
#endif
}
void r_draw_rectfill_h_gradient(int x, int y, int w, int h,EGL_COLOR startcolor,EGL_COLOR endcolor)
{
	_r_rectfill_gradient(x,y, w, h,startcolor,endcolor,FALSE);
}
void draw_rectfill_h_gradient(int x, int y, int w, int h,EGL_COLOR startcolor,EGL_COLOR endcolor)
{
	_r_rectfill_gradient(x,y, w, h,startcolor,endcolor,FALSE);;
#ifdef HW_2D_ENGINE
	return;
#endif
	SURFACE_FLUSH();
}

void r_draw_rectfill_gradient(int x, int y, int w, int h,EGL_COLOR startcolor,EGL_COLOR endcolor,BOOL bVertical)
{
	_r_rectfill_gradient(x,y, w, h,startcolor,endcolor,bVertical);
}
void draw_rectfill_gradient(int x, int y, int w, int h,EGL_COLOR startcolor,EGL_COLOR endcolor,BOOL bVertical)
{
	_r_rectfill_gradient(x,y, w, h,startcolor,endcolor,bVertical);;
#ifdef HW_2D_ENGINE
	return;
#endif
	SURFACE_FLUSH();
}
