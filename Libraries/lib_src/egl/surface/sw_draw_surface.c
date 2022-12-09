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
#include "egl_typedef.h"
#include <math.h>
/*
return true, if coordinate is in clip-windows, otherwise return false
*/
static bool check_clipwindow(SURFACE* dsrf,int* dx, int* dy,int* sx,int* sy,int* w,int* h)
{
	int diff;
	if(*dx < dsrf->clip.x)
	{
		diff = dsrf->clip.x - *dx;
		*dx = dsrf->clip.x;
		*sx += diff;
		*w -= diff;
		if(*w<=0)
			return false;
	}
	if(*dy < dsrf->clip.y)
	{
		diff = dsrf->clip.y - *dy;
		*dy=dsrf->clip.y;
		*sy+=diff;
		*h -= diff;
		if(*h<=0)
			return false;
	}
	if((*dx + *w)>dsrf->clip.endx)
	{
		diff = (*dx + *w) - dsrf->clip.endx;
		*w -=diff;
		if(*w<=0)
			return false;
	}
	if((*dy + *h)>dsrf->clip.endy)
	{
		diff = (*dy + *h) - dsrf->clip.endy ;
		*h -=diff;
		if(*h<=0)
			return false;
	}
	return true;
}

/*
RGB888 ==> RGB888 or ARGB ==> RGB888
*/
static BOOL blit32to32( U8* dest, SURFACE* dsrf, U8* src, SURFACE* ssrf, int w, int h )
{
	register int x, y;
	register U8 r, g, b, a;
	register U32 bg_r, bg_g, bg_b;
    int spitch, dpitch;
	U32* argbbuf;

    spitch = ssrf->pitch;
    dpitch = dsrf->pitch;
	if( ssrf->bpp == 32 )
	{
		if( ssrf->pixtype == PIX_FMT_ARGB )
		{
			for( y = 0; y < h; y++ )
			{
				argbbuf = ( U32* )( ( U32 )src + ( spitch * y ) );
				rRGB* rrgbbuf = ( rRGB* )( ( U32 )dest + ( dpitch * y ) );
				for( x = 0; x < w; x++ )
				{
					a = argbbuf[x] >> 24;
					if( a )
					{
						b = argbbuf[x] >> 0;
						g = argbbuf[x] >> 8;
						r = argbbuf[x] >> 16;
						if( a != 255 )
						{
							a = 255 - a;
							bg_r = rrgbbuf[x].rrgb.r;
							bg_g = rrgbbuf[x].rrgb.g;
							bg_b = rrgbbuf[x].rrgb.b;
							r += ( ( ( bg_r - ( int )r ) * a ) >> 8 );
							g += ( ( ( bg_g - ( int )g ) * a ) >> 8 );
							b += ( ( ( bg_b - ( int )b ) * a ) >> 8 );
						}
						rrgbbuf[x] = ( rRGB )MAKE_RGB888( r, g, b );
					}
				}
			}
			dcache_invalidate_way();
			return TRUE;
		}
		else
		{
			DEBUGPRINTF( "Not Supported format yet(Source %dbpp, Destination %d)\n", ssrf->bpp, dsrf->bpp );
			return FALSE;
		}
	}
	else if( ssrf->bpp == 16 )
	{
		for( y = 0; y < h; y++ )
		{
			RGB565* buf565 = ( RGB565* )( ( U32 )src + ( ssrf->pitch * y ) );
			rRGB* rrgbbuf = ( rRGB* )( ( U32 )dest + ( dsrf->pitch * y ) );
			for( x = 0; x < w; x++ )
			{
				r = buf565[x].r;
				g = buf565[x].g;
				b = buf565[x].b;
				rrgbbuf[x] = ( rRGB )MAKE_RGB888( r, g, b );
			}
		}
		dcache_invalidate_way();
		return TRUE;
	}
	else if( ssrf->bpp == 8 )
	{
		if( ssrf->pal == 0 )
			return FALSE;
		sARGB* pal = ( sARGB* )ssrf->pal->colors;
		for( y = 0; y < h; y++ )
		{
			U8* indexbuf = ( U8* )( ( U32 )src + ( ssrf->pitch * y ) );
			rRGB* rrgbbuf = ( rRGB* )( ( U32 )dest + ( dsrf->pitch * y ) );
			if( ssrf->pixtype == PIX_FMT_A000 )
			{
				for( x = 0; x < w; x++ )
				{
					a = indexbuf[x];
					if( a )
					{
						r = pal->r;
						g = pal->g;
						b = pal->b;
						if( a != 255 ) //
						{
							//a = 255-a;
							bg_r = rrgbbuf[x].rrgb.r;
							bg_g = rrgbbuf[x].rrgb.g;
							bg_b = rrgbbuf[x].rrgb.b;
							r += ( ( ( bg_r - ( int )r ) * a ) >> 8 );
							g += ( ( ( bg_g - ( int )g ) * a ) >> 8 );
							b += ( ( ( bg_b - ( int )b ) * a ) >> 8 );
						}
						rrgbbuf[x].data32 = MAKE_RGB888( r, g, b );
					}
				}
			}
			else
			{
				for( x = 0; x < w; x++ )
				{
					int index = indexbuf[x];
					if( index != ssrf->pal->colorkey )
						rrgbbuf[x].data32 = MAKE_RGB888( pal[index].r, pal[index].g, pal[index].b );
				}
			}
		}
		dcache_invalidate_way();
		return TRUE;
	} 
	return FALSE;
}

/*
	ARGB ==> RGB565
*/
static BOOL blit32to16( U8* dest, SURFACE* dsrf, U8* src, SURFACE* ssrf, int w, int h )
{
    register int x, y;
	register U8 r, g, b, a;
	register U32 bg_r, bg_g, bg_b;
    int spitch, dpitch;
	U32* argbbuf;
	U16* bg16buf;

    spitch = ssrf->pitch;
    dpitch = dsrf->pitch;
    if( ssrf->pixtype == PIX_FMT_ARGB )
    {
        for( y = 0; y < h; y++ )
        {
            argbbuf = ( U32* )( ( U32 )src + ( spitch * y ) );
            bg16buf = ( U16* )( ( U32 )dest + ( dpitch * y ) );
            for( x = 0; x < w; x++ )
            {
                a = argbbuf[x] >> 24;
                if( a )
                {
                    b = argbbuf[x] >> 0;
                    g = argbbuf[x] >> 8;
                    r = argbbuf[x] >> 16;

                    if( a != 255 ) //
                    {
                        a = 255 - a;
                        U16 bg_rgb = bg16buf[x];
                        bg_r = ( bg_rgb & 0xf800 ) >> 8;
                        bg_g = ( bg_rgb & 0x07e0 ) >> 3;
                        bg_b = ( bg_rgb & 0x01f ) << 3;
                        r += ( ( ( bg_r - ( int )r ) * a ) >> 8 );
                        g += ( ( ( bg_g - ( int )g ) * a ) >> 8 );
                        b += ( ( ( bg_b - ( int )b ) * a ) >> 8 );
                    }
                    bg16buf[x] = MAKE_RGB565( r, g, b );
                }
            }
        }
        dcache_invalidate_way();
        return TRUE;
    }//ssrf->pixtype == PIX_FMT_ARGB
    else if( ssrf->pixtype == PIX_FMT_RGB888 )
    {
        for( y = 0; y < h; y++ )
        {
            rRGB* rgbbuf = ( rRGB* )( ( U32 )src + ( spitch * y ) );
            U16* bg16buf = ( U16* )( ( U32 )dest + ( dpitch * y ) );
            for( x = 0; x < w; x++ )
            {
                r = rgbbuf[x].rrgb.r;
                g = rgbbuf[x].rrgb.g;
                b = rgbbuf[x].rrgb.b;
                bg16buf[x] = MAKE_RGB565( r, g, b );
            }
        }
        dcache_invalidate_way();
        return TRUE;
    }//ssrf->pixtype == PIX_FMT_RGB888
    else
    {
        DEBUGPRINTF( "Not Supported format (Source %dbpp, Destination %d)\n", ssrf->bpp, dsrf->bpp );
        return FALSE;
    }
}
static BOOL blit8to16(U8* dest, SURFACE* dsrf, U8* src, SURFACE* ssrf, int w, int h )
{
	register int x;
	register int y;
	register sARGB* pal ;
	register U8 r, g, b,a;
	register U32 bg_r, bg_g, bg_b;
	register int spitch, dpitch;

	if( ssrf->pal == 0 )
		return FALSE;
	spitch = ssrf->pitch;
	dpitch = dsrf->pitch;

	pal = ( sARGB* )ssrf->pal->colors;
	for( y = 0; y < h; y++ )
	{
		U8* indexbuf = ( U8* )( ( U32 )src + ( spitch * y ) );
		U16* bg16buf = ( U16* )( ( U32 )dest + ( dpitch * y ) );
		if( ssrf->pixtype == PIX_FMT_A000 ) //first palette is used for color
		{
			r = pal->r;
			g = pal->g;
			b = pal->b;
			U16 rgb565 = MAKE_RGB565( r, g, b );
			for( x = 0; x < w; x++ )
			{
				a = indexbuf[x];
				if( a )
				{
					if( a != 255 ) //
					{
						//a = 255-a;
						U16 bg_rgb = bg16buf[x];
						bg_r = ( bg_rgb & 0xf800 ) >> 8;
						bg_r |= 0x7;
						bg_g = ( bg_rgb & 0x07e0 ) >> 3;
						bg_g |= 0x3;
						bg_b = ( bg_rgb & 0x01f ) << 3;
						bg_b |= 0x3;
						r += ( ( ( bg_r - ( int )r ) * a ) >> 8 );
						g += ( ( ( bg_g - ( int )g ) * a ) >> 8 );
						b += ( ( ( bg_b - ( int )b ) * a ) >> 8 );
						bg16buf[x] = MAKE_RGB565(r,g,b);
					}
					else
						bg16buf[x] = rgb565;
				}
			}
		}
		else
		{
			for( x = 0; x < w; x++ )
			{
				int index = indexbuf[x];
				if( index != ssrf->pal->colorkey )
					bg16buf[x] = MAKE_RGB565( pal[index].r, pal[index].g, pal[index].b );
			}
		}
	}
	dcache_invalidate_way();
	return TRUE;
}
static BOOL blit8to32(U8* dest, SURFACE* dsrf, U8* src, SURFACE* ssrf, int w, int h )
{
	register int x;
	register int y;
	register sARGB* pal ;
	register U8 r, g, b,a;
	register U32 bg_r, bg_g, bg_b;
	register int spitch, dpitch;

	if( ssrf->pal == 0 )
		return FALSE;
	spitch = ssrf->pitch;
	dpitch = dsrf->pitch;

	pal = ( sARGB* )ssrf->pal->colors;
	for( y = 0; y < h; y++ )
	{
		U8* indexbuf = ( U8* )( ( U32 )src + ( spitch * y ) );
		U32* bg32buf = ( U32* )( ( U32 )dest + ( dpitch * y ) );
		if( ssrf->pixtype == PIX_FMT_A000 ) //first palette is used for color
		{
			r = pal->r;
			g = pal->g;
			b = pal->b;
			U16 rgb = MAKE_RGB888( r, g, b );
			for( x = 0; x < w; x++ )
			{
				a = indexbuf[x];
				if( a )
				{
					if( a != 255 ) //
					{
						//a = 255-a;
						U32 bg_rgb = bg32buf[x];
						EXTRACT_RGB(bg_rgb,bg_r,bg_g,bg_b);
						r += ( ( ( bg_r - ( int )r ) * a ) >> 8 );
						g += ( ( ( bg_g - ( int )g ) * a ) >> 8 );
						b += ( ( ( bg_b - ( int )b ) * a ) >> 8 );
						bg32buf[x] = MAKE_RGB888(r,g,b);
					}
					else
						bg32buf[x] = rgb;
				}
			}
		}
		else
		{
			for( x = 0; x < w; x++ )
			{
				int index = indexbuf[x];
				if( index != ssrf->pal->colorkey )
					bg32buf[x] = MAKE_RGB888( pal[index].r, pal[index].g, pal[index].b );
			}
		}
	}
	dcache_invalidate_way();
	return TRUE;
}

static BOOL blit4to16(U8* dest, SURFACE* dsrf, U8* src, SURFACE* ssrf, int w, int h )
{
	register int x;
	register int y;
	register sARGB* pal ;	
	register int spitch, dpitch;

	if( ssrf->pal == 0 )
		return FALSE;
	spitch = ssrf->pitch;
	dpitch = dsrf->pitch;	
	pal = ( sARGB* )ssrf->pal->colors;
	for( y = 0; y < h; y++ )
	{
		U8* indexbuf = ( U8* )( ( U32 )src + ( spitch * y ) );
		U16* bg16buf = ( U16* )( ( U32 )dest + ( dpitch *  y ) );

		for( x = 0; x < w/2; x++ )
		{
			int index1 = (indexbuf[x] & 0xf0) >> 4;
			int index2 = indexbuf[x] & 0xf;

			if( index1 != ssrf->pal->colorkey )
				bg16buf[x*2] = MAKE_RGB565( pal[index1].r, pal[index1].g, pal[index1].b );						
			if( index2 != ssrf->pal->colorkey )
				bg16buf[x*2+1] = MAKE_RGB565( pal[index2].r, pal[index2].g, pal[index2].b );			
		}		
	}
	dcache_invalidate_way();
	return TRUE;
}

static BOOL blit4to32(U8* dest, SURFACE* dsrf, U8* src, SURFACE* ssrf, int w, int h )
{
	register int x;
	register int y;
	register sARGB* pal ;	
	register int spitch, dpitch;

	if( ssrf->pal == 0 )
		return FALSE;
	spitch = ssrf->pitch;
	dpitch = dsrf->pitch;
	pal = ( sARGB* )ssrf->pal->colors;
	for( y = 0; y < h; y++ )
	{		
		U8* indexbuf = ( U8* )( ( U32 )src + ( spitch * y ) );
		U32* bg32buf = ( U32* )( ( U32 )dest + ( dpitch * y ) );

		for( x = 0; x < w/2; x++ )
		{						
			int index1 = (indexbuf[x] & 0xf0) >> 4;
			int index2 = indexbuf[x] & 0xf;

			if( index1 != ssrf->pal->colorkey )
				bg32buf[x*2] = MAKE_RGB888( pal[index1].r, pal[index1].g, pal[index1].b );						
			if( index2 != ssrf->pal->colorkey )
				bg32buf[x*2+1] = MAKE_RGB888( pal[index2].r, pal[index2].g, pal[index2].b );						
		}		
	}
	dcache_invalidate_way();
	return TRUE;
}
/*
ignore destination alpha value
*/
BOOL sw_draw_surface_rect( SURFACE* ssrf, int dx, int dy, int sx, int sy, int w, int h )
{
	U8* src;
	U8* dest;
	SURFACE* dsrf = get_draw_target();
	if( dy >= dsrf->h )
		return FALSE;
	if( dx >= dsrf->w )
		return FALSE;
	if( ( dsrf->w - dx ) < w )
		w = dsrf->w - dx;
	if( ( dsrf->h - dy ) < h )
		h = dsrf->h - dy;
	if( !( ( dsrf->bpp == 16 ) || ( dsrf->bpp == 32 ) ) )
		return FALSE;
	if( check_clipwindow( dsrf, &dx, &dy, &sx, &sy, &w, &h ) == false )
		return FALSE;
	src = ( U8* )( ( ( U32 )ssrf->pixels ) + ( ssrf->pitch * sy ) + ( sx * ( ssrf->bpp / 8 ) ) );
	dest = ( U8* )( ( ( U32 )dsrf->pixels ) + ( dsrf->pitch * dy ) + ( dx * ( dsrf->bpp / 8 ) ) );
	if( ( dsrf->bpp == ssrf->bpp ) && ( dsrf->pixtype == ssrf->pixtype ) )
	{
		dcache_invalidate_way();
		dma_blockcpy( dest, src, dsrf->pitch, ssrf->pitch, w * ( dsrf->bpp / 8 ), h );
		return TRUE;
	}
	else
	{
		if( dsrf->bpp == 16 )
		{
			if( ssrf->bpp == 32 )
			{
				return blit32to16( dest, dsrf, src, ssrf, w, h );
			}//ssrf->bpp==32
			else if( ssrf->bpp == 8 )
			{
				return blit8to16(dest, dsrf, src, ssrf, w, h );
			} // end ssrf->bpp==8
			else if( ssrf->bpp == 4 )
			{
				return blit4to16(dest, dsrf, src, ssrf, w, h );
			} // end ssrf->bpp==8
		}
		else if( dsrf->bpp == 32 )
		{
			if(dsrf->pixtype == PIX_FMT_RGB888 )
				return blit32to32(dest, dsrf, src, ssrf, w, h );
			else if(ssrf->bpp==8)
				return blit8to32(dest, dsrf, src, ssrf, w, h );
			else if(ssrf->bpp==4)
				return blit4to32(dest, dsrf, src, ssrf, w, h );
		}
	}
	DEBUGPRINTF( "Not Supported format yet(Source %dbpp, Destination %d)\n", ssrf->bpp, dsrf->bpp );
	return FALSE;
}

BOOL sw_draw_surface( SURFACE* src_surf, int dx, int dy )
{
	return sw_draw_surface_rect( src_surf, dx, dy, 0, 0, src_surf->w, src_surf->h );
}


static BOOL ScaleLine(int dx,int dy,int dw,SURFACE* srcf,int sx,int sy, int sw)
{
	int NumPixels = dw;
	int IntPart = sw / dw;
	int FractPart = sw % dw;
	int E = 0;
	U8 a,r,g,b;

	SURFACE* targetsurface = get_draw_target();	
	if(targetsurface->bpp==16)
	{
		U16* dest = (U16*)(((U32)targetsurface->pixels)+(targetsurface->pitch*dy)+(dx*2));
		if(srcf->bpp==16)
		{
			U16* src = (U16*)(((U32)srcf->pixels)+(srcf->pitch*sy)+(sx*srcf->bpp/8));
			while (NumPixels-- > 0) {
				*dest=*src;
				dest++;
				src += IntPart;
				E += FractPart;
				if (E >= dw) {
					E -= dw;
					src++;
				} /* if */
			} /* while */
			return TRUE;
		}
		else if(srcf->bpp==32)
		{
			U32* src = (U32*)(((U32)srcf->pixels)+(srcf->pitch*sy)+(sx*srcf->bpp/8));
			if(srcf->pixtype == PIX_FMT_ARGB)
			{
				while (NumPixels-- > 0) {
					a = (*src) >> 24;
					if(a)
					{
						b = (*src) >> 0;
						g = (*src) >> 8;
						r = (*src) >> 16;
						if(a!=255)
						{
							a=255-a;
							U8 bg_r,bg_g,bg_b;
							U16 bg_rgb = *dest;

							bg_r = (bg_rgb&0xf800)>>8;
							bg_g = (bg_rgb&0x07e0)>>3;
							bg_b = (bg_rgb&0x01f)<<3;
							r += (((bg_r- (int)r)*a)>>8);
							g += (((bg_g- (int)g)*a)>>8);
							b += (((bg_b- (int)b)*a)>>8);
						}
						*dest=MAKE_RGB565(r,g,b);
					}
					dest++;
					src += IntPart;
					E += FractPart;
					if (E >= dw) {
						E -= dw;
						src++;
					} /* if */
				} /* while */
				return TRUE;
			}
			else if(srcf->pixtype==PIX_FMT_RGB888)
			{
				while (NumPixels-- > 0) {
					b = (*src) >> 0;
					g = (*src) >> 8;
					r = (*src) >> 16;
					*dest=MAKE_RGB565(r,g,b);
					dest++;
					src += IntPart;
					E += FractPart;
					if (E >= dw) {
						E -= dw;
						src++;
					} /* if */
				} /* while */
				return TRUE;
			}
		}
		else if(srcf->bpp==8)
		{
			U8* src = (U8*)(((U32)srcf->pixels)+(srcf->pitch*sy)+(sx*srcf->bpp/8));
			sARGB* pal = (sARGB*)srcf->pal->colors;
			if(srcf->pixtype == PIX_FMT_A000)//first palette is used for color
			{
				U16 rgb565;
				r = pal->r;
				g = pal->g;
				b = pal->b;
				rgb565 = MAKE_RGB565(r,g,b );

				while (NumPixels-- > 0) {
					a = *src;					
					if(a)
					{						
						if(a!=255)
						{							
							U8 bg_r,bg_g,bg_b;
							U16 bg_rgb = *dest;
							U8 r2=r,g2=g,b2=b;							
							a=255-a;
							bg_r = (bg_rgb&0xf800)>>8;
							bg_g = (bg_rgb&0x07e0)>>3;
							bg_b = (bg_rgb&0x01f)<<3;
							r2 += (((bg_r- (int)r2)*a)>>8);
							g2 += (((bg_g- (int)g2)*a)>>8);
							b2 += (((bg_b- (int)b2)*a)>>8);

							*dest = MAKE_RGB565(r2,g2,b2);
						}
						else
						{
							*dest = rgb565;
						}					
					}
					dest++;
					src += IntPart;
					E += FractPart;
					if (E >= dw) {
						E -= dw;
						src++;
					} /* if */
				} /* while */
				return TRUE;
			}
			else
			{
				while (NumPixels-- > 0) {
					if(*src != srcf->pal->colorkey)
					{
						*dest = MAKE_RGB565(pal[*src].r,pal[*src].g,pal[*src].b );
					}
					dest++;
					src += IntPart;
					E += FractPart;
					if (E >= dw) {
						E -= dw;
						src++;
					} /* if */
				} /* while */
				return TRUE;
			}
		}
		else if(srcf->bpp==4)
		{
			U8* src = (U8*)(((U32)srcf->pixels)+(srcf->pitch*sy)+(sx*srcf->bpp/8));
			sARGB* pal = (sARGB*)srcf->pal->colors;
			U8* src_org = src;
			U32  src_offset = 0;
			while (NumPixels-- > 0) {
				if(*src != srcf->pal->colorkey)
				{
					U8 index = *(src_org+(src_offset>>1));				
					if(src_offset&1)
						index = index & 0xf;
					else
						index = (index>>4)&0xf;

					*dest = MAKE_RGB565(pal[index].r,pal[index].g,pal[index].b );
				}
				dest++;
				src_offset += IntPart;
				E += FractPart;				
				if (E >= dw) {
					E -= dw;
					src_offset++;
				} /* if */
			} /* while */
			return TRUE;
		}
	}
	else if(targetsurface->bpp==32) //dest PIX_FMT_RGB888
	{
		U32* dest = (U32*)(((U32)targetsurface->pixels)+(targetsurface->pitch*dy)+(dx*4));
		if(srcf->bpp==16)
		{
			U16* src = (U16*)(((U32)srcf->pixels)+(srcf->pitch*sy)+(sx*srcf->bpp/8));
			while (NumPixels-- > 0) {
				r = ((*src)>>8) & 0xf8;
				g = ((*src)>>3) & 0xfc;
				b = ((*src)<<3);
				*dest=MAKE_RGB888(r,g,b);

				dest++;
				src += IntPart;
				E += FractPart;
				if (E >= dw) {
					E -= dw;
					src++;
				} /* if */
			} /* while */
			return TRUE;
		}
		else if(srcf->bpp==32)
		{
			U32* src = (U32*)(((U32)srcf->pixels)+(srcf->pitch*sy)+(sx*srcf->bpp/8));
			U8 a,r,g,b;
			if(srcf->pixtype == PIX_FMT_ARGB)
			{
				while (NumPixels-- > 0) {
					a = (*src) >> 24;
					if(a)
					{
						b = (*src) >> 0;
						g = (*src) >> 8;
						r = (*src) >> 16;
						if(a!=255)
						{
							a=255-a;
							U8 bg_r,bg_g,bg_b;
							bg_r = (*dest)>>16;
							bg_g = (*dest)>>8;
							bg_b = (U8)(*dest);
							r += (((bg_r- (int)r)*a)>>8);
							g += (((bg_g- (int)g)*a)>>8);
							b += (((bg_b- (int)b)*a)>>8);
						}
						*dest=MAKE_RGB888(r,g,b);
					}
					dest++;
					src += IntPart;
					E += FractPart;
					if (E >= dw) {
						E -= dw;
						src++;
					} /* if */
				} /* while */
				return TRUE;
			}
			else if(srcf->pixtype==PIX_FMT_RGB888)
			{
				while (NumPixels-- > 0) {
					*dest=*src;
					dest++;
					src += IntPart;
					E += FractPart;
					if (E >= dw) {
						E -= dw;
						src++;
					} /* if */
				} /* while */
				return TRUE;
			}
		}
		else if(srcf->bpp==8)
		{
			U8* src = (U8*)(((U32)srcf->pixels)+(srcf->pitch*sy)+(sx*srcf->bpp/8));
			sARGB* pal = (sARGB*)srcf->pal->colors;
			if(srcf->pixtype == PIX_FMT_A000)//first palette is used for color
			{
				U32 rgb888;
				r = pal->r;
				g = pal->g;
				b = pal->b;
				rgb888 = MAKE_RGB888(r,g,b);
				while (NumPixels-- > 0) {
					a = *src;
					if(a)
					{
						if(a!=255)
						{							
							U8 bg_r,bg_g,bg_b;
							U32 bg_rgb = *dest;
							U8 r2=r,g2=g,b2=b;							
							a=255-a;
							bg_r = (U8)(bg_rgb>>16);
							bg_g = (U8)(bg_rgb>>8);
							bg_b = (U8)(bg_rgb);
							r2 += (((bg_r- (int)r2)*a)>>8);
							g2 += (((bg_g- (int)g2)*a)>>8);
							b2 += (((bg_b- (int)b2)*a)>>8);

							*dest = MAKE_RGB888(r2,g2,b2);
						}
						else
						{
							*dest = rgb888;
						}
					}
					dest++;
					src += IntPart;
					E += FractPart;
					if (E >= dw) {
						E -= dw;
						src++;
					} /* if */
				} /* while */
				return TRUE;
			}
			else
			{
				while (NumPixels-- > 0) {
					if(*src != srcf->pal->colorkey)
					{
						*dest = MAKE_RGB888(pal[*src].r,pal[*src].g,pal[*src].b );
					}
					dest++;
					src += IntPart;
					E += FractPart;
					if (E >= dw) {
						E -= dw;
						src++;
					} /* if */
				} /* while */
				return TRUE;
			}
		}
		else if(srcf->bpp==4)
		{
			U8* src = (U8*)(((U32)srcf->pixels)+(srcf->pitch*sy)+(sx*srcf->bpp/8));
			sARGB* pal = (sARGB*)srcf->pal->colors;
			U8* src_org = src;
			U32  src_offset = 0;
			while (NumPixels-- > 0) {
				if(*src != srcf->pal->colorkey)
				{
					U8 index = *(src_org+(src_offset>>1));				
					if(src_offset&1)
						index = index & 0xf;
					else
						index = (index>>4)&0xf;

					*dest = MAKE_RGB888(pal[index].r,pal[index].g,pal[index].b );
				}
				dest++;
				src_offset += IntPart;
				E += FractPart;				
				if (E >= dw) {
					E -= dw;
					src_offset++;
				} /* if */
			} /* while */
			return TRUE;
		}
	}
	debugstring("Not Supports format ("__FILE__")\r\n");
	return FALSE;
}

BOOL sw_draw_surface_scalerect(SURFACE* src_surf,int dx, int dy,int dw,int dh,int sx, int sy, int sw, int sh)
{
	//calculate clip and surface 
	float d;
	SURFACE* targetsurface = get_draw_target();
	if(dx>= targetsurface->w)
		return FALSE;
	if(dy>=targetsurface->h)
		return FALSE;
	if(sx>=src_surf->w)
		return FALSE;
	if(sy>=src_surf->h)
		return FALSE;
	if(sx+sw>src_surf->w)
		sw = src_surf->w-sx;
	if(sy+sh>src_surf->h)
		sh = src_surf->h-sy;

	if(dx<targetsurface->clip.x)
	{
		d = (float)sw/dw;
		sx += (int)(d*(targetsurface->clip.x-dx));
		sw -= (int)(d*(targetsurface->clip.x-dx));
		dw -= (targetsurface->clip.x-dx);
		dx=targetsurface->clip.x;
	}

	if(dy<targetsurface->clip.y)
	{
		d = (float)sh/dh;
		sy += (int)(d*(targetsurface->clip.y-dy));
		sh -= (int)(d*(targetsurface->clip.y-dy));
		dh -= (targetsurface->clip.y-dy);
		dy=targetsurface->clip.y;
	}

	if(dx+dw > targetsurface->clip.endx)
	{
		d = (float)sw/dw;
		sw -= (int)(d*((dw+dx) - targetsurface->clip.endx));
		dw =targetsurface->clip.endx-dx;
	}
	if(dy+dh > targetsurface->clip.endy)
	{
		d = (float)sh/dh;
		sh -= (int)(d*((dh+dy) - targetsurface->clip.endy));
		dh =targetsurface->clip.endy-dy;
	}

	int NumPixels = dh;
	int IntPart = (sh/ dh);
	int FractPart = sh% dh;
	int E = 0;
	while(NumPixels > 0 )
	{
		if(ScaleLine(dx,dy,dw,src_surf,sx,sy,sw)==FALSE)
		{
			dcache_invalidate_way();
			return FALSE;
		}
		dy++;
		sy+=IntPart;
		E += FractPart;
		if(E>=dh)
		{
			E-=dh;
			sy++;
		}
		NumPixels--;
	}
	dcache_invalidate_way();
	return TRUE;
}

BOOL sw_draw_surface_scale(SURFACE* src_surf,int dx, int dy,int dw,int dh)
{
	return sw_draw_surface_scalerect(src_surf,dx, dy,dw,dh,0, 0, src_surf->w,src_surf->h);
}

