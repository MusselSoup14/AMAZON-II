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
#include "object/egl_scrollbar.h"

static void r_make_scbar( EGL_OBJECT_PTR scbar )
{
    SCBAR_DATA* scbardata = ( SCBAR_DATA* )scbar->selfdata;
    DRAW_RECTFILL( scbar->rect.x, scbar->rect.y, scbar->rect.w, scbar->rect.h, scbardata->m_bg_color );
    theme_draw_frame( scbar->rect.x, scbar->rect.y, scbar->rect.w, scbar->rect.h );
    if( scbardata->m_bVerticalMode )
    {
        theme_draw_up_arrow_btn( scbar->rect.x, scbar->rect.y, scbardata->m_left_box_w, scbardata->m_left_box_h, FALSE );
        theme_draw_down_arrow_btn( scbar->rect.x, scbar->rect.y + scbar->rect.h - scbardata->m_left_box_h, scbardata->m_right_box_w, scbardata->m_right_box_h, FALSE );
        theme_draw_scroll_thumb( scbar->rect.x + 1, scbar->rect.y + scbardata->m_left_box_h + scbardata->m_pos, scbardata->m_thumb_box_w, scbardata->m_thumb_box_h, scbardata->m_bVerticalMode, FALSE );
    }
    else
    {
        theme_draw_left_arrow_btn( scbar->rect.x, scbar->rect.y, scbardata->m_left_box_w, scbardata->m_left_box_h, FALSE );
        theme_draw_right_arrow_btn( scbar->rect.x + scbar->rect.w - scbardata->m_left_box_w, scbar->rect.y, scbardata->m_left_box_w, scbardata->m_left_box_h, FALSE );
        theme_draw_scroll_thumb( scbar->rect.x + scbardata->m_left_box_w + scbardata->m_pos, scbar->rect.y + 1, scbardata->m_thumb_box_w, scbardata->m_thumb_box_h, scbardata->m_bVerticalMode, FALSE );
    }
}

static void draw( EGL_OBJECT_PTR scbar )
{
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
    {
        SURFACE* oldtarget;
        int old_x = scbar->rect.x;
        int old_y = scbar->rect.y;

        scbar->selfsurface = CREATE_SURFACE( scbar->rect.w, scbar->rect.h, EGL_SCREEN_BPP );
        if( scbar->selfsurface == NULL )
        {
            r_make_scbar( scbar );
            return;
        }

        scbar->rect.x = scbar->rect.y = 0;
        oldtarget = EGL_SET_TARGET_SURFACE( scbar->selfsurface );
        r_make_scbar( scbar );
        EGL_SET_TARGET_SURFACE( oldtarget );
        scbar->rect.x = old_x;
        scbar->rect.y = old_y;
        DRAW_SURFACE( scbar->selfsurface, scbar->rect.x, scbar->rect.y );
        RELEASE_SURFACE( scbar->selfsurface );
    }
#else
    {
        r_make_scbar( scbar );
    }
#endif
}

static SCBAR_DATA* create_selfdata()
{
    SCBAR_DATA* scbardata = ( SCBAR_DATA* )malloc( sizeof( SCBAR_DATA ) );
    return scbardata;
}
static void delete_selfdata( void* selfdata )
{
    if( selfdata )
        free( selfdata );
}

static void egl_delete_scrollbar( EGL_HANDLE hObj )
{
    EGL_OBJECT_PTR pObj;
    if( hObj == NULL )
        return ;
    pObj = ( EGL_OBJECT_PTR )hObj;
    delete_selfdata( pObj->selfdata );
    prv_egl_delete_object( pObj );
}

static void* scbar_msg_hanlder( EGL_MSG* pMsg )
{
    EGL_OBJECT_PTR scbar = ( EGL_OBJECT_PTR )pMsg->hObj;
    SCBAR_DATA* scbardata = ( SCBAR_DATA* )scbar->selfdata;
    int x = pMsg->param.point.x;
    int y = pMsg->param.point.y;
    int re = 0;
    switch ( pMsg->msgID )
    {
    case EGL_MSG_DRAW:
        draw( scbar );
        break;
    case EGL_MSG_FOCUS:
    case EGL_MSG_UNFOCUS:
    case EGL_MSG_KEY_UP:
    case EGL_MSG_KEY_DOWN:
        break;
    case EGL_MSG_TOUCHED:
        if( ( x > scbar->rect.x ) && ( x < scbar->rect.x + scbar->rect.w ) && ( y > scbar->rect.y ) && ( y < scbar->rect.y + scbar->rect.h ) )
        {
            if( scbardata->m_bVerticalMode )
            {
                if( y > scbar->rect.y + scbardata->m_left_box_h && y < scbardata->m_thumb_box_y + scbardata->m_pos )
                {
                    scbardata->m_upcount -= scbardata -> m_viewcount;
                    if( scbardata -> m_upcount < 0 )
                        scbardata -> m_upcount = 0;
                    egl_scroll_set_position( ( EGL_HANDLE )pMsg->hObj, scbardata->m_totalcount, scbardata->m_viewcount, scbardata->m_upcount );
                    re = -1;
                }
                else if( y < scbar->rect.y + scbar->rect.h - scbardata->m_left_box_h && y > scbardata->m_thumb_box_y + scbardata->m_thumb_box_h + scbardata->m_pos )
                {
                    scbardata->m_upcount += scbardata -> m_viewcount;
                    if( scbardata -> m_upcount > scbardata->m_totalcount - scbardata -> m_viewcount )
                        scbardata -> m_upcount = scbardata->m_totalcount - scbardata -> m_viewcount;
                    egl_scroll_set_position( ( EGL_HANDLE )pMsg->hObj, scbardata->m_totalcount, scbardata->m_viewcount, scbardata->m_upcount );
                    re = 1;
                }
                else
                {
                    re = 0;
                    return ( void* )re;
                }
            }
            else
            {
                if( x > scbar->rect.x + scbardata->m_left_box_w && x < scbardata->m_thumb_box_x + scbardata->m_pos )
                {
                    scbardata->m_upcount -= scbardata -> m_viewcount;
                    if( scbardata -> m_upcount < 0 )
                        scbardata -> m_upcount = 0;
                    egl_scroll_set_position( ( EGL_HANDLE )pMsg->hObj, scbardata->m_totalcount, scbardata->m_viewcount, scbardata->m_upcount );
                    re = -1;
                }
                else if( x < scbar->rect.x + scbar->rect.w - scbardata->m_left_box_w && x > scbardata->m_thumb_box_x + scbardata->m_thumb_box_w + scbardata->m_pos )
                {
                    scbardata->m_upcount += scbardata -> m_viewcount;
                    if( scbardata -> m_upcount > scbardata->m_totalcount - scbardata -> m_viewcount )
                        scbardata -> m_upcount = scbardata->m_totalcount - scbardata -> m_viewcount;
                    egl_scroll_set_position( ( EGL_HANDLE )pMsg->hObj, scbardata->m_totalcount, scbardata->m_viewcount, scbardata->m_upcount );
                    re = 1;
                }
                else
                {
                    re = 0;
                    return ( void* )re;
                }
            }
        }
        scbar->bNeedRedraw = TRUE;
        if( scbar->cb )
        {
            scbar->cb( pMsg->hObj, SCBAR_CLICKED );
        }
        return ( void* )re;
        break;
    case EGL_MSG_UNTOUCHED:
        if( ( x > scbar->rect.x ) && ( x < scbar->rect.x + scbar->rect.w ) && ( y > scbar->rect.y ) && ( y < scbar->rect.y + scbar->rect.h ) )
        {
            if( scbardata->m_bVerticalMode )
            {
                if( y < scbar->rect.y + scbardata->m_left_box_h )
                {
                    scbardata->m_upcount -= scbardata -> m_viewcount;
                    if( scbardata -> m_upcount < 0 )
                        scbardata -> m_upcount = 0;
                    egl_scroll_set_position( ( EGL_HANDLE )pMsg->hObj, scbardata->m_totalcount, scbardata->m_viewcount, scbardata->m_upcount );
                    re = -1;
                }
                else if( y > scbar->rect.y + scbar->rect.h - scbardata->m_left_box_h )
                {
                    scbardata->m_upcount += scbardata -> m_viewcount;
                    if( scbardata -> m_upcount > scbardata->m_totalcount - scbardata -> m_viewcount )
                        scbardata -> m_upcount = scbardata->m_totalcount - scbardata -> m_viewcount;
                    egl_scroll_set_position( ( EGL_HANDLE )pMsg->hObj, scbardata->m_totalcount, scbardata->m_viewcount, scbardata->m_upcount );
                    re = 1;
                }
                else
                {
                    re = 0;
                    return ( void* )re;
                }
            }
            else
            {
                if( x < scbar->rect.x + scbardata->m_left_box_w )
                {
                    scbardata->m_upcount -= scbardata -> m_viewcount;
                    if( scbardata -> m_upcount < 0 )
                        scbardata -> m_upcount = 0;
                    egl_scroll_set_position( ( EGL_HANDLE )pMsg->hObj, scbardata->m_totalcount, scbardata->m_viewcount, scbardata->m_upcount );
                    re = -1;
                }
                else if( x > scbar->rect.x + scbar->rect.w - scbardata->m_left_box_w )
                {
                    scbardata->m_upcount += scbardata -> m_viewcount;
                    if( scbardata -> m_upcount > scbardata->m_totalcount - scbardata -> m_viewcount )
                        scbardata -> m_upcount = scbardata->m_totalcount - scbardata -> m_viewcount;
                    egl_scroll_set_position( ( EGL_HANDLE )pMsg->hObj, scbardata->m_totalcount, scbardata->m_viewcount, scbardata->m_upcount );
                    re = 1;
                }
                else
                {
                    re = 0;
                    return ( void* )re;
                }
            }
        }
        scbar->bNeedRedraw = TRUE;
        if( scbar->cb )
        {
            scbar->cb( pMsg->hObj, SCBAR_CLICKED );
        }
        return ( void* )re;
        break;
    case EGL_MSG_MOVE:
        scbar->rect.x = pMsg->param.point.x;
        scbar->rect.y = pMsg->param.point.y;
        scbar->bNeedRedraw = TRUE;
        break;
    case EGL_MSG_DELETE:
        egl_delete_scrollbar( pMsg->hObj );
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

EGL_HANDLE egl_create_scrollbar( int x, int y, int w, int h, int totalcount, int viewcount, BOOL bVertical )
{
    SCBAR_DATA* scbardata ;
    EGL_OBJECT_PTR scbar;
    scbar = prv_egl_create_object( x, y, w, h );
    if( scbar == NULL )
    {
        return NULL;
    }
    scbardata = create_selfdata();
    if( scbardata == NULL )
    {
        prv_egl_delete_object( scbar );
        return NULL;
    }
    scbar->cb = NULL;
    scbardata->m_bVerticalMode = bVertical;
    scbardata->m_pos = 0;
    scbardata->m_bg_color = MAKE_COLORREF( 255, 255, 255 );   // b,g,r°ª
    scbardata->m_totalcount = totalcount;
    scbardata->m_viewcount = viewcount;
    scbardata->m_upcount = 0;
	scbardata->label = 0;	
    scbar->selfdata = scbardata;
    if( scbardata->m_bVerticalMode )
    {
        scbardata->m_left_box_h = scbar->rect.w;
        scbardata->m_left_box_w = scbar->rect.w;
        scbardata->m_right_box_h = scbar->rect.w;
        scbardata->m_right_box_w = scbar->rect.w;
        scbardata->m_center_box_h = scbar->rect.h  - scbardata->m_left_box_h * 2;
        scbardata->m_thumb_box_x = scbar->rect.x + 1;
        scbardata->m_thumb_box_y = scbar->rect.y + scbardata->m_left_box_h;
        scbardata->m_thumb_box_h = scbardata->m_center_box_h / scbardata->m_totalcount * scbardata->m_viewcount;
        scbardata->m_thumb_box_w = scbar->rect.w - 2;
        scbardata->m_maxpos = h - scbardata->m_left_box_h * 2 - scbardata->m_thumb_box_h;
    }
    else
    {
        scbardata->m_left_box_h = scbar->rect.h;
        scbardata->m_left_box_w = scbar->rect.h;
        scbardata->m_right_box_h = scbar->rect.h;
        scbardata->m_right_box_w = scbar->rect.h;
        scbardata->m_center_box_w = scbar->rect.w - scbardata->m_left_box_w * 2;
        scbardata->m_thumb_box_x = scbar->rect.x + scbardata->m_left_box_w;
        scbardata->m_thumb_box_y = scbar->rect.y + 1;
        scbardata->m_thumb_box_w = scbardata->m_center_box_w / scbardata->m_totalcount * scbardata->m_viewcount;
        scbardata->m_thumb_box_h = scbar->rect.h - 2;
        scbardata->m_maxpos = w - scbardata->m_left_box_w * 2 - scbardata->m_thumb_box_w;
    }
    scbar->selfdata = scbardata;
    scbar->msg_hanlder = scbar_msg_hanlder;
    return ( EGL_HANDLE )scbar;
}

BOOL egl_scrollbar_callback( EGL_HANDLE hObj, EVENT_CALLBACK cb )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    pObj->cb = cb;
    return TRUE;
}

void egl_scroll_set_position( EGL_HANDLE hObj, int totalcount, int viewcount, int precount )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    SCBAR_DATA* scbardata = ( SCBAR_DATA* )pObj->selfdata;
    scbardata->m_totalcount = totalcount;
    scbardata->m_viewcount = viewcount;
    scbardata->m_upcount = precount;
    if( scbardata->m_bVerticalMode )
    {
        scbardata->m_thumb_box_h = viewcount * scbardata->m_center_box_h / totalcount;
        scbardata->m_pos = precount * scbardata->m_center_box_h / totalcount;
        if( scbardata->m_pos + scbardata->m_thumb_box_h > scbardata->m_center_box_h || totalcount == precount + viewcount )
        {
            scbardata->m_pos = scbardata->m_center_box_h - scbardata->m_thumb_box_h;
        }
    }
    else
    {
        scbardata->m_thumb_box_w = viewcount * scbardata->m_center_box_w / totalcount;
        scbardata->m_pos = precount * scbardata->m_center_box_w / totalcount;
        if( scbardata->m_pos + scbardata->m_thumb_box_w > scbardata->m_center_box_w || totalcount == precount + viewcount )
        {
            scbardata->m_pos = scbardata->m_center_box_w - scbardata->m_thumb_box_w;
        }
    }
	CRITICAL_ENTER();
    pObj->bNeedRedraw = TRUE;
	CRITICAL_EXIT();
}

void egl_scroll_get_position( EGL_HANDLE hObj, int* totalcount, int* viewcount, int* upcount )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    SCBAR_DATA* scbardata = ( SCBAR_DATA* )pObj->selfdata;
	CRITICAL_ENTER();
    *totalcount = scbardata->m_totalcount;
    *viewcount = scbardata -> m_viewcount;
    *upcount = scbardata->m_upcount;
	CRITICAL_EXIT();
}

void egl_scroll_set_totalcount( EGL_HANDLE hObj, int totalcount )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    SCBAR_DATA* scbardata = ( SCBAR_DATA* )pObj->selfdata;
    egl_scroll_set_position( hObj, totalcount, scbardata->m_viewcount, scbardata->m_upcount );
}

void egl_scroll_set_viewcount( EGL_HANDLE hObj, int viewcount )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    SCBAR_DATA* scbardata = ( SCBAR_DATA* )pObj->selfdata;
    egl_scroll_set_position( hObj, scbardata->m_totalcount, viewcount, scbardata->m_upcount );
}

void egl_scroll_set_upcount( EGL_HANDLE hObj, int upcount )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    SCBAR_DATA* scbardata = ( SCBAR_DATA* )pObj->selfdata;
    egl_scroll_set_position( hObj, scbardata->m_totalcount, scbardata->m_viewcount, upcount );
}

int egl_scroll_get_totalcount( EGL_HANDLE hObj )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    SCBAR_DATA* scbardata = ( SCBAR_DATA* )pObj->selfdata;
    return scbardata->m_totalcount;
}

int egl_scroll_get_viewcount( EGL_HANDLE hObj )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    SCBAR_DATA* scbardata = ( SCBAR_DATA* )pObj->selfdata;
    return scbardata->m_viewcount;
}

int egl_scroll_get_upcount( EGL_HANDLE hObj )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    SCBAR_DATA* scbardata = ( SCBAR_DATA* )pObj->selfdata;
    return scbardata->m_upcount;
}

void egl_scroll_set_bgcolor( EGL_HANDLE hObj, unsigned char r, unsigned char g, unsigned char b )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    SCBAR_DATA* scbardata = ( SCBAR_DATA* )pObj->selfdata;
    scbardata->m_bg_color = MAKE_COLORREF( r, g, b );
	CRITICAL_ENTER();
    pObj->bNeedRedraw = TRUE;
	CRITICAL_EXIT();
}

void egl_scroll_set_size( EGL_HANDLE hObj, int w, int h )
{
    EGL_OBJECT_PTR scbar = ( EGL_OBJECT_PTR )hObj;
    SCBAR_DATA* scbardata = ( SCBAR_DATA* )scbar->selfdata;
    scbar->rect.w = w;
    scbar->rect.h = h;
    if( scbardata->m_bVerticalMode )
    {
        scbardata->m_left_box_h = scbar->rect.w;
        scbardata->m_left_box_w = scbar->rect.w;
        scbardata->m_right_box_h = scbar->rect.w;
        scbardata->m_right_box_w = scbar->rect.w;
        scbardata->m_center_box_h = scbar->rect.h  - scbardata->m_left_box_h * 2;
        scbardata->m_thumb_box_x = scbar->rect.x + 1;
        scbardata->m_thumb_box_y = scbar->rect.y + scbardata->m_left_box_h;
        scbardata->m_thumb_box_h = scbardata->m_center_box_h / scbardata->m_totalcount * scbardata->m_viewcount;
        scbardata->m_thumb_box_w = scbar->rect.w - 2;
        scbardata->m_maxpos = h - scbardata->m_left_box_h * 2 - scbardata->m_thumb_box_h;
    }
    else
    {
        scbardata->m_left_box_h = scbar->rect.h;
        scbardata->m_left_box_w = scbar->rect.h;
        scbardata->m_right_box_h = scbar->rect.h;
        scbardata->m_right_box_w = scbar->rect.h;
        scbardata->m_center_box_w = scbar->rect.w - scbardata->m_left_box_w * 2;
        scbardata->m_thumb_box_x = scbar->rect.x + scbardata->m_left_box_w;
        scbardata->m_thumb_box_y = scbar->rect.y + 1;
        scbardata->m_thumb_box_w = scbardata->m_center_box_w / scbardata->m_totalcount * scbardata->m_viewcount;
        scbardata->m_thumb_box_h = scbar->rect.h - 2;
        scbardata->m_maxpos = w - scbardata->m_left_box_w * 2 - scbardata->m_thumb_box_w;
    }
    scbar->selfdata = scbardata;
	CRITICAL_ENTER();
    scbar->bNeedRedraw = TRUE;
	CRITICAL_EXIT();
}

BOOL egl_release_scrollbar( EGL_HANDLE hObj )
{
	if(hObj == NULL)
		return FALSE;	
	egl_delete_scrollbar( hObj );	
	return TRUE;
}
