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
#include <stdlib.h>
#include "primitives/draw_line.h"
#include "primitives/draw_poly.h"
#include "egl_private.h"

void r_draw_polyline(EGL_POINT* p, int n,EGL_COLOR c)
{
	int i;
	for(i=1;i<n;i++)
		r_draw_line(p[i-1].x,p[i-1].y,p[i].x,p[i].y,c);
}

void draw_polyline(EGL_POINT* p, int n,EGL_COLOR c)
{
	r_draw_polyline(p, n,c);
	SURFACE_FLUSH();
}
void sw_draw_polyline(SURFACE* dst, EGL_POINT* p, int n,EGL_COLOR c)
{
	int i;
	for(i=1;i<n;i++)
		sw_draw_line(dst,p[i-1].x,p[i-1].y,p[i].x,p[i].y,c);
	SURFACE_FLUSH();
}
void sw_draw_polyline_aa(SURFACE* dst, EGL_POINT* p, int n,EGL_COLOR c)
{
	int i;
	for(i=1;i<n;i++)
		sw_draw_line_aa(dst,p[i-1].x,p[i-1].y,p[i].x,p[i].y,c);
	SURFACE_FLUSH();
}

void r_draw_polyline_aa(EGL_POINT* p, int n,EGL_COLOR c)
{
	int i;
	for(i=1;i<n;i++)
		r_draw_line_aa(p[i-1].x,p[i-1].y,p[i].x,p[i].y,c);
}

void r_sw_draw_polyline_aa(SURFACE* dst,EGL_POINT* p, int n,EGL_COLOR c)
{
	int i;
	for(i=1;i<n;i++)
		r_sw_draw_line_aa(dst,p[i-1].x,p[i-1].y,p[i].x,p[i].y,c);
}

void draw_polyline_aa(EGL_POINT* p, int n,EGL_COLOR c)
{
	r_draw_polyline_aa(p, n,c);
	SURFACE_FLUSH();
}


void r_draw_polygon(EGL_POINT* ptable,int n,EGL_COLOR c)
{
	r_draw_polyline(ptable,n,c);
	r_draw_line(ptable[0].x,ptable[0].y,ptable[n-1].x,ptable[n-1].y,c);
}

void draw_polygon(EGL_POINT* ptable,int n,EGL_COLOR c)
{
	r_draw_polygon(ptable,n,c);
	SURFACE_FLUSH();
}
void sw_draw_polygon(SURFACE* dst, EGL_POINT* ptable,int n,EGL_COLOR c)
{
	sw_draw_polyline(dst,ptable,n,c);
	sw_draw_line(dst,ptable[0].x,ptable[0].y,ptable[n-1].x,ptable[n-1].y,c);
	SURFACE_FLUSH();
}
void sw_draw_polygon_aa(SURFACE* dst, EGL_POINT* ptable,int n,EGL_COLOR c)
{
	sw_draw_polyline_aa(dst,ptable,n,c);
	sw_draw_line_aa(dst,ptable[0].x,ptable[0].y,ptable[n-1].x,ptable[n-1].y,c);
	SURFACE_FLUSH();
}

void r_draw_polygon_aa(EGL_POINT* ptable,int n,EGL_COLOR c)
{
	r_draw_polyline_aa(ptable,n,c);
	r_draw_line_aa(ptable[0].x,ptable[0].y,ptable[n-1].x,ptable[n-1].y,c);
}

void draw_polygon_aa(EGL_POINT* ptable,int n,EGL_COLOR c)
{
	r_draw_polygon_aa(ptable,n,c);
	SURFACE_FLUSH();
}
