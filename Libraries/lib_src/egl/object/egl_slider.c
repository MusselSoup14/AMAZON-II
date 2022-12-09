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
#include "egl_window.h"
#include "egl_private.h"
#include "egl_theme.h"
#include <stdlib.h>
#include "object/egl_label.h"
#include "object/egl_slider.h"


static void r_make_slr( EGL_OBJECT_PTR slr )
{
	SLR_DATA* slrdata = ( SLR_DATA* )slr->selfdata;
	int i = 0;
	int total_stepsize = 0;
	if( slrdata->m_bVerticalMode )
	{
		slrdata->m_bar_box_x = slr->rect.x + ( slr->rect.w / 2 ) - ( slrdata -> m_bar_box_w / 2 );
		slrdata->m_bar_box_y = slr->rect.y + slrdata->m_thumb_box_h / 2;
	}
	else
	{
		slrdata->m_bar_box_x = slr->rect.x + slrdata->m_thumb_box_w / 2;
		slrdata->m_bar_box_y = slr->rect.y + ( slr->rect.h / 2 ) - ( slrdata->m_bar_box_h / 2 );
	}
	if( slrdata->m_colorless )
		DRAW_RECTFILL( slr->rect.x, slr->rect.y, slr->rect.w, slr->rect.h, 0xffffff );
	else
		DRAW_RECTFILL( slr->rect.x, slr->rect.y, slr->rect.w, slr->rect.h, EGL_CONFIG_WINDOW_DEFAULT_BG_COLOR );
	DRAW_RECTFILL( slrdata->m_bar_box_x, slrdata->m_bar_box_y, slrdata->m_bar_box_w, slrdata->m_bar_box_h, slrdata->m_bar_color );
	if( slrdata->m_bVerticalMode )
	{
		int y = slrdata->m_bar_box_y;
		while( y + total_stepsize < slrdata->m_bar_box_y + slrdata->m_bar_box_h )
		{
			if( slrdata->m_tickstyle == TICK_TOPLEFT || slrdata->m_tickstyle == TICK_BOTH )
				DRAW_HLINE( slr->rect.x, y + total_stepsize, slr->rect.x + 8, slrdata->m_tick_color );
			if( slrdata->m_tickstyle == TICK_BOTTOMRIGHT || slrdata->m_tickstyle == TICK_BOTH )
				DRAW_HLINE( slr->rect.x + slr->rect.w - 8, y + total_stepsize, slr->rect.x + slr->rect.w, slrdata->m_tick_color );
			i++;
			total_stepsize = ( slrdata->m_stepsize * slrdata->m_tickfrequency * i / 1024 );
		}
		if( slrdata->m_tickstyle == TICK_TOPLEFT || slrdata->m_tickstyle == TICK_BOTH )
			DRAW_HLINE( slr->rect.x, slrdata->m_bar_box_y + slrdata->m_bar_box_h - 1, slr->rect.x + 8, slrdata->m_tick_color );
		if( slrdata->m_tickstyle == TICK_BOTTOMRIGHT || slrdata->m_tickstyle == TICK_BOTH )
			DRAW_HLINE( slr->rect.x + slr->rect.w - 8, slrdata->m_bar_box_y + slrdata->m_bar_box_h - 1, slr->rect.x + slr->rect.w, slrdata->m_tick_color );
	}
	else
	{
		int x = slrdata->m_bar_box_x;
		while( x + total_stepsize < slrdata->m_bar_box_x + slrdata->m_bar_box_w )
		{
			if( slrdata->m_tickstyle == TICK_TOPLEFT || slrdata->m_tickstyle == TICK_BOTH )
				DRAW_VLINE( x + total_stepsize, slr->rect.y, slr->rect.y + 8, slrdata->m_tick_color );
			if( slrdata->m_tickstyle == TICK_BOTTOMRIGHT || slrdata->m_tickstyle == TICK_BOTH )
				DRAW_VLINE( x + total_stepsize, slr->rect.y + slr->rect.h - 8, slr->rect.y + slr->rect.h, slrdata->m_tick_color );
			i++;
			total_stepsize = ( slrdata->m_stepsize * slrdata->m_tickfrequency * i / 1024 );
		}
		if( slrdata->m_tickstyle == TICK_TOPLEFT || slrdata->m_tickstyle == TICK_BOTH )
			DRAW_VLINE( slrdata->m_bar_box_x + slrdata->m_bar_box_w - 1, slr->rect.y, slr->rect.y + 8, slrdata->m_tick_color );
		if( slrdata->m_tickstyle == TICK_BOTTOMRIGHT || slrdata->m_tickstyle == TICK_BOTH )
			DRAW_VLINE( slrdata->m_bar_box_x + slrdata->m_bar_box_w - 1, slr->rect.y + slr->rect.h - 8, slr->rect.y + slr->rect.h, slrdata->m_tick_color );
	}
	if( slrdata->m_bVerticalMode )
	{
		theme_draw_scroll_thumb( slr->rect.x + slrdata->m_thumb_box_x, slr->rect.y + slrdata->m_thumb_box_y, slrdata->m_thumb_box_w, slrdata->m_thumb_box_h, slrdata->m_bVerticalMode, TRUE );
	}
	else
	{
		theme_draw_scroll_thumb( slr->rect.x + slrdata->m_thumb_box_x, slr->rect.y + slrdata->m_thumb_box_y, slrdata->m_thumb_box_w, slrdata->m_thumb_box_h, slrdata->m_bVerticalMode, TRUE );
	}
}

static void draw( EGL_OBJECT_PTR slr )
{
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
	SLR_DATA* slrdata = ( SLR_DATA* )slr->selfdata;
	SURFACE* oldtarget;
	int old_bar_box_x = slrdata->m_bar_box_x;
	int old_bar_box_y = slrdata->m_bar_box_y;
	//int old_thumb_box_x = slrdata->m_thumb_box_x;
	//int old_thumb_box_y = slrdata->m_thumb_box_y;
	int old_x = slr->rect.x;
	int old_y = slr->rect.y;
	slr->rect.x = slr->rect.y = 0;
	//slrdata->m_thumb_box_x = slr->rect.x + ( slr->rect.w / 2 ) - ( slrdata->m_thumb_box_w / 2 );
	//slrdata->m_thumb_box_y = slr->rect.y;
	if( slrdata->m_bVerticalMode )
	{
		slrdata->m_bar_box_x = slr->rect.x + ( slr->rect.w / 2 ) - ( slrdata -> m_bar_box_w / 2 );
		slrdata->m_bar_box_y = slr->rect.y + slrdata->m_thumb_box_h / 2;
	}
	else
	{
		slrdata->m_bar_box_x = slr->rect.x + slrdata->m_thumb_box_w / 2;
		slrdata->m_bar_box_y = slr->rect.y + ( slr->rect.h / 2 ) - ( slrdata->m_bar_box_h / 2 );
	}
	oldtarget = EGL_SET_TARGET_SURFACE( slr->selfsurface );
	r_make_slr( slr );
	EGL_SET_TARGET_SURFACE( oldtarget );
	slr->rect.x = old_x;
	slr->rect.y = old_y;
	slrdata->m_bar_box_x = old_bar_box_x;
	slrdata->m_bar_box_y = old_bar_box_y;
	//slrdata->m_thumb_box_x = old_thumb_box_x;
	//slrdata->m_thumb_box_y = old_thumb_box_y;
	DRAW_SURFACE( slr->selfsurface, slr->rect.x, slr->rect.y );
#else
	r_make_slr( slr );
#endif
}

static SLR_DATA* create_selfdata()
{
	SLR_DATA* slrdata = ( SLR_DATA* )malloc( sizeof( SLR_DATA ) );
	return slrdata;
}
static void delete_selfdata( void* selfdata )
{	
	if( selfdata )
		free( selfdata );
}

static void egl_delete_slider( EGL_HANDLE hObj )
{
	EGL_OBJECT_PTR pObj;
	if( hObj == NULL )
		return ;
	pObj = ( EGL_OBJECT_PTR )hObj;
	delete_selfdata( pObj->selfdata );
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
	if( pObj->selfsurface )
		RELEASE_SURFACE( pObj->selfsurface );
#endif
	prv_egl_delete_object( pObj );
}

static void* slr_msg_hanlder( EGL_MSG* pMsg )
{
	EGL_OBJECT_PTR slr = ( EGL_OBJECT_PTR )pMsg->hObj;
	SLR_DATA* slrdata = ( SLR_DATA* )slr->selfdata;
	EGL_POINT point = pMsg->param.point;
	int x, y;
	switch ( pMsg->msgID )
	{
	case EGL_MSG_DRAW:
		draw( slr );
		break;
	case EGL_MSG_FOCUS:
		break;
	case EGL_MSG_UNFOCUS:
	case EGL_MSG_KEY_UP:
		break;
	case EGL_MSG_KEY_DOWN:
		break;
	case EGL_MSG_TOUCHED:
		break;
	case EGL_MSG_UNTOUCHED:
		if( slrdata->m_colorless )
			egl_window_invalidate_rect( &slr->rect );
		x = point.x;
		y = point.y;
		if( slrdata->m_bVerticalMode )
		{
			if( y < slr->rect.y + slrdata->m_thumb_box_y )
				egl_slider_stepit( ( EGL_HANDLE )pMsg->hObj, FALSE );
			else if( y > slr->rect.y + slrdata->m_thumb_box_y + slrdata->m_thumb_box_h )
				egl_slider_stepit( ( EGL_HANDLE )pMsg->hObj, TRUE );
		}
		else
		{
			if( x < slr->rect.x + slrdata->m_thumb_box_x )
				egl_slider_stepit( ( EGL_HANDLE )pMsg->hObj, FALSE );
			else if( x > slr->rect.x + slrdata->m_thumb_box_x + slrdata->m_thumb_box_w )
				egl_slider_stepit( ( EGL_HANDLE )pMsg->hObj, TRUE );
		}
		if( slr->cb )
			slr->cb( pMsg->hObj, SLR_CLICKED );
		break;
	case EGL_MSG_MOVE:
		slr->rect.x = pMsg->param.point.x;
		slr->rect.y = pMsg->param.point.y;
		slr->bNeedRedraw = TRUE;
		break;
	case EGL_MSG_DELETE:
		egl_delete_slider( pMsg->hObj );
		break;
	case EGL_MSG_TIMETICK:
	default:
		break;
	}
	return NULL;
}

/*
* GLOBAL API
*/

EGL_HANDLE egl_create_slider( int x, int y, int w, int h, int range, TICKSTYLE style, BOOL bVertical )
{
	SLR_DATA* slrdata ;
	EGL_OBJECT_PTR slr;
	slr = prv_egl_create_object( x, y, w, h );
	if( slr == NULL )
	{
		return NULL;
	}
	slrdata = create_selfdata();
	if( slrdata == NULL )
	{
		prv_egl_delete_object( slr );
		return NULL;
	}
	slr->cb = NULL;
	slrdata -> m_bVerticalMode = bVertical;
	slrdata -> m_colorless = FALSE;
	slrdata -> m_pos_color = 0;
	slrdata -> m_bar_color = MAKE_COLORREF( 0, 0, 0 );
	slrdata -> m_tick_color = MAKE_COLORREF( 70, 104, 153 );
	slrdata -> m_pos = 0;
	slrdata -> m_minpos = 0;
	slrdata -> m_range = range;
	slrdata -> m_maxpos = slrdata -> m_range;
	slrdata -> m_tickfrequency = 1;
	slrdata -> m_tickstyle = style;
	slrdata -> label = 0;
	if( slrdata->m_bVerticalMode )
	{
		slrdata->m_thumb_box_w = w - 16;
		slrdata->m_thumb_box_h = 10;
		slrdata->m_thumb_box_x = 8;
		slrdata->m_thumb_box_y = 0;
		slrdata->m_bar_box_w = 2;
		slrdata->m_bar_box_h = h;
		slrdata->m_bar_box_x = x + ( w / 2 ) - ( slrdata -> m_bar_box_w / 2 );
		slrdata->m_bar_box_y = y + slrdata->m_thumb_box_h / 2;
		slrdata->m_bar_box_h -= slrdata->m_thumb_box_h;
		slrdata -> m_stepsize = slrdata->m_bar_box_h * 1024 / ( slrdata->m_range - 1 );
	}
	else
	{
		slrdata->m_thumb_box_w = 10;
		slrdata->m_thumb_box_h = h - 16;
		slrdata->m_thumb_box_x = 0;
		slrdata->m_thumb_box_y = 8;
		slrdata->m_bar_box_h = 2;
		slrdata->m_bar_box_w = w;
		slrdata->m_bar_box_x = x + slrdata->m_thumb_box_w / 2;
		slrdata->m_bar_box_y = y + ( h / 2 ) - ( slrdata->m_bar_box_h / 2 );
		slrdata->m_bar_box_w -= slrdata->m_thumb_box_w;
		slrdata -> m_stepsize = slrdata->m_bar_box_w * 1024 / ( slrdata->m_range - 1 );
	}
	slr->selfdata = slrdata;
	slr->msg_hanlder = slr_msg_hanlder;
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
	{
		slr->selfsurface = CREATE_SURFACE( w, h, 32 );
		slr->selfsurface->pixtype = PIX_FMT_ARGB;
		if( slr->selfsurface == NULL )
		{
			delete_selfdata( slrdata );
			prv_egl_delete_object( slr );
			return FALSE;
		}
	}
#endif
	return ( EGL_HANDLE )slr;
}

BOOL egl_slider_callback( EGL_HANDLE hObj, EVENT_CALLBACK cb )
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	CRITICAL_ENTER();
	pObj->cb = cb;
	CRITICAL_EXIT();
	return TRUE;
}

void egl_slider_set_pos( EGL_HANDLE hObj, int nPos )
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	SLR_DATA* slrdata = ( SLR_DATA* )pObj->selfdata;
	CRITICAL_ENTER();
	slrdata->m_pos = nPos;
	if ( slrdata->m_pos < slrdata->m_minpos )
		slrdata->m_pos = slrdata->m_minpos;
	if ( slrdata->m_pos > slrdata->m_maxpos - 1 )
		slrdata->m_pos = slrdata->m_maxpos - 1;
	if( slrdata->m_bVerticalMode )
	{
		slrdata->m_thumb_box_y =  slrdata->m_stepsize * slrdata->m_pos / 1024;
	}
	else
	{
		slrdata->m_thumb_box_x =  slrdata->m_stepsize * slrdata->m_pos / 1024;
	}
	pObj->bNeedRedraw = TRUE;
	CRITICAL_EXIT();
}

int egl_slider_get_pos( EGL_HANDLE hObj )
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	SLR_DATA* slrdata = ( SLR_DATA* )pObj->selfdata;
	return slrdata->m_pos;
}

void egl_slider_get_range( EGL_HANDLE hObj, int* lpMinPos, int* lpMaxPos )
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	SLR_DATA* slrdata = ( SLR_DATA* )pObj->selfdata;
	CRITICAL_ENTER();
	*lpMaxPos = slrdata->m_maxpos;
	*lpMinPos = slrdata->m_minpos;
	CRITICAL_EXIT();
}

void egl_slider_set_range( EGL_HANDLE hObj, int nMinPos, int nMaxPos )
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	SLR_DATA* slrdata = ( SLR_DATA* )pObj->selfdata;
	CRITICAL_ENTER();
	slrdata->m_range = nMaxPos - nMinPos;
	if( slrdata->m_bVerticalMode )
	{
		slrdata->m_thumb_box_h = slrdata->m_bar_box_h * slrdata->m_stepsize / slrdata->m_range;
		if ( slrdata->m_thumb_box_h < 8 )
		{
			slrdata->m_thumb_box_h = 8;
		}
	}
	else
	{
		slrdata->m_thumb_box_w = slrdata->m_bar_box_w * slrdata->m_stepsize / slrdata->m_range;
		if ( slrdata->m_thumb_box_w < 8 )
		{
			slrdata->m_thumb_box_w = 8;
		}
	}
	slrdata->m_minpos = nMinPos;
	slrdata->m_maxpos = nMaxPos;
	pObj->bNeedRedraw = TRUE;
	CRITICAL_EXIT();
}

void egl_slider_stepit( EGL_HANDLE hObj, BOOL inc )
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	SLR_DATA* slrdata = ( SLR_DATA* )pObj->selfdata;
	if ( inc == TRUE )
	{
		slrdata->m_pos += 1;
		if ( slrdata->m_pos > slrdata->m_maxpos )
			slrdata->m_pos = slrdata->m_maxpos;
	}
	else
	{
		slrdata->m_pos -= 1;
		if ( slrdata->m_pos < slrdata->m_minpos )
			slrdata->m_pos = slrdata->m_minpos;
	}
	egl_slider_set_pos( hObj, slrdata->m_pos );
}

void egl_slider_set_tick_frequency( EGL_HANDLE hObj, int freq )
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	SLR_DATA* slrdata = ( SLR_DATA* )pObj->selfdata;
	CRITICAL_ENTER();
	slrdata->m_tickfrequency = freq;
	pObj->bNeedRedraw = TRUE;
	CRITICAL_EXIT();
}

void egl_slider_set_tick_style( EGL_HANDLE hObj, TICKSTYLE style )
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	SLR_DATA* slrdata = ( SLR_DATA* )pObj->selfdata;
	CRITICAL_ENTER();
	slrdata->m_tickstyle = style;
	pObj->bNeedRedraw = TRUE;
	CRITICAL_EXIT();
}

void egl_slider_set_thumb_size( EGL_HANDLE hObj, int width, int high )
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	SLR_DATA* slrdata = ( SLR_DATA* )pObj->selfdata;
	CRITICAL_ENTER();
	if( slrdata->m_bVerticalMode )
	{
		slrdata->m_thumb_box_w = width;
		slrdata->m_thumb_box_h = high;
		slrdata->m_thumb_box_x = pObj->rect.x + ( pObj->rect.w / 2 ) - ( slrdata->m_thumb_box_w / 2 );
		slrdata->m_thumb_box_y = slrdata->m_bar_box_y;
	}
	else
	{
		slrdata->m_thumb_box_w = width;
		slrdata->m_thumb_box_h = high;
		slrdata->m_thumb_box_x = slrdata->m_bar_box_x;
		slrdata->m_thumb_box_y = pObj->rect.y + ( pObj->rect.h / 2 ) - ( slrdata->m_thumb_box_h / 2 );
	}
	pObj->bNeedRedraw = TRUE;
	CRITICAL_EXIT();
}

void egl_slider_get_thumb_size( EGL_HANDLE hObj, int* w, int* h )
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	SLR_DATA* slrdata = ( SLR_DATA* )pObj->selfdata;
	CRITICAL_ENTER();
	*w = slrdata->m_thumb_box_w;
	*h = slrdata->m_thumb_box_h;
	CRITICAL_EXIT();
}

void egl_slider_set_barcolor( EGL_HANDLE hObj, EGL_COLOR clr)
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	SLR_DATA* slrdata = ( SLR_DATA* )pObj->selfdata;
	CRITICAL_ENTER();
	slrdata->m_bar_color = clr;
	pObj->bNeedRedraw = TRUE;
	CRITICAL_EXIT();
}

void egl_slider_set_tickcolor( EGL_HANDLE hObj, EGL_COLOR clr)
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	SLR_DATA* slrdata = ( SLR_DATA* )pObj->selfdata;
	CRITICAL_ENTER();
	slrdata->m_tick_color = clr;
	pObj->bNeedRedraw = TRUE;
	CRITICAL_EXIT();
}

void egl_slider_set_transparent( EGL_HANDLE hObj, BOOL bflag )
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	SLR_DATA* slrdata = ( SLR_DATA* )pObj->selfdata;
	CRITICAL_ENTER();
	slrdata->m_colorless = bflag;
	pObj->bNeedRedraw = TRUE;
	CRITICAL_EXIT();
}

BOOL egl_release_slider( EGL_HANDLE hObj )
{
	if(hObj == NULL)
		return FALSE;	
	egl_delete_slider( hObj );	
	return TRUE;
}
