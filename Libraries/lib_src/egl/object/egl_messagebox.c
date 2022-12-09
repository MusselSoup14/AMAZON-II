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
#include "egl_window.h"
#include "font/egl_font.h"
#include "font/textout.h"
#include "egl_private.h"
#include "egl_theme.h"
#include "object/egl_messagebox.h"

typedef struct _tmyMsgBtn
{
	EGL_RECT rect;
	EGL_RECT text_rect;
	EGL_RECT btn_rect[3];
	int type;
	char* str;
} MSBBTN;

int  egl_show_messagebox( const char* text, int flags, BOOL ( *user_input_fuction )( EGL_MSG* pMsg ) )
{
	int x, y, w, h;
	int fx,fy,fw,fh;
	int i;
	int btn_w;
	int btn_h;
	int curpressed = -1;
	int btncnt = 0;

	MSBBTN btn[3];
	EGL_MSG msg;
	EGL_RECT msgbox_rect;
	EGL_RECT text_rect;
	msgbox_rect.x = EGL_SCREEN_WIDTH / 2 - 150;
	msgbox_rect.y = EGL_SCREEN_HEIGHT / 2 - 100;
	msgbox_rect.w = 300;
	msgbox_rect.h = 200;
	//show message box
	x = fx = msgbox_rect.x;
	y = fy = msgbox_rect.y;
	w = fw = msgbox_rect.w;
	h = fh = msgbox_rect.h;
	theme_draw_box( fx, fy, fw, fh, FALSE );
	text_rect.x = x + 10;
	text_rect.y = y + 10;
	text_rect.w = w - 20;
	text_rect.h = ( h - 20 ) / 2;
	draw_text_in_box( egl_sys_font, &text_rect, text, EGL_ALIGN_CENTER );
	btn_h = 30;
	if( flags == MB_OKCANCEL )
	{
		btn_w = ( w - 30 ) / 2;
		btn[0].type = IDOK;
		btn[0].str = "OK";
		btn[1].type = IDCANCEL;
		btn[1].str = "CANCEL";

		btn[0].rect.x = 10 + x;
		btn[1].rect.x = 10 + x + btn_w + 10 ;

		btn[0].rect.y =btn[1].rect.y = y + text_rect.h + 50;
		btn[0].rect.w =btn[1].rect.w = btn_w;
		btn[0].rect.h =btn[1].rect.h = btn_h;

		btncnt = 2;
	}
	else if( flags == MB_YESNO )
	{
		btn_w = ( w - 30 ) / 2;
		btn[0].type = IDYES;
		btn[0].str = "YES";
		btn[1].type = IDNO;
		btn[1].str = "NO";

		btn[0].rect.x = 10 + x;
		btn[1].rect.x = 10 + x + btn_w + 10 ;

		btn[0].rect.y =btn[1].rect.y = y + text_rect.h + 50;
		btn[0].rect.w =btn[1].rect.w = btn_w;
		btn[0].rect.h =btn[1].rect.h = btn_h;

		btncnt = 2;
	}
	else if( flags == MB_YESNOCANCEL )
	{
		btn_w = ( w - 40 ) / 3;
		btn[0].type = IDYES;
		btn[0].str = "YES";
		btn[1].type = IDNO;
		btn[1].str = "NO";
		btn[2].type = IDCANCEL;
		btn[2].str = "CANCEL";

		btn[0].rect.x = 10 + x;
		btn[1].rect.x = 10 + x + btn_w + 10 ;
		btn[2].rect.x = 10 + x + btn_w + 10 + btn_w + 10;

		btn[0].rect.y =btn[1].rect.y =btn[2].rect.y = y + text_rect.h + 50;
		btn[0].rect.w =btn[1].rect.w =btn[2].rect.w = btn_w;
		btn[0].rect.h =btn[1].rect.h =btn[2].rect.h = btn_h;

		btncnt = 3;
	}
	else//if(flags == MB_OK)
	{
		btn_w = w - 20 ;
		btn[0].type = IDOK;
		btn[0].str = "OK";

		btn[0].rect.x = 10 + x;

		btn[0].rect.y = y + text_rect.h + 50;
		btn[0].rect.w = btn_w;
		btn[0].rect.h = btn_h;

		btncnt = 1;
	}
	for( i = 0; i < btncnt; i++ )
	{
		theme_draw_button( btn[i].rect.x, btn[i].rect.y, btn[i].rect.w, btn[i].rect.h, FALSE );
		draw_text_in_box( egl_sys_font, &btn[i].rect, btn[i].str, EGL_ALIGN_CENTER );
	}
#if (EGL_DOUBLE_BUFFER_MODE!=0)
	EGL_FLIP();
#endif
	while( 1 )
	{
		if( user_input_fuction( &msg ) )
		{
			//check point
			if( msg.msgID == EGL_MSG_UNTOUCHED )
			{
				x = msg.param.point.x;
				y = msg.param.point.y;
				for( i = 0; i < btncnt; i++ )
				{
					if( ( x > btn[i].rect.x )  && ( x < ( btn[i].rect.x + btn[i].rect.w ) ) && ( y > ( btn[i].rect.y ) ) && ( y < ( btn[i].rect.y + btn[i].rect.h ) ) )
					{
						//ispressed
						if( i == curpressed )
							break;
					}
				}
				if( i != btncnt )
					break;

#if (EGL_DOUBLE_BUFFER_MODE!=0)
				theme_draw_box( fx, fy, fw, fh, FALSE );
				draw_text_in_box( egl_sys_font, &text_rect, text, EGL_ALIGN_CENTER );
				for( i = 0; i < btncnt; i++ )
				{
					theme_draw_button( btn[i].rect.x, btn[i].rect.y, btn[i].rect.w, btn[i].rect.h, FALSE );
					draw_text_in_box( egl_sys_font, &btn[i].rect, btn[i].str, EGL_ALIGN_CENTER );
				}
				EGL_FLIP();
#else
				if( curpressed != -1 )
				{
					//draw unpressed stat
					theme_draw_button( btn[curpressed].rect.x, btn[curpressed].rect.y, btn[curpressed].rect.w, btn[curpressed].rect.h, FALSE );
					draw_text_in_box( egl_sys_font, &btn[curpressed].rect, btn[curpressed].str, EGL_ALIGN_CENTER );
					curpressed = -1;
				}
#endif
			}
			else if( msg.msgID == EGL_MSG_TOUCHED )
			{
				x = msg.param.point.x;
				y = msg.param.point.y;
				for( i = 0; i < btncnt; i++ )
				{
					if( ( x > btn[i].rect.x )  && ( x < ( btn[i].rect.x + btn[i].rect.w ) ) && ( y > ( btn[i].rect.y ) ) && ( y < ( btn[i].rect.y + btn[i].rect.h ) ) )
					{
						curpressed = i;
#if (EGL_DOUBLE_BUFFER_MODE==0)
						//draw selected button
						theme_draw_button( btn[i].rect.x, btn[i].rect.y, btn[i].rect.w, btn[i].rect.h, TRUE );
						draw_text_in_box( egl_sys_font, &btn[i].rect, btn[i].str, EGL_ALIGN_CENTER );
#endif
						break;
					}
				}
				if(i==btncnt)
					continue;
#if (EGL_DOUBLE_BUFFER_MODE!=0)
				theme_draw_box( fx, fy, fw, fh, FALSE );
				draw_text_in_box( egl_sys_font, &text_rect, text, EGL_ALIGN_CENTER );
				for( i = 0; i < btncnt; i++ )
				{
					if(curpressed !=i)
						theme_draw_button( btn[i].rect.x, btn[i].rect.y, btn[i].rect.w, btn[i].rect.h, FALSE );
					else
						theme_draw_button( btn[i].rect.x, btn[i].rect.y, btn[i].rect.w, btn[i].rect.h, TRUE );
					draw_text_in_box( egl_sys_font, &btn[i].rect, btn[i].str, EGL_ALIGN_CENTER );

				}
				EGL_FLIP();
#endif

			}
		}
	}
	egl_window_invalidate_rect( &msgbox_rect );
	return btn[i].type;
}
