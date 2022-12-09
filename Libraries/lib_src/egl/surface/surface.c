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
#include "sdk.h"

#include "amazon2/tmem.h"

static SURFACE* cur_targt_surface =NULL;
//for sw rendering
SURFACE* ISPM set_draw_target(SURFACE* surf) 
{
	SURFACE* old = cur_targt_surface;
	cur_targt_surface = surf;
	return old;
}
//for sw rendering
SURFACE* ISPM get_draw_target()
{
	ge_wait_cmd_complete();
	return cur_targt_surface;
}

CLIP_RECT draw_set_clip_window(SURFACE* dst, CLIP_RECT* pRect)
{
	CLIP_RECT old = dst->clip;
	dst->clip = *pRect;
	return old;
}
void draw_get_clip_window(SURFACE* dst, CLIP_RECT* pRect)
{
	*pRect = dst->clip;
}

PALETTE* create_palette(int bpp, int nColors)
{
	PALETTE* pal = (PALETTE*)malloc(sizeof(PALETTE));
	if(pal==0)
		return 0;
	memset(pal,0,sizeof(PALETTE));
	if(tmemalign(&pal->tm,4096,(bpp/8)*nColors)==false)
	{
		free(pal);
		return 0;
	}
	pal->colors= (EGL_COLOR*)(pal->tm.offset + g_TMEM_StartAddr);
	pal->bpp=bpp;
	pal->nColors= nColors;
	pal->colorkey=-1;
	return pal;
}

SURFACE* create_surface(U32 w,U32 h, U32 bpp)
{
	if(!( (bpp == 32) || (bpp == 24) ||(bpp == 16) || (bpp == 4)|| (bpp == 8) ))
		return 0;
	SURFACE* surface=NULL;

	if((w==0)||(h==0))
		return NULL;
	if(w>2047)
		return NULL;
	surface = (SURFACE*)malloc( sizeof(SURFACE) );
	if(surface == 0 )
	{
		debugstring("Not enough memory\r\n");
		return 0;
	}
	memset(surface,0,sizeof(SURFACE));
	surface->bpp = bpp;
	surface->w = w;
	surface->h = h;
	surface->clip.x=0;
	surface->clip.y=0;
	surface->clip.endx=w;
	surface->clip.endy=h;
	if(bpp==32)
	{
		surface->pixtype=PIX_FMT_ARGB;
	}
	else if(bpp==24)
	{
		surface->pixtype=PIX_FMT_RGB888;
	}
	else
	{
		if(bpp==16)
			surface->pixtype=PIX_FMT_RGB565;	
		else
		{
			surface->pixtype=PIX_FMT_PALETTE;	
			if(bpp==4)
				surface->pal = create_palette(32,16);
			else
				surface->pal = create_palette(32,256);
			if(surface->pal==0)
			{
				free(surface);
				return 0;
			}
		}

	}
	int i;
	for(i=3;i<0x13;i++)
	{
		if(surface->w <= (1<<i))
		{
			break;
		}
	}
	surface->TileXSize= i-3;
#if 0
	if(bpp==24)
	{
		surface->pitch = (1<<i)*4;
		if(surface->pitch == (2048*4))
			surface->pitch-=4;
	}
	else
	{
		surface->pitch = ((1<<i)*bpp)/8;
		if(surface->pitch/(bpp/8) == 2048)
			surface->pitch-=(bpp/8);
	}

#else
	if(bpp==24)
	{
		surface->pitch = surface->w*4;
	}
	else
		surface->pitch = (surface->w*bpp)/8; 
	//4byte align
	surface->pitch = (surface->pitch +3) & ~3;
#endif
	for(i=3;i<0x13;i++)
	{
		if(surface->h <= (1<<i))
		{
			break;
		}
	}
	surface->TileYSize= i-3;
	U32 texturewidth,textureheight;
	texturewidth =surface->pitch;
	//textureheight =  1<<(surface->TileYSize + 3);
	textureheight =  surface->h;
	if(tmemalign(&surface->tm,128,texturewidth*textureheight)==false)
	{
		if(surface->pal)
		{
			release_palette(surface->pal);
		}
		free(surface);
		return 0;
	}
	surface->pixels =(void*)(g_TMEM_StartAddr + surface->tm.offset);
	return surface;
}

void release_palette(PALETTE* pal)
{
	if(pal==0)
		return ;
	tmemfree(&pal->tm);
	free(pal);
}

void release_surface(SURFACE* surf)
{
	ASSERT(surf!=0);
	if(surf==0)
		return;
	if(surf->pixels)
	{
		tmemfree(&surf->tm);
	}
	if(surf->pal)
	{
		release_palette(surf->pal);
	}
	free(surf);
}

void set_colors(SURFACE* surf,U32* colors,int index, int cnt)
{
	memcpy(surf->pal->colors+index,colors,cnt*4);
}
