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
#include <string.h>
#include <stdlib.h>
#include "primitives/draw_line.h"
#include "primitives/draw_poly.h"
#include "egl_private.h"

#ifdef HW_2D_ENGINE
static BOOL b_force_sw_mode = FALSE;
#else
static BOOL b_force_sw_mode = TRUE;
#endif
static SURFACE* swtarget=NULL;

#define HAVE_FLOAT 1
#define swap(a,b) do { a ^= b; b ^= a; a ^= b; } while (0)

typedef struct {
	int     x1, y1, x2, y2;
#if HAVE_FLOAT
	double  x, m;
#else
	int     cx, fn, mn, d;
#endif
} edge_t;

static int edge_cmp(const void *lvp, const void *rvp)
{
	/* convert from void pointers to structure pointers */
	const edge_t *lp = (const edge_t *)lvp;
	const edge_t *rp = (const edge_t *)rvp;

	/* if the minimum y values are different, sort on minimum y */
	if (lp->y1 != rp->y1)
		return lp->y1 - rp->y1;

	/* if the current x values are different, sort on current x */
#if HAVE_FLOAT
	if (lp->x < rp->x)
		return -1;
	else if (lp->x > rp->x)
		return +1;
#else
	if (lp->cx != rp->cx)
		return lp->cx - rp->cx;
#endif

	/* otherwise they are equal */
	return 0;
}

void r_draw_polygonfill(EGL_POINT* pointtable,int count,EGL_COLOR c)
{
	edge_t *get;		/* global edge table */
	int     nge = 0;	/* num global edges */
	int     cge = 0;	/* cur global edge */

	edge_t *aet;		/* active edge table */
	int     nae = 0;	/* num active edges */

	int     i, y;

	if (count < 3) {
		/* error, polygons require at least three edges (a triangle) */
		return;
	}
#ifdef _MSC_VER
	get = (edge_t *) calloc(count, sizeof(edge_t));
	aet = (edge_t *) calloc(count, sizeof(edge_t));
	if ((get == 0) || (aet == 0)) {
		/* error, couldn't allocate one or both of the needed tables */
		if (get)
			free(get);
		if (aet)
			free(aet);
		return;
	}
#else
	//use local array , C99
	edge_t sget[count],saet[count];
	get = sget;
	aet = saet;
	memset(get,0,count*sizeof(edge_t));
	memset(aet,0,count*sizeof(edge_t));
#endif

	/* setup the global edge table */
	for (i = 0; i < count; ++i) {
		get[nge].x1 = pointtable[i].x;
		get[nge].y1 = pointtable[i].y;
		get[nge].x2 = pointtable[(i + 1) % count].x;
		get[nge].y2 = pointtable[(i + 1) % count].y;
		if (get[nge].y1 != get[nge].y2) {
			if (get[nge].y1 > get[nge].y2) {
				swap(get[nge].x1, get[nge].x2);
				swap(get[nge].y1, get[nge].y2);
			}
#if HAVE_FLOAT
			get[nge].x = get[nge].x1;
			get[nge].m = get[nge].x2 - get[nge].x1;
			get[nge].m /= get[nge].y2 - get[nge].y1;
#else
			get[nge].cx = get[nge].x1;
			get[nge].mn = get[nge].x2 - get[nge].x1;
			get[nge].d = get[nge].y2 - get[nge].y1;
			get[nge].fn = get[nge].mn / 2;
#endif
			++nge;
		}
	}

	qsort(get, nge, sizeof(get[0]), edge_cmp);

	/* start with the lowest y in the table */
	y = get[0].y1;

	do {

		/* add edges to the active table from the global table */
		while ((nge > 0) && (get[cge].y1 == y)) {
			aet[nae] = get[cge++];
			--nge;
			aet[nae++].y1 = 0;
		}

		qsort(aet, nae, sizeof(aet[0]), edge_cmp);

		/* using odd parity, render alternating line segments */
		for (i = 1; i < nae; i += 2) {
#if HAVE_FLOAT
			int     l = (int)aet[i - 1].x;
			int     r = (int)aet[i].x;
#else
			int     l = (int)aet[i - 1].cx;
			int     r = (int)aet[i].cx;
#endif
			if (r > l)
			{
				if(b_force_sw_mode)
					sw_draw_hline(swtarget,l,y,r,c);
				else
					r_draw_hline(l,y,r,c);
			}
		}

		/* prepare for the next scan line */
		y++;

		/* remove inactive edges from the active edge table */
		/* or update the current x position of active edges */
		for (i = 0; i < nae; ++i) {
			if (aet[i].y2 == y)
				aet[i--] = aet[--nae];
			else {
#if HAVE_FLOAT
				aet[i].x += aet[i].m;
#else
				aet[i].fn += aet[i].mn;
				if (aet[i].fn < 0) {
					aet[i].cx += aet[i].fn / aet[i].d - 1;
					aet[i].fn %= aet[i].d;
					aet[i].fn += aet[i].d;
				}
				if (aet[i].fn >= aet[i].d) {
					aet[i].cx += aet[i].fn / aet[i].d;
					aet[i].fn %= aet[i].d;
				}
#endif
			}
		}

		/* keep doing this while there are any edges left */
	} while ((nae > 0) || (nge > 0));

#ifdef _MSC_VER
	/* all done, free the edge tables */
	free(get);
	free(aet);
#endif
}

void draw_polygonfill(EGL_POINT* pointtable,int count,EGL_COLOR c)
{
	r_draw_polygonfill(pointtable,count,c);
	SURFACE_FLUSH();
}

void sw_draw_polygonfill(SURFACE* dst,EGL_POINT* pointtable,int count,EGL_COLOR c)
{
	b_force_sw_mode = TRUE;
	swtarget = dst;
	if(swtarget == NULL)
	{
		b_force_sw_mode = FALSE;
		return;
	}
	r_draw_polygonfill(pointtable,count,c);
	SURFACE_FLUSH();
	b_force_sw_mode = FALSE;
}
