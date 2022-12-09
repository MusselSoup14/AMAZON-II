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

typedef struct _tagEGL_Font 
{
	int (*draw)(struct _tagEGL_Font* pFont,int x, int y,const char* str);
	int (*draw_pivot)(struct _tagEGL_Font* pFont,int x, int y,const char* str,int pivot);//PIVOT_RIGHT or PIVOT_LEFT
	void (*draw_len)(struct _tagEGL_Font* pFont,int x, int y,const char* str,int len);
	int (*text_width)(struct _tagEGL_Font* pFont,const char* str);//return count in pixels
	int (*text_count_in_width)(struct _tagEGL_Font* pFont,const char* str,int w);//return byte count, not character count
	EGL_COLOR (*set_color)(struct _tagEGL_Font* pFont,EGL_COLOR color);
	EGL_COLOR (*set_bkcolor)(struct _tagEGL_Font* pFont,EGL_COLOR color);
	EGL_COLOR (*get_color)(struct _tagEGL_Font* pFont);
	EGL_COLOR (*get_bkcolor)(struct _tagEGL_Font* pFont);
	void (*set_bkmode)(struct _tagEGL_Font* pFont,int mode);
	int h;//read only
	void* ext;//used by internal function
}EGL_FONT;

EGL_FONT* egl_create_default_font();//called by ut_init()
EGL_FONT* egl_create_font_from_font(EGL_FONT* font);//return clone from exist font
void egl_font_set_bkmode(EGL_FONT* font,int mode);
EGL_COLOR egl_font_set_bk_color(EGL_FONT* font,EGL_COLOR clr);
EGL_COLOR egl_font_get_color(EGL_FONT* font);
EGL_COLOR egl_font_set_color(EGL_FONT* font,EGL_COLOR clr);


#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
