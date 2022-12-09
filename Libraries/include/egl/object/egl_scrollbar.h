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
        SCBAR_CLICKED = 0,
        SCBAR_MAX,
    }
    SCROLLBAR_EVENT;

    typedef struct _tag_scbar_data
    {
        EGL_HANDLE label;
        BOOL m_bVerticalMode;
        int m_pos;
        int m_minpos;
        int m_maxpos;

        int m_totalcount;
        int m_viewcount;
        int m_upcount;

        EGL_COLOR m_thumb_color;
        EGL_COLOR m_bg_color;

        int m_left_box_h;
        int m_left_box_w;

        int m_right_box_h;
        int m_right_box_w;

        int m_center_box_h;
        int m_center_box_w;

        int m_thumb_box_x;
        int m_thumb_box_y;
        int m_thumb_box_h;
        int m_thumb_box_w;
    } SCBAR_DATA;

    EGL_HANDLE egl_create_scrollbar( int x, int y, int w, int h, int totalcount, int viewcount, BOOL bVertical );
    BOOL egl_scrollbar_callback( EGL_HANDLE hObj, EVENT_CALLBACK cb );

    void egl_scroll_set_position( EGL_HANDLE hObj, int totalcount, int viewcount, int upcount );
    void egl_scroll_get_position( EGL_HANDLE hObj, int* totalcount, int* viewcount, int* upcount );
    void egl_scroll_set_totalcount( EGL_HANDLE hObj, int totalcount );
    int egl_scroll_get_totalcount( EGL_HANDLE hObj );
    void egl_scroll_set_viewcount( EGL_HANDLE hObj, int viewcount );
    int egl_scroll_get_viewcount( EGL_HANDLE hObj );
    void egl_scroll_set_upcount( EGL_HANDLE hObj, int upcount );
    int egl_scroll_get_upcount( EGL_HANDLE hObj );
    void egl_scroll_set_bgcolor( EGL_HANDLE hObj, unsigned char r, unsigned char g, unsigned char b );
    void egl_scroll_set_size( EGL_HANDLE hObj, int w, int h );
	BOOL egl_release_scrollbar( EGL_HANDLE hObj );
#define SCBAR_HORIZONTAL 0
#define SCBAR_VERTICAL 1

#pragma once
#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif

