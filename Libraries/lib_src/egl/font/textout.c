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
#include "font/textout.h"

#include <stdlib.h>
#include <string.h>


int draw_text(EGL_FONT* font,int x, int y, const char* text)
{
	return font->draw(font,x,y,text);
}

int draw_text_pivot(EGL_FONT* font,int x, int y, const char* text,int pivot)
{
	return font->draw_pivot(font,x,y,text,pivot);
}

void draw_text_len(EGL_FONT* font,int x, int y,const char* text, int len)
{
	font->draw_len(font,x,y,text,len);
}
//return count in pixels
int text_width(EGL_FONT* font,const char* str)
{
	return font->text_width(font,str);
}
int text_count_in_width(EGL_FONT* font,const char* str,int w)
{
	return font->text_count_in_width(font,str,w);
}
