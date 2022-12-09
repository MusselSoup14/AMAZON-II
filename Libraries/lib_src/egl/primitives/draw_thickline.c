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
#include "primitives/draw_rect.h"
#include "primitives/draw_poly.h"
#include "egl_private.h"
#include <math.h>

/*!
\brief The structure passed to the internal Bresenham iterator.
*/
typedef struct
{
	S16 x, y;
	int dx, dy, s1, s2, swapdir, error;
	U32 count;
} BresenhamIterator;

typedef struct
{
	EGL_COLOR color;
	SURFACE* dst;
	int u, v;		/* delta x , delta y */
	int ku, kt, kv, kd;	/* loop constants */
	int oct2;
	int quad4;
	int last1x, last1y, last2x, last2y, first1x, first1y, first2x, first2y, tempx, tempy;
} MurphyIterator;

/*!
\brief Internal function to initialize the Bresenham line iterator.

Example of use:
SDL_gfxBresenhamIterator b;
_bresenhamInitialize (&b, x1, y1, x2, y2);
do {
plot(b.x, b.y);
} while (_bresenhamIterate(&b)==0);

\param b Pointer to struct for bresenham line drawing state.
\param x1 X coordinate of the first point of the line.
\param y1 Y coordinate of the first point of the line.
\param x2 X coordinate of the second point of the line.
\param y2 Y coordinate of the second point of the line.

\returns Returns 0 on success, -1 on failure.
*/
static int _bresenhamInitialize(BresenhamIterator *b, S16 x1, S16 y1, S16 x2, S16 y2)
{
	int temp;
	if (b == NULL)
	{
		return(-1);
	}
	b->x = x1;
	b->y = y1;
	/* dx = abs(x2-x1), s1 = sign(x2-x1) */
	if ((b->dx = x2 - x1) != 0)
	{
		if (b->dx < 0)
		{
			b->dx = -b->dx;
			b->s1 = -1;
		}
		else
		{
			b->s1 = 1;
		}
	}
	else
	{
		b->s1 = 0;
	}
	/* dy = abs(y2-y1), s2 = sign(y2-y1)    */
	if ((b->dy = y2 - y1) != 0)
	{
		if (b->dy < 0)
		{
			b->dy = -b->dy;
			b->s2 = -1;
		}
		else
		{
			b->s2 = 1;
		}
	}
	else
	{
		b->s2 = 0;
	}
	if (b->dy > b->dx)
	{
		temp = b->dx;
		b->dx = b->dy;
		b->dy = temp;
		b->swapdir = 1;
	}
	else
	{
		b->swapdir = 0;
	}
	b->count = (b->dx < 0) ? 0 : (unsigned int)b->dx;
	b->dy <<= 1;
	b->error = b->dy - b->dx;
	b->dx <<= 1;
	return(0);
}

static void _murphyParaline(MurphyIterator *m, int x, int y, int d1)
{
	int p;
	d1 = -d1;
	if (m->dst->bpp == 16)
	{
		U8 r, g, b;
		U16 color;
		EXTRACT_RGB(m->color, r, g, b);
		color = MAKE_RGB565(r, g, b);
		for (p = 0; p <= m->u; p++)
		{
			//check clip windows
			bool invalid = true;
			if (x < m->dst->clip.x)
				invalid = false;
			if (x > m->dst->clip.endx)
				invalid = false;
			if (y < m->dst->clip.y)
				invalid = false;
			if (y > m->dst->clip.endy)
				invalid = false;
			if (invalid)
				*GETPOINT16(m->dst, x, y) = color;
			if (d1 <= m->kt)
			{
				if (m->oct2 == 0)
				{
					x++;
				}
				else
				{
					if (m->quad4 == 0)
					{
						y++;
					}
					else
					{
						y--;
					}
				}
				d1 += m->kv;
			}
			else
			{
				x++;
				if (m->quad4 == 0)
				{
					y++;
				}
				else
				{
					y--;
				}
				d1 += m->kd;
			}
		}
	}
	else
	{
		for (p = 0; p <= m->u; p++)
		{
			//check clip windows
			bool invalid = true;
			if (x < m->dst->clip.x)
				invalid = false;
			if (x > m->dst->clip.endx)
				invalid = false;
			if (y < m->dst->clip.y)
				invalid = false;
			if (y > m->dst->clip.endy)
				invalid = false;
			if (invalid)
				*GETPOINT32(m->dst, x, y) = m->color;
			if (d1 <= m->kt)
			{
				if (m->oct2 == 0)
				{
					x++;
				}
				else
				{
					if (m->quad4 == 0)
					{
						y++;
					}
					else
					{
						y--;
					}
				}
				d1 += m->kv;
			}
			else
			{
				x++;
				if (m->quad4 == 0)
				{
					y++;
				}
				else
				{
					y--;
				}
				d1 += m->kd;
			}
		}

	}
	m->tempx = x;
	m->tempy = y;
}


/*!
\brief Internal function to move Bresenham line iterator to the next position.

Maybe updates the x and y coordinates of the iterator struct.

\param b Pointer to struct for bresenham line drawing state.

\returns Returns 0 on success, 1 if last point was reached, 2 if moving past end-of-line, -1 on failure.
*/
static int _bresenhamIterate(BresenhamIterator *b)
{
	if (b == NULL)
	{
		return (-1);
	}
	/* last point check */
	if (b->count == 0)
	{
		return (2);
	}
	while (b->error >= 0)
	{
		if (b->swapdir)
		{
			b->x += b->s1;
		}
		else
		{
			b->y += b->s2;
		}
		b->error -= b->dx;
	}
	if (b->swapdir)
	{
		b->y += b->s2;
	}
	else
	{
		b->x += b->s1;
	}
	b->error += b->dy;
	b->count--;
	/* count==0 indicates "end-of-line" */
	return ((b->count) ? 0 : 1);
}

/*!
\brief Internal function to to draw one iteration of the Murphy algorithm.

\param m Pointer to struct for murphy iterator.
\param miter Iteration count.
\param ml1bx X coordinate of a point.
\param ml1by Y coordinate of a point.
\param ml2bx X coordinate of a point.
\param ml2by Y coordinate of a point.
\param ml1x X coordinate of a point.
\param ml1y Y coordinate of a point.
\param ml2x X coordinate of a point.
\param ml2y Y coordinate of a point.

*/
static void _murphyIteration(MurphyIterator *m, U8 miter,
	U16 ml1bx, U16 ml1by, U16 ml2bx, U16 ml2by,
	U16 ml1x, U16 ml1y, U16 ml2x, U16 ml2y)
{
	int atemp1, atemp2;
	int ftmp1, ftmp2;
	U16 m1x, m1y, m2x, m2y;
	U16 fix, fiy, lax, lay, curx, cury;
	EGL_POINT pt[4];
	BresenhamIterator b;
	if (miter > 1)
	{
		if (m->first1x != -32768)
		{
			fix = (m->first1x + m->first2x) / 2;
			fiy = (m->first1y + m->first2y) / 2;
			lax = (m->last1x + m->last2x) / 2;
			lay = (m->last1y + m->last2y) / 2;
			curx = (ml1x + ml2x) / 2;
			cury = (ml1y + ml2y) / 2;
			atemp1 = (fix - curx);
			atemp2 = (fiy - cury);
			ftmp1 = atemp1 * atemp1 + atemp2 * atemp2;
			atemp1 = (lax - curx);
			atemp2 = (lay - cury);
			ftmp2 = atemp1 * atemp1 + atemp2 * atemp2;
			if (ftmp1 <= ftmp2)
			{
				m1x = m->first1x;
				m1y = m->first1y;
				m2x = m->first2x;
				m2y = m->first2y;
			}
			else
			{
				m1x = m->last1x;
				m1y = m->last1y;
				m2x = m->last2x;
				m2y = m->last2y;
			}
			atemp1 = (m2x - ml2x);
			atemp2 = (m2y - ml2y);
			ftmp1 = atemp1 * atemp1 + atemp2 * atemp2;
			atemp1 = (m2x - ml2bx);
			atemp2 = (m2y - ml2by);
			ftmp2 = atemp1 * atemp1 + atemp2 * atemp2;
			if (ftmp2 >= ftmp1)
			{
				ftmp1 = ml2bx;
				ftmp2 = ml2by;
				ml2bx = ml2x;
				ml2by = ml2y;
				ml2x = ftmp1;
				ml2y = ftmp2;
				ftmp1 = ml1bx;
				ftmp2 = ml1by;
				ml1bx = ml1x;
				ml1by = ml1y;
				ml1x = ftmp1;
				ml1y = ftmp2;
			}
			_bresenhamInitialize(&b, m2x, m2y, m1x, m1y);
			if (m->dst->bpp == 16)
			{
				U8 r, g, blue;
				U16 color;
				EXTRACT_RGB(m->color, r, g, blue);
				color = MAKE_RGB565(r, g, blue);
				do
				{
					*GETPOINT16(m->dst, b.x, b.y) = color;
				} while (_bresenhamIterate(&b) == 0);
				_bresenhamInitialize(&b, m1x, m1y, ml1bx, ml1by);
				do
				{
					*GETPOINT16(m->dst, b.x, b.y) = color;
				} while (_bresenhamIterate(&b) == 0);
				_bresenhamInitialize(&b, ml1bx, ml1by, ml2bx, ml2by);
				do
				{
					*GETPOINT16(m->dst, b.x, b.y) = color;
				} while (_bresenhamIterate(&b) == 0);
				_bresenhamInitialize(&b, ml2bx, ml2by, m2x, m2y);
				do
				{
					*GETPOINT16(m->dst, b.x, b.y) = color;
				} while (_bresenhamIterate(&b) == 0);
			}
			else if (m->dst->bpp == 32)
			{
				do
				{
					*GETPOINT32(m->dst, b.x, b.y) = m->color;
				} while (_bresenhamIterate(&b) == 0);
				_bresenhamInitialize(&b, m1x, m1y, ml1bx, ml1by);
				do
				{
					*GETPOINT32(m->dst, b.x, b.y) = m->color;
				} while (_bresenhamIterate(&b) == 0);
				_bresenhamInitialize(&b, ml1bx, ml1by, ml2bx, ml2by);
				do
				{
					*GETPOINT32(m->dst, b.x, b.y) = m->color;
				} while (_bresenhamIterate(&b) == 0);
				_bresenhamInitialize(&b, ml2bx, ml2by, m2x, m2y);
				do
				{
					*GETPOINT32(m->dst, b.x, b.y) = m->color;
				} while (_bresenhamIterate(&b) == 0);
			}

			pt[0].x = m1x;
			pt[1].x = m2x;
			pt[2].x = ml1bx;
			pt[3].x = ml2bx;
			pt[0].y = m1y;
			pt[1].y = m2y;
			pt[2].y = ml1by;
			pt[3].y = ml2by;
			{
				SURFACE* old = EGL_SET_TARGET_SURFACE(m->dst);
				r_draw_polygon(pt, 4, m->color);
				EGL_SET_TARGET_SURFACE(old);
			}
		}
	}
	m->last1x = ml1x;
	m->last1y = ml1y;
	m->last2x = ml2x;
	m->last2y = ml2y;
	m->first1x = ml1bx;
	m->first1y = ml1by;
	m->first2x = ml2bx;
	m->first2y = ml2by;
}

/* Windows targets do not have lrint, so provide a local inline version */
#if defined(_MSC_VER)
/* Detect 64bit and use intrinsic version */
#ifdef _M_X64
#include <emmintrin.h>
static INLINE long
lrint(float f)
{
	return _mm_cvtss_si32(_mm_load_ss(&f));
}
#elif defined(_M_IX86)
INLINE long int
lrint(double flt)
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
#error lrint needed for MSVC 
#endif
#endif
#define HYPOT(x,y) sqrt((double)(x)*(double)(x)+(double)(y)*(double)(y))

static void _murphyWideline(MurphyIterator *m, int x1, int y1, int x2, int y2, U8 width, U8 miter)
{
	float offset = (float)width / 2.f;
	int temp;
	int ptx, pty, ptxx, ptxy, ml1x = 0, ml1y = 0, ml2x = 0, ml2y = 0, ml1bx = 0, ml1by = 0, ml2bx = 0, ml2by = 0;
	int d0, d1;		/* difference terms d0=perpendicular to line, d1=along line */
	int q;			/* pel counter,q=perpendicular to line */
	int tmp;
	int dd;			/* distance along line */
	int tk;			/* thickness threshold */
	double ang;		/* angle for initial point calculation */
	double sang, cang;
	/* Initialisation */
	m->u = x2 - x1;	/* delta x */
	m->v = y2 - y1;	/* delta y */
	if (m->u < 0)  	/* swap to make sure we are in quadrants 1 or 4 */
	{
		temp = x1;
		x1 = x2;
		x2 = temp;
		temp = y1;
		y1 = y2;
		y2 = temp;
		m->u *= -1;
		m->v *= -1;
	}
	if (m->v < 0)  	/* swap to 1st quadrant and flag */
	{
		m->v *= -1;
		m->quad4 = 1;
	}
	else
	{
		m->quad4 = 0;
	}
	if (m->v > m->u)  	/* swap things if in 2 octant */
	{
		tmp = m->u;
		m->u = m->v;
		m->v = tmp;
		m->oct2 = 1;
	}
	else
	{
		m->oct2 = 0;
	}
	m->ku = m->u + m->u;	/* change in l for square shift */
	m->kv = m->v + m->v;	/* change in d for square shift */
	m->kd = m->kv - m->ku;	/* change in d for diagonal shift */
	m->kt = m->u - m->kv;	/* diag/square decision threshold */
	d0 = 0;
	d1 = 0;
	dd = 0;
	ang = atan((double)m->v / (double)m->u);	/* calc new initial point - offset both sides of ideal */
	sang = sin(ang);
	cang = cos(ang);
	if (m->oct2 == 0)
	{
		ptx = x1 + (int)lrint(offset * sang);
		if (m->quad4 == 0)
		{
			pty = y1 - (int)lrint(offset * cang);
		}
		else
		{
			pty = y1 + (int)lrint(offset * cang);
		}
	}
	else
	{
		ptx = x1 - (int)lrint(offset * cang);
		if (m->quad4 == 0)
		{
			pty = y1 + (int)lrint(offset * sang);
		}
		else
		{
			pty = y1 - (int)lrint(offset * sang);
		}
	}
	/* used here for constant thickness line */
	tk = (int)(4. * HYPOT(ptx - x1, pty - y1) * HYPOT(m->u, m->v));
	if (miter == 0)
	{
		m->first1x = -32768;
		m->first1y = -32768;
		m->first2x = -32768;
		m->first2y = -32768;
		m->last1x = -32768;
		m->last1y = -32768;
		m->last2x = -32768;
		m->last2y = -32768;
	}
	ptxx = ptx;
	ptxy = pty;
	for (q = 0; dd <= tk; q++)  	/* outer loop, stepping perpendicular to line */
	{
		_murphyParaline(m, ptx, pty, d1);	/* call to inner loop - right edge */
		if (q == 0)
		{
			ml1x = ptx;
			ml1y = pty;
			ml1bx = m->tempx;
			ml1by = m->tempy;
		}
		else
		{
			ml2x = ptx;
			ml2y = pty;
			ml2bx = m->tempx;
			ml2by = m->tempy;
		}
		if (d0 < m->kt)  	/* square move */
		{
			if (m->oct2 == 0)
			{
				if (m->quad4 == 0)
				{
					pty++;
				}
				else
				{
					pty--;
				}
			}
			else
			{
				ptx++;
			}
		}
		else  	/* diagonal move */
		{
			dd += m->kv;
			d0 -= m->ku;
			if (d1 < m->kt)  	/* normal diagonal */
			{
				if (m->oct2 == 0)
				{
					ptx--;
					if (m->quad4 == 0)
					{
						pty++;
					}
					else
					{
						pty--;
					}
				}
				else
				{
					ptx++;
					if (m->quad4 == 0)
					{
						pty--;
					}
					else
					{
						pty++;
					}
				}
				d1 += m->kv;
			}
			else  	/* double square move, extra parallel line */
			{
				if (m->oct2 == 0)
				{
					ptx--;
				}
				else
				{
					if (m->quad4 == 0)
					{
						pty--;
					}
					else
					{
						pty++;
					}
				}
				d1 += m->kd;
				if (dd > tk)
				{
					_murphyIteration(m, miter, ml1bx, ml1by, ml2bx, ml2by, ml1x, ml1y, ml2x, ml2y);
					return;	/* breakout on the extra line */
				}
				_murphyParaline(m, ptx, pty, d1);
				if (m->oct2 == 0)
				{
					if (m->quad4 == 0)
					{
						pty++;
					}
					else
					{
						pty--;
					}
				}
				else
				{
					ptx++;
				}
			}
		}
		dd += m->ku;
		d0 += m->kv;
	}
	_murphyIteration(m, miter, ml1bx, ml1by, ml2bx, ml2by, ml1x, ml1y, ml2x, ml2y);
}

void r_draw_thickline(int x1, int y1, int x2, int y2, U8 width, EGL_COLOR color)
{

	int wh;
	MurphyIterator m;
	SURFACE* dst = EGL_GET_TARGET_SURFACE();
	if (dst == NULL) return;
	if (width < 1) return;
	/* Special case: thick "point" */
	if ((x1 == x2) && (y1 == y2))
	{
		wh = width / 2;
		r_draw_rectfill(x1 - wh, y1 - wh, width, width, color);
		return;
	}

	m.dst = dst;
	m.color = color;
	_murphyWideline(&m, x1, y1, x2, y2, width, 0);
}


void draw_thickline(int x1, int y1, int x2, int y2, U8 width, EGL_COLOR color)
{
	if (width == 1)
	{
		r_draw_line(x1, y1, x2, y2, color);
		return;
	}
	r_draw_thickline(x1, y1, x2, y2, width, color);
	SURFACE_FLUSH();
}
