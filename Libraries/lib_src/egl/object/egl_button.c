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
#include "object/egl_button.h"

static void egl_delete_button( EGL_HANDLE hObj );
static void r_make_btn( EGL_OBJECT_PTR btn )
{
    EGL_OBJECT_PTR label;
    EGL_MSG Msg;
    BTN_DATA* btndata = ( BTN_DATA* )btn->selfdata;
    theme_draw_button( btn->rect.x, btn->rect.y, btn->rect.w, btn->rect.h, btndata->bPressed );
    label = ( EGL_OBJECT_PTR )btndata->label;
    Msg.msgID = EGL_MSG_DRAW;
    Msg.hObj = btndata->label;
    label->msg_hanlder( &Msg );
}
static void draw( EGL_OBJECT_PTR btn )
{
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
    {
        SURFACE* surf;
        BTN_DATA* btndata = ( BTN_DATA* )btn->selfdata;
        if( btndata->bPressed )
            surf = btndata->pressedsurface;
        else
            surf = btn->selfsurface;
        DRAW_SURFACE( surf, btn->rect.x, btn->rect.y );
    }
#else
    r_make_btn( btn );
#endif
}

static void* btn_msg_hanlder( EGL_MSG* pMsg )
{
    EGL_OBJECT_PTR label;
    EGL_OBJECT_PTR btn = ( EGL_OBJECT_PTR )pMsg->hObj;
    BTN_DATA* btndata = ( BTN_DATA* )btn->selfdata;
    label = ( EGL_OBJECT_PTR )btndata->label;
    switch ( pMsg->msgID )
    {
    case EGL_MSG_DRAW:
        draw( btn );
        break;
    case EGL_MSG_TOUCHED:
        btndata->bPressed = TRUE;
        btn->bNeedRedraw = TRUE;
		if( btn->cb )
		{
			btn->cb( pMsg->hObj, BTN_PRESSED );
		}
        break;
    case EGL_MSG_UNTOUCHED:
        btndata->bPressed = FALSE;
        btn->bNeedRedraw = TRUE;
        if( btn->cb )
        {
            btn->cb( pMsg->hObj, BTN_CLICKED );
        }
        break;
    case EGL_MSG_MOVE:
        btn->rect.x = label->rect.x = pMsg->param.point.x;
        btn->rect.y = label->rect.y = pMsg->param.point.y;
        btn->bNeedRedraw = TRUE;
        break;
    case EGL_MSG_DELETE:
        egl_delete_button( pMsg->hObj );
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

static BTN_DATA* create_selfdata()
{
    BTN_DATA* btndata = ( BTN_DATA* )malloc( sizeof( BTN_DATA ) );
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

EGL_HANDLE egl_create_button( int x, int y, int w, int h, const char* text )
{
    BTN_DATA* btndata ;
    EGL_OBJECT_PTR btn;
    EGL_HANDLE label = egl_create_label( x, y, w, h, text );
    if( label == NULL )
        return NULL;
    btn = prv_egl_create_object( x, y, w, h );
    if( btn == NULL )
    {
        prv_egl_delete_object( ( EGL_OBJECT_PTR )label );
        return NULL;
    }
    btndata = create_selfdata();
    if( btndata == NULL )
    {
        prv_egl_delete_object( btn );
        return NULL;
    }
    btndata->label = label;
    btndata->bPressed = FALSE;
    btn->selfdata = btndata;
    btn->msg_hanlder = btn_msg_hanlder;
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
    {
        SURFACE* oldtarget;
        btn->selfsurface = CREATE_SURFACE( w, h, EGL_SCREEN_BPP );
        if( btn->selfsurface == NULL )
        {
            delete_selfdata( btndata );
            prv_egl_delete_object( btn );
            return FALSE;
        }
        btndata->pressedsurface = CREATE_SURFACE( w, h, EGL_SCREEN_BPP );;
        if( btndata->pressedsurface == NULL )
        {
            delete_selfdata( btndata );
            prv_egl_delete_object( btn );
            RELEASE_SURFACE( btn->selfsurface );
            return FALSE;
        }
        btn->rect.x = btn->rect.y = 0;
        ( ( EGL_OBJECT_PTR )label )->rect.x = ( ( EGL_OBJECT_PTR )label )->rect.y = 0;
        oldtarget = EGL_SET_TARGET_SURFACE( btn->selfsurface );
        r_make_btn( btn );
        EGL_SET_TARGET_SURFACE( btndata->pressedsurface );
        btndata->bPressed = TRUE;
        r_make_btn( btn );
        EGL_SET_TARGET_SURFACE( oldtarget );
        ( ( EGL_OBJECT_PTR )label )->rect.x = btn->rect.x = x;
        ( ( EGL_OBJECT_PTR )label )->rect.y = btn->rect.y = y;
        btndata->bPressed = FALSE;
    }
#endif
    return ( EGL_HANDLE )btn;
}
static void egl_delete_button( EGL_HANDLE hObj )
{
    EGL_OBJECT_PTR pObj;
    if( hObj == NULL )
        return ;
    pObj = ( EGL_OBJECT_PTR )hObj;
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
    if( pObj->selfsurface )
        RELEASE_SURFACE( pObj->selfsurface );
    {
        BTN_DATA* pData = ( BTN_DATA* )pObj->selfdata;
        if( pData->pressedsurface )
            RELEASE_SURFACE( pData->pressedsurface );
    }
#endif
	BTN_DATA* btndata = (BTN_DATA*)pObj->selfdata;
    egl_release_label( btndata-> label );
    delete_selfdata( pObj->selfdata );
    prv_egl_delete_object( pObj );
}

BOOL egl_button_callback( EGL_HANDLE hObj, EVENT_CALLBACK cb )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	CRITICAL_ENTER();
    pObj->cb = cb;
	CRITICAL_EXIT();
    return TRUE;
}

BOOL egl_release_button(EGL_HANDLE hObj )
{
	if(hObj == NULL)
		return FALSE;
	egl_delete_button( hObj );	
	return TRUE;
}
