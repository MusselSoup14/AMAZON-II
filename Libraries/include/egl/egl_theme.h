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
extern "C"{
#endif

	BOOL theme_init();
	EGL_COLOR theme_get_ctrl_color();
	EGL_COLOR theme_get_ctrl_text_color();
	EGL_COLOR theme_get_wnd_bg_color();

	void theme_draw_frame(int x, int y, int w, int h); //if only frame, call this
	void theme_draw_box(int x, int y, int w, int h,BOOL selected); 
	void theme_draw_button(int x, int y, int w, int h,BOOL selected);
	void theme_draw_scroll_thumb(int x, int y, int w, int h,BOOL vertical,BOOL selected);
	void theme_draw_up_arrow_btn(int x, int y, int w, int h,BOOL selected);
	void theme_draw_down_arrow_btn(int x, int y, int w, int h,BOOL selected);
	void theme_draw_left_arrow_btn(int x, int y, int w, int h,BOOL selected);
	void theme_draw_right_arrow_btn(int x, int y, int w, int h,BOOL selected);
	void theme_draw_radio_btn(int x, int y, int w, int h,BOOL selected);
	void theme_draw_check_btn(int x, int y, int w, int h,BOOL selected);
	int theme_frame_dw();
	int theme_frame_dh();


#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
