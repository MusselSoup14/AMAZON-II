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


#define GETPOINT8(dst,x,y)	(U8*)((U8*)dst->pixels + x + (y * dst->pitch))
#define GETPOINT16(dst,x,y)	(U16*)((U8*)dst->pixels + (x * 2) + (y * dst->pitch))
#define GETPOINT32(dst,x,y)	(U32*)((U8*)dst->pixels + (x * 4) + (y * dst->pitch))

// "c" is not EGL_COLOR type
#define SETPIXEL8(dst,x,y,c)	(*(U8*)((U8*)dst->pixels + x + (y * dst->pitch))=(U8)c)
#define SETPIXEL16(dst,x,y,c)	(*(U16*)((U8*)dst->pixels + (x * 2) + (y * dst->pitch))=(U16)c)
#define SETPIXEL32(dst,x,y,c)	(*(U32*)((U8*)dst->pixels + (x * 4) + (y * dst->pitch))=c)


SURFACE* set_draw_target(SURFACE* surf) ; //set targetsurface
SURFACE* get_draw_target();//get targetsurface
SURFACE* create_surface(U32 w,U32 h, U32 bpp);

#define PIVOT_RIGHT (1)
#define PIVOT_LEFT (1<<1)
#define PIVOT_180 (1<<2)
#define PIVOT_VFLIP (1<<4)
#define PIVOT_HFLIP (1<<5)
SURFACE* create_surface_from(SURFACE* src,U32 option);//if option is 0, make clone
void release_surface(SURFACE* surf);

PALETTE* create_palette(int bpp, int nColors);
void release_palette(PALETTE* pal);//called by release_surface

BOOL draw_surface_rect(SURFACE* src_surf,int dx, int dy, int sx,int sy,int w,int h);
BOOL draw_surface(SURFACE* src_surf,int dx,int dy);
BOOL draw_surface_scalerect(SURFACE* src_surf,int dx, int dy,int dw,int dh,int sx, int sy, int sw, int sh);
BOOL draw_surface_scale(SURFACE* src_surf,int dx, int dy,int dw,int dh);

BOOL draw_surface_rotate ( SURFACE *src, S32 cdx, S32 cdy, S32 ctx, S32 cty,float zoom, U32 degrees );

CLIP_RECT draw_set_clip_window(SURFACE* dst, CLIP_RECT* pRect);
void draw_get_clip_window(SURFACE* dst, CLIP_RECT* pRect);

BOOL intersect_rect(EGL_RECT* drect,EGL_RECT* srect,EGL_RECT* intersect);

#ifdef HW_2D_ENGINE
BOOL set_texture(SURFACE* src);
BOOL draw_texture(SURFACE* src, int x, int y);
BOOL draw_texture_rect( SURFACE *surf, int dx, int dy, int sx, int sy, int w, int h );
BOOL draw_texture_scalerect(SURFACE* surf,int dx, int dy, int dw, int dh,int sx, int sy ,int sw, int sh);
#endif

