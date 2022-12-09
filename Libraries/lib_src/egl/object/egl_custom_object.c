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
#include <stdlib.h>

static void* mymsg_handler( EGL_MSG* pMsg )
{
    void* ( *msg_handler )( EGL_MSG * pMsg );
    EGL_OBJECT_PTR pObj = ( EGL_OBJECT_PTR )pMsg->hObj;
    msg_handler = ( void * ( * )( EGL_MSG * pMsg ) )pObj->selfdata;
    if( pMsg->msgID != EGL_MSG_DELETE )
    {
        return msg_handler( pMsg ); //call user function
    }
    msg_handler( pMsg ); //call user function
    if( pMsg->msgID == EGL_MSG_DELETE )
    {
        prv_egl_delete_object( pObj );
    }
    return NULL;
}

EGL_HANDLE egl_create_custom_object( int x, int y, int w, int h, void * ( *msg_handler )( EGL_MSG* pMsg ) )
{
    EGL_OBJECT_PTR pOjb ;
    if( msg_handler == NULL )
        return NULL;
    pOjb = prv_egl_create_object( x, y, w, h );
    pOjb->msg_hanlder = mymsg_handler;
    pOjb->selfdata = msg_handler;
    return ( EGL_HANDLE )pOjb;
}
