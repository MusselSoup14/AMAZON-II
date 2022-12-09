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
static SURFACE* cur_osd_surf = NULL;
static int _osd_draw( SURFACE* surf, int x, int y )
{

    int osd_fmt ;
    switch( surf->pixtype )
    {
    case PIX_FMT_RGB565:
        osd_fmt = OSD_565;
        break;
    case PIX_FMT_A1RGB555:
        osd_fmt = OSD_A8888;
        break;
    case PIX_FMT_PALETTE:
        if( surf->bpp == 8 )
            osd_fmt = OSD_8BPP;
        else if( surf->bpp == 4 )
            osd_fmt = OSD_4BPP;
        else
            return -1;
        break;
    case PIX_FMT_RGB888:
        osd_fmt = OSD_888;
        break;
    case PIX_FMT_ARGB:
        osd_fmt = OSD_A8888;
        break;
    case PIX_FMT_A1RGB:
        osd_fmt = OSD_A1888;
        break;
    case PIX_FMT_A4RGB:
        osd_fmt = OSD_A4888;
        break;
    case PIX_FMT_A1PALETTE:
        if( surf->bpp == 8 )
            osd_fmt = OSD_8BPP_A;
        else if( surf->bpp == 4 )
            osd_fmt = OSD_4BPP_A;
        else
            return -1;
        break;
    default:
        return -1;
    }
    if( surf->bpp <= 8 )
    {
        int i;
        U32* pal = ( U32* )surf->pal->colors;
        for( i = 0; i < ( 1 << surf->bpp ); i++ )
        {
            *R_OSDPAL = pal[i];
        }
        if( surf->pal->colorkey != -1 )
        {
            *R_OSDCOLORKEY = surf->pal->colors[surf->pal->colorkey];
        }
    }
    *R_OSDBA0 = ( U32 )surf->pixels;
    *R_OSDBA1 = ( U32 )surf->pixels;
    *R_OSDIMGW = surf->pitch * 8 / surf->bpp;
    *R_OSDIMGH = surf->h;
    *R_OSDTARGETWIDTH = surf->w;
    *R_OSDTARGETHEIGHT = surf->h;
    *R_OSDTARGETX = x;
    *R_OSDTARGETY = y;
    return osd_fmt;
}

void osd_set_pal( EGL_COLOR* pal, int num )
{
    //clear pal
    volatile vU32 temp = *R_DC_STAT;
    int i;
    temp;//avoid warning
    for( i = 0; i < num; i++ )
        *R_OSDPAL = pal[i];

}

BOOL osd_set_surface( SURFACE* surf, int x, int y )
{
    int osd_fmt = _osd_draw( surf, x, y );
    if( osd_fmt == -1 )
        return FALSE;
    if( osd_fmt == OSD_4BPP || osd_fmt == OSD_8BPP || osd_fmt == OSD_565 || osd_fmt == OSD_888 )
    {
        *R_OSDALPHA0 = 0xff;
        *R_OSDALPHA1 = 0xff;
        *R_OSDCNTL = ( 1 << 24 ) | ( osd_fmt << 12 ) | ( 1 << 4 );
    }
    else
    {
        *R_OSDCNTL = ( 1 << 24 ) | ( osd_fmt << 12 ) | ( 1 << 4 ) | ( 1 << 1 );
    }
    if( ( surf->bpp <= 8 ) & ( surf->pal->colorkey != -1 ) )
    {
        *R_OSDCNTL |= ( 1 << 9 ); //colorkey enable
    }
    cur_osd_surf = surf;
    return TRUE;
}

BOOL osd_set_surface_with_alpha( SURFACE* surf, int x, int y, U8 alpha )
{
    int osd_fmt = _osd_draw( surf, x, y );
    if( osd_fmt == -1 )
        return FALSE;
    *R_OSDALPHA0 = alpha;
    *R_OSDALPHA1 = alpha;

    *R_OSDCNTL = ( 1 << 24 ) | ( osd_fmt << 12 ) | ( 1 << 4 ) ;
    return TRUE;
}
void osd_set_alpha( U8 alpha )
{
    *R_OSDALPHA0 = alpha;
    *R_OSDALPHA1 = alpha;
}

void osd_off()
{
    *R_OSDCNTL = 0;
}

void osd_set_color_key( EGL_COLOR c )
{
    *R_OSDCOLORKEY = c;
}
