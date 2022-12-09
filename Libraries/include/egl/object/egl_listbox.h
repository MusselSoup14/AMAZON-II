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
#pragma once
#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
extern "C" {
#endif
    typedef enum
    {
        LIST_CHANGED = 0,
        LIST_MAX,
    }
    LIST_EVENT;

    typedef struct _tagListItem
    {
        BOOL selected;
        EGL_HANDLE label;
        void *itemdata;
        struct _tagListItem *prev;
        struct _tagListItem *next;
    } ListItem;


    typedef struct _tag_list_data
    {
        EGL_HANDLE scrollbar;
        int m_scrollbar_width;
        ListItem* listhead;
        ListItem* listtail;
        int m_listcount;
        int m_cursel;			// single mode
        int m_selcnt;			// multiple mode
        BOOL m_bMultiple;
        BOOL m_bscrollauto;
        int m_x;
        int m_y;
        int m_w;
        int m_h;
        int m_item_x;
        int m_next_item_y;
        int m_item_w;
        int m_item_h;
        EGL_COLOR m_bgcolor;
        EGL_COLOR m_textcolor;
        EGL_COLOR m_seltextcolor;
        EGL_COLOR m_selbgcolor;
        BOOL m_selfdraw;
        int m_viewStartIndex;
        int m_listalign;
    } LIST_DATA;

    EGL_HANDLE egl_create_listbox( int x, int y, int w, int h, BOOL bMultiple );
    BOOL egl_listbox_callback( EGL_HANDLE hObj, EVENT_CALLBACK cb );
    void egl_listbox_additem( EGL_HANDLE hObj, const char* text );
    void egl_listbox_delitem( EGL_HANDLE hObj );
    BOOL egl_listbox_delitem_text( EGL_HANDLE hObj, const char* text );
    BOOL egl_listbox_delitem_num( EGL_HANDLE hObj, int num );
    void egl_listbox_alldelitem( EGL_HANDLE hObj );
    const char ** egl_listbox_get_all_itemlist( EGL_HANDLE hObj, int *itemcnt );
    const char* egl_listbox_get_sel_item( EGL_HANDLE hObj, int *index );
    const char ** egl_listbox_get_multiple_sel_itemlist( EGL_HANDLE hObj, int *selcnt );
    void egl_listbox_set_textalign( EGL_HANDLE hObj, int align );
	
    void egl_listbox_set_bgcolor( EGL_HANDLE hObj, EGL_COLOR clr);
    void egl_listbox_set_textcolor( EGL_HANDLE hObj, EGL_COLOR clr);
    void egl_listbox_set_selbgcolor( EGL_HANDLE hObj, EGL_COLOR clr);
    void egl_listbox_set_seltextcolor( EGL_HANDLE hObj, EGL_COLOR clr);
	
    void egl_listbox_set_scrollwidth( EGL_HANDLE hObj, int width );

    void egl_listbox_set_scrollauto( EGL_HANDLE hObj, BOOL b );
    BOOL egl_listbox_change_item_text( EGL_HANDLE hObj, const char* text, const char* changetext );
    BOOL egl_listbox_change_item_num( EGL_HANDLE hObj, int num, const char* changetext );
	BOOL egl_release_listbox( EGL_HANDLE hObj );

#pragma once
#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
