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
#include "font/egl_font.h"
#include "font/textout.h"
#include "egl_theme.h"
#include "egl_private.h"

#define USE_INTERNAL_IMAGE 1
#if USE_INTERNAL_IMAGE==1
#include "checkbox_img.c"
#include "radiobox_img.c"
static SURFACE* m_surf_checkbox;
static SURFACE* m_surf_radiobox;
static SURFACE* load_internal_image( int w, int h, int bpp, unsigned char* data )
{
    SURFACE* surf = CREATE_SURFACE( w, h, bpp );
    int i;
    const unsigned char* src;
    EGL_COLOR* dst;
    EGL_COLOR c;
    src = data;
    dst = ( EGL_COLOR* )surf->pixels;
    for( i = 0; i < h; i++ )
    {
        int k;
        for( k = 0; k < w * 4; )
        {
            U8 r, g, b, a;
            r = src[k];
            k++;
            g = src[k];
            k++;
            b = src[k];
            k++;
            a = src[k];
            k++;
            c = MAKE_COLORARGB( a, r, g, b );
            dst[k / 4 - 1] = c;
        }
        dst += ( surf->pitch / 4 );
        src += ( w * 4 );
    }
	surf->pixtype = PIX_FMT_ARGB;
    return surf;
}

#endif

#define PUSH_VERTEX(X,Y) p[i].x=X;p[i].y=Y;i++
//static EGL_COLOR sel_gr_start_color = MAKE_COLORREF( 98, 149, 214 );
static EGL_COLOR sel_gr_end_color = MAKE_COLORREF( 70, 104, 153 );
static EGL_COLOR gr_start_color = MAKE_COLORREF( 246, 246, 246 );
static EGL_COLOR gr_end_color = MAKE_COLORREF( 223, 223, 223 );

static EGL_COLOR m_ctrlColor = EGL_CONFIG_OBJECT_BG_COLOR;

BOOL theme_init()
{
#if USE_INTERNAL_IMAGE==1
    m_surf_checkbox = load_internal_image( checkbox_image.width, checkbox_image.height, checkbox_image.bytes_per_pixel * 8, ( unsigned char* )checkbox_image.pixel_data );
    if( m_surf_checkbox == NULL )
        return FALSE;
    m_surf_radiobox = load_internal_image( radiobox_image.width, radiobox_image.height, radiobox_image.bytes_per_pixel * 8, ( unsigned char* )radiobox_image.pixel_data );
    if( m_surf_radiobox == NULL )
        return FALSE;
#endif
    return TRUE;
}
int theme_frame_dw()
{
    return 2;
}
int theme_frame_dh()
{
    return 2;
}
void theme_draw_frame( int x, int y, int w, int h )
{
    DRAW_HLINE( x + 2, y, x + w - 2, MAKE_COLORREF( 182, 187, 191 ) );
    DRAW_HLINE( x + 2, y + 1, x + w - 2, m_ctrlColor );
    //corner
    DRAW_HLINE( x + w - 2, y, x + w - 1, MAKE_COLORREF( 209, 211, 212 ) );
    DRAW_HLINE( x + w - 2, y + 1, x + w - 1, MAKE_COLORREF( 218, 218, 218 ) );
    DRAW_HLINE( x + w - 1, y + 1, x + w, MAKE_COLORREF( 201, 204, 206 ) );
    DRAW_VLINE( x + w - 1, y + 2, y + h - 2, MAKE_COLORREF( 149, 154, 158 ) );
    DRAW_VLINE( x + w - 2, y + 2, y + h - 2, MAKE_COLORREF( 243, 243, 244 ) );
    //corner
    DRAW_HLINE( x + w - 2, y + h - 1, x + w - 1, MAKE_COLORREF( 209, 211, 212 ) );
    DRAW_HLINE( x + w - 2, y + h - 2, x + w - 1, MAKE_COLORREF( 218, 218, 218 ) );
    DRAW_HLINE( x + w - 1, y + h - 2, x + w, MAKE_COLORREF( 201, 204, 206 ) );
    DRAW_HLINE( x + 2, y + h - 1, x + w - 2, MAKE_COLORREF( 182, 187, 191 ) );
    DRAW_HLINE( x + 2, y + h - 2, x + w - 2, MAKE_COLORREF( 243, 243, 244 ) );
    //corner
    DRAW_HLINE( x + 1, y + h - 1, x + 2, MAKE_COLORREF( 209, 211, 212 ) );
    DRAW_HLINE( x + 1, y + h - 2, x + 2, MAKE_COLORREF( 218, 218, 218 ) );
    DRAW_HLINE( x, y + h - 2, x + 1, MAKE_COLORREF( 209, 211, 212 ) );
    DRAW_VLINE( x, y + 2, y + h - 2, MAKE_COLORREF( 187, 191, 195 ) );
    DRAW_VLINE( x + 1, y + 2, y + h - 2, MAKE_COLORREF( 252, 252, 253 ) );
    //corner
    DRAW_HLINE( x + 1, y, x + 2, MAKE_COLORREF( 209, 211, 212 ) );
    DRAW_HLINE( x + 1, y + 1, x + 2, MAKE_COLORREF( 218, 218, 218 ) );
    DRAW_HLINE( x, y + 1, x + 1, MAKE_COLORREF( 209, 211, 212 ) );
}

void theme_draw_button( int x, int y, int w, int h, BOOL selected )
{
    theme_draw_frame( x, y, w, h );
    if( selected )
    {
        DRAW_RECTFILL( x + 2, y + 2, w - 4, h - 4, sel_gr_end_color );
        //DRAW_RECTFILLGRADIENT(x + 3, y + 3 , w - 6, ( h - 6 ), m_sel_gr_start_color, m_sel_gr_end_color,TRUE );
    }
    else
        DRAW_RECTFILLGRADIENT(x + 2, y + 2 , w - 4, ( h - 4 ), gr_start_color, gr_end_color,TRUE );
}

void theme_draw_box( int x, int y, int w, int h, BOOL selected )
{
    theme_draw_frame( x, y, w, h );
    DRAW_RECTFILL( x + theme_frame_dw(), y + theme_frame_dh(), w - theme_frame_dw() * 2, h - theme_frame_dh() * 2, m_ctrlColor );
}
void theme_draw_scroll_thumb( int x, int y, int w, int h, BOOL vertical, BOOL selected )
{
    int center_x, center_y;
    int linelen;
    EGL_COLOR col = MAKE_COLORREF( 140, 140, 140 );
    EGL_COLOR col2 = MAKE_COLORREF( 210, 210, 210 );
    theme_draw_box( x, y, w, h, selected );
    center_x = x + w / 2 - 1;
    center_y = y + h / 2 - 1;
    if( vertical )
    {
        linelen = ( w - 6 ) / 2;
        DRAW_HLINE( center_x - ( linelen / 2 ), center_y, ( center_x - ( linelen / 2 ) ) + linelen, col );
        DRAW_HLINE( center_x - ( linelen / 2 ), center_y + 1, ( center_x - ( linelen / 2 ) ) + linelen, col2 );
        if( h > 20 )
        {
            DRAW_HLINE( center_x - ( linelen / 2 ), center_y - 4, ( center_x - ( linelen / 2 ) ) + linelen, col );
            DRAW_HLINE( center_x - ( linelen / 2 ), center_y - 3, ( center_x - ( linelen / 2 ) ) + linelen, col2 );
            DRAW_HLINE( center_x - ( linelen / 2 ), center_y + 4, ( center_x - ( linelen / 2 ) ) + linelen, col );
            DRAW_HLINE( center_x - ( linelen / 2 ), center_y + 5, ( center_x - ( linelen / 2 ) ) + linelen, col2 );
        }
    }
    else
    {
        linelen = ( h - 6 ) / 2;
        DRAW_VLINE( center_x, ( center_y - ( linelen / 2 ) ), ( center_y - ( linelen / 2 ) ) + linelen, col );
        DRAW_VLINE( center_x + 1, ( center_y - ( linelen / 2 ) ), ( center_y - ( linelen / 2 ) ) + linelen, col2 );
        if( w > 20 )
        {
            DRAW_VLINE( center_x - 4, ( center_y - ( linelen / 2 ) ), ( center_y - ( linelen / 2 ) ) + linelen, col );
            DRAW_VLINE( center_x - 3, ( center_y - ( linelen / 2 ) ), ( center_y - ( linelen / 2 ) ) + linelen, col2 );
            DRAW_VLINE( center_x + 4, ( center_y - ( linelen / 2 ) ), ( center_y - ( linelen / 2 ) ) + linelen, col );
            DRAW_VLINE( center_x + 5, ( center_y - ( linelen / 2 ) ), ( center_y - ( linelen / 2 ) ) + linelen, col2 );
        }
    }
}

void theme_draw_up_arrow_btn( int x, int y, int w, int h, BOOL selected )
{
    int center_x;
    int center_y;
    EGL_POINT p[3];
    int i = 0;
    EGL_COLOR c =  MAKE_COLORREF ( 0, 0, 0 ) ;
    center_x = x + ( w / 2 );
    center_y = y + ( h / 2 );
    theme_draw_box( x, y, w, h, selected );
    PUSH_VERTEX ( center_x - 4, center_y + 4 );
    PUSH_VERTEX ( center_x , center_y - 4 );
    PUSH_VERTEX ( center_x + 4, center_y + 4 );
    DRAW_POLYGONFILL( p, i, c );
}

void theme_draw_down_arrow_btn( int x, int y, int w, int h, BOOL selected )
{
    int center_x;
    int center_y;
    EGL_POINT p[3];
    int i = 0;
    EGL_COLOR c =  MAKE_COLORREF ( 0, 0, 0 ) ;
    center_x = x + ( w / 2 );
    center_y = y + ( h / 2 );
    theme_draw_box( x, y, w, h, selected );
    PUSH_VERTEX ( center_x - 4, center_y - 4 );
    PUSH_VERTEX( center_x , center_y + 4 );
    PUSH_VERTEX( center_x + 4, center_y - 4 );
    DRAW_POLYGONFILL( p, i, c );
}

void theme_draw_left_arrow_btn( int x, int y, int w, int h, BOOL selected )
{
    int center_x;
    int center_y;
    EGL_POINT p[3];
    int i = 0;
    EGL_COLOR c =  MAKE_COLORREF ( 0, 0, 0 ) ;
    center_x = x + ( w / 2 );
    center_y = y + ( h / 2 );
    theme_draw_box( x, y, w, h, selected );
    PUSH_VERTEX ( center_x - 4, center_y );
    PUSH_VERTEX ( center_x + 4, center_y - 4 );
    PUSH_VERTEX ( center_x + 4, center_y + 4 );
    DRAW_POLYGONFILL( p, i, c );
}

void theme_draw_right_arrow_btn( int x, int y, int w, int h, BOOL selected )
{
    int center_x;
    int center_y;
    EGL_POINT p[3];
    int i = 0;
    EGL_COLOR c =  MAKE_COLORREF ( 0, 0, 0 ) ;
    center_x = x + ( w / 2 );
    center_y = y + ( h / 2 );
    theme_draw_box( x, y, w, h, selected );
    PUSH_VERTEX( center_x + 4, center_y );
    PUSH_VERTEX( center_x - 4, center_y - 4 );
    PUSH_VERTEX( center_x - 4, center_y + 4 );
    DRAW_POLYGONFILL( p, i, c );
}

void theme_draw_radio_btn( int x, int y, int w, int h, BOOL selected )
{
#if USE_INTERNAL_IMAGE==1
    if( selected )
        DRAW_SURFACE_RECT( m_surf_radiobox, x, y, 0, 32, 32, 32);
    else
        DRAW_SURFACE_RECT( m_surf_radiobox, x, y, 0, 0, 32, 32);
#else
    int cx = x + w / 2 ;
    int cy = y + h / 2 ;
    int r = w / 2;
    DRAW_CIRCLE( cx, cy, r, MAKE_COLORREF( 84, 84, 84 ) );
    if( selected )
    {
        DRAW_CIRCLEFILL( cx, cy, r - 3, MAKE_COLORREF( 0, 153, 204 ) );
        DRAW_ELLIPSE_AA( cx, cy, r - 2, r - 2, MAKE_COLORREF( 128, 128, 128 ) );
    }
#endif
}

void theme_draw_check_btn( int x, int y, int w, int h, BOOL selected )
{
#if USE_INTERNAL_IMAGE==1
    if( selected )
        DRAW_SURFACE_RECT( m_surf_checkbox, x, y, 0, 32, w, h);
    else
        DRAW_SURFACE_RECT( m_surf_checkbox, x, y, 0, 0, w, h);
#else
    theme_draw_box( x, y, w, h, selected );
    if( selected )
    {
        int sx = x + 4;
        int ex = x + w;
        int sy = y + 5;
        int ey = y + 3;
        int cx = x + w / 2;
        int cy = y + h - 7;
        int i;
        for( i = 0; i < 4; i++ )
        {
            DRAW_LINE( sx, sy + i, cx, cy + i, MAKE_COLORREF( 0, 153, 204 ) );
            DRAW_LINE( cx, cy + i, ex, ey + i, MAKE_COLORREF( 0, 153, 204 ) );
        }
    }
#endif
}

