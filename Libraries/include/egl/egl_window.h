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

	void egl_user_key_input( BOOL bKeyDown, U32 val );
	void egl_user_touch_input( BOOL bPressed, EGL_POINT* pt );
	void egl_draw();
	EGL_HANDLE egl_create_window( const char* title );
	BOOL egl_window_show( EGL_HANDLE hWin, BOOL bShow );
	BOOL egl_window_isshow( EGL_HANDLE hWin);
	void egl_window_invalidate();
	void egl_window_invalidate_rect( EGL_RECT* pRect );
	void egl_window_redraw_rect( EGL_RECT* pRect );
	BOOL egl_window_set_bg( EGL_HANDLE hWin, SURFACE* Img );
	SURFACE* egl_window_get_bg( EGL_HANDLE hWin );
	BOOL egl_window_add_object( EGL_HANDLE hWin, EGL_HANDLE hObj );
	BOOL egl_window_delete_object( EGL_HANDLE hWin, EGL_HANDLE hObj );
	void egl_window_set_callback( EGL_HANDLE hWin, EVENT_CALLBACK cb );
	EGL_HANDLE egl_window_get_active();
	bool egl_enable_object(EGL_HANDLE h,bool b);
	void egl_visible_object(EGL_HANDLE h,bool b);
	BOOL egl_release_window(EGL_HANDLE hObj);
#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
