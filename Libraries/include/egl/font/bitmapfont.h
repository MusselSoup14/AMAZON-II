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

EGL_FONT *create_bmpfont ( const char *fname );
void release_bmpfont(EGL_FONT* pFont);

int bmpfont_draw (EGL_FONT* pFont, int x, int y, const char *str );
int bmpfont_draw_vleft( EGL_FONT* font,int x, int y, const char *text );
int bmpfont_draw_vright( EGL_FONT* font,int x, int y, const char *text );

/* API, Only for bitmap-font */
typedef enum
{
	NONE,
	UTF8,
	UTF16
} EFontTextEncoding;
void bmpfont_settextencoding (EGL_FONT* pFont, EFontTextEncoding encoding );

bool bmpfont_setkerning( EGL_FONT* pFont, int k );
SURFACE* bmpfont_makesurface( EGL_FONT* pFont,char* text );

bool bmpfont_setautokerning (EGL_FONT* pFont,bool b );


#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
