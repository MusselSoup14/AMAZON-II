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
#include "object/egl_animation.h"
#include <stdlib.h>

static void mydraw(EGL_OBJECT_PTR pObj)
{	
	ANIMATION_DATA* pAni = (ANIMATION_DATA*)(pObj->selfdata);
	
#if (EGL_DOUBLE_BUFFER_MODE==0) // Single Buffer Mode
	if(pAni->cnt==0)
	{
		static bool recursive=false;
		if(recursive)
			return;
		recursive = true;
		egl_window_redraw_rect(&pObj->rect);
		recursive = false;
		DRAW_SURFACE(pAni->surflist[pAni->curnum],pObj->rect.x,pObj->rect.y);
		pAni->curnum++;
		if(pAni->curnum==pAni->surfcnt)
			pAni->curnum=0;		
	}
	if(pAni->delaycnt)
	{
		pAni->cnt++;
		if(pAni->cnt==pAni->delaycnt)
			pAni->cnt=0;
	}
#else                          // Double Buffer Mode
	if(pAni->cnt==0)
	{
		if(!pAni->delaycnt)
			DRAW_SURFACE(pAni->surflist[pAni->curnum],pObj->rect.x,pObj->rect.y);		
		
		pAni->curnum++;
		if(pAni->curnum==pAni->surfcnt)
			pAni->curnum=0;			
	}
	if(pAni->delaycnt)
	{
		DRAW_SURFACE(pAni->surflist[pAni->curnum],pObj->rect.x,pObj->rect.y);
		pAni->cnt++;
		if(pAni->cnt==pAni->delaycnt)
			pAni->cnt=0;
	}
#endif		
		
	if(pAni->surfcnt>1)
		pObj->bNeedRedraw=TRUE;//for next frame drawing
}

static void* msg_handler(EGL_MSG* pMsg)
{
	EGL_OBJECT_PTR pObj = EGL_HANDLE_TO_OBJECT(pMsg->hObj);
	switch(pMsg->msgID)
	{
	case EGL_MSG_DRAW:
		mydraw(pObj);
		break;
	case EGL_MSG_UNTOUCHED:
		if(pObj->cb)
			pObj->cb(pMsg->hObj,(int)ANI_CLICKED);
		break;
	default:
		break;
	}
	return NULL;
}

EGL_HANDLE egl_create_animation(int x, int y, int w, int h,SURFACE** surflist,int surfcnt,int delaycnt)
{
	ANIMATION_DATA* ani_data ;
	EGL_OBJECT_PTR pObj= prv_egl_create_object(x,y,w,h);
	int i;
	if(pObj==NULL)
		return NULL;
	if(surfcnt==0)
		return NULL;
	if(surflist==NULL)
		return NULL;

	ani_data = (ANIMATION_DATA*)malloc(sizeof(ANIMATION_DATA));
	if(ani_data==NULL)
	{
		prv_egl_delete_object(pObj);
		return NULL;
	}
	ani_data->surflist = (SURFACE**)malloc(sizeof(SURFACE*)*surfcnt);
	if(ani_data->surflist ==NULL)
	{
		prv_egl_delete_object(pObj);
		free(ani_data);
		return NULL;
	}
	ani_data->cnt=0;
	for(i=0;i<surfcnt;i++)
		ani_data->surflist[i]=surflist[i];

	ani_data->surfcnt=surfcnt;
	ani_data->curnum=0;
	ani_data->delaycnt=delaycnt;
	pObj->selfdata = ani_data;
	pObj->msg_hanlder = msg_handler;
	return (EGL_HANDLE)pObj;
}

static void delete_selfdata( void* selfdata )
{
    if( selfdata )
    {	
       free( selfdata );
    }
}

static void egl_delete_animation( EGL_HANDLE hObj )
{
    EGL_OBJECT_PTR pObj;    
    pObj = ( EGL_OBJECT_PTR )hObj;
    ANIMATION_DATA* ani_data = (ANIMATION_DATA*)pObj->selfdata;	
    free(ani_data->surflist);
    delete_selfdata( pObj->selfdata );	
    prv_egl_delete_object( pObj );
}

BOOL egl_release_animation(EGL_HANDLE hObj )
{
	if(hObj == NULL)
		return TRUE;	
	egl_delete_animation( hObj );
	hObj = NULL;	
	return FALSE;
}
