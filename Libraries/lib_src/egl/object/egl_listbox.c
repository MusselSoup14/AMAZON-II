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

#include <stdlib.h>
#include <string.h>
#include "egl_typedef.h"
#include "egl_base.h"
#include "egl_private.h"
#include "egl_theme.h"
#include "object/egl_label.h"
#include "object/egl_listbox.h"
#include "object/egl_scrollbar.h"
#include "font/egl_font.h"
#include "font/textout.h"
 
#define SEL_CHECK(tx,ty,endx,endy,X,Y) (( ((X)>(tx)) && ((X)<(endx)) && ((Y)>(ty)) && ((Y)<(endy)) )?1:0)

static void move_lb( EGL_OBJECT_PTR list, int x, int y )
{
    LIST_DATA* listdata = ( LIST_DATA* )list->selfdata;
    ListItem* item = listdata->listhead;
    EGL_OBJECT_PTR label;
    EGL_OBJECT_PTR scroll;
    EGL_MSG Msg;
    int move_x = x - list->rect.x;
    int move_y = y - list->rect.y;
    list->rect.x += move_x;
    list->rect.y += move_y;
    listdata->m_x += move_x;
    listdata->m_y += move_y;
    listdata->m_item_x += move_x;
    listdata->m_next_item_y += move_y;
    while( item != NULL )
    {
        label = ( EGL_OBJECT_PTR )item->label;
        label->rect.x += move_x;
        label->rect.y += move_y;
        item = item->next;
    }
    scroll = ( EGL_OBJECT_PTR )listdata->scrollbar;
    Msg.msgID = EGL_MSG_MOVE;
    Msg.hObj = listdata->scrollbar;
    Msg.param.point.x = ( scroll->rect.x += move_x );
    Msg.param.point.y = ( scroll->rect.y += move_y );
    scroll->msg_hanlder( &Msg );
}

static void draw( EGL_OBJECT_PTR list )
{
    LIST_DATA* listdata = ( LIST_DATA* )list->selfdata;
    EGL_OBJECT_PTR label;
    EGL_OBJECT_PTR scroll =  ( EGL_OBJECT_PTR )listdata->scrollbar;
    EGL_MSG Msg;
    ListItem* item = listdata->listhead;
    int viewcnt = 0;
    int index = 0;
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
    SURFACE* oldtarget;
    int old_x = list->rect.x;
    int old_y = list->rect.y;
    move_lb( list, 0, 0 );
    oldtarget = EGL_SET_TARGET_SURFACE( list->selfsurface );
#endif
    theme_draw_frame( list->rect.x, list->rect.y, list->rect.w - scroll->rect.w, list->rect.h );
    DRAW_RECTFILL( listdata->m_x, listdata->m_y, listdata->m_item_w, listdata->m_h, listdata->m_bgcolor );
    viewcnt = list->rect.h / listdata->m_item_h;
    item = listdata->listhead;

    while( item != NULL )
    {
        if( index > listdata->m_viewStartIndex - 1 && index < listdata->m_viewStartIndex + viewcnt )
        {
            label = ( EGL_OBJECT_PTR )item->label;
            if( listdata->m_bMultiple == TRUE )
            {
                if( item->selected == TRUE )
                {
                    DRAW_RECTFILL( label->rect.x, label->rect.y, label->rect.w, label->rect.h, listdata->m_selbgcolor );
                    egl_label_set_color( ( EGL_HANDLE )label, listdata->m_seltextcolor );
                }
                else
                    egl_label_set_color( ( EGL_HANDLE )label, listdata->m_textcolor );
            }
            else
            {
                if( index == listdata->m_cursel )
                {
                    DRAW_RECTFILL( label->rect.x, label->rect.y, label->rect.w, label->rect.h, listdata->m_selbgcolor );
                    egl_label_set_color( ( EGL_HANDLE )label, listdata->m_seltextcolor );
                }
                else
                    egl_label_set_color( ( EGL_HANDLE )label, listdata->m_textcolor );
            }
            Msg.msgID = EGL_MSG_DRAW;
            Msg.hObj = item->label;
            label->msg_hanlder( &Msg );
            //egl_font_set_color( label->pFont, listdata->m_textcolor );
        }
        item = item->next;
        index++;
    }
	
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
    EGL_SET_TARGET_SURFACE( oldtarget );
    move_lb( list, old_x, old_y );
    if( listdata->m_listcount > viewcnt )
    {
        //scroll = ( EGL_OBJECT_PTR )listdata->scrollbar;
        DRAW_SURFACE_RECT( list->selfsurface, list->rect.x, list->rect.y, 0, 0, list->rect.w - scroll->rect.w, list->rect.h );
    }
    else
    {
        DRAW_SURFACE_RECT( list->selfsurface, list->rect.x, list->rect.y, 0, 0, list->rect.w - scroll->rect.w, list->rect.h );
        //DRAW_SURFACE( list->selfsurface, list->rect.x, list->rect.y );
    }
#endif

    if( listdata->m_listcount > viewcnt )
    {
        scroll = ( EGL_OBJECT_PTR )listdata->scrollbar;
        Msg.msgID = EGL_MSG_DRAW;
        Msg.hObj = listdata->scrollbar;
        scroll->msg_hanlder( &Msg );
    }

    else
    {
        theme_draw_up_arrow_btn( list->rect.x + list->rect.w - scroll->rect.w, list->rect.y, scroll->rect.w, scroll->rect.w, FALSE );
        theme_draw_down_arrow_btn( list->rect.x + list->rect.w - scroll->rect.w, list->rect.y + list->rect.h - scroll->rect.w, scroll->rect.w, scroll->rect.w, FALSE );
        theme_draw_scroll_thumb( list->rect.x + list->rect.w - scroll->rect.w, list->rect.y + scroll->rect.w, scroll->rect.w, list->rect.h - (2*scroll->rect.w), TRUE, FALSE );
    }
}

static void delete_selfdata( void* selfdata )
{
    if( selfdata )
        free( selfdata );
}

static void egl_delete_listbox( EGL_HANDLE hObj )
{
    EGL_OBJECT_PTR pObj;
    LIST_DATA* listdata;
    EGL_MSG Msg;
    if( hObj == NULL )
        return ;
	egl_listbox_alldelitem(hObj);
    pObj = ( EGL_OBJECT_PTR )hObj;
    listdata = ( LIST_DATA* )pObj->selfdata;
    // delete scrollbar
    Msg.msgID = EGL_MSG_DELETE;
    Msg.hObj = listdata->scrollbar;
    ( ( EGL_OBJECT_PTR )listdata->scrollbar )->msg_hanlder( &Msg );
    delete_selfdata( pObj->selfdata );
    prv_egl_delete_object( pObj );
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
    if( pObj->selfsurface )
        RELEASE_SURFACE( pObj->selfsurface );
#endif
}

static void* list_msg_hanlder( EGL_MSG* pMsg )
{
    EGL_OBJECT_PTR list = ( EGL_OBJECT_PTR )pMsg->hObj;
    LIST_DATA* listdata = ( LIST_DATA* )list->selfdata;
    EGL_OBJECT_PTR scroll = ( EGL_OBJECT_PTR )listdata->scrollbar;
    ListItem* item = listdata->listhead;
    EGL_OBJECT_PTR label = ( EGL_OBJECT_PTR )item->label;
    EGL_MSG Msg;
    int x = pMsg->param.point.x;
    int y = pMsg->param.point.y;
    int index = 0;
    int viewcnt;
    int scroll_ret;
    viewcnt = list->rect.h / listdata->m_item_h;
    switch ( pMsg->msgID )
    {
    case EGL_MSG_DRAW:
        draw( list );
        break;
    case EGL_MSG_DELETE:
        egl_delete_listbox( ( EGL_HANDLE )list );
        break;
    case EGL_MSG_FOCUS:
    case EGL_MSG_UNFOCUS:
    case EGL_MSG_KEY_UP:
    case EGL_MSG_KEY_DOWN:
    case EGL_MSG_TOUCHED:
        if( SEL_CHECK( scroll->rect.x, scroll->rect.y, scroll->rect.x + scroll->rect.w, scroll->rect.y + scroll->rect.h, x, y ) )
        {
            int remain_count = 0;
            int offset = 0;
            Msg.msgID = EGL_MSG_TOUCHED;
            Msg.hObj = listdata->scrollbar;
            Msg.param.point = pMsg->param.point;
            scroll_ret = ( int )scroll->msg_hanlder( &Msg );
            // top , left
            if( scroll_ret < 0 )
            {
                if( listdata->m_viewStartIndex > 0 )
                {
                    if(listdata->m_viewStartIndex > viewcnt)  
                    {
                        offset = viewcnt;
                    }
                    else
                    {
                        offset = listdata->m_viewStartIndex;
                    }

                    while( item != NULL )
                    {
                        label = ( EGL_OBJECT_PTR )item->label;
                        label->rect.y += ( label->rect.h * offset );
                        item = item->next;
                    }
                    listdata->m_viewStartIndex -= offset;
                    listdata->m_next_item_y += ( label->rect.h * offset );
                    list->bNeedRedraw = TRUE;
                }
                break;
            }
            // bottom, right
            else if( scroll_ret > 0 )
            {
                if( listdata->m_viewStartIndex < listdata->m_listcount - ( list->rect.h / listdata->m_item_h ) )
                {
                    remain_count = listdata->m_listcount - listdata->m_viewStartIndex - viewcnt;

                    if(remain_count > viewcnt)
                    {
                        offset = viewcnt;
                    }
                    else
                    {
                        offset = remain_count;
                    }

                    while( item != NULL )
                    {
                        label = ( EGL_OBJECT_PTR )item->label;
                        label->rect.y -= ( label->rect.h * offset );
                        item = item->next;
                    }
                    listdata->m_viewStartIndex += offset;
                    listdata->m_next_item_y -= ( label->rect.h * offset );
                    list->bNeedRedraw = TRUE;
                }
                break;
            }
        }
        break;
    case EGL_MSG_UNTOUCHED:
        if( listdata->m_listcount > viewcnt )
        {
            if( SEL_CHECK( scroll->rect.x, scroll->rect.y, scroll->rect.x + scroll->rect.w, scroll->rect.y + scroll->rect.h, x, y ) )
            {
                int remain_count = 0;
                int offset = 0;
                Msg.msgID = EGL_MSG_UNTOUCHED;
                Msg.hObj = listdata->scrollbar;
                Msg.param.point = pMsg->param.point;
                scroll_ret = ( int )scroll->msg_hanlder( &Msg );
                // top , left
                if( scroll_ret < 0 )
                {
                    if( listdata->m_viewStartIndex > 0 )
                    {
                        if(listdata->m_viewStartIndex > viewcnt)
                        {
                            offset = viewcnt;
                        }
                        else
                        {
                            offset = listdata->m_viewStartIndex;
                        }

                        while( item != NULL )
                        {
                            label = ( EGL_OBJECT_PTR )item->label;
                            label->rect.y += ( label->rect.h * offset );
                            item = item->next;
                        }
                        listdata->m_next_item_y += ( label->rect.h * offset );
                        listdata->m_viewStartIndex -= offset;
                        list->bNeedRedraw = TRUE;
                    }
                    break;
                }
                // bottom, right
                else if( scroll_ret > 0 )
                {
                    if( listdata->m_viewStartIndex < listdata->m_listcount - ( list->rect.h / listdata->m_item_h ) )
                    {
                        remain_count = listdata->m_listcount - listdata->m_viewStartIndex - viewcnt;

                        if(remain_count > viewcnt)
                        {
                            offset = viewcnt;
                        }
                        else
                        {
                            offset = remain_count;
                        }

                        while( item != NULL )
                        {
                            label = ( EGL_OBJECT_PTR )item->label;
                            label->rect.y -= ( label->rect.h * offset );
                            item = item->next;
                        }
                        listdata->m_viewStartIndex += offset;
                        listdata->m_next_item_y -= ( label->rect.h * offset );
                        list->bNeedRedraw = TRUE;
                    }
                    break;
                }
            }
        }
        while( item != NULL )
        {
            label = ( EGL_OBJECT_PTR )item->label;
            if( SEL_CHECK( label->rect.x, label->rect.y, ( label->rect.x + label->rect.w ), ( label->rect.y + label->rect.h ), x, y ) )
            {
                if( listdata->m_bMultiple == TRUE )
                {
                    if( item->selected == TRUE )
                    {
                        item->selected = FALSE;
                        listdata->m_selcnt--;
                    }
                    else
                    {
                        item->selected = TRUE;
                        listdata->m_selcnt++;
                    }
                    listdata->m_selfdraw = TRUE;
                    list->bNeedRedraw = TRUE;
                    if( list->cb )
                        list->cb( pMsg->hObj, LIST_CHANGED );
                    break;
                }
                else
                {
                    if( listdata->m_cursel != index )
                    {
                        listdata->m_cursel = index;
                        listdata->m_selfdraw = TRUE;
                        list->bNeedRedraw = TRUE;
                        if( list->cb )
                            list->cb( pMsg->hObj, LIST_CHANGED );
                    }
                    break;
                }
            }
            item = item->next;
            index++;
            if( index == listdata->m_viewStartIndex + viewcnt )
                break;
        }
        break;
    case EGL_MSG_MOVE:
        move_lb( list, pMsg->param.point.x, pMsg->param.point.y );
        break;
    case EGL_MSG_TIMETICK:
    default:
        break;
    }
    return NULL;
}

static LIST_DATA* create_selfdata()
{
    LIST_DATA* listdata = ( LIST_DATA* )malloc( sizeof( LIST_DATA ) );
    if( listdata == NULL )
        return NULL;
    listdata->scrollbar = NULL;
    listdata->m_listcount = 0;
    listdata->m_cursel = -1;
    listdata->m_selcnt = 0;
    listdata->listhead = NULL;
    listdata->listtail = NULL;
    listdata->m_viewStartIndex = 0;
    listdata->m_selfdraw = FALSE;
    return listdata;
}

/*
* GLOBAL API
*/

EGL_HANDLE egl_create_listbox( int x, int y, int w, int h, BOOL bMultiple )
{
    EGL_OBJECT_PTR list;
    LIST_DATA* listdata ;
    EGL_HANDLE scroll = egl_create_scrollbar( x + w - 20, y, 20, h, 1, 1, 1 );
    list = prv_egl_create_object( x, y, w, h );
    if( list == NULL )
    {
        return NULL;
    }
    listdata = create_selfdata();
    if( listdata == NULL )
    {
        prv_egl_delete_object( list );
        return NULL;
    }
    listdata->scrollbar = scroll;
    listdata->m_bMultiple = bMultiple;
    listdata->m_bscrollauto = FALSE;
    listdata->m_listalign = EGL_ALIGN_CENTER;
    listdata->m_x = x + theme_frame_dw();
    listdata->m_y = y + theme_frame_dh();
    listdata->m_w = w - ( theme_frame_dw() * 2 );
    listdata->m_h = h - ( theme_frame_dw() * 2 );
    listdata->m_item_x = x + theme_frame_dw();
    listdata->m_next_item_y = y + theme_frame_dh();
    listdata->m_item_w = w - ( theme_frame_dw() * 2 );
    listdata->m_item_h = list->pFont->h + 6;
    listdata->m_textcolor = 0x0;
    listdata->m_bgcolor = MAKE_COLORREF( 0xff, 0xff, 0xff );
    listdata->m_seltextcolor = MAKE_COLORREF( 0xff, 0xff, 0xff );
    listdata->m_selbgcolor = MAKE_COLORREF( 0x00, 0x00, 0x66 );
    list->selfdata = listdata;
    list->msg_hanlder = list_msg_hanlder;
#if EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE==1
    {
        list->selfsurface = CREATE_SURFACE( w, h, EGL_SCREEN_BPP );
        if( list->selfsurface == NULL )
        {
            delete_selfdata( listdata );
            prv_egl_delete_object( list );
            return FALSE;
        }
    }
#endif
    return ( EGL_HANDLE )list;
}

BOOL egl_listbox_callback( EGL_HANDLE hObj, EVENT_CALLBACK cb )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    pObj->cb = cb;
    return TRUE;
}

void egl_listbox_additem( EGL_HANDLE hObj, const char* text )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    LIST_DATA* listdata = ( LIST_DATA* )pObj->selfdata;
    ListItem* item = ( ListItem* )malloc( sizeof( ListItem ) );
    EGL_HANDLE label;
//  EGL_OBJECT_PTR scroll = ( EGL_OBJECT_PTR )listdata->scrollbar;
    EGL_OBJECT_PTR plabel = ( EGL_OBJECT_PTR )item->label;
    int viewcnt = 0;
    if( item == NULL )
        return ;
    item->selected = FALSE;
    item->next = NULL;
	item->prev = NULL;
    label = egl_create_label( listdata->m_item_x, listdata->m_next_item_y, listdata->m_item_w, listdata->m_item_h, text );
    if( label == NULL )
        return;

    egl_label_set_align(label,listdata->m_listalign);
	
    listdata->m_listcount++;
    listdata->m_next_item_y += listdata->m_item_h;

    item->label = label;
    if( listdata->listhead == NULL )
    {
        listdata->listhead = listdata->listtail = item;
        pObj->bNeedRedraw = TRUE;
        return;
    }

    listdata->listtail->next = item;
    item->prev = listdata->listtail;
    listdata->listtail = item;
    viewcnt = pObj->rect.h / listdata->m_item_h;

    if(listdata->m_bscrollauto) 
    {
        if( listdata->m_listcount > viewcnt )
        {
            if(listdata->m_viewStartIndex != listdata->m_listcount - viewcnt-1)
            {
                egl_scroll_set_upcount( listdata->scrollbar, listdata->m_listcount - viewcnt-1);
                item = listdata->listhead;
                while( item != NULL )
                {
                    plabel = ( EGL_OBJECT_PTR )item->label;
                    plabel->rect.y -= ( plabel->rect.h * (listdata->m_listcount - viewcnt - listdata->m_viewStartIndex - 1));
                    item = item->next;
                }
                listdata->m_viewStartIndex = listdata->m_listcount - viewcnt - 1;
                listdata->m_next_item_y =  plabel->rect.y +  listdata->m_item_h;
            }

            listdata->m_next_item_y -= listdata->m_item_h;

            egl_scroll_set_totalcount( listdata->scrollbar, listdata->m_listcount );
            egl_scroll_set_viewcount( listdata->scrollbar, viewcnt );
            egl_scroll_set_upcount( listdata->scrollbar, egl_scroll_get_upcount(listdata->scrollbar) + 1);
			
            item = listdata->listhead;
            while( item != NULL )
            {
                plabel = ( EGL_OBJECT_PTR )item->label;
                plabel->rect.y -= ( plabel->rect.h);
                item = item->next;
            }
            listdata->m_viewStartIndex += 1;
        }
    }
    else
    {
        if( listdata->m_listcount > viewcnt )
        {
            if(listdata->m_viewStartIndex != listdata->m_listcount - viewcnt-1)
            {
                egl_scroll_set_totalcount( listdata->scrollbar, listdata->m_listcount );
            }
            else
            {
                listdata->m_next_item_y -= listdata->m_item_h;
                egl_scroll_set_totalcount( listdata->scrollbar, listdata->m_listcount );
                egl_scroll_set_viewcount( listdata->scrollbar, viewcnt );
                egl_scroll_set_upcount( listdata->scrollbar, egl_scroll_get_upcount(listdata->scrollbar) + 1);
                item = listdata->listhead;
				
                while( item != NULL )
                {
                    plabel = ( EGL_OBJECT_PTR )item->label;
                    plabel->rect.y -= ( plabel->rect.h);
                    item = item->next;
                }
                listdata->m_viewStartIndex += 1;
            }
        }
    }

    // list redraw
    pObj->bNeedRedraw = TRUE;
}

void egl_listbox_delitem( EGL_HANDLE hObj )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    LIST_DATA* listdata = ( LIST_DATA* )pObj->selfdata;
    ListItem* item = listdata->listtail;
    EGL_OBJECT_PTR plabel;
    EGL_MSG Msg;
    int viewcnt;
    if( item == NULL )
    {
        return;
    }

    viewcnt = pObj->rect.h / listdata->m_item_h;

    if(egl_scroll_get_upcount(listdata->scrollbar) == listdata->m_viewStartIndex && listdata->m_listcount > viewcnt && listdata->m_viewStartIndex != 0)
    {
        egl_scroll_set_totalcount( listdata->scrollbar, listdata->m_listcount );
        egl_scroll_set_viewcount( listdata->scrollbar, viewcnt );
        egl_scroll_set_upcount( listdata->scrollbar, egl_scroll_get_upcount(listdata->scrollbar)-1);
        item = listdata->listhead;
        while( item->next != NULL )
        {
            plabel = ( EGL_OBJECT_PTR )item->label;
            plabel->rect.y += ( plabel->rect.h);
            item = item->next;
        }
        listdata->m_viewStartIndex -= 1;
    }
    else
    {
        listdata->m_next_item_y -= listdata->m_item_h;
    }

    listdata->m_listcount--;

    if( item->prev == NULL )
    {
        // last item
        listdata->listhead = listdata->listtail = NULL;
        // delete label
        Msg.msgID = EGL_MSG_DELETE;
        Msg.hObj = item->label;
        ( ( EGL_OBJECT_PTR )item->label )->msg_hanlder( &Msg );
        free( item );
        pObj->bNeedRedraw = TRUE;
        return;
    }
    else
    {
        item->prev->next = NULL;
        listdata->listtail = item->prev;
        // delete label
        Msg.msgID = EGL_MSG_DELETE;
        Msg.hObj = item->label;
        ( ( EGL_OBJECT_PTR )item->label )->msg_hanlder( &Msg );
        free( item );
    }

    if( listdata->scrollbar != NULL )
    {
        egl_scroll_set_totalcount( listdata->scrollbar, listdata->m_listcount );
        egl_scroll_set_viewcount( listdata->scrollbar, viewcnt );
    }

    // list redraw
    pObj->bNeedRedraw = TRUE;
}

BOOL egl_listbox_delitem_text( EGL_HANDLE hObj, const char* text )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    LIST_DATA* listdata = ( LIST_DATA* )pObj->selfdata;
    ListItem* item = listdata->listhead;
    EGL_OBJECT_PTR label;
    LABEL_DATA* labeldata;
    int viewcnt;
    int index = 0;
    int del_index =  -1;
    EGL_MSG Msg;;

    if( item == NULL )
    {
        return FALSE;
    }

    while( item != NULL )
    {
        label = ( EGL_OBJECT_PTR )item->label;
        labeldata = ( LABEL_DATA* )label->selfdata;

        viewcnt = pObj->rect.h / listdata->m_item_h;
        if( strcmp( text, labeldata->str ) == 0 )
        {
            if(index == listdata->m_listcount-1)
            {
                egl_listbox_delitem(hObj);
                return TRUE;
            }

            if(egl_scroll_get_upcount(listdata->scrollbar) == 0)
            {
                listdata->m_next_item_y -= listdata->m_item_h;
            }
            listdata->m_listcount--;
            del_index = index;
			
			
			if( item == listdata->listhead )
			{
				item->next->prev = NULL;
				listdata->listhead = item->next;		
			}
			else if( item == listdata->listtail )
			{
				item->prev->next = NULL;
				listdata->listtail = item->prev;		
			}
			else
			{
				item->next->prev = item->prev;
				item->prev->next = item->next;
			}
			
            // delete label
            Msg.msgID = EGL_MSG_DELETE;
            Msg.hObj = item->label;
            ( ( EGL_OBJECT_PTR )item->label )->msg_hanlder( &Msg );
            free( item );
            viewcnt = pObj->rect.h / listdata->m_item_h;
            if( listdata->scrollbar != NULL )
            {
                egl_scroll_set_totalcount( listdata->scrollbar, listdata->m_listcount );
                egl_scroll_set_viewcount( listdata->scrollbar, viewcnt );
            }
            break;
        }
        item = item->next;
        index++;
    }
    // item이 삭제되었으면 이후의 item 들의 좌표 변경
    if( del_index >= 0 )
    {
         index = 0;
        item = listdata->listhead;

        while( item != NULL )
        {
            if( index >= del_index )
            {
                label = ( EGL_OBJECT_PTR )item->label;
                label->rect.y -= listdata->m_item_h;
            }
            item = item->next;
            index++;
        }

        if(listdata->m_listcount >= viewcnt && del_index <= viewcnt && listdata->m_viewStartIndex > 0)
        {
            egl_scroll_set_upcount(listdata->scrollbar,egl_scroll_get_upcount(listdata->scrollbar) - 1);
            egl_scroll_set_totalcount( listdata->scrollbar, listdata->m_listcount );
            egl_scroll_set_viewcount( listdata->scrollbar, viewcnt );

            listdata->m_viewStartIndex-=1;

            item = listdata->listhead;
            while( item != NULL )
            {
                label = ( EGL_OBJECT_PTR )item->label;
                label->rect.y += listdata->m_item_h;
                item = item->next;
            }            
        }

        pObj->bNeedRedraw = TRUE;
        return TRUE;
    }
    return FALSE;
}

void egl_listbox_alldelitem( EGL_HANDLE hObj )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    LIST_DATA* listdata = ( LIST_DATA* )pObj->selfdata;
    ListItem* item = listdata->listtail;
    EGL_MSG Msg;	
	
    if( item == NULL )
    {
        // empty item
        return;
    }	
    while( item->prev != NULL )
    {
        listdata->m_listcount--;
        listdata->m_next_item_y -= listdata->m_item_h;
        item->prev->next = NULL;
        listdata->listtail = item->prev;
        // delete label
        Msg.msgID = EGL_MSG_DELETE;
        Msg.hObj = item->label;
        ( ( EGL_OBJECT_PTR )item->label )->msg_hanlder( &Msg );
        free( item );
        item = listdata->listtail;
    }
    // last data
    if( item == listdata->listhead )
    {
        listdata->m_listcount--;
        listdata->m_next_item_y -= listdata->m_item_h;
        listdata->listhead = listdata->listtail = NULL;
        // delete label
        Msg.msgID = EGL_MSG_DELETE;
        Msg.hObj = item->label;
        ( ( EGL_OBJECT_PTR )item->label )->msg_hanlder( &Msg );
        free( item );
    }
	
	//initialize
	listdata->m_next_item_y = listdata->m_y;
	egl_scroll_set_upcount(listdata->scrollbar,0);
	listdata->m_viewStartIndex = 0;
	
    // set item width
    listdata->m_item_w = pObj->rect.w - ( theme_frame_dw() * 2 );
    listdata->m_cursel = -1;	
    // list redraw
    CRITICAL_ENTER();
    pObj->bNeedRedraw = TRUE;
    CRITICAL_EXIT();
    return;
}

const char ** egl_listbox_get_all_itemlist( EGL_HANDLE hObj, int *itemcnt )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    LIST_DATA* listdata = ( LIST_DATA* )pObj->selfdata;
    ListItem* item = listdata->listhead;
    EGL_OBJECT_PTR label;
    LABEL_DATA* labeldata;
    const char **retstr;
    int i = 0;
    int cnt = 0;
    if( listdata->m_bMultiple == TRUE )
        return NULL;
    cnt = listdata->m_listcount ;
    if(  cnt <= 0 )
    {
        *itemcnt = 0;
        return NULL;
    }
    *itemcnt = cnt ;
    retstr = ( const char ** )malloc( cnt * sizeof( char * ) );
    if( retstr == NULL )
        return NULL;
    while( item != NULL )
    {
        label = ( EGL_OBJECT_PTR )item->label;
        labeldata = ( LABEL_DATA* )label->selfdata;
        retstr[i] = labeldata->str;
        item = item->next;
        i++;
    }
    return retstr;
}

const char ** egl_listbox_get_multiple_sel_itemlist( EGL_HANDLE hObj, int *selcnt )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    LIST_DATA* listdata = ( LIST_DATA* )pObj->selfdata;
    ListItem* item = listdata->listhead;
    EGL_OBJECT_PTR label;
    LABEL_DATA* labeldata;
    const char **retstr;
    int i = 0;
    if( listdata->m_bMultiple == FALSE )
        return NULL;
    if( ( *selcnt = listdata->m_selcnt ) <= 0 )
    {
        *selcnt = 0;
        return NULL;
    }
    retstr = ( const char ** )malloc( *selcnt * sizeof( char * ) );
    while( item != NULL )
    {
        label = ( EGL_OBJECT_PTR )item->label;
        labeldata = ( LABEL_DATA* )label->selfdata;
        if( item->selected == TRUE )
        {
            retstr[i] = labeldata->str;
            i++;
        }
        item = item->next;
    }
    return retstr;
}

const char* egl_listbox_get_sel_item( EGL_HANDLE hObj, int *index )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    LIST_DATA* listdata = ( LIST_DATA* )pObj->selfdata;
    ListItem* item = listdata->listhead;
    EGL_OBJECT_PTR label;
    LABEL_DATA* labeldata;
    const char *retstr = NULL;
    int tag = 0;
    if( ( *index = listdata->m_cursel ) < 0 )
    {
        *index = -1;
        return NULL;
    }
    while( item != NULL )
    {
        label = ( EGL_OBJECT_PTR )item->label;
        labeldata = ( LABEL_DATA* )label->selfdata;
        if( tag == *index )
        {
            retstr = labeldata->str;
            break;
        }
        item = item->next;
        tag++;
    }
    return retstr;
}

void egl_listbox_set_bgcolor( EGL_HANDLE hObj, EGL_COLOR clr)
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    LIST_DATA* listdata = ( LIST_DATA* )pObj->selfdata;
    CRITICAL_ENTER();
    listdata->m_bgcolor = clr;
    pObj->bNeedRedraw = TRUE;
    CRITICAL_EXIT();
}

void egl_listbox_set_textcolor( EGL_HANDLE hObj, EGL_COLOR clr)
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    LIST_DATA* listdata = ( LIST_DATA* )pObj->selfdata;
    CRITICAL_ENTER();
    listdata->m_textcolor = clr;
    pObj->bNeedRedraw = TRUE;
    CRITICAL_EXIT();
}

void egl_listbox_set_selbgcolor( EGL_HANDLE hObj, EGL_COLOR clr)
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    LIST_DATA* listdata = ( LIST_DATA* )pObj->selfdata;
    CRITICAL_ENTER();
    listdata->m_selbgcolor = clr;
    pObj->bNeedRedraw = TRUE;
    CRITICAL_EXIT();
}

void egl_listbox_set_seltextcolor( EGL_HANDLE hObj, EGL_COLOR clr)
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    LIST_DATA* listdata = ( LIST_DATA* )pObj->selfdata;
    CRITICAL_ENTER();
    listdata->m_seltextcolor = clr;
    pObj->bNeedRedraw = TRUE;
    CRITICAL_EXIT();
}

// align : EGL_ALIGN_LEFT,EGL_ALIGN_RIGHT,EGL_ALIGN_TOP,EGL_ALIGN_BOTTOM,EGL_ALIGN_CENTER,EGL_ALIGN_MULTI_LINE
void egl_listbox_set_textalign( EGL_HANDLE hObj, int align )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    LIST_DATA* listdata = ( LIST_DATA* )pObj->selfdata;
    ListItem* item = listdata->listhead;    
    listdata->m_listalign = align;
    while( item != NULL )
    {        
		egl_label_set_align((EGL_HANDLE)item->label,align);        
        item = item->next;
    }
    CRITICAL_ENTER();
    pObj->bNeedRedraw = TRUE;
    CRITICAL_EXIT();
}

void egl_listbox_set_scrollwidth( EGL_HANDLE hObj, int width )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    LIST_DATA* listdata = ( LIST_DATA* )pObj->selfdata;
    EGL_OBJECT_PTR pScroll = ( EGL_OBJECT_PTR )listdata->scrollbar;
    ListItem* item = listdata->listhead;
    EGL_OBJECT_PTR pLabel;
    // set scroll width, x
    egl_scroll_set_size( ( EGL_HANDLE )pScroll, width - theme_frame_dw(), pScroll->rect.h );
    pScroll->rect.x = pObj->rect.x + pObj->rect.w - pScroll->rect.w;
    listdata->m_item_w = listdata->m_w - pScroll->rect.w + theme_frame_dw();;
    while( item != NULL )
    {
        pLabel = ( EGL_OBJECT_PTR )item->label;
        pLabel->rect.w = listdata->m_item_w;
        item = item->next;
    }
    CRITICAL_ENTER();
    pObj->bNeedRedraw = TRUE;
    CRITICAL_EXIT();
}

BOOL egl_listbox_change_item_text( EGL_HANDLE hObj, const char* text, const char* changetext )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    LIST_DATA* listdata = ( LIST_DATA* )pObj->selfdata;
    ListItem* item = listdata->listhead;
    EGL_OBJECT_PTR label;
    LABEL_DATA* labeldata;
    int ch_index =  -1;
    if( item == NULL )
    {
        return FALSE;
    }
    while( item != NULL )
    {
        label = ( EGL_OBJECT_PTR )item->label;
        labeldata = ( LABEL_DATA* )label->selfdata;
        if( strcmp( text, labeldata->str ) == 0 )
        {
			free(labeldata->str);
			labeldata->str = (char*)strdup(changetext);            
            ch_index = 1;
            break;
        }
        item = item->next;
    }

    if( ch_index == -1 )
    {
        return FALSE;
    }

    pObj->bNeedRedraw = TRUE;
    return TRUE;

}

BOOL egl_listbox_change_item_num( EGL_HANDLE hObj, int num, const char* changetext )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    LIST_DATA* listdata = ( LIST_DATA* )pObj->selfdata;
    ListItem* item = listdata->listhead;
    EGL_OBJECT_PTR label;
    LABEL_DATA* labeldata;
    int i=0;

    if( item == NULL )
    {
        return FALSE;
    }

    if(num >= listdata->m_listcount)
    {
        return FALSE;
    }

    for(i=0; i<num; i++)
    {
        item = item->next;
        if( item == NULL )
        {
            return FALSE;
        }
    }

    label = ( EGL_OBJECT_PTR )item->label;
    labeldata = ( LABEL_DATA* )label->selfdata;

	free(labeldata->str);
	labeldata->str = (char*)strdup(changetext);
	
    pObj->bNeedRedraw = TRUE;
    return TRUE;
}

void egl_listbox_set_scrollauto( EGL_HANDLE hObj, BOOL b )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    LIST_DATA* listdata = ( LIST_DATA* )pObj->selfdata;

    listdata->m_bscrollauto = b;
}

BOOL egl_listbox_delitem_num( EGL_HANDLE hObj, int num )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )hObj;
    LIST_DATA* listdata = ( LIST_DATA* )pObj->selfdata;
    ListItem* item = listdata->listhead;
    EGL_OBJECT_PTR label;
    LABEL_DATA* labeldata;
    int viewcnt;
    int index = 0;
    int del_index =  -1;
    EGL_MSG Msg;;
    int i=0;

    if( item == NULL || num > listdata->m_listcount - 1 )
    {
        return FALSE;
    }
	
    if( num == listdata->m_listcount - 1)
    {
        egl_listbox_delitem(hObj);
        return TRUE;
    }

    for(i=0; i<num; i++)
    {
        item = item->next;
        if( item == NULL )
        {
            return FALSE;
        }
    }

    label = ( EGL_OBJECT_PTR )item->label;
    labeldata = ( LABEL_DATA* )label->selfdata;
    viewcnt = pObj->rect.h / listdata->m_item_h;

    listdata->m_next_item_y -= listdata->m_item_h;    
    listdata->m_listcount--;
    del_index = num;
	
    if( item == listdata->listhead )
	{
		item->next->prev = NULL;
        listdata->listhead = item->next;		
	}
    else if( item == listdata->listtail )
	{
		item->prev->next = NULL;
        listdata->listtail = item->prev;		
	}
	else
	{
		item->next->prev = item->prev;
		item->prev->next = item->next;
	}
	
    // delete label
    Msg.msgID = EGL_MSG_DELETE;
    Msg.hObj = item->label;
    ( ( EGL_OBJECT_PTR )item->label )->msg_hanlder( &Msg );
    free( item );
    viewcnt = pObj->rect.h / listdata->m_item_h;

    if( listdata->scrollbar != NULL )
    {
        egl_scroll_set_totalcount( listdata->scrollbar, listdata->m_listcount );
        egl_scroll_set_viewcount( listdata->scrollbar, viewcnt );
    }

    // item이 삭제되었으면 이후의 item 들의 좌표 변경
    if( del_index >= 0 )
    {
        index = 0;
        item = listdata->listhead;

        while( item != NULL )
        {
            if( index >= del_index )
            {
                label = ( EGL_OBJECT_PTR )item->label;
                label->rect.y -= listdata->m_item_h;
            }
            item = item->next;
            index++;
        }

        if(listdata->m_listcount >= viewcnt && del_index <= viewcnt && listdata->m_viewStartIndex > 0)
        {
            egl_scroll_set_upcount(listdata->scrollbar,egl_scroll_get_upcount(listdata->scrollbar) - 1);
            egl_scroll_set_totalcount( listdata->scrollbar, listdata->m_listcount );
            egl_scroll_set_viewcount( listdata->scrollbar, viewcnt );

            listdata->m_viewStartIndex-=1;

            item = listdata->listhead;
            while( item != NULL )
            {
                label = ( EGL_OBJECT_PTR )item->label;
                label->rect.y += listdata->m_item_h;
                item = item->next;
            }
            listdata->m_next_item_y += listdata->m_item_h;
        }

        pObj->bNeedRedraw = TRUE;
        return TRUE;
    }
    return FALSE;
}

BOOL egl_release_listbox( EGL_HANDLE hObj )
{
	if(hObj == NULL)
		return FALSE;	
	egl_delete_listbox( hObj );	
	return TRUE;
}
