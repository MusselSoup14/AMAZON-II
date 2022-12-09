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
#include "egl_config.h"
#include "font/egl_font.h"
#include "font/textout.h"


#define DECLARE_HANDLE(name) struct name##__{int unused;}; typedef struct name##__ *name
    DECLARE_HANDLE( EGL_HANDLE );


    typedef enum enumMSG
    {
        EGL_MSG_DRAW = 0,
        EGL_MSG_DELETE,
        EGL_MSG_FOCUS,
        EGL_MSG_UNFOCUS,
        EGL_MSG_KEY_UP,
        EGL_MSG_KEY_DOWN,
        EGL_MSG_TOUCHED,
        EGL_MSG_UNTOUCHED,
        EGL_MSG_MOVE,
        EGL_MSG_TIMETICK
    } EGL_MSG_ID;

    typedef enum
    {
        EGL_OBJECT_BUTTON = 0,
        EGL_OBJECT_IMAGE,
        EGL_OBJECT_TOGGLE_BUTTON,
        EGL_OBJECT_CHECK_BOX,
        EGL_OBJECT_RADIO_BUTTON,
        EGL_OBJECT_LIST_BOX,
        EGL_OBJECT_LABEL,
        EGL_OBJECT_SLIDER,
        EGL_OBJECT_SCROLLBAR,
        EGL_OBJECT_PROGRESS_BAR,
		EGL_OBJECT_CIRCLE_GAUGE,
		EGL_OBJECT_BAR_GAUGE,
		EGL_OBJECT_PICTURE,
        EGL_OBJEC_MAX
    } EGL_OBJECT_TYPE;

    typedef struct _tagMessage
    {
        EGL_MSG_ID msgID;
        EGL_HANDLE hObj;//receiver
        EGL_HANDLE hWin;//receiver window
        union
        {
            EGL_POINT point;
            U32	key;
        } param;

    } EGL_MSG;


    typedef void ( *USER_CALLBACK )( EGL_HANDLE h );
    typedef void ( *EVENT_CALLBACK )( EGL_HANDLE h, int evt );

    typedef enum
    {
        WINDOW_CREATED = 0,
        WINDOW_ACTIVATED,
        WINDOW_DEACTIVATED,
    } WINDOW_EVENT;

    /*
    Text alignment
    */

#define		EGL_ALIGN_LEFT (1<<0)
#define		EGL_ALIGN_RIGHT (1<<1)
#define		EGL_ALIGN_TOP (1<<2)
#define 	EGL_ALIGN_BOTTOM (1<<3)
#define 	EGL_ALIGN_CENTER (1<<4)
#define		EGL_ALIGN_MULTI_LINE (1<<5)

	struct _tagEGL_WINDOW;

	typedef struct _tagEGL_OBJECT
	{
		EGL_OBJECT_TYPE type;
		struct _tagEGL_WINDOW* parentWin;
		EGL_RECT rect;
		BOOL bNeedRedraw;
		BOOL bEnabled;
		BOOL bVisible;
		EGL_FONT* pFont;
		void* ( *msg_hanlder )( EGL_MSG* pMsg );
		EVENT_CALLBACK	cb;

		SURFACE* selfsurface;

		void* selfdata;//for internal used
	} EGL_OBJECT, *EGL_OBJECT_PTR;

	typedef struct _tagEGL_WINDOW
	{
		char title[EGL_CONFIG_MAX_TITILE];
		EGL_RECT rect;
		BOOL bNeedRedraw;
		EGL_RECT redraw_rect;
		SURFACE* bg;
		EGL_OBJECT_PTR objets[EGL_CONFIG_MAX_OBJECT_IN_WINDOW];
		int object_cnt;
		EVENT_CALLBACK cb;
	} EGL_WINDOW, *EGL_WINDOW_PTR;

    /*
    Global API
    */
    BOOL egl_init();
    void* egl_sendmsg( EGL_HANDLE h,EGL_MSG_ID msgID);
    void egl_show_object( EGL_HANDLE hObj, BOOL bShow );
	void egl_object_set_redraw(EGL_HANDLE handle);
	EGL_FONT* egl_get_font(EGL_HANDLE h);
	EGL_FONT* egl_set_font(EGL_HANDLE h,EGL_FONT* font);

	EGL_FONT* egl_create_default_font();
	EGL_FONT* egl_get_default_font();

#define EGL_HANDLE_TO_OBJECT(handle)	((EGL_OBJECT_PTR)handle)


#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
