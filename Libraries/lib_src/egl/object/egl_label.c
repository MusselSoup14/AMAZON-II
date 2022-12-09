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
#include "font/egl_font.h"
#include "font/textout.h"
#include "egl_private.h"
#include "object/egl_label.h"
#include <stdlib.h>
#include <string.h>

static void egl_delete_label( EGL_OBJECT_PTR pObj );

INLINE static void draw( EGL_OBJECT_PTR pObj )
{
	EGL_COLOR org_clr;
    LABEL_DATA* ldata = ( LABEL_DATA* )pObj->selfdata;
	if(ldata->bErase_bg)
	{
		if(ldata->selfrecusive)
			return;
		ldata->selfrecusive = TRUE;
		egl_window_redraw_rect(&pObj->rect);
		ldata->selfrecusive = FALSE;
	}
	org_clr = egl_font_get_color(pObj->pFont);
	egl_font_set_color(pObj->pFont,ldata->font_clr);
    draw_text_in_box( pObj->pFont, &pObj->rect, ldata->str, ldata->align );
	egl_font_set_color(pObj->pFont,org_clr);
}
static void* msg_handler( EGL_MSG* pMsg )
{	
    EGL_OBJECT_PTR label = ( EGL_OBJECT_PTR )pMsg->hObj;    
    
    switch ( pMsg->msgID )
    {
    case EGL_MSG_DRAW:
        draw( ( EGL_OBJECT_PTR )pMsg->hObj );
        return NULL;
    case EGL_MSG_DELETE:
        egl_delete_label( ( EGL_OBJECT_PTR )pMsg->hObj );
        break;
    case EGL_MSG_UNTOUCHED:        
        if( label->cb )
        {
            label->cb( pMsg->hObj, LABEL_CLICKED);
        }        
		break;
    case EGL_MSG_FOCUS:
    case EGL_MSG_UNFOCUS:
    case EGL_MSG_KEY_UP:
    case EGL_MSG_KEY_DOWN:
    case EGL_MSG_TOUCHED:
    case EGL_MSG_TIMETICK:
    default:
        break;
    }
    return NULL;
}
static LABEL_DATA* create_selfdata( const char* text )
{
    LABEL_DATA* data = ( LABEL_DATA* )malloc( sizeof( LABEL_DATA ) );
    if( data == NULL )
        return NULL;
	data->selfrecusive=FALSE;
	data->bErase_bg = FALSE;
    data->str = ( char* )strdup( text );
    data->align = EGL_ALIGN_CENTER;
    return data;
}
static void delete_selfdata( void* selfdata )
{
    LABEL_DATA* data = ( LABEL_DATA* )selfdata;
    if( data )
    {
        if( data->str )
            free( data->str );
        free( data );
    }
}

/*
* GLOBAL API
*/
EGL_HANDLE egl_create_label( int x, int y, int w, int h, const char* text )
{
    EGL_OBJECT_PTR label;
    LABEL_DATA* selfdata = create_selfdata( text );
    if( selfdata == NULL	)
        return NULL;
    label = prv_egl_create_object( x, y, w, h );
    if( label == NULL )
    {
        delete_selfdata( selfdata );
        return NULL;
    }
	selfdata->font_clr = egl_font_get_color(egl_sys_font);
    label->selfdata = selfdata;
    label->msg_hanlder = msg_handler;
	label->cb = NULL;
    return ( EGL_HANDLE )label;
}

void egl_label_set_redraw_bg(EGL_HANDLE h,BOOL b)
{
	LABEL_DATA* selfdata = (LABEL_DATA* )(EGL_HANDLE_TO_OBJECT(h)->selfdata);
	selfdata->bErase_bg =b;
}
static void egl_delete_label( EGL_OBJECT_PTR pObj )
{
    delete_selfdata( pObj->selfdata );
    prv_egl_delete_object( pObj );
}

BOOL egl_label_callback( EGL_HANDLE hObj, EVENT_CALLBACK cb )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
	CRITICAL_ENTER();
    pObj->cb = cb;
	CRITICAL_EXIT();
    return TRUE;
}

BOOL egl_label_set_text(EGL_HANDLE h,char* text)
{
	EGL_OBJECT_PTR pObj=EGL_HANDLE_TO_OBJECT(h);
	LABEL_DATA* label = (LABEL_DATA*)pObj->selfdata;
	CRITICAL_ENTER();
	if(label->str)
		free(label->str);
	label->str=(char*)strdup(text);
	if(label->str==NULL)
	{
		CRITICAL_EXIT();
		return FALSE;
	}
	pObj->bNeedRedraw=TRUE;
	CRITICAL_EXIT();
	return TRUE;
}
void egl_label_set_color(EGL_HANDLE h, EGL_COLOR clr)
{
	EGL_OBJECT_PTR pObj=EGL_HANDLE_TO_OBJECT(h);
	LABEL_DATA* label = (LABEL_DATA*)pObj->selfdata;
	CRITICAL_ENTER();
	label->font_clr = clr;
	CRITICAL_EXIT();
}

void egl_label_set_align(EGL_HANDLE h, int align)
{
	EGL_OBJECT_PTR pObj=EGL_HANDLE_TO_OBJECT(h);
	LABEL_DATA* label = (LABEL_DATA*)pObj->selfdata;
	label->align = align;	
}

BOOL egl_release_label( EGL_HANDLE  hObj )
{
	if(hObj == NULL)
		return TRUE;	
	egl_delete_label( ( EGL_OBJECT_PTR )hObj );	
	return FALSE;
}
