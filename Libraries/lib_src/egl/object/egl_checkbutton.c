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
#include "egl_theme.h"
#include <stdlib.h>
#include "object/egl_label.h"
#include "object/egl_checkbutton.h"
#include "stdio.h"

static void r_make_checkbtn( EGL_OBJECT_PTR checkbtn )
{
	EGL_OBJECT_PTR label;
	EGL_MSG Msg;
	CHECK_DATA* checkbtndata = ( CHECK_DATA* )checkbtn->selfdata;
	if( checkbtndata->m_style == CHECK_STYLE_CHECKBUTTON )
	{
		DRAW_RECTFILL( checkbtn->rect.x - 1, checkbtn->rect.y - 1, checkbtn->rect.w + 3, checkbtn->rect.h + 3, EGL_CONFIG_WINDOW_DEFAULT_BG_COLOR );
		theme_draw_check_btn( checkbtn->rect.x + checkbtndata->m_check_x, checkbtn->rect.y + checkbtndata->m_check_y, CHECK_RADIO_SIZE, CHECK_RADIO_SIZE, checkbtndata->m_bChecked );
	}
	else
	{
		DRAW_RECTFILL( checkbtn->rect.x - 1, checkbtn->rect.y - 1, checkbtn->rect.w + 3, checkbtn->rect.h + 3, EGL_CONFIG_WINDOW_DEFAULT_BG_COLOR );
		theme_draw_radio_btn( checkbtn->rect.x + checkbtndata->m_check_x, checkbtn->rect.y + checkbtndata->m_check_y, CHECK_RADIO_SIZE, CHECK_RADIO_SIZE, checkbtndata->m_bChecked );
	}
	label = ( EGL_OBJECT_PTR )checkbtndata->label;
	if( label )
	{
		Msg.msgID = EGL_MSG_DRAW;
		Msg.hObj = checkbtndata->label;
		label->msg_hanlder( &Msg );
	}
}

static void draw( EGL_OBJECT_PTR checkbtn )
{
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
	{
		CHECK_DATA* checkbtndata = ( CHECK_DATA* )checkbtn->selfdata;

		if( checkbtndata->m_bChecked )
			DRAW_SURFACE( checkbtndata->surf_check, checkbtn->rect.x, checkbtn->rect.y );
		else
			DRAW_SURFACE( checkbtn->selfsurface, checkbtn->rect.x, checkbtn->rect.y );
	}
#else
	r_make_checkbtn( checkbtn );
#endif
}

static CHECK_DATA* create_selfdata()
{
	CHECK_DATA* checkbtndata = ( CHECK_DATA* )malloc( sizeof( CHECK_DATA ) );
	return checkbtndata;
}
static void delete_selfdata( void* selfdata )
{
	CHECK_DATA* checkbtndata = ( CHECK_DATA* )selfdata;
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
	if(checkbtndata->surf_check)
		RELEASE_SURFACE( checkbtndata->surf_check );
#endif
	EGL_OBJECT_PTR label = ( EGL_OBJECT_PTR )checkbtndata->label;
	EGL_MSG Msg;
	Msg.msgID = EGL_MSG_DELETE;
	Msg.hObj = checkbtndata->label;
	label->msg_hanlder( &Msg );
	if( selfdata )
		free( selfdata );
}

static void egl_delete_checkbutton( EGL_HANDLE hObj )
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

static void* checkbtn_msg_hanlder( EGL_MSG* pMsg )
{
	EGL_OBJECT_PTR checkbtn = ( EGL_OBJECT_PTR )pMsg->hObj;
	CHECK_DATA* checkbtndata = ( CHECK_DATA* )checkbtn->selfdata;
	EGL_OBJECT_PTR label = ( EGL_OBJECT_PTR )checkbtndata->label;
	switch ( pMsg->msgID )
	{
	case EGL_MSG_DRAW:
		draw( checkbtn );
		break;
	case EGL_MSG_TOUCHED:
		break;
	case EGL_MSG_UNTOUCHED:
		if( checkbtndata->m_bChecked )
		{
			checkbtndata->m_bChecked = FALSE;
			if( checkbtn->cb )
			{
				checkbtn->cb( pMsg->hObj, CHECK_UNCHECKED );
			}
		}
		else
		{
			checkbtndata->m_bChecked = TRUE;
			if( checkbtn->cb )
			{
				checkbtn->cb( pMsg->hObj, CHECK_CHECKED );
			}
		}
		checkbtn->bNeedRedraw = TRUE;
		break;
	case EGL_MSG_MOVE:
		checkbtn->rect.x = pMsg->param.point.x;
		checkbtn->rect.y = pMsg->param.point.y;
		label->rect.x = pMsg->param.point.x + checkbtndata->m_check_x + CHECK_RADIO_SIZE;
		label->rect.y = pMsg->param.point.y;
		checkbtn->bNeedRedraw = TRUE;
		break;
	case EGL_MSG_DELETE:
		egl_delete_checkbutton( pMsg->hObj );
		break;
	case EGL_MSG_FOCUS:
	case EGL_MSG_UNFOCUS:
	case EGL_MSG_KEY_UP:
	case EGL_MSG_KEY_DOWN:
	case EGL_MSG_TIMETICK:
	default:
		break;
	}
	return NULL;
}
/*
* GLOBAL API
*/


EGL_HANDLE egl_create_checkbutton( int x, int y, int w, int h, const char* text, CHECK_STYLE style )
{
	CHECK_DATA* checkbtndata ;
	EGL_OBJECT_PTR checkbtn;
	EGL_HANDLE label;
	EGL_OBJECT_PTR labelobj;
	LABEL_DATA* labeldata;
	int labelwidth = 0;
	int check_x = 0;
	int check_y = 0;
	labelwidth = text_width( egl_sys_font, text );
	// check box 32 x 32
	// text height 16
	check_x = ( ( w - CHECK_RADIO_SIZE - labelwidth ) / 2 );
	if( check_x < 0 )
	{
		check_x = 0;
	}
	check_y = ( ( h - 32 ) / 2 );
	if( w < CHECK_RADIO_SIZE + 16 )
		return NULL;
	label = egl_create_label( x + check_x + CHECK_RADIO_SIZE, y , w - CHECK_RADIO_SIZE , h , text );
	labelobj = ( EGL_OBJECT_PTR )label;
	labeldata = ( LABEL_DATA* )labelobj->selfdata;
	labeldata->align = EGL_ALIGN_LEFT;
	if( label == NULL )
		return NULL;
	checkbtn = prv_egl_create_object( x, y, w, h );
	if( checkbtn == NULL )
	{
		prv_egl_delete_object( ( EGL_OBJECT_PTR )label );
		return NULL;
	}
	checkbtndata = create_selfdata();
	if( checkbtndata == NULL )
	{
		prv_egl_delete_object( checkbtn );
		return NULL;
	}
	checkbtndata->m_check_x = check_x;
	checkbtndata->m_check_y = check_y;
	checkbtndata->label = label;
	checkbtndata->m_style = style;
	checkbtndata->m_bChecked = FALSE;
	checkbtn->selfdata = checkbtndata;
	checkbtn->msg_hanlder = checkbtn_msg_hanlder;
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
	{
		CHECK_DATA* checkbtndata = ( CHECK_DATA* )checkbtn->selfdata;
		EGL_OBJECT_PTR label = label = ( EGL_OBJECT_PTR )checkbtndata->label;
		SURFACE* oldtarget;
		int old_x = checkbtn->rect.x;
		int old_y = checkbtn->rect.y;
		checkbtn->selfsurface = CREATE_SURFACE( w, h, EGL_SCREEN_BPP );
		if( checkbtn->selfsurface == NULL )
		{
			delete_selfdata( checkbtndata );
			prv_egl_delete_object( checkbtn );
			return FALSE;
		}
		checkbtndata->surf_check = CREATE_SURFACE( w, h, EGL_SCREEN_BPP );
		if( checkbtndata->surf_check == NULL )
		{
			delete_selfdata( checkbtndata );
			prv_egl_delete_object( checkbtn );
			return FALSE;
		}
		checkbtn->rect.x = checkbtn->rect.y = 0;
		label->rect.x =  checkbtndata->m_check_x + CHECK_RADIO_SIZE;
		label->rect.y = 0;
		oldtarget = EGL_SET_TARGET_SURFACE( checkbtn->selfsurface );
		r_make_checkbtn( checkbtn );
		EGL_SET_TARGET_SURFACE( oldtarget );
		oldtarget = EGL_SET_TARGET_SURFACE( checkbtndata->surf_check );
		checkbtndata->m_bChecked = TRUE;
		r_make_checkbtn( checkbtn );
		checkbtndata->m_bChecked = FALSE;
		EGL_SET_TARGET_SURFACE( oldtarget );
		checkbtn->rect.x = old_x;
		checkbtn->rect.y = old_y;
		label->rect.x = old_x + checkbtndata->m_check_x + CHECK_RADIO_SIZE;
		label->rect.y = old_y;
	}
#endif
	return ( EGL_HANDLE )checkbtn;
}

BOOL egl_checkbutton_callback( EGL_HANDLE hObj, EVENT_CALLBACK cb )
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	CRITICAL_ENTER();
	pObj->cb = cb;
	CRITICAL_EXIT();
	return TRUE;
}

void egl_checkbutton_set_check( EGL_HANDLE hObj, BOOL b )
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	CHECK_DATA* checkdata = ( CHECK_DATA* )pObj->selfdata;
	checkdata->m_bChecked = b;
	CRITICAL_ENTER();
	pObj->bNeedRedraw = TRUE;
	CRITICAL_EXIT();
}

BOOL egl_checkbutton_get_check( EGL_HANDLE hObj )
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	CHECK_DATA* checkdata = ( CHECK_DATA* )pObj->selfdata;
	return checkdata->m_bChecked;
}

void egl_checkbutton_set_style( EGL_HANDLE hObj, int style )
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	CHECK_DATA* checkdata = ( CHECK_DATA* )pObj->selfdata;
	checkdata->m_style = style;
	CRITICAL_ENTER();
	pObj->bNeedRedraw = TRUE;
	CRITICAL_EXIT();
}

BOOL egl_release_checkbutton( EGL_HANDLE hObj )
{
	if(hObj == NULL)
		return FALSE;	
	egl_delete_checkbutton( hObj );	
	return TRUE;
}
