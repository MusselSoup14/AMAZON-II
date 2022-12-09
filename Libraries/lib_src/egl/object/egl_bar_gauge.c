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
#include "object/egl_bar_gauge.h"
#include "egl_theme.h"
#include "primitives/draw_rect.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void draw_pointer( SURFACE* dst, int center_x, int center_y, int rad, EGL_COLOR c, BOOL bVertical )
{
#ifndef HW_2D_ENGINE
	SURFACE* old = EGL_SET_TARGET_SURFACE(dst);
#endif
	//draw pointer
	EGL_POINT p[3];
	rad--;
	if( bVertical )
	{
		p[0].x = center_x - rad;
		p[0].y = center_y - rad ;
		p[1].x = center_x ;
		p[1].y = center_y + rad ;
		p[2].x = center_x + rad ;
		p[2].y = center_y - rad ;
#ifdef HW_2D_ENGINE
		sw_draw_polygonfill(dst,p,3,c);
#else
		DRAW_POLYGONFILL( p, 3, c );
#endif
		p[0].x--;
		p[0].y--;
		p[1].y++;
		p[2].x++;
		p[2].y--;
#ifdef HW_2D_ENGINE
		sw_draw_polygon(dst,p,3,MAKE_COLORREF(0,0,0));
#else
		DRAW_POLYGON( p, 3, MAKE_COLORREF(0,0,0) );
#endif
	}
	else
	{
		p[0].x = center_x - rad;
		p[0].y = center_y - rad ;
		p[1].x = center_x + rad;
		p[1].y = center_y ;
		p[2].x = center_x - rad ;
		p[2].y = center_y + rad ;
#ifdef HW_2D_ENGINE
		sw_draw_polygonfill(dst,p,3,c);
#else
		DRAW_POLYGONFILL( p, 3, c );
#endif
		p[0].x--;
		p[0].y--;
		p[1].x++;
		p[2].x--;
		p[2].y++;
#ifdef HW_2D_ENGINE
		sw_draw_polygon(dst,p,3,MAKE_COLORREF(0,0,0));
#else
		DRAW_POLYGON( p, 3, MAKE_COLORREF(0,0,0) );//black
#endif
	}
#ifndef HW_2D_ENGINE
	EGL_SET_TARGET_SURFACE(old);
#endif
}


static SURFACE* make_bar_pointer_surface(EGL_OBJECT_PTR pObj)
{
	int margin;
	int rad;
	EGL_BAR_GAUGE_DATA* gauge;
	SURFACE* surf;
	gauge = ( EGL_BAR_GAUGE_DATA* )pObj->selfdata;
	margin = gauge->margin;
	rad = margin - 1;
	gauge->pointer_size = margin*2;
	surf = CREATE_SURFACE(margin*2,margin*2,32);
	surf->pixtype = PIX_FMT_ARGB;
	memset(surf->pixels,0,surf->pitch*surf->h);
	if(surf ==NULL)
		return NULL;
	draw_pointer( surf,rad, rad,rad , MAKE_COLORREF( 205, 92, 92 ), gauge->bVertical==TRUE?FALSE:TRUE );
	
	return surf;
}

static void draw_bar_pointer(EGL_OBJECT_PTR pObj)
{
	int x;
	int y;
	int margin;
	EGL_BAR_GAUGE_DATA* gauge;
	gauge = ( EGL_BAR_GAUGE_DATA* )pObj->selfdata;
	margin = gauge->margin;
	gauge->pointer_size = margin*2;
	if( gauge->bVertical )
	{
		x = pObj->rect.x + margin/2-1;
		y = pObj->rect.y + pObj->rect.h - margin - ((gauge->ppv*gauge->cur_value)/10000) - gauge->pointer_size/2;
	}
	else
	{
		x = pObj->rect.x + margin + (gauge->ppv*gauge->cur_value)/10000 - gauge->pointer_size/2;
		y = pObj->rect.y + margin/2;
	}
	DRAW_SURFACE(gauge->surf_pointer,x,y);
}

static void make_bar_bg( EGL_OBJECT_PTR pObj )
{
	int x;
	int y;
	int ppt;//pixel per tick
	int tickmark = 0;
	int margin;
	char text[16];
	EGL_COLOR tickcolor;
	EGL_BAR_GAUGE_DATA* gauge;
	gauge = ( EGL_BAR_GAUGE_DATA* )pObj->selfdata;
	tickcolor = MAKE_COLORREF( 112, 128, 144 );
	margin = gauge->margin;
	if( gauge->bg == NULL )
	{
		//drawbox
		DRAW_RECT( pObj->rect.x, pObj->rect.y, pObj->rect.w, pObj->rect.h, MAKE_COLORREF( 177, 197, 222 ) );
		if( gauge->bVertical )
		{
			DRAW_RECTFILLGRADIENT( pObj->rect.x + 1, pObj->rect.y + 1, pObj->rect.w - 2, pObj->rect.h - 2, MAKE_COLORREF( 177, 197, 222 ), MAKE_COLORREF( 251, 252, 253 ), TRUE );
			x = pObj->rect.x + margin;//left/top margin
			y = pObj->rect.y + pObj->rect.h - margin;
			//*1024 is for resolution
			ppt = ( pObj->rect.h - ( margin * 2 ) ) * 1024 / ( ( gauge->max_value - gauge->min_value ) / gauge->tikcmarkstep );
			y *= 1024;
			for( ; tickmark <= gauge->max_value; y -= ppt, tickmark += gauge->tikcmarkstep )
			{
				if( ( tickmark % gauge->tikcmarkmajorstep ) == 0 )
				{
					DRAW_HLINE( x, y / 1024, x + ( gauge->tickmarksize * 2 ), tickcolor );
					sprintf( text, "%d", tickmark );
					draw_text(pObj->pFont, x + ( gauge->tickmarksize * 2 ) + 5, y / 1024 + 1 - ( pObj->pFont->h / 2 ), text );
				}
				else
					DRAW_HLINE( x, y / 1024, x + gauge->tickmarksize, tickcolor );
			}
		}
		else
		{
			DRAW_RECTFILLGRADIENT( pObj->rect.x + 1, pObj->rect.y + 1, pObj->rect.w - 2, pObj->rect.h - 2, MAKE_COLORREF( 251, 252, 253 ), MAKE_COLORREF( 177, 197, 222 ), FALSE );
			sprintf( text, "%d", gauge->max_value );
			x = pObj->rect.x + margin;//left/top margin
			y = pObj->rect.y + margin;
			ppt = ( pObj->rect.w - ( 2 * margin ) ) * 1024 / ( ( gauge->max_value - gauge->min_value ) / gauge->tikcmarkstep );
			x *= 1024;
			for( ; tickmark <= gauge->max_value; x += ppt, tickmark += gauge->tikcmarkstep )
			{
				if( ( tickmark % gauge->tikcmarkmajorstep ) == 0 )
				{
					DRAW_VLINE( x / 1024, y, y + ( gauge->tickmarksize * 2 ), tickcolor );
					sprintf( text, "%d", tickmark );
					draw_text(pObj->pFont, x / 1024 + 1 - ( text_width( pObj->pFont, text ) / 2 ), y + ( gauge->tickmarksize * 2 ) + 5, text );
				}
				else
					DRAW_VLINE( x / 1024, y, y + gauge->tickmarksize, tickcolor );
			}
		}
	}
}
//#undef EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE
//#define  EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE 0
static void bar_draw( EGL_OBJECT_PTR pObj )
{
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
	DRAW_SURFACE( pObj->selfsurface, pObj->rect.x, pObj->rect.y );
#else
	make_bar_bg( pObj );
#endif
	draw_bar_pointer(pObj);
}

INLINE static void egl_delete_bar_gauge( EGL_OBJECT_PTR gauge )
{
	EGL_BAR_GAUGE_DATA* gauge_data = (EGL_BAR_GAUGE_DATA*)gauge->selfdata;
	free(gauge_data->surf_pointer);
	free(gauge->selfdata);
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
	{
		if(gauge->selfsurface)
			RELEASE_SURFACE(gauge->selfsurface );
	}
#endif
	prv_egl_delete_object( gauge );
}

static void* bar_msg_handler( EGL_MSG* pMsg )
{
	EGL_OBJECT_PTR gauge = ( EGL_OBJECT_PTR )pMsg->hObj;
	switch ( pMsg->msgID )
	{
	case EGL_MSG_DRAW:
		bar_draw( ( EGL_OBJECT_PTR )pMsg->hObj );
		break;
	case EGL_MSG_MOVE:
		gauge->rect.x = pMsg->param.point.x;
		gauge->rect.y = pMsg->param.point.y;
		gauge->bNeedRedraw = TRUE;
		break;
	case EGL_MSG_DELETE:
		egl_delete_bar_gauge( gauge );
		break;
	default:
		break;
	}
	return NULL;
}

EGL_HANDLE egl_create_bar_gauge( int x, int y, int w, int h, EGL_BAR_GAUGE_INFO* pInfo )
{
	EGL_OBJECT_PTR handle ;
	EGL_BAR_GAUGE_DATA* gauge_data ;
	if( pInfo->bg != NULL )
		handle = prv_egl_create_object( x, y, pInfo->bg->w, pInfo->bg->h );
	else
		handle = prv_egl_create_object( x, y, w, h );
	if( handle == NULL )
		return NULL;
	gauge_data = ( EGL_BAR_GAUGE_DATA* )malloc( sizeof( EGL_BAR_GAUGE_DATA ) );
	if(gauge_data==NULL)
	{
		prv_egl_delete_object(handle);
		return NULL;
	}
	gauge_data->min_value = pInfo->min_value;
	gauge_data->max_value = pInfo->max_value;
	gauge_data->cur_value = 0;
	gauge_data->tickmarksize = pInfo->tickmarksize;
	gauge_data->tikcmarkmajorstep = pInfo->tickmarkmajorstep;
	gauge_data->tikcmarkstep = pInfo->tickmarkstep;
	gauge_data->bVertical = pInfo->bVertical;
	gauge_data->bg = pInfo->bg;
	handle->selfdata = gauge_data;
	handle->msg_hanlder = bar_msg_handler;

	if( gauge_data->bVertical )
	{
		gauge_data->margin = handle->pFont->h / 2 + 2;
		gauge_data->ppv = ( handle->rect.h - ( gauge_data->margin * 2 ) ) * 10000 /  ( gauge_data->max_value - gauge_data->min_value );
	}
	else
	{
		char text[16];
		sprintf( text, "%d", gauge_data->max_value );
		gauge_data->margin = text_width( handle->pFont, text ) / 2 + 1;
		gauge_data->ppv = ( handle->rect.w - ( 2 * gauge_data->margin ) ) * 10000/  ( gauge_data->max_value - gauge_data->min_value ) ;
	}

	gauge_data->surf_pointer = make_bar_pointer_surface(handle);
	if(gauge_data->surf_pointer==NULL)
	{
		free( gauge_data );
		prv_egl_delete_object( handle );
		return NULL;
	}

#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
	{
		SURFACE* oldtarget;
		handle->selfsurface = CREATE_SURFACE( w, h, EGL_SCREEN_BPP );
		if( handle->selfsurface == NULL )
		{
			RELEASE_SURFACE(gauge_data->surf_pointer);
			free( gauge_data );
			prv_egl_delete_object( handle );
			return FALSE;
		}
		handle->rect.x = handle->rect.y = 0;
		oldtarget = EGL_SET_TARGET_SURFACE( handle->selfsurface );
		make_bar_bg( handle );
		EGL_SET_TARGET_SURFACE( oldtarget );
		handle->rect.x = x;
		handle->rect.y = y;
	}
#endif
	return ( EGL_HANDLE )handle;
}
BOOL egl_bar_gauge_set_value( EGL_HANDLE h, int value )
{
	EGL_BAR_GAUGE_DATA* gauge;
	EGL_OBJECT_PTR pObj;
	pObj = ( EGL_OBJECT_PTR )h;
	gauge = ( EGL_BAR_GAUGE_DATA* )pObj->selfdata;
	if(gauge->cur_value==value)
		return TRUE;
	if( value < gauge->min_value )
		return FALSE;
	if( value > gauge->max_value )
		return FALSE;
	CRITICAL_ENTER();
	gauge->cur_value = value;
	pObj->bNeedRedraw = TRUE;
	CRITICAL_EXIT();
	return TRUE;
}
int egl_bar_gauge_get_value( EGL_HANDLE h )
{
	EGL_BAR_GAUGE_DATA* gauge;
	EGL_OBJECT_PTR pObj;
	pObj = ( EGL_OBJECT_PTR )h;
	CRITICAL_ENTER();
	gauge = ( EGL_BAR_GAUGE_DATA* )pObj->selfdata;
	CRITICAL_EXIT();
	return gauge->cur_value;
}

BOOL egl_release_bar_gauge(EGL_HANDLE hObj )
{
	if( hObj == NULL )
		return FALSE;
	egl_delete_bar_gauge( (EGL_OBJECT_PTR)hObj );	
	return TRUE;
}
