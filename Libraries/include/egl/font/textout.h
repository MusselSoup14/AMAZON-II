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

typedef struct _tag_draw_text_opt
{
	int align;
	int line_spacing;
	bool wordsplit;
} DRAW_TEXT_IN_BOX_OPT;

int draw_text(EGL_FONT* font,int x, int y, const char* text);
int draw_text_pivot(EGL_FONT* font,int x, int y, const char* text,int pivot);
void draw_text_len(EGL_FONT* font,int x, int y,const char* text, int len);
void draw_text_in_box(EGL_FONT* font,EGL_RECT* pRect,const char* text,int align);
void draw_text_in_box_ex(EGL_FONT* font, EGL_RECT* pRect, const char* text, DRAW_TEXT_IN_BOX_OPT* opt);
int text_width(EGL_FONT* font,const char* str);//return count in pixels
int text_count_in_width(EGL_FONT* font,const char* str,int w);//return count in bytes

#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
