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
#include "egl_window.h"
#include "object/egl_button.h"
#include "object/egl_image_button.h"

#include <stdlib.h>

static void egl_delete_image_button( EGL_HANDLE hObj );

static void draw( EGL_OBJECT_PTR btn )
{
	IMAGE_BTN_DATA* btndata = ( IMAGE_BTN_DATA* )btn->selfdata;
	SURFACE* surf;
	if( btndata->bPressed == TRUE )
	{
		if(btndata->pressedsurface!=NULL)
			surf = btndata->pressedsurface;
		else
			surf = btn->selfsurface;
	}
	else
		surf = btn->selfsurface;
	if(surf!=NULL)
	{
		if(surf->pixtype==PIX_FMT_ARGB)
		{
			//redraw back ground but not me;
			if(btndata->bRecursive)
				return;
			btndata->bRecursive=TRUE;
			egl_window_redraw_rect(&btn->rect);
			btndata->bRecursive=FALSE;
		}
		DRAW_SURFACE_RECT( surf, btn->rect.x, btn->rect.y, 0, 0, btn->rect.w, btn->rect.h);
	}
}

static void* btn_msg_hanlder( EGL_MSG* pMsg )
{
	EGL_OBJECT_PTR btn = ( EGL_OBJECT_PTR )pMsg->hObj;
	IMAGE_BTN_DATA* btndata = ( IMAGE_BTN_DATA* )btn->selfdata;
	switch ( pMsg->msgID )
	{
	case EGL_MSG_DRAW:
		draw( btn );
		break;
	case EGL_MSG_TOUCHED:
		if(btndata->bPressed !=TRUE)
		{
			btndata->bPressed = TRUE;
			if(btndata->pressedsurface!=NULL)
				btn->bNeedRedraw = TRUE;
			if( btn->cb )
			{
				btn->cb( pMsg->hObj, BTN_PRESSED );
			}
		}
		break;
	case EGL_MSG_UNTOUCHED:
		if(btndata->bPressed !=FALSE)
		{
			btndata->bPressed = FALSE;
			if(btndata->pressedsurface!=NULL)
				btn->bNeedRedraw = TRUE;
			if( btn->cb )
			{
				btn->cb( pMsg->hObj, BTN_CLICKED );
			}
		}
		break;
	case EGL_MSG_MOVE:
		btn->rect.x = pMsg->param.point.x;
		btn->rect.y = pMsg->param.point.y;
		btn->bNeedRedraw = TRUE;
		break;
	case EGL_MSG_DELETE:
		egl_delete_image_button( pMsg->hObj );
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

static IMAGE_BTN_DATA* create_selfdata()
{
	IMAGE_BTN_DATA* btndata = ( IMAGE_BTN_DATA* )malloc( sizeof( IMAGE_BTN_DATA ) );
	return btndata;
}
static void delete_selfdata( void* selfdata )
{
	if( selfdata )
	{
		free( selfdata );
	}
}
/*
* GLOBAL API
*/

EGL_HANDLE egl_create_image_button( SURFACE* img, SURFACE* pressed_img, int x, int y, int w, int h )
{
	IMAGE_BTN_DATA* btndata ;
	EGL_OBJECT_PTR btn;
	btn = prv_egl_create_object( x, y, w, h );
	if( btn == NULL )
	{
		return NULL;
	}
	btndata = create_selfdata();
	if( btndata == NULL )
	{
		prv_egl_delete_object( btn );
		return NULL;
	}
	btndata->bRecursive=FALSE;
	btndata->bPressed = FALSE;
	btndata->pressedsurface = pressed_img;
	btn->selfsurface = img;
	btn->selfdata = btndata;
	btn->msg_hanlder = btn_msg_hanlder;
	return ( EGL_HANDLE )btn;
}
static void egl_delete_image_button( EGL_HANDLE hObj )
{
	EGL_OBJECT_PTR pObj;
	if( hObj == NULL )
		return ;
	pObj = ( EGL_OBJECT_PTR )hObj;
	delete_selfdata( pObj->selfdata );
	prv_egl_delete_object( pObj );
}

BOOL egl_image_button_callback( EGL_HANDLE hObj, EVENT_CALLBACK cb )
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	CRITICAL_ENTER();
	pObj->cb = cb;
	CRITICAL_EXIT();
	return TRUE;
}

BOOL egl_release_image_button( EGL_HANDLE hObj )
{	
	if(hObj == NULL)
		return FALSE;	
	egl_delete_image_button( hObj );	
	return TRUE;
}
