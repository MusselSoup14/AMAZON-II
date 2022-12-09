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
#include "font/bitfont.h"

EGL_FONT* egl_sys_font;
void egl_font_set_bkmode(EGL_FONT* font,int mode)
{
	font->set_bkmode(font,mode);
}
EGL_COLOR egl_font_get_bk_color(EGL_FONT* font)
{
	return font->get_bkcolor(font);
}
EGL_COLOR egl_font_set_bk_color(EGL_FONT* font,EGL_COLOR clr)
{
	return font->set_bkcolor(font,clr);
}
EGL_COLOR egl_font_get_color(EGL_FONT* font)
{
	return font->get_color(font);
}
EGL_COLOR egl_font_set_color(EGL_FONT* font,EGL_COLOR clr)
{
	return font->set_color(font,clr);
}
EGL_FONT* egl_create_default_font()
{
	return create_bitfont();
}
EGL_FONT* egl_get_default_font()
{
	return egl_sys_font;
}

EGL_FONT* egl_get_font(EGL_HANDLE h)
{
	EGL_OBJECT_PTR pObj = EGL_HANDLE_TO_OBJECT(h);
	return pObj->pFont;
}
EGL_FONT* egl_set_font(EGL_HANDLE h, EGL_FONT* font)
{
	EGL_FONT* old;
	EGL_OBJECT_PTR pObj = EGL_HANDLE_TO_OBJECT(h);
	CRITICAL_ENTER();
	old = pObj->pFont;
	pObj->pFont = font;
	CRITICAL_EXIT();
	return old;
}
