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

	SURFACE* setdrawtarget(SURFACE* surf);
	SURFACE* getdrawtarget();
	SURFACE* createsurface(U32 w,U32 h, U32 bpp);
	SURFACE* createsurface_from(SURFACE* src,U32 option);//if option is 0, make clone
	void releasesuface(SURFACE* surf);
	PALETTE* createpalette(int bpp, int nColors);
	void releasepalette(PALETTE* pal);//called by release_surface
	BOOL drawsurfaceRect(SURFACE* src_surf,int dx, int dy, int sx,int sy,int w,int h);
	BOOL drawsurface(SURFACE* src_surf,int dx,int dy);
	BOOL drawsurfacescalerect(SURFACE* src_surf,int dx, int dy,int dw,int dh,int sx, int sy, int sw, int sh);
	BOOL drawsurfacescale(SURFACE* src_surf,int dx, int dy,int dw,int dh);
	CLIP_RECT drawsetclipwindow(SURFACE* dst, CLIP_RECT* pRect);

#define createsurface  create_surface
#define createsurface_from  create_surface_from
#define releasesurface  release_surface

#define createpalette  create_palette
#define releasepalette  release_palette
#define drawsurfaceRect draw_surface_rect
#define drawsurface draw_surface
#define drawsurfacescalerect  draw_surface_scalerect
#define drawsurfacescale  draw_surface_scale
#define drawsetclipwindow  draw_set_clip_window

	void drawsetrgb( U8 sr, U8 sg, U8 sb );
	void drawgetrgb( U8* dr, U8* dg, U8* db );
	void drawputpixel( int x, int y, U8 r, U8 g, U8 b );
	void drawline( int x1, int y1, int x2, int y2 );
	void line2( int x, int y, int x1, int y2 );
	void xyline3( int x, int y, int x1, int y2, int x3 );
	void yxline( int x, int y, int endy );
	void yxline2( int x, int y, int y1, int x2, int y3 );

	void drawrect( int x, int y, int w, int h );
	void drawrectfill( int x, int y, int w, int h );
	void drawcircle ( int x, int y, int rad );
	void drawcirclefill( int x, int y, int rad );
	void drawellipse( int x, int y, int Xradius, int Yradius );
	void drawellipsefill( int x, int y, int Xradius, int Yradius );
	
#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
