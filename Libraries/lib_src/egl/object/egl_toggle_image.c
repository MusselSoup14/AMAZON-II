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
#include "object/egl_toggle_image.h"

static void egl_delete_toggle_image( EGL_HANDLE pObj );
static void draw( EGL_OBJECT_PTR pObj )
{
    SURFACE* surf;
    TOGGLE_IMAGE_DATA* data = ( TOGGLE_IMAGE_DATA* )pObj->selfdata;
    if( data->bOn == TRUE )
        surf = data->surf_on;
    else
        surf = pObj->selfsurface;
    DRAW_SURFACE_RECT( surf, pObj->rect.x, pObj->rect.y, 0, 0, pObj->rect.w, pObj->rect.h );
}

static void* my_msg_hanlder( EGL_MSG* pMsg )
{
    TOGGLE_IMAGE_DATA* data;
    EGL_OBJECT_PTR my = ( EGL_OBJECT_PTR )pMsg->hObj;
    data = ( TOGGLE_IMAGE_DATA* )my->selfdata;
    switch ( pMsg->msgID )
    {
    case EGL_MSG_DRAW:
        draw( my );
        break;
    case EGL_MSG_TOUCHED:
        break;
    case EGL_MSG_UNTOUCHED:
        if( data->bOn )
        {
            CRITICAL_ENTER();
            data->bOn = FALSE;
            CRITICAL_EXIT();
            if( my->cb )
                my->cb( pMsg->hObj, TOGGLE_IMAGE_OFF );
            if( data->bOn == FALSE )
                my->bNeedRedraw = TRUE;
        }
        else
        {
            CRITICAL_ENTER();
            data->bOn = TRUE;
            CRITICAL_EXIT();
            if( my->cb )
                my->cb( pMsg->hObj, TOGGLE_IMAGE_ON );
            if( data->bOn == TRUE )
                my->bNeedRedraw = TRUE;
        }
        break;
    case EGL_MSG_MOVE:
        my->rect.x = pMsg->param.point.x;
        my->rect.y = pMsg->param.point.y;
        my->bNeedRedraw = TRUE;
        break;
    case EGL_MSG_DELETE:
        egl_delete_toggle_image( (EGL_HANDLE) pMsg->hObj );
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
static void egl_delete_toggle_image( EGL_HANDLE hObj )
{
	EGL_OBJECT_PTR pObj;	
    pObj = ( EGL_OBJECT_PTR )hObj;
    TOGGLE_IMAGE_DATA* data = ( TOGGLE_IMAGE_DATA* )( pObj->selfdata );
    free( data );
    prv_egl_delete_object( pObj );
}
/*
* GLOBAL API
*/

EGL_HANDLE egl_create_toggle_image( SURFACE* surf_off, SURFACE* surf_on, int x, int y, int w, int h )
{
    EGL_OBJECT_PTR pObj;
    TOGGLE_IMAGE_DATA* pData;
    pObj = prv_egl_create_object( x, y, w, h );
    if( pObj == NULL )
    {
        return NULL;
    }
    pData = ( TOGGLE_IMAGE_DATA* )malloc( sizeof( TOGGLE_IMAGE_DATA ) );
    if( pData == NULL )
    {
        prv_egl_delete_object( pObj );
        return NULL;
    }
    pData->bOn = FALSE;
    pData->surf_on = surf_on;
    pObj->selfsurface = surf_off;
    pObj->msg_hanlder = my_msg_hanlder;
    pObj->selfdata = ( void* )pData;
    return ( EGL_HANDLE )pObj;
}

BOOL egl_toggle_image_callback( EGL_HANDLE hObj, EVENT_CALLBACK cb )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    CRITICAL_ENTER();
    pObj->cb = cb;
    CRITICAL_EXIT();
    return TRUE;
}
BOOL egl_toggle_image_set_on( EGL_HANDLE hObj, BOOL b )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    TOGGLE_IMAGE_DATA* pData = ( TOGGLE_IMAGE_DATA* )pObj->selfdata;
    CRITICAL_ENTER();
    if( pData->bOn != b )
    {
        pData->bOn = b;
        pObj->bNeedRedraw = TRUE;
    }
    CRITICAL_EXIT();
    return TRUE;
}

BOOL egl_release_toggle_image( EGL_HANDLE hObj )
{
	if(hObj == NULL)
		return FALSE;	
	egl_delete_toggle_image( hObj );	
	return TRUE;
}
