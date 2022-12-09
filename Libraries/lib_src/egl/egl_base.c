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
#include "egl_window.h"
#include "egl_theme.h"
#include "font/bitfont.h"

void* egl_sendmsg( EGL_HANDLE h,EGL_MSG_ID msgID)
{
	EGL_MSG Msg;
    EGL_OBJECT_PTR pObj;
	Msg.msgID = msgID;
    //window_msg_handler( pMsg->hWin, pMsg );
    pObj = ( EGL_OBJECT_PTR )h;
    return pObj->msg_hanlder( &Msg);
}

void egl_show_object( EGL_HANDLE h, BOOL bShow )
{
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )h;
    if( pObj == NULL )
        return ;
    if(bShow==FALSE && pObj->bVisible==TRUE && pObj->parentWin==(EGL_WINDOW_PTR)egl_window_get_active())
	{
		egl_window_invalidate_rect(&pObj->rect);
		CRITICAL_ENTER();
	}
	else
	{
		CRITICAL_ENTER();
		pObj->bNeedRedraw = TRUE;
	}
	pObj->bVisible = bShow;
	CRITICAL_EXIT();
}


BOOL egl_init()
{
    if( theme_init() == FALSE )
        return FALSE;
    egl_sys_font = egl_create_default_font();
    if( egl_sys_font == NULL )
        return FALSE;
    return TRUE;
}

/*
void egl_delete_object( EGL_HANDLE handle )
{
  egl_sendmsg(handle,EGL_MSG_DELETE);
}
*/
void egl_object_set_redraw(EGL_HANDLE handle)
{
	EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )handle;
	CRITICAL_ENTER();
	pObj->bNeedRedraw=TRUE;
	CRITICAL_EXIT();
}
