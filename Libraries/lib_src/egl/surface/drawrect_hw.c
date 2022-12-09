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
#include "amazon2/hw_graphics.h"

BOOL ge_check_clipwindow( S32* InitDX, S32* InitDY, S32* EndX, S32* EndY,
						  S32* InitSX, S32* InitSY, S32 dxSx, S32 dxSy, S32 dySx, S32 dySy );
/*
* H/W rendering
*/
void ge_draw_rectfill( int x, int y, int w, int h, EGL_COLOR c )
{
	int EndX, EndY;
	U8 r, g, b;
	SURFACE* targetsurf = get_ge_target_buffer();
	if( w <= 0 )
		return;
	if( h <= 0 )
		return;
	if ( x >= targetsurf->w )
		return;
	if ( y >= targetsurf->h )
		return;
	EXTRACT_RGB( c, r, g, b );
	EndX = x + w - 1;
	EndY = y + h - 1;
	if( targetsurf->clip.x > x )
	{
		x = targetsurf->clip.x;
	}
	if( targetsurf->clip.y > y )
	{
		y = targetsurf->clip.y;
	}
	if( targetsurf->clip.endx < EndX )
	{
		EndX = targetsurf->clip.endx;
	}
	if( targetsurf->clip.endy < EndY )
	{
		EndY = targetsurf->clip.endy;
	}

	CRITICAL_ENTER();
	GE_WriteCmd( 1, ( U16 )( ( 1 << 0 ) | ( 1 << 4 ) ) );
	if( get_screen_bpp() == 32 )
	{
		GE_WriteCmd( 0, ( U16 )( ( 1 << 14 ) | ( 1 << 8 ) ) );
	}
	else
	{
		GE_WriteCmd( 0, ( U16 )( 1 << 8 ) );
	}

	GE_WriteCmd( 2, ( U16 )x );
	GE_WriteCmd( 3, ( U16 )y );
	GE_WriteCmd( 4, ( U16 )EndX );
	GE_WriteCmd( 5, ( U16 )EndY );
	GE_WriteCmd( 21, 0 );
	GE_WriteCmd( 22, ( U16 )( g << 8 | b )  );
	GE_WriteCmd( 23, ( U16 )r );
	ge_end_cmd();
	CRITICAL_EXIT();
}

void ge_draw_rectfillalpha( int x, int y, int w, int h, EGL_COLOR acolor )
{
	int EndX, EndY;
	U8 a, r, g, b;
	SURFACE* targetsurf = get_ge_target_buffer();
	EXTRACT_ARGB( acolor, a, r, g, b );
	EndX = x + w - 1;
	EndY = y + h - 1;
	if( targetsurf->clip.x > x )
	{
		x = targetsurf->clip.x;
	}
	if( targetsurf->clip.y > y )
	{
		y = targetsurf->clip.y;
	}
	if( targetsurf->clip.endx < EndX )
	{
		EndX = targetsurf->clip.endx;
	}
	if( targetsurf->clip.endy < EndY )
	{
		EndY = targetsurf->clip.endy;
	}
	a = 255 - a;
	U16 sblendmode = BLEND_INVERSION | BLEND_CONSTANT;
	U16 dblendmode = BLEND_CONSTANT;

	GE_WriteCmd( 1, ( U16 )( ( 1 << 0 ) | ( 1 << 4 ) | ( 1 << 3 ) ) );
	if (get_screen_bpp() == 32)
	{
		GE_WriteCmd(0, (U16)((1 << 14) | (1 << 9) | (1 << 8)));
	}
	else
	{
		GE_WriteCmd(0, (U16)((1 << 9) | (1 << 8)));
	}

	GE_WriteCmd( 2, ( U16 )x );
	GE_WriteCmd( 3, ( U16 )y );
	GE_WriteCmd( 4, ( U16 )EndX );
	GE_WriteCmd( 5, ( U16 )EndY );
	GE_WriteCmd ( 18, a << 8 | a );
	GE_WriteCmd ( 19, a << 8 | a );
	GE_WriteCmd ( 20, dblendmode << 8 | sblendmode );
	GE_WriteCmd( 21, 0 );
	GE_WriteCmd( 22, ( U16 )( g << 8 | b )  );
	GE_WriteCmd( 23, ( U16 )r );

	ge_end_cmd();
}

