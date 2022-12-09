/******************************************************************************
 * Copyright (C) 2013      Advanced Digital Chips Inc. 
 *						http://www.adc.co.kr
 * Author : Software Team.
 ******************************************************************************/

#include "egl_typedef.h"
#include "egl_base.h"
#include "egl_private.h"
#include "object/egl_circle_gauge.h"
#include "egl_theme.h"
#include "primitives/draw_rect.h"
#include "util/fixed_sincos.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#  ifndef M_PI
#    define M_PI            3.14159265358979323846f
#    define M_PI_2          1.57079632679489661923f
#    define M_PI_4          0.78539816339744830962f
#    define M_1_PI          0.31830988618379067154f
#    define M_2_PI          0.63661977236758134308f
#  endif // !M_PI

static int value2angle( EGL_CIRCLE_GAUGE_DATA* pData, int value )
{
	int newangle;
	int percent = ( value * 100 ) / ( pData->max_value - pData->min_value );
	newangle = ( pData->max_angle - pData->min_angle ) * percent;
	newangle /= 100;
	return ( pData->min_angle + newangle );
}
static void r_circle_draw( EGL_OBJECT_PTR pObj )
{
	int x;
	int y;
	int n;
	EGL_POINT pt[4];
	EGL_CIRCLE_GAUGE_DATA* gauge ;
	gauge = ( EGL_CIRCLE_GAUGE_DATA* )pObj->selfdata;
	if(gauge->bg!=NULL)
		DRAW_SURFACE( gauge->bg, pObj->rect.x, pObj->rect.y );
	//draw needle
	//n = gauge->needle_length/10;
	n = 5;
	x = pObj->rect.x + gauge->needle_x;
	y = pObj->rect.y + gauge->needle_y;
	pt[0].x = x;
	pt[0].y = y;
	pt[1].x = pt[0].x - n;
	pt[1].x = x + ( ( n * egl_icos(gauge->cur_angle + 180 + 45 ) ) / 1024 );; //+180 for clockwise
	pt[1].y = y + ( ( n * egl_isin( gauge->cur_angle + 180 + 45 )  ) / 1024 );
	pt[2].x = x + ( ( gauge->needle_length * egl_icos( gauge->cur_angle + 180 ) ) / 1024 );; //+180 for clockwise
	pt[2].y = y + ( ( gauge->needle_length * egl_isin( gauge->cur_angle + 180 ) ) / 1024 );
	pt[3].x = x + ( ( n * egl_icos( gauge->cur_angle + 180 - 45 ) ) / 1024 );; //+180 for clockwise
	pt[3].y = y + ( ( n * egl_isin( gauge->cur_angle + 180 - 45 ) ) / 1024 );
	DRAW_POLYGONFILL( pt, 4, MAKE_COLORREF( 255, 0, 0 ) );
}
static void circle_move(EGL_OBJECT_PTR pObj,int x, int y )
{
	int offset_x,offset_y;
	offset_x = x-pObj->rect.x;
	offset_y = y-pObj->rect.y;
	pObj->rect.x += offset_x;
	pObj->rect.y += offset_y;
}

static void circle_draw( EGL_OBJECT_PTR pObj )
{
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1  
	SURFACE* oldtarget;
	int org_x,org_y;
	oldtarget = EGL_SET_TARGET_SURFACE(pObj->selfsurface);
	org_x = pObj->rect.x;
	org_y = pObj->rect.y;
	circle_move(pObj,0,0);
#endif
	r_circle_draw(pObj);
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
	EGL_SET_TARGET_SURFACE(oldtarget);
	circle_move(pObj,org_x,org_y);
	DRAW_SURFACE(pObj->selfsurface, pObj->rect.x, pObj->rect.y);
#endif

}
INLINE static void circle_delete( EGL_OBJECT_PTR pObj )
{
	free( pObj->selfdata );
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
	if(pObj->selfsurface!=NULL)
		RELEASE_SURFACE(pObj->selfsurface);
#endif
	prv_egl_delete_object( pObj );
}
static void* circle_msg_handler( EGL_MSG* pMsg )
{
	switch ( pMsg->msgID )
	{
	case EGL_MSG_DRAW:
		circle_draw( ( EGL_OBJECT_PTR )pMsg->hObj );
		break;
	case EGL_MSG_DELETE:
		circle_delete( ( EGL_OBJECT_PTR )pMsg->hObj );
		break;
	case EGL_MSG_MOVE:
		circle_move( ( EGL_OBJECT_PTR )pMsg->hObj,pMsg->param.point.x,pMsg->param.point.y );
		break;
	default:
		break;
	}
	return NULL;
}


EGL_HANDLE egl_create_circle_gauge( SURFACE* bg, int x, int y, EGL_CIRCLE_GAUGE_INFO* pInfo )
{
	EGL_OBJECT_PTR h;
	EGL_CIRCLE_GAUGE_DATA* gauge;
	h = prv_egl_create_object( x, y, bg->w, bg->h );
	if(h==NULL)
		return NULL;
	gauge = ( EGL_CIRCLE_GAUGE_DATA* )malloc( sizeof( EGL_CIRCLE_GAUGE_DATA ) );
	if(gauge==NULL)
	{
		prv_egl_delete_object(h);
		return NULL;
	}
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
	h->selfsurface = CREATE_SURFACE(bg->w,bg->h,bg->bpp);
	h->selfsurface->pixtype = bg->pixtype;		
	if(bg->pixtype == PIX_FMT_ARGB)
	{		
		memset(h->selfsurface->pixels,0,bg->w*bg->h*4);
	}	
	
	if(h->selfsurface ==NULL)
	{
		free(gauge);
		prv_egl_delete_object(h);
		return NULL;
	}
#endif
	gauge->needle_x = pInfo->needle_x;
	gauge->needle_y = pInfo->needle_y;
	gauge->needle_length = pInfo->needle_length;
	gauge->needle_thick = pInfo->needle_thick;
	gauge->min_value = pInfo->min_value;
	gauge->max_value = pInfo->max_value;
	gauge->min_angle = pInfo->min_angle;
	gauge->max_angle = pInfo->max_angle;
	gauge->cur_value = 0;
	gauge->cur_angle = pInfo->min_angle;
	gauge->bg = bg;
	h->selfdata = gauge;
	h->msg_hanlder = circle_msg_handler;

	return ( EGL_HANDLE )h;
}

BOOL egl_circle_gauge_set_value( EGL_HANDLE h, int value )
{
	EGL_CIRCLE_GAUGE_DATA* pData ;
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )h;
	pData = ( EGL_CIRCLE_GAUGE_DATA* )pObj->selfdata;
	if(pData->cur_value==value)
		return TRUE;
	if( value > pData->max_value )
		pData->cur_value = pData->max_value;
	else
		pData->cur_value = value;
	//value to angle;
	pData->cur_angle = value2angle( pData, pData->cur_value );
	CRITICAL_ENTER();
	pObj->bNeedRedraw = TRUE;
	CRITICAL_EXIT();
	return TRUE;
}
int egl_circle_gauge_get_value( EGL_HANDLE h )
{
	EGL_CIRCLE_GAUGE_DATA* pData ;
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )h;
	pData = ( EGL_CIRCLE_GAUGE_DATA* )pObj->selfdata;
	return pData->cur_value;
}

