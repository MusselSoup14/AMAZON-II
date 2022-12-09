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
/*  
/	Code Page
/	437   U.S. (OEM)
/   720   Arabic (OEM)
/   1256  Arabic (Windows)
/   737   Greek (OEM)
/   1253  Greek (Windows)
/   1250  Central Europe (Windows)
/   775   Baltic (OEM)
/   1257  Baltic (Windows)
/   850   Multilingual Latin 1 (OEM)
/   852   Latin 2 (OEM)
/   1252  Latin 1 (Windows)
/   855   Cyrillic (OEM)
/   1251  Cyrillic (Windows)
/   866   Russian (OEM)
/   857   Turkish (OEM)
/   1254  Turkish (Windows)
/   858   Multilingual Latin 1 + Euro (OEM)
/   862   Hebrew (OEM)
/   1255  Hebrew (Windows)
/   874   Thai (OEM, Windows)
/   1258  Vietnam (OEM, Windows)
*/

#ifndef CONFIG_CODE_PAGE
#define CONFIG_CODE_PAGE 949
#endif

#define HW_2D_ENGINE
#define EGL_DOUBLE_BUFFER_MODE 1

#define SURFACE_IN_CACHE 0

#ifdef _MSC_VER //for simulator
#undef SURFACE_IN_CACHE
#define SURFACE_IN_CACHE 0
#endif 

#if SURFACE_IN_CACHE
void dcache_invalidate_way();
#define SURFACE_FLUSH() dcache_invalidate_way()
#else
#define SURFACE_FLUSH()	do{}while(0)
#endif 


#define EGL_CONFIG_WINDOW_DEFAULT_BG_COLOR	MAKE_COLORREF(240,240,240)

#define EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE 0
#define EGL_CONFIG_MAX_OBJECT_IN_WINDOW 30
#define EGL_CONFIG_OBJECT_BG_COLOR MAKE_COLORREF(0xff,0xff,0xff)
/*
	Max string length
*/
#define EGL_CONFIG_MAX_TITILE 32

#if ( (EGL_DOUBLE_BUFFER_MODE != 0) && (EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE!=0))
#error "if EGL_DOUBLE_BUFFER_MODE is not 0, EGL_CONFIG_OBJECT_HAVE_SELF_SURFACE must be set 0"
#endif
