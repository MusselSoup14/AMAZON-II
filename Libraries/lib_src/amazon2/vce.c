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
//---------------------------------------------------------------------
// Video Capture Engine
//---------------------------------------------------------------------

#define YCCAP_EN              1<<0
#define HALF_REQ              1<<1
#define UYVY		          2<<2
#define VYUY		          3<<2
#define YUYV		          0<<2
#define YVYU		          1<<2
#define DECOD_EN              1<<4
#define VCLK_INV	          1<<5
#define ODD_INTEN             1<<6
#define EVEN_INTEN            1<<7
//#define USE2NDBUF	          1<<8
//#define PXCNTLNVLD	      1<<9
#define COPYOFF	      		1<<10
#define CSCEN		  		1<<11
#define CS_RGB888	  		1<<12
#define RGB565		  		0
#define HALFLINE	  		1<<13
#define EVENFLD	  	  		1<<14
#define ODDFLD	  	  		~EVENFLD
#define ALLFLD	  	  		1<<15


extern SURFACE g_FrameSurface[4] ;

BOOL vce_on_surface( SURFACE* dst, int x, int y, bool halfmode )
{
    if( halfmode )
    {
        if( ( ( dst->w - x ) < 360 ) || ( ( dst->h - y ) < 240 ) )
            return FALSE;

    }
    else
    {
        if( ( ( dst->w - x ) < 720 ) || ( ( dst->h - y ) < 480 ) )
            return FALSE;
    }
	
	int bytepp;
	if(dst->bpp<16)
		return FALSE;
	if(dst->bpp==16)
		bytepp=2;
	else  // 24, 32bpp
		bytepp=4;
	//PRINTVAR(bytepp);
    *R_VCE_NUMLINECUT0 = 0;
    *R_VCE_NUMLINECUT1 = 0;
    *R_VCE_NUMPXLCUT0  = 0;
    *R_VCE_NUMPXLCUT1  = 0;
    *R_VCE_WIDTH = dst->pitch / bytepp;
	
    *R_VCE_YBUF( 0 ) = *R_VCE_YBUF( 1 ) = *R_VCE_YBUF( 2 ) = *R_VCE_YBUF( 3 ) = ( U32 )dst->pixels + ( x * bytepp ) + ( y * dst->pitch );
    int mode = YCCAP_EN | DECOD_EN | CSCEN | UYVY ;
    if ( bytepp == 4 )
    {
        mode |= CS_RGB888;
    }
    if( halfmode )
    {
        mode |= HALFLINE;
        *R_VCE_OFFSET =   *R_VCE_WIDTH - 360;
    }
    else
    {
        mode |= ALLFLD;
        *R_VCE_OFFSET 	= *R_VCE_WIDTH - 720;
    }
	
    *R_VCE_CTRL = mode;
    return TRUE;
}

void vce_off()
{
    *R_DC_FESR = 0;
    *R_DC_FESR = ( 1 << 4 ) | ( 1 << 1 );
    *R_VCE_CTRL = 0;
}
