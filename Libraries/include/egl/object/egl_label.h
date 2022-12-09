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
		LABEL_CLICKED = 0,		
        LABEL_MAX,
    }LABEL_EVENT;

    typedef struct _tag_label_data
    {
        char* str;
        int align;
		EGL_COLOR font_clr;
		BOOL bErase_bg;//redraw back ground, default is FALSE
		BOOL selfrecusive;
    } LABEL_DATA;

    EGL_HANDLE egl_create_label( int x, int y, int w, int h, const char* text );
	BOOL egl_label_callback( EGL_HANDLE hObj, EVENT_CALLBACK cb );
	BOOL egl_label_set_text(EGL_HANDLE h,char* text);
	void egl_label_set_redraw_bg(EGL_HANDLE h,BOOL b);
	void egl_label_set_color(EGL_HANDLE h, EGL_COLOR clr);
	void egl_label_set_align(EGL_HANDLE h, int align);
	BOOL egl_release_label( EGL_HANDLE  hObj );
#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
