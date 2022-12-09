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
#include "object/egl_progressbar.h"
#include "object/egl_label.h"
#include <string.h>

static void r_make_pgbar( EGL_OBJECT_PTR pgbar )
{
    EGL_OBJECT_PTR label;
    EGL_MSG Msg;
    PGBAR_DATA* pgbardata = ( PGBAR_DATA* )pgbar->selfdata;
    int i = 0;
    theme_draw_frame( pgbar->rect.x, pgbar->rect.y, pgbar->rect.w, pgbar->rect.h );
    if( pgbardata->m_bVertical )
    {
        DRAW_RECTFILL( pgbar->rect.x + theme_frame_dw(), pgbar->rect.y + theme_frame_dh(), pgbar->rect.w - theme_frame_dw() * 2, pgbar->rect.h - theme_frame_dh() * 2 - pgbardata->m_pos, pgbardata->m_bg_color );
        DRAW_RECTFILL( pgbar->rect.x + theme_frame_dw(), pgbar->rect.y + pgbar->rect.h - theme_frame_dh() - pgbardata->m_pos, pgbar->rect.w - theme_frame_dh() * 2, pgbardata->m_pos, pgbardata->m_pos_color );
        if( pgbardata->m_style )
            for( i = 1; i < 10; i++ )
                DRAW_RECTFILL( pgbar->rect.x + theme_frame_dh(), pgbar->rect.y + pgbardata->m_bar_width * i - 4, pgbar->rect.w - theme_frame_dh() * 2, 4, pgbardata->m_bg_color );
    }
    else
    {
        DRAW_RECTFILL( pgbar->rect.x + theme_frame_dw() + pgbardata->m_pos, pgbar->rect.y + theme_frame_dh(), pgbar->rect.w - theme_frame_dw() * 2 - pgbardata->m_pos, pgbar->rect.h - theme_frame_dh() * 2, pgbardata->m_bg_color );
        DRAW_RECTFILL( pgbar->rect.x + theme_frame_dw(), pgbar->rect.y + theme_frame_dh(), pgbardata->m_pos, pgbar->rect.h - theme_frame_dh() * 2, pgbardata->m_pos_color );
        if( pgbardata->m_style )
            for( i = 1; i < 10; i++ )
                DRAW_RECTFILL( pgbar->rect.x + pgbardata->m_bar_width * i - 4, pgbar->rect.y + theme_frame_dh(), 4, pgbar->rect.h - theme_frame_dh() * 2, pgbardata->m_bg_color );
    }
    label = ( EGL_OBJECT_PTR )pgbardata->label;
    if( label )
    {
        Msg.msgID = EGL_MSG_DRAW;
        Msg.hObj = pgbardata->label;
        label->msg_hanlder( &Msg );
    }
}


static void draw( EGL_OBJECT_PTR pgbar )
{
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
    {
        PGBAR_DATA* pgbardata = ( PGBAR_DATA* )pgbar->selfdata;
        EGL_OBJECT_PTR label = label = ( EGL_OBJECT_PTR )pgbardata->label;
        SURFACE* oldtarget;
        int old_x = pgbar->rect.x;
        int old_y = pgbar->rect.y;

        pgbar->rect.x = label->rect.x = 0;
        pgbar->rect.y = label->rect.y = 0;

        oldtarget = EGL_SET_TARGET_SURFACE( pgbar->selfsurface );
        r_make_pgbar( pgbar );
        EGL_SET_TARGET_SURFACE( oldtarget );

        pgbar->rect.x = label->rect.x = old_x;
        pgbar->rect.y = label->rect.y = old_y;

        DRAW_SURFACE( pgbar->selfsurface, pgbar->rect.x, pgbar->rect.y );
    }
#else
    r_make_pgbar( pgbar );
#endif
}

static PGBAR_DATA* create_selfdata()
{
    PGBAR_DATA* pgbardata = ( PGBAR_DATA* )malloc( sizeof( PGBAR_DATA ) );
    return pgbardata;
}
static void delete_selfdata( void* selfdata )
{
    PGBAR_DATA* pgbardata = ( PGBAR_DATA* )selfdata;
    EGL_OBJECT_PTR label = ( EGL_OBJECT_PTR )pgbardata->label;
    EGL_MSG Msg;
    Msg.msgID = EGL_MSG_DELETE;
    Msg.hObj = pgbardata->label;
    label->msg_hanlder( &Msg );
    if( selfdata )
        free( selfdata );
}

static void egl_delete_progressbar( EGL_HANDLE hObj )
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

static void* pgbar_msg_hanlder( EGL_MSG* pMsg )
{
    EGL_OBJECT_PTR pgbar = ( EGL_OBJECT_PTR )pMsg->hObj;
    PGBAR_DATA* pgbardata = ( PGBAR_DATA* )pgbar->selfdata;
    EGL_OBJECT_PTR label = ( EGL_OBJECT_PTR )pgbardata->label;
    switch ( pMsg->msgID )
    {
    case EGL_MSG_DRAW:
        draw( pgbar );
        break;
    case EGL_MSG_MOVE:
        pgbar->rect.x = label->rect.x = pMsg->param.point.x;
        pgbar->rect.y = label->rect.y = pMsg->param.point.y;
        pgbar->bNeedRedraw = TRUE;
        break;
    case EGL_MSG_DELETE:
        egl_delete_progressbar( pMsg->hObj );
        break;
    case EGL_MSG_FOCUS:
    case EGL_MSG_UNFOCUS:
    case EGL_MSG_KEY_UP:
    case EGL_MSG_KEY_DOWN:
    case EGL_MSG_TOUCHED:
    case EGL_MSG_UNTOUCHED:
    case EGL_MSG_TIMETICK:
    default:
        break;
    }
    return NULL;
}
/*
* GLOBAL API
*/

EGL_HANDLE egl_create_progressbar( int x, int y, int w, int h, const char* text, PROGRESSBAR_STYLE style, BOOL bVertical )
{
    PGBAR_DATA* pgbardata ;
    EGL_OBJECT_PTR pgbar;
    EGL_HANDLE label = egl_create_label( x, y, w, h, text );
    if( label == NULL )
        return NULL;
    pgbar = prv_egl_create_object( x, y, w, h );
    if( pgbar == NULL )
    {
        prv_egl_delete_object( ( EGL_OBJECT_PTR )label );
        return NULL;
    }
    pgbardata = create_selfdata();
    if( pgbardata == NULL )
    {
        prv_egl_delete_object( pgbar );
        return NULL;
    }
    pgbardata->m_style = style;
    pgbardata->m_bVertical = bVertical;
    pgbardata->m_pos = 0;						         //default 0%
    pgbardata->m_pos_color = MAKE_COLORREF( 70, 104, 153 );
    pgbardata->m_bg_color = MAKE_COLORREF( 255, 255, 255 );
    pgbardata->m_text_color = MAKE_COLORREF( 0, 0, 0 );
    if( bVertical )
    {
        pgbardata->m_bar_width = pgbar->rect.h / 10;  // divide style
    }
    else
    {
        pgbardata->m_bar_width = pgbar->rect.w / 10;  // divide style
    }
    pgbardata->label = label;
    pgbar->selfdata = pgbardata;
    pgbar->msg_hanlder = pgbar_msg_hanlder;
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
    {
        pgbar->selfsurface = CREATE_SURFACE( w, h, EGL_SCREEN_BPP );
        if( pgbar->selfsurface == NULL )
        {
            delete_selfdata( pgbar );
            prv_egl_delete_object( pgbar );
            return FALSE;
        }
    }
#endif
    return ( EGL_HANDLE )pgbar;
}

BOOL egl_progressbar_callback( EGL_HANDLE hObj, EVENT_CALLBACK cb )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    pObj->cb = cb;
    return TRUE;
}

void egl_progressbar_set_barcolor( EGL_HANDLE hObj, EGL_COLOR clr )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    PGBAR_DATA* pgbardata = ( PGBAR_DATA* )pObj->selfdata;
    pgbardata->m_pos_color = clr;
    pObj->bNeedRedraw = TRUE;
}

void egl_progressbar_set_bgcolor( EGL_HANDLE hObj, EGL_COLOR clr)
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    PGBAR_DATA* pgbardata = ( PGBAR_DATA* )pObj->selfdata;
    pgbardata->m_bg_color = clr;
    pObj->bNeedRedraw = TRUE;
}

void egl_progressbar_set_pos( EGL_HANDLE hObj, int value ) // value = 0 ~ 100
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    PGBAR_DATA* pgbardata = ( PGBAR_DATA* )pObj->selfdata;
    if( value > 100 )
        value = 100;
    if( pgbardata->m_value == value )
        return;
    pgbardata->m_value = value;
    if( pgbardata->m_bVertical )
    {
        pgbardata -> m_pos = ( pObj->rect.h - theme_frame_dw() * 2 ) * value / 100;
    }
    else
    {
        pgbardata -> m_pos = ( pObj->rect.w - theme_frame_dw() * 2 ) * value / 100;
    }
    pObj->bNeedRedraw = TRUE;
}

int egl_progressbar_get_pos( EGL_HANDLE hObj )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    PGBAR_DATA* pgbardata = ( PGBAR_DATA* )pObj->selfdata;
    if( pgbardata->m_bVertical )
        return pgbardata -> m_pos * 100 / ( pObj->rect.h - theme_frame_dw() * 2 );
    return pgbardata -> m_pos * 100 / ( pObj->rect.w - theme_frame_dw() * 2 );
}

void egl_progressbar_set_text( EGL_HANDLE hObj, const char* text )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    PGBAR_DATA* pgbardata = ( PGBAR_DATA* )pObj->selfdata;
    EGL_OBJECT_PTR label = ( EGL_OBJECT_PTR )pgbardata->label;
    LABEL_DATA* labeldata = ( LABEL_DATA* )label->selfdata;
    if( labeldata->str )
        free( labeldata->str );
    labeldata->str = ( char* )strdup( text );
    pObj->bNeedRedraw = TRUE;
}

void egl_progressbar_set_textcolor( EGL_HANDLE hObj, EGL_COLOR clr)
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    PGBAR_DATA* pgbardata = ( PGBAR_DATA* )pObj->selfdata;
    LABEL_DATA* label = ( LABEL_DATA* )pgbardata->label;
    egl_label_set_color( ( EGL_HANDLE )label, clr );
    pObj->bNeedRedraw = TRUE;
}

BOOL egl_release_progressbar( EGL_HANDLE hObj )
{
	if(hObj == NULL)
		return FALSE;	
	egl_delete_progressbar( hObj );	
	return TRUE;
}
