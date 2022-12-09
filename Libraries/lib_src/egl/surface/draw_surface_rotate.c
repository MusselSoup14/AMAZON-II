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
#include <math.h>
extern BOOL ge_draw ( SURFACE *src, S32 InitDX, S32 InitDY, S32 EndX, S32 EndY,
					 S32 InitSX, S32 InitSY, S32 dxSx, S32 dxSy, S32 dySx, S32 dySy );
/*
for the speed, we do not use "const". DRAM is faster than flash.
*/
static float __sintab[91] =
{
	0.0000000000000000f,	0.0174524064372835f,	0.0348994967025010f,	0.0523359562429438f,
	0.0697564737441253f,	0.0871557427476582f,	0.1045284632676535f,	0.1218693434051475f,
	0.1391731009600654f,	0.1564344650402309f,	0.1736481776669303f,	0.1908089953765448f,
	0.2079116908177593f,	0.2249510543438650f,	0.2419218955996677f,	0.2588190451025208f,
	0.2756373558169992f,	0.2923717047227367f,	0.3090169943749474f,	0.3255681544571567f,
	0.3420201433256687f,	0.3583679495453003f,	0.3746065934159120f,	0.3907311284892738f,
	0.4067366430758002f,	0.4226182617406994f,	0.4383711467890774f,	0.4539904997395468f,
	0.4694715627858908f,	0.4848096202463370f,	0.5000000000000000f,	0.5150380749100542f,
	0.5299192642332050f,	0.5446390350150271f,	0.5591929034707468f,	0.5735764363510461f,
	0.5877852522924731f,	0.6018150231520483f,	0.6156614753256583f,	0.6293203910498375f,
	0.6427876096865393f,	0.6560590289905073f,	0.6691306063588582f,	0.6819983600624985f,
	0.6946583704589973f,	0.7071067811865475f,	0.7193398003386511f,	0.7313537016191705f,
	0.7431448254773942f,	0.7547095802227720f,	0.7660444431189780f,	0.7771459614569709f,
	0.7880107536067220f,	0.7986355100472928f,	0.8090169943749474f,	0.8191520442889918f,
	0.8290375725550417f,	0.8386705679454240f,	0.8480480961564260f,	0.8571673007021123f,
	0.8660254037844386f,	0.8746197071393958f,	0.8829475928589269f,	0.8910065241883679f,
	0.8987940462991670f,	0.9063077870366500f,	0.9135454576426009f,	0.9205048534524403f,
	0.9271838545667874f,	0.9335804264972017f,	0.9396926207859084f,	0.9455185755993168f,
	0.9510565162951536f,	0.9563047559630355f,	0.9612616959383189f,	0.9659258262890683f,
	0.9702957262759965f,	0.9743700647852352f,	0.9781476007338056f,	0.9816271834476640f,
	0.9848077530122081f,	0.9876883405951377f,	0.9902680687415703f,	0.9925461516413220f,
	0.9945218953682733f,	0.9961946980917455f,	0.9975640502598242f,	0.9986295347545739f,
	0.9993908270190957f,	0.9998476951563912f,	1.0000000000000000f,
};

static float mysin ( U32 angle )
{
	angle %= 360;

	if ( angle <= 90 )
		return __sintab[angle];
	else if ( angle <= 180 )
		return __sintab[180 - angle];
	else if ( angle <= 270 )
		return -__sintab[angle - 180];
	else
		return -__sintab[360 - angle];
}
static float mycos ( U32 angle )
{
	return mysin ( angle + 90 );
}
S32 sqrtI2I(S32 v)
{
	S32 t, q, b, r;
	r = v;           // r = v - x©÷
	b = 0x40000000;  // a©÷
	q = 0;           // 2ax
	while (b > 0)
	{
		t = q + b;   // t = 2ax + a©÷
		q >>= 1;     // if a' = a/2, then q' = q/2
		if (r >= t) // if (v - x©÷) >= 2ax + a©÷
		{
			r -= t;  // r' = (v - x©÷) - (2ax + a©÷)
			q += b;  // if x' = (x + a) then ax' = ax + a©÷, thus q' = q' + b
		}
		b >>= 2;     // if a' = a/2, then b' = b / 4
	}
	return q;
}
BOOL draw_surface_rotate ( SURFACE *src, S32 cdx, S32 cdy, S32 ctx, S32 cty,float zoom, U32 degrees )
{
	float sinval,cosval;

	S32 InitDX, InitDY;
	S32 EndX, EndY;
	S32 InitSX, InitSY;
	S32 dxSx, dxSy;
	S32 dySx, dySy;
	S32 dx,dy;
	S32 x,y;
	S32 cosa;
	S32 sina;
	S32 rhzoom;	
	SURFACE* targetsurf = get_ge_target_buffer();

	x = y = 0;
	dx = get_screen_width();
	dy = get_screen_height();

	S32 crosslen;
	//crosslen = sqrt((src->w*src->w)+(src->h*src->h));
	crosslen = sqrtI2I((src->w*src->w)+(src->h*src->h));
	crosslen*=zoom;

	long tx,ty;
	sinval = mysin( degrees );
	cosval = mycos( degrees );
	if (zoom == 1.0f)
	{
		tx = (-cdx) * cosval + (-cdy) * sinval;
		ty = (cdx) * sinval + (-cdy) * cosval;
	}
	else
	{
		tx = (-cdx / zoom) * cosval + (-cdy / zoom) * sinval;
		ty = (cdx / zoom) * sinval + (-cdy / zoom) * cosval;
	}
	if( zoom<=0 )	rhzoom = 0;
	else			rhzoom = (S32)((float)(1<<9)/zoom);
	cosa = (S32)(rhzoom * cosval);
	sina = (S32)(rhzoom * sinval);

	if( dx<=0 || dy<=0 ) 
		return FALSE;

	InitDX	= x;
	InitDY	= y;
	EndX	= x+dx-1;
	EndY	= y+dy-1;

	InitSX	= (x+tx+ctx)*512;
	dxSx	= cosa;
	dxSy	= -sina;

	InitSY	= (y+ty+cty)*512;
	dySx	= sina;
	dySy	= cosa;

	x = cdx - crosslen;
	y = cdy - crosslen;
	dx = cdx + crosslen;
	dy = cdy + crosslen;
	if(x<0)
		x = 0;
	if(y<0)
		y = 0;
	
	CLIP_RECT tmp_clip;	
	draw_get_clip_window(targetsurf,&tmp_clip);
	
	CLIP_RECT clip;
	clip.x = tmp_clip.x;
	clip.y = y;
	clip.endx = tmp_clip.endx;
	clip.endy = y+dy;	
	
	if(x > tmp_clip.x)
	{
		clip.x = x;
	}
	
	if(x+dx < tmp_clip.endx)
	{
		clip.endx = x+dx;
	}
	
	CLIP_RECT orgclip = draw_set_clip_window(targetsurf,&clip);	
		
	int orgmode = src->drawmode;
	src->drawmode |= DRAWMODE_NOREPEAT;
	bool re = ge_draw ( src, InitDX, InitDY, EndX, EndY, InitSX, InitSY, dxSx, dxSy, dySx, dySy );
	src->drawmode = orgmode;
	draw_set_clip_window(targetsurf,&orgclip);
	return re;
}
