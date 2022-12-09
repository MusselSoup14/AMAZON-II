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
#include "egl_private.h"
#include "primitives/draw_line.h"
#include "primitives/draw_arc.h"
#include "primitives/draw_poly.h"
#include "util/fixed_sincos.h"

#ifdef HW_2D_ENGINE
void ge_wait_cmd_complete();
void ge_draw_rectfill(int x, int y, int w, int h,EGL_COLOR c);
#endif

#ifdef _MSC_VER
#include <memory.h>
#endif
#include <stdlib.h>
#include <math.h>

INLINE static void _putpixel(SURFACE* dst, int x, int y, EGL_COLOR c)
{
	U8 r,g,b;
	if(dst->bpp==16)
	{
		U16* pixel = GETPOINT16(dst,x,y);
		EXTRACT_RGB(c,r,g,b);
		*pixel = MAKE_RGB565(r,g,b);
	}
	else
	{
		U32* pixel = GETPOINT32(dst,x,y);
		*pixel = c;
	}
}

INLINE static void _putpixel16(SURFACE* dst, int x, int y, EGL_COLOR c)
{
	U8 r,g,b;
	U16* pixel = GETPOINT16(dst,x,y);
	EXTRACT_RGB(c,r,g,b);
	*pixel = MAKE_RGB565(r,g,b);
}

INLINE static void _putpixel_a(SURFACE* dst, int x, int y, EGL_COLOR c,U8 alpha)
{
	U8 sR,sG,sB;
	U8 dR,dG,dB;
	EGL_COLOR dc;
	U32* pixel ;
	pixel = GETPOINT32(dst,x,y);
	if(alpha==0xff)
	{
		*pixel = c;
		return;
	}
	EXTRACT_RGB(c,sR,sG,sB);
	dc = *pixel;
	EXTRACT_RGB(dc,dR,dG,dB);
	dR = dR + ((sR - dR) * alpha >> 8);
	dG = dG + ((sG - dG) * alpha >> 8);
	dB = dB + ((sB - dB) * alpha >> 8);
	*pixel = MAKE_COLORREF(dR,dG,dB);
}


typedef unsigned int uint32_t;

/* Draw a point in the foreground color, applying clipping if necessary*/
/*static*/ 
INLINE static void drawpoint(SURFACE* dst, int x, int y, EGL_COLOR c)
{
	if(dst->bpp==16)
	{
		U8 r,g,b;
		U16 rgb565;
		EXTRACT_RGB(c,r,g,b);
		rgb565 = MAKE_RGB565(r,g,b);
		SETPIXEL16(dst,x,y,rgb565);
	}
	else if(dst->bpp==32)
	{
		SETPIXEL32(dst,x,y,c);
	}
	else
	{
		SETPIXEL8(dst,x,y,c);
	}
}

/* outline and filled arc and pie types*/
#define TYPE_ARC		0x0001	/* arc*/
#define TYPE_OUTLINE	0x0002
#define TYPE_ARCOUTLINE	0x0003	/* arc + outline*/
#define TYPE_PIE		0x0004	/* pie (filled)*/
#define TYPE_ELLIPSE	0x0008	/* ellipse outline*/
#define TYPE_ELLIPSEFILL	0x0010	/* ellipse filled*/

/* Fill mode  */
#define MWFILL_SOLID          0  
#define MWFILL_STIPPLE        1  
#define MWFILL_OPAQUE_STIPPLE 2  
#define MWFILL_TILE           3

/* Clip areas*/
#define CLIP_VISIBLE		0
#define CLIP_INVISIBLE		1
#define CLIP_PARTIAL		2
static void _ArcAngle(int x0, int y0, int rx, int ry,
	int angle1, int angle2, int type,EGL_COLOR c)
{
	int s ;
	int e ;
	int fx,fy;
	int lx,ly;
	int i,x,y;
	int k;

	EGL_POINT* pts;

	x = 0, y = 0;
	fx = 0, fy = 0;
	lx = 0, ly = 0;
	s = angle1;
	e = angle2;


	if ((s% 360) == (e % 360)) {
		s = 0;
		e = 360;
	} else {
		if (s > 360)
			s %= 360;
		if (e > 360)
			e %= 360;
		while (s < 0)
			s += 360;
		while (e < s)
			e += 360;
		if (s == e) {
			s = 0;
			e = 360;
		}
	}
	pts = (EGL_POINT*)malloc(sizeof(EGL_POINT) * ((e-s)+2));
	pts[0].x = x0;
	pts[0].y = y0;
	/* generate arc points*/
	for (k=1,i = s; i <= e;k++, i++) {
		/* add 1 to rx/ry to smooth small radius arcs*/
		x = ((long)  egl_icos(i % 360) * (long) (rx + 1) / 1024) + x0;
		y = ((long) -egl_isin(i % 360) * (long) (ry + 1) / 1024) + y0;
		pts[k].x = x;
		pts[k].y = y;
	}

	if (type & TYPE_OUTLINE)
		r_draw_polygon(pts,k,c);
	else
		r_draw_polygonfill(pts,k,c);
	free(pts);
}

/* argument holder for pie, arc and ellipse functions*/
typedef struct {
	SURFACE* dst;
	EGL_COLOR c;
	int	x0, y0;		/* center*/
	int	rx, ry;		/* radii*/
	int	ax, ay;		/* start point*/
	int	bx, by;		/* end point*/
	int	adir;		/* start pt: 1=bottom half, -1=top half*/
	int	bdir;		/* end pt:  -1=bottom half,  1=top half*/
	int	type;		/* MWARC, MWARCOUTLINE, MWPIE, MWELLIPSE etc*/
} SLICE;

/*
 * Clip a line segment for arc or pie drawing.
 * Returns 0 if line is clipped or on acceptable side, 1 if it's vertically
 * on other side, otherwise 3.
 */
static int
clip_line(SLICE *slice, int xe, int ye, int dir, int y, int *x0,
	int *x1)
{
#if 1
	/*
	 * kluge: handle 180 degree case
	 */
	if (y >= 0 && ye == 0) {
/*printf("cl %d,%d %d,%d %d,%d %d,%d %d,%d\n", xe, ye, y, dir,
slice->ax, slice->ay, slice->bx, slice->by, slice->adir, slice->bdir);*/
		/* bottom 180*/
		if (slice->adir < 0) {
			if (slice->ay || slice->by)
				return 1;
			if (slice->ax == -slice->bx)
				return 0;
		}
		return 3;
	}
#endif
	/* hline on the same vertical side with the given edge? */
	if ((y >= 0 && ye >= 0) || (y < 0 && ye < 0)) {
		int x;

		if (ye == 0) x = xe; else
		x = (int)(long)xe * y / ye;

		if (x >= *x0 && x <= *x1) {
			if (dir > 0)
				*x0 = x;
			else
				*x1 = x;
			return 0;
		} else {
			if (dir > 0) {
				if (x <= *x0)
					return 0;
			} else {
				if (x >= *x1)
					return 0;
			}
		}
		return 3;
	}
	return 1;
}

static void
mw_draw_line(SLICE *slice, int x0, int y, int x1)
{
	int	dbl = (slice->adir > 0 && slice->bdir < 0);
	int 	discard, ret;
	int	x2 = x0, x3 = x1;
	if (y == 0) {
		if (slice->type != TYPE_PIE)
			return;
		/* edges on different sides */
		if ((slice->ay <= 0 && slice->by >= 0) ||
		    (slice->ay >= 0 && slice->by <= 0)) {
			if (slice->adir < 0)  {
				if (x1 > 0)
					x1 = 0;
			}
			if (slice->bdir > 0) {
				if (x0 < 0)
					x0 = 0;
			}
		} else {
			if (!dbl) {
			  drawpoint(slice->dst, slice->x0, slice->y0,slice->c);
				return;
			}
		}
		  r_draw_hline(slice->x0 + x0, slice->x0 + x1, slice->y0,slice->c);
		return;
	}

	/* clip left edge / line */
	ret = clip_line(slice, slice->ax, slice->ay, slice->adir, y, &x0, &x1);

	if (dbl) {
		if (!ret) {
			  r_draw_hline(slice->x0 + x0, slice->x0 + x1, slice->y0 + y,slice->c);

			x0 = x2;
			x1 = x3;
		}
	} else {
		if (ret > 1) {
			return;
		}
	}

	discard = ret;
	ret = clip_line(slice, slice->bx, slice->by, slice->bdir, y, &x0, &x1);

	discard += ret;
	if (discard > 2 && !(dbl && ret == 0 && discard == 3)) {
		return;
	}
	if (discard == 2) {
		/* line on other side than slice */
		if (slice->adir < 0 || slice->bdir > 0) {
			return;
		}
	}
	  r_draw_hline(slice->x0 + x0, slice->x0 + x1, slice->y0 + y,slice->c);
}

/*
 * draw one line segment or set of points, called from drawarc routine
 *
 * Note that this is called for all rows in one quadrant of the ellipse.
 * It mirrors vertically & horizontally to get the entire ellipse.
 *
 * It passes on co-ordinates for the *entire* ellipse - for pie and
 * arc, clipping is done later to ensure that only the requested angle
 * gets drawn.
 */
static void
drawarcsegment(SLICE *slice, int xp, int yp, int drawon)
{
	
	switch (slice->type) {
	case TYPE_ELLIPSEFILL:
		/* draw ellipse fill segment*/
                /* First, save the dash settings, because we don't want to use them here */
	    r_draw_hline(slice->x0-xp, slice->y0-yp,slice->x0+xp+1, slice->c);
	    r_draw_hline(slice->x0-xp, slice->y0+yp,slice->x0+xp+1, slice->c);
	  return;

	case TYPE_ELLIPSE:
	  if (!drawon) return;
		/* set four points symmetrically situated around a point*/
	  drawpoint(slice->dst, slice->x0 + xp, slice->y0 + yp,slice->c);
	  drawpoint(slice->dst, slice->x0 - xp, slice->y0 + yp,slice->c);
	  drawpoint(slice->dst, slice->x0 + xp, slice->y0 - yp,slice->c);
	  drawpoint(slice->dst, slice->x0 - xp, slice->y0 - yp,slice->c);
		return;

	case TYPE_PIE:
		/* draw top and bottom halfs of pie*/
		mw_draw_line(slice, -xp, -yp, +xp);
		mw_draw_line(slice, -xp, +yp, +xp);
		return;

	default:	/* MWARC, MWARCOUTLINE*/
		/* set four points symmetrically around a point and clip*/
		mw_draw_line(slice, +xp, +yp, +xp);
		mw_draw_line(slice, -xp, +yp, -xp);
		mw_draw_line(slice, +xp, -yp, +xp);
		mw_draw_line(slice, -xp, -yp, -xp);
		return;
	}
}

/* General routine to plot points on an arc.  Used by arc, pie and ellipse*/
static void
drawarc(SLICE *slice)
{
	int xp, yp;		/* current point (based on center) */
	int rx, ry;
	long Asquared;		/* square of x semi axis */
	long TwoAsquared;
	long Bsquared;		/* square of y semi axis */
	long TwoBsquared;
	long d;
	long dx, dy;

	rx = slice->rx;
	ry = slice->ry;

	xp = 0;
	yp = ry;
	Asquared = rx * rx;
	TwoAsquared = 2 * Asquared;
	Bsquared = ry * ry;
	TwoBsquared = 2 * Bsquared;
	d = Bsquared - Asquared * ry + (Asquared >> 2);
	dx = 0;
	dy = TwoAsquared * ry;

	while (dx < dy) {

		/*
		 * Only draw if one of the following conditions holds:
		 * - We're drawing an outline - i.e. slice->type is
		 *   not MWPIE or MWELLIPSEFILL
		 * - We're about to move on to the next Y co-ordinate
		 *   (i.e. we're drawing a filled shape and we're at
		 *   the widest point for this Y co-ordinate).
		 *   This is the case if d (the error term) is >0
		 * Otherwise, we draw multiple times, which messes up
		 * with SRC_OVER or XOR modes.
		 */
		if ((d > 0) || ((slice->type != TYPE_PIE) && (slice->type != TYPE_ELLIPSEFILL))) {
			drawarcsegment(slice, xp, yp, 1);
		}

		if (d > 0) {
			yp--;
			dy -= TwoAsquared;
			d -= dy;
		}
		xp++;
		dx += TwoBsquared;
		d += (Bsquared + dx);
	}

	d += ((3L * (Asquared - Bsquared) / 2L - (dx + dy)) >> 1);

	while (yp >= 0) {
		drawarcsegment(slice, xp, yp, 1);
		if (d < 0) {
			xp++;
			dx += TwoBsquared;
			d += dx;
		}
		yp--;
		dy -= TwoAsquared;
		d += (Asquared - dy);
	}

}

/**
 * Draw an ellipse using the current clipping region and foreground color.
 * This draws in the outline of the ellipse, or fills it.
 * Integer only routine.
 *
 * @param psd Destination surface.
 * @param x Center of ellipse (X co-ordinate).
 * @param y Center of ellipse (Y co-ordinate).
 * @param rx Radius of ellipse in X direction.
 * @param ry Radius of ellipse in Y direction.
 * @param fill Nonzero for a filled ellipse, zero for an outline.
 */
void
GdEllipse(int x, int y, int rx, int ry, BOOL fill,EGL_COLOR c)
{
	SLICE	slice;
	SURFACE* dst = EGL_GET_TARGET_SURFACE();

	if (rx < 0 || ry < 0)
		return;

	slice.dst = dst;
	slice.c = c;
	slice.x0 = x;
	slice.y0 = y;
	slice.rx = rx;
	slice.ry = ry;
	slice.type = fill? TYPE_ELLIPSEFILL: TYPE_ELLIPSE;
	/* other elements unused*/
	drawarc(&slice);
}
void draw_arc2( int x, int y, int rx, int ry,int ax, int ay, int bx, int by, EGL_COLOR c )
{
	r_draw_arc2( x, y, rx, ry,ax, ay, bx, by, c );

	SURFACE_FLUSH();
}
void r_draw_arc2( int x, int y, int rx, int ry,int ax, int ay, int bx, int by, EGL_COLOR c )
{
	SLICE	slice;
	slice.dst= EGL_GET_TARGET_SURFACE();
	slice.c = c;
	slice.x0 = x;
	slice.y0 = y;
	slice.rx = rx;
	slice.ry = ry;
	slice.ax=ax;
	slice.ay=ay;
	slice.bx=bx;
	slice.by=by;
	slice.adir = 1;
	slice.bdir=-1;
	slice.type = TYPE_ARC;
	drawarc(&slice);
}
void r_draw_arc( int x, int y, int rx, int ry,int a1, int a2,EGL_COLOR c )
{
	_ArcAngle(x,y,rx,ry,a1,a2,0,c);
}
void draw_arc( int x, int y, int rx, int ry,int a1, int a2,EGL_COLOR c )
{
	_ArcAngle(x,y,rx,ry,a1,a2,0,c);
	SURFACE_FLUSH();
}

void r_draw_pie(int x, int y, int rx, int ry,int a1, int a2,EGL_COLOR c)
{
	_ArcAngle(x,y,rx,ry,a1,a2,TYPE_OUTLINE,c);
}
void draw_pie(int x, int y, int rx, int ry,int a1, int a2,EGL_COLOR c)
{
	r_draw_pie(x, y, rx, ry,a1, a2,c);
	SURFACE_FLUSH();
	
}
void r_draw_piefill( int x, int y, int rx, int ry,int a1, int a2,EGL_COLOR c )
{
	_ArcAngle(x,y,rx,ry,a1,a2,TYPE_PIE,c);
}
void draw_piefill( int x, int y, int rx, int ry,int a1, int a2,EGL_COLOR c )
{
	r_draw_piefill( x, y, rx, ry,a1, a2,c );
	SURFACE_FLUSH();
}
void r_draw_ellipse( int x, int y, int rx, int ry,EGL_COLOR c )
{
	GdEllipse(x,y,rx,ry,0,c);
}
void draw_ellipse( int x, int y, int rx, int ry,EGL_COLOR c )
{
	r_draw_ellipse( x, y, rx, ry,c );
	SURFACE_FLUSH();
}
void r_draw_ellipsefill( int x, int y, int rx, int ry,EGL_COLOR c )
{
	GdEllipse(x,y,rx,ry,1,c);
}
void draw_ellipsefill( int x, int y, int rx, int ry,EGL_COLOR c )
{
	r_draw_ellipsefill( x, y, rx, ry,c );
	SURFACE_FLUSH();
}

void draw_circle(int x, int y, int rad,EGL_COLOR color)
{
	r_draw_circle(x, y, rad,color);
	SURFACE_FLUSH();
}

void r_draw_circle(int x, int y, int rad,EGL_COLOR color)
{
	GdEllipse(x,y,rad,rad,0,color);
}

void draw_circlefill(int x, int y, int rad,EGL_COLOR c)
{
	r_draw_circlefill(x,y,rad,c);
	SURFACE_FLUSH();
}
void r_draw_circlefill(int x, int y, int rad,EGL_COLOR c)
{
#if 1
	GdEllipse(x,y,rad,rad,1,c);
#else
	int cx = 0;
	int cy = rad;
	int ocx = (int) -1;
	int ocy = (int) -1;
	int df = 1 - rad;
	int d_e = 3;
	int d_se = -2 * rad + 5;
	int xpcx, xmcx, xpcy, xmcy;
	int ypcy, ymcy, ypcx, ymcx;
	if(rad<1)
		return ;

	/*
	* Draw 
	*/

	do {
		xpcx = x + cx;
		xmcx = x - cx;
		xpcy = x + cy;
		xmcy = x - cy;
		if (ocy != cy) {
			if (cy > 0) {
				ypcy = y + cy;
				ymcy = y - cy;
				r_draw_hline(dst,xmcx, ypcy, xpcx,c);
				r_draw_hline(dst,xmcx, ymcy,xpcx,c);
			} else {
				r_draw_hline(dst,xmcx, xpcx, y,c);
			}
			ocy = cy;
		}
		if (ocx != cx) {
			if (cx != cy) {
				if (cx > 0) {
					ypcx = y + cx;
					ymcx = y - cx;
					r_draw_hline(dst,xmcy, ymcx, xpcy,c);
					r_draw_hline(dst,xmcy, ypcx, xpcy,c);
				} else {
					r_draw_hline(dst,xmcy, y, xpcy,c);
				}
			}
			ocx = cx;
		}
		/*
		* Update 
		*/
		if (df < 0) {
			df += d_e;
			d_e += 2;
			d_se += 2;
		} else {
			df += d_se;
			d_e += 2;
			d_se += 4;
			cy--;
		}
		cx++;
	} while (cx <= cy);
#endif
}


/*!
\brief Internal function to calculate bezier interpolator of data array with ndata values at position 't'.

\param data Array of values.
\param ndata Size of array.
\param t Position for which to calculate interpolated value. t should be between [0, ndata].

\returns Interpolated value at position t, value[0] when t<0, value[n-1] when t>n.
*/
static double _evaluateBezier (double *data, int ndata, double t) 
{
	double mu, result;
	int n,k,kn,nn,nkn;
	double blend,muk,munk;

	/* Sanity check bounds */
	if (t<0.0) {
		return(data[0]);
	}
	if (t>=(double)ndata) {
		return(data[ndata-1]);
	}

	/* Adjust t to the range 0.0 to 1.0 */ 
	mu=t/(double)ndata;

	/* Calculate interpolate */
	n=ndata-1;
	result=0.0;
	muk = 1;
	munk = pow(1-mu,(double)n);
	for (k=0;k<=n;k++) {
		nn = n;
		kn = k;
		nkn = n - k;
		blend = muk * munk;
		muk *= mu;
		munk /= (1-mu);
		while (nn >= 1) {
			blend *= nn;
			nn--;
			if (kn > 1) {
				blend /= (double)kn;
				kn--;
			}
			if (nkn > 1) {
				blend /= (double)nkn;
				nkn--;
			}
		}
		result += data[k] * blend;
	}

	return (result);
}
/* Windows targets do not have lrint, so provide a local inline version */
#if defined(_MSC_VER)
/* Detect 64bit and use intrinsic version */
#ifdef _M_X64
#include <emmintrin.h>
static __inline long 
	lrint(float f) 
{
	return _mm_cvtss_si32(_mm_load_ss(&f));
}
#elif defined(_M_IX86)
INLINE long int
	lrint (double flt)
{	
	int intgr;
	_asm
	{
		fld flt
			fistp intgr
	};
	return intgr;
}
#else
#error lrint needed for MSVC on non X86/AMD64/ARM targets.
#endif
#endif
void draw_bezier(EGL_POINT* pts,int n, int s,EGL_COLOR c)
{
	r_draw_bezier(pts,n, s,c);
	SURFACE_FLUSH();
}
void r_draw_bezier(EGL_POINT* pts,int n, int s,EGL_COLOR c)
{
	int result;
	int i;
	double t, stepsize;
	int x1, y1, x2, y2;

#ifdef _WIN32
	double *x;
	double *y;
	x = (double*)malloc(sizeof(double)*(n+1));
	y = (double*)malloc(sizeof(double)*(n+1));
#else
	double x[n+1];
	double y[n+1];
#endif
	if(x==NULL || y==NULL)
		return ;
	/*
	* Sanity check 
	*/
	if (n < 3) {
		return ;
	}
	if (s < 2) {
		return ;
	}

	/*
	* Variable setup 
	*/
	stepsize=(double)1.0/(double)s;

	for (i=0; i<n; i++) {
		x[i]=(double)pts[i].x;
		y[i]=(double)pts[i].y;
	}      
	x[n]=(double)pts[0].x;
	y[n]=(double)pts[0].y;

	/*
	* Draw 
	*/
	result = 0;
	t=0.0;
	x1=(int)lrint(_evaluateBezier(x,n+1,t));
	y1=(int)lrint(_evaluateBezier(y,n+1,t));
	for (i = 0; i <= (n*s); i++) {
		t += stepsize;
		x2=(int)_evaluateBezier(x,n,t);
		y2=(int)_evaluateBezier(y,n,t);
		r_draw_line(x1, y1, x2, y2,c);
		x1 = x2;
		y1 = y2;
	}
#ifdef WIN32
	free(x);free(y);
#endif
}


/*!
\brief Draw anti-aliased ellipse with blending.

Note: Based on code from Anders Lindstroem, which is based on code from sge library, 
which is based on code from TwinLib.

\param dst The surface to draw on.
\param x X coordinate of the center of the aa-ellipse.
\param y Y coordinate of the center of the aa-ellipse.
\param rx Horizontal radius in pixels of the aa-ellipse.
\param ry Vertical radius in pixels of the aa-ellipse.
\param color The color value of the aa-ellipse to draw (0xRRGGBBAA). 

\returns Returns 0 on success, -1 on failure.
*/
void draw_ellipse_aa(int x, int y, int rx, int ry, EGL_COLOR color)
{
	r_draw_ellipse_aa(x, y, rx, ry, color);
	SURFACE_FLUSH();
}
void r_draw_ellipse_aa(int x, int y, int rx, int ry, EGL_COLOR color)
{
	int left, right, top, bottom;
	int x1,y1,x2,y2;
	int i;
	int a2, b2, ds, dt, dxt, t, s, d;
	int xp, yp, xs, ys, dyt, od, xx, yy, xc2, yc2;
	float cp;
	double sab;
	U8 weight, iweight;
	int result;
	SURFACE* dst = EGL_GET_TARGET_SURFACE();

	/*
	* Sanity check radii 
	*/
	if ((rx < 0) || (ry < 0)) {
		return ;
	}

	/*
	* Special case for rx=0 - draw a vline 
	*/
	if (rx == 0) {
		r_draw_vline(x, y - ry, y + ry, color);
		return;
	}
	/*
	* Special case for ry=0 - draw an hline 
	*/
	if (ry == 0) {
		r_draw_hline(x - rx, x + rx, y, color);
		return;
	}

	/*
	* Get circle and clipping boundary and 
	* test if bounding box of circle is visible 
	*/
	x2 = x + rx;
	left = dst->clip.x;
	if (x2<left) {
		return;
	} 
	x1 = x - rx;
	right = dst->clip.endx;
	if (x1>right) {
		return;
	} 
	y2 = y + ry;
	top = dst->clip.y;
	if (y2<top) {
		return;
	} 
	y1 = y - ry;
	bottom = dst->clip.endy;
	if (y1>bottom) {
		return;
	} 

	/* Variable setup */
	a2 = rx * rx;
	b2 = ry * ry;

	ds = 2 * a2;
	dt = 2 * b2;

	xc2 = 2 * x;
	yc2 = 2 * y;

	sab = sqrt((double)(a2 + b2));
	od = (int)lrint(sab*0.01) + 1; /* introduce some overdraw */
	dxt = (int)lrint((double)a2 / sab) + od;

	t = 0;
	s = -2 * a2 * ry;
	d = 0;

	xp = x;
	yp = y - ry;

	/* Draw */
	result = 0;

	/* "End points" */
	_putpixel(dst, xp, yp, color);
	_putpixel(dst, xc2 - xp, yp, color);
	_putpixel(dst, xp, yc2 - yp, color);
	_putpixel(dst, xc2 - xp, yc2 - yp, color);

	for (i = 1; i <= dxt; i++) {
		xp--;
		d += t - b2;

		if (d >= 0)
			ys = yp - 1;
		else if ((d - s - a2) > 0) {
			if ((2 * d - s - a2) >= 0)
				ys = yp + 1;
			else {
				ys = yp;
				yp++;
				d -= s + a2;
				s += ds;
			}
		} else {
			yp++;
			ys = yp + 1;
			d -= s + a2;
			s += ds;
		}

		t -= dt;

		/* Calculate alpha */
		if (s != 0) {
			cp = (float) abs(d) / (float) abs(s);
			if (cp > 1.0) {
				cp = 1.0;
			}
		} else {
			cp = 1.0;
		}

		/* Calculate weights */
		weight = (U8) (cp * 255);
		iweight = 255 - weight;

		/* Upper half */
		xx = xc2 - xp;
		_putpixel_a(dst, xp, yp, color, iweight);
		_putpixel_a(dst, xx, yp, color, iweight);

		_putpixel_a(dst, xp, ys, color, weight);
		_putpixel_a(dst, xx, ys, color, weight);

		/* Lower half */
		yy = yc2 - yp;
		_putpixel_a(dst, xp, yy, color, iweight);
		_putpixel_a(dst, xx, yy, color, iweight);

		yy = yc2 - ys;
		_putpixel_a(dst, xp, yy, color, weight);
		_putpixel_a(dst, xx, yy, color, weight);
	}

	/* Replaces original approximation code dyt = abs(yp - yc); */
	dyt = (int)lrint((double)b2 / sab ) + od;    

	for (i = 1; i <= dyt; i++) {
		yp++;
		d -= s + a2;

		if (d <= 0)
			xs = xp + 1;
		else if ((d + t - b2) < 0) {
			if ((2 * d + t - b2) <= 0)
				xs = xp - 1;
			else {
				xs = xp;
				xp--;
				d += t - b2;
				t -= dt;
			}
		} else {
			xp--;
			xs = xp - 1;
			d += t - b2;
			t -= dt;
		}

		s += ds;

		/* Calculate alpha */
		if (t != 0) {
			cp = (float) abs(d) / (float) abs(t);
			if (cp > 1.0) {
				cp = 1.0;
			}
		} else {
			cp = 1.0;
		}

		/* Calculate weight */
		weight = (U8) (cp * 255);
		iweight = 255 - weight;

		/* Left half */
		xx = xc2 - xp;
		yy = yc2 - yp;
		_putpixel_a(dst, xp, yp, color, iweight);
		_putpixel_a(dst, xx, yp, color, iweight);

		_putpixel_a(dst, xp, yy, color, iweight);
		_putpixel_a(dst, xx, yy, color, iweight);

		/* Right half */
		xx = xc2 - xs;
		_putpixel_a(dst, xs, yp, color, weight);
		_putpixel_a(dst, xx, yp, color, weight);

		_putpixel_a(dst, xs, yy, color, weight);
		_putpixel_a(dst, xx, yy, color, weight);

	}
}
