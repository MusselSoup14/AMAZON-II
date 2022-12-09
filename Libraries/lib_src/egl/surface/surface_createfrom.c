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

static SURFACE* _createsurf_copy_internal(SURFACE* src)
{
	SURFACE* surface = create_surface(src->w,src->h,src->bpp);
	if(surface==0)
		return 0;
	surface->pixtype = src->pixtype;
	if(src->pal)
	{
		surface->pal->colorkey = src->pal->colorkey;
		surface->pal->nColors = src->pal->nColors;
		memcpy(surface->pal->colors,src->pal->colors,src->pal->nColors*4);
	}
	return surface;
}
static SURFACE* _createsurf_copy_internal_yx(SURFACE* src)
{
	SURFACE* surface = create_surface(src->h,src->w,src->bpp);
	if(surface==0)
		return 0;
	surface->pixtype = src->pixtype;
	if(src->pal)
	{
		surface->pal->colorkey = src->pal->colorkey;
		surface->pal->nColors = src->pal->nColors;
		memcpy(surface->pal->colors,src->pal->colors,src->pal->nColors*4);
	}
	return surface;
}

static SURFACE* _copy_clone(SURFACE* src)
{
	int y;
	SURFACE* surface = _createsurf_copy_internal(src);
	if(surface==0)
		return 0;	
	for(y=0;y<src->h;y++)
	{
		U8* dest = (U8*)(surface->pixels + (surface->pitch * y));
		U8* srcpixel= (U8*)(src->pixels + (src->pitch*y));
		memcpy(dest,srcpixel,surface->pitch);
	}		
	return surface;
}
static SURFACE* _copy_vflip(SURFACE* src)
{
	int y;
	SURFACE* surface = _createsurf_copy_internal(src);
	if(surface==0)
		return 0;
	for(y=0;y<src->h;y++)
	{
		U8* dest = (U8*)surface->pixels + (surface->pitch * y);
		U8* srcpixel= (U8*)src->pixels + (src->pitch*(src->h-1-y));
		memcpy(dest,srcpixel,surface->pitch);
	}
	return surface;
}
static SURFACE* _copy_hflip(SURFACE* src)
{
	int x;
	int y;
	int bpp = src->bpp/8;
	SURFACE* surface = _createsurf_copy_internal(src);
	if(surface==0)
		return 0;
	
	if(bpp==4)
	{
		for(y=0;y<src->h;y++)
		{
			U32* dest = (U32*)((U8*)surface->pixels + (surface->pitch * y));
			U32* srcpixel= (U32*)((U8*)src->pixels + (src->pitch*y) + (bpp*(src->w-1)));
			for(x=0;x<src->w;x++)
			{
				*dest = *srcpixel;
				dest++;
				srcpixel--;
			}
		}
	}
	else if(bpp==2)
	{
		for(y=0;y<src->h;y++)
		{
			U16* dest = (U16*)((U8*)surface->pixels + (surface->pitch * y));
			U16* srcpixel= (U16*)((U8*)src->pixels + (src->pitch*y) + (bpp*(src->w-1)));
			for(x=0;x<src->w;x++)
			{
				*dest = *srcpixel;
				dest++;
				srcpixel--;
			}
		}
	}
	else
	{
		for(y=0;y<src->h;y++)
		{
			U8* dest = (U8*)surface->pixels + (surface->pitch * y);
			U8* srcpixel= (U8*)src->pixels + (src->pitch*y) + (bpp*(src->w-1));
			for(x=0;x<src->w;x++)
			{
				*dest = *srcpixel;
				dest++;
				srcpixel--;
			}
		}
	}
	return surface;
}
static SURFACE* _copy_right(SURFACE* src)
{
	int i, k;
	int bpp = src->bpp/8;//byte per pixel
	SURFACE* surface = _createsurf_copy_internal_yx(src);
	if(surface==0)
		return 0;
	if(bpp==4)
	{
		for(i=0;i<surface->h;i++)
		{
			U8* srcpixel = (U8*)src->pixels + (src->pitch * (src->h-1)) + (bpp*i);
			U32* dest = (U32*)((U32)surface->pixels + (surface->pitch*i));
			for(k=0;k<surface->w;k++)
			{
				U32 data = *(U32*)srcpixel;
				*dest = data;
				dest++;
				srcpixel -= src->pitch;
			}
		}
	}
	if(bpp==2) //16bit
	{
		for(i=0;i<surface->h;i++)
		{
			U8* srcpixel = (U8*)src->pixels + (src->pitch * (src->h-1)) + (bpp*i);
			U16* dest = (U16*)((U32)surface->pixels + (surface->pitch*i));
			for(k=0;k<surface->w;k++)
			{
				U16 data = *(U16*)srcpixel;
				*dest = data;
				dest++;
				srcpixel -= src->pitch;
			}
		}
	}
	else if(bpp == 1)
	{
		for(i=0;i<surface->h;i++)
		{
			U8* srcpixel = (U8*)src->pixels + (src->pitch * (src->h-1)) + (bpp*i);
			U8* dest = (U8*)((U32)surface->pixels + (surface->pitch*i));
			for(k=0;k<surface->w;k++)
			{
				U8 data = *(U8*)srcpixel;
				*dest = data;
				dest++;
				srcpixel -= src->pitch;
			}
		}
	}
	return surface;
}
static SURFACE* _copy_right_vflip(SURFACE* src)
{
	int i, k;
	int bpp = src->bpp/8;//byte per pixel
	SURFACE* surface = _createsurf_copy_internal_yx(src);
	if(surface==0)
		return 0;
	if(bpp==4)
	{
		for(i=0;i<surface->h;i++)
		{
			U8* srcpixel = (U8*)src->pixels + (src->pitch * src->h) - (bpp*(1+i));
			U32* dest = (U32*)((U32)surface->pixels + (surface->pitch*i));
			for(k=0;k<surface->w;k++)
			{
				U32 data = *(U32*)srcpixel;
				*dest = data;
				dest++;
				srcpixel -= src->pitch;
			}
		}
	}
	if(bpp==2) //16bit
	{
		for(i=0;i<surface->h;i++)
		{
			U8* srcpixel = (U8*)src->pixels + (src->pitch * src->h) - (bpp*(1+i));
			U16* dest = (U16*)((U32)surface->pixels + (surface->pitch*i));
			for(k=0;k<surface->w;k++)
			{
				U16 data = *(U16*)srcpixel;
				*dest = data;
				dest++;
				srcpixel -= src->pitch;
			}
		}
	}
	else if(bpp == 1)
	{
		for(i=0;i<surface->h;i++)
		{
			U8* srcpixel = (U8*)src->pixels + (src->pitch * src->h) - (bpp*(1+i));
			U8* dest = (U8*)((U32)surface->pixels + (surface->pitch*i));
			for(k=0;k<surface->w;k++)
			{
				U8 data = *(U8*)srcpixel;
				*dest = data;
				dest++;
				srcpixel -= src->pitch;
			}
		}
	}
	return surface;
}
static SURFACE*_copy_right_hflip(SURFACE* src)
{
	int i, k;
	int bpp = src->bpp/8;//byte per pixel
	SURFACE* surface = _createsurf_copy_internal_yx(src);
	if(surface==0)
		return 0;
	if(bpp==4)
	{
		for(i=0;i<surface->h;i++)
		{
			U8* srcpixel = (U8*)src->pixels + (bpp*i);
			U32* dest = (U32*)((U32)surface->pixels + (surface->pitch*i));
			for(k=0;k<surface->w;k++)
			{
				U32 data = *(U32*)srcpixel;
				*dest = data;
				dest++;
				srcpixel += src->pitch;
			}
		}
	}
	if(bpp==2) //16bit
	{
		for(i=0;i<surface->h;i++)
		{
			U8* srcpixel = (U8*)src->pixels + (bpp*i);
			U16* dest = (U16*)((U32)surface->pixels + (surface->pitch*i));
			for(k=0;k<surface->w;k++)
			{
				U16 data = *(U16*)srcpixel;
				*dest = data;
				dest++;
				srcpixel += src->pitch;
			}
		}
	}
	else if(bpp == 1)
	{
		for(i=0;i<surface->h;i++)
		{
			U8* srcpixel = (U8*)src->pixels + (bpp*i);
			U8* dest = (U8*)((U32)surface->pixels + (surface->pitch*i));
			for(k=0;k<surface->w;k++)
			{
				U8 data = *(U8*)srcpixel;
				*dest = data;
				dest++;
				srcpixel += src->pitch;
			}
		}
	}
	return surface;
}

static SURFACE*  _copy_left(SURFACE* src)
{
	int i, k;
	int bpp = src->bpp/8;//byte per pixel
	SURFACE* surface = _createsurf_copy_internal_yx(src);
	if(surface==0)
		return 0;
	if(bpp==4)
	{
		for(i=0;i<surface->h;i++)
		{
			U8* srcpixel = (U8*)src->pixels + src->pitch - (bpp*(i+1));
			U32* dest = (U32*)((U32)surface->pixels + (surface->pitch*i));
			for(k=0;k<surface->w;k++)
			{
				U32 data = *(U32*)srcpixel;
				*dest = data;
				dest++;
				srcpixel += src->pitch;
			}
		}
	}
	if(bpp==2)
	{
		for(i=0;i<surface->h;i++)
		{
			U8* srcpixel = (U8*)src->pixels + src->pitch - (bpp*(i+1));
			U16* dest = (U16*)((U32)surface->pixels + (surface->pitch*i));
			for(k=0;k<surface->w;k++)
			{
				U16 data = *(U16*)srcpixel;
				*dest = data;
				dest++;
				srcpixel += src->pitch;
			}
		}
	}
	else if(bpp == 1)
	{
		for(i=0;i<surface->h;i++)
		{
			U8* srcpixel = (U8*)src->pixels + src->pitch - (bpp*(i+1));
			U8* dest = (U8*)((U32)surface->pixels + (surface->pitch*i));
			for(k=0;k<surface->w;k++)
			{
				U8 data = *(U8*)srcpixel;
				*dest = data;
				dest++;
				srcpixel += src->pitch;
			}
		}
	}
	return surface;
}


static SURFACE*  _copy_180(SURFACE* src)
{
	int x;
	int y;
	int bpp = src->bpp/8;
	SURFACE* surface = _createsurf_copy_internal(src);
	if(surface==0)
		return 0;

	if(bpp==4)
	{
		for(y=0;y<src->h;y++)
		{
			U32* dest = (U32*)((U8*)surface->pixels + (surface->pitch * y));
			U32* srcpixel= (U32*)((U8*)src->pixels + (src->pitch*(src->h-1-y)) + (bpp*(src->w-1)));
			for(x=0;x<src->w;x++)
			{
				*dest = *srcpixel;
				dest++;
				srcpixel--;
			}
		}
	}
	else if(bpp == 2)
	{
		for(y=0;y<src->h;y++)
		{
			U16* dest = (U16*)((U8*)surface->pixels + (surface->pitch * y));
			U16* srcpixel= (U16*)((U8*)src->pixels + (src->pitch*(src->h-1-y)) + (bpp*(src->w-1)));
			for(x=0;x<src->w;x++)
			{
				*dest = *srcpixel;
				dest++;
				srcpixel--;
			}
		}
	}
	else //8bit
	{
		for(y=0;y<src->h;y++)
		{
			U8* dest = (U8*)((U8*)surface->pixels + (surface->pitch * y));
			U8* srcpixel= (U8*)((U8*)src->pixels + (src->pitch*(src->h-1-y)) + (bpp*(src->w-1)));
			for(x=0;x<src->w;x++)
			{
				*dest = *srcpixel;
				dest++;
				srcpixel--;
			}
		}
	}
	return surface;
}



SURFACE* createsurface_from(SURFACE* src,U32 option)
{
	SURFACE* surface= 0;
	U8 pivot = option & 0xf;
	U8 flipmode = option&0xf0;


	if(pivot == 0)
	{
		switch(flipmode)
		{
		case 0:
			return _copy_clone(src);
		case PIVOT_HFLIP:
			return _copy_hflip(src);
		case PIVOT_VFLIP:
			return _copy_vflip(src);
		}
	}
	else if(pivot == PIVOT_LEFT)
	{
		switch(flipmode)
		{
		case 0:
			return _copy_left(src);
		case PIVOT_HFLIP:
			return _copy_right_vflip(src);
		case PIVOT_VFLIP:
			return _copy_right_hflip(src);
		}
	}
	else if(pivot == PIVOT_RIGHT)
	{
		switch(flipmode)
		{
		case 0:
			return _copy_right(src);
		case PIVOT_HFLIP:
			return _copy_right_hflip(src);
		case PIVOT_VFLIP:
			return _copy_right_vflip(src);
		}
	}
	else if(pivot == PIVOT_180)
	{
		switch(flipmode)
		{
		case 0:
			return _copy_180(src);
		case PIVOT_HFLIP:
			return _copy_vflip(src);
		case PIVOT_VFLIP:
			return _copy_hflip(src);
		}
	}
	else
	{
		DEBUGPRINTF("Invalid option(%d)\r\n",option);
		return 0;
	}
	return surface;
}
