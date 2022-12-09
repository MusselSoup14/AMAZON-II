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
/**
 * \file
 *
 * \brief SUF Image Driver
 */
#include "sdk.h"

// version 0x100
typedef struct
{
    unsigned int magic;
    unsigned int version;
    unsigned short w;//image width
    unsigned short h;//image height
    unsigned short lpitch ;// allocated memory width 8,16,32,64,128,256,512,1024
    unsigned char bCompress;//0 : no compress, 1 : RLE
    unsigned char bpp;
} SURF_HEADER;

static SURFACE* load_surf_r( FIL* fp )
{
    SURFACE* surf;
    SURF_HEADER surfheader;
    int nRead;

    f_read( fp, &surfheader, sizeof( SURF_HEADER ), &nRead );
    if ( surfheader.magic != ( ( 'S' ) + ( 'U' << 8 ) + ( 'R' << 16 ) + ( 'F' << 24 ) ) )
    {
        debugstring( "Invalid SUF-ID\r\n" );
        return NULL;
    }
    if ( surfheader.bCompress )
    {
        debugstring( "Not supported Format(Compressed)\r\n" );
        return NULL;
    }

    if ( surfheader.bpp == 32 )
    {
        surf = create_surface( surfheader.w, surfheader.h, 32 );
        if ( surf == NULL )
        {
            debugstring( "Error : insufficient memory\n" );
            return NULL;
        }
        surf->pixtype = PIX_FMT_ARGB;
    }
    else if ( surfheader.bpp == 24 )
    {
        surf = create_surface( surfheader.w, surfheader.h, 32 );
        if ( surf == NULL )
        {
            debugstring( "Error : insufficient memory\n" );
            return NULL;
        }
        surf->pixtype = PIX_FMT_RGB888;
    }
    else
    {
        surf = create_surface( surfheader.w, surfheader.h, surfheader.bpp );
        if ( surf == NULL )
        {
            debugstring( "Error : insufficient memory\n" );
            return NULL;
        }
        if ( surfheader.bpp == 16 )
            surf->pixtype = PIX_FMT_RGB565;
        else if ( surfheader.bpp == 8 )
            surf->pixtype = PIX_FMT_PALETTE;
        else
        {
            debugstring( "Invalid Pixel format\r\n" );
            release_surface( surf );
            return NULL;
        }
    }

    if ( surf->pitch < surfheader.lpitch )
    {
        debugstring( "Invalid line pitch\r\n" );
        release_surface( surf );
        return NULL;
    }
    surf->pitch = surfheader.lpitch;
    if ( surfheader.bpp != 8 )
    {
        f_read( fp, surf->pixels, ( surf->pitch * surf->h ), &nRead );
    }
    else
    {
        f_read( fp, surf->pixels, surf->h * surf->pitch, &nRead );
        f_read( fp, surf->pal->colors, 1024, &nRead );
    }
    return surf;
}
/**
 * Load surface from a named SUF file.
 *
 * \param [in] fname Pointer to a null-terminated string .
 *
 * \return null if it fails, else the new surface pointer.
 */
SURFACE* loadsurf( char* fname )
{

    SURFACE* surf;
    FIL fp;
    FRESULT res = f_open( &fp, fname, FA_READ | FA_OPEN_EXISTING );
    if( res != FR_OK )
    {
        DEBUGPRINTF( "Cannot open : %s\r\n", fname );
        return 0;
    }
    int fsize = f_size( &fp );
    if( fsize < sizeof( SURF_HEADER ) )
        return 0;

    surf = load_surf_r( &fp );

    f_close( &fp );
    return surf;
}

SURFACE* loadsurf_filepos( FIL* fp )
{
    return load_surf_r( fp );
}

BOOL loadsurf_filepos_on_surface( SURFACE* dst, FIL* fp, int x, int y )
{
    SURF_HEADER surfheader;
    int nRead;

    f_read( fp, &surfheader, sizeof( SURF_HEADER ), &nRead );
    if ( surfheader.magic != ( ( 'S' ) + ( 'U' << 8 ) + ( 'R' << 16 ) + ( 'F' << 24 ) ) )
    {
        debugstring( "Invalid SUF-ID\r\n" );
        return FALSE;
    }
    if ( surfheader.bCompress )
    {
        debugstring( "Not supported Format(Compressed)\r\n" );
        return FALSE;
    }
    if ( surfheader.bpp != dst->bpp )
    {
        DEBUGPRINTF( "not equal to bpp\r\n" );
        return FALSE;
    }
    if ( surfheader.lpitch != surfheader.lpitch )
    {
        debugstring( "Invalid line pitch\r\n" );
        return FALSE;
    }
    int srcpitch = dst->pitch;
    int dstpitch = surfheader.lpitch;
    int linecnt = dst->h - y;
    if( linecnt > surfheader.h )
        linecnt = surfheader.h;
    int w = dst->w - x;
    if( w > surfheader.w )
        w = surfheader.w;
    int bytecnt = w * surfheader.bpp / 8;
    U8 *tar;
    switch ( surfheader.bpp )
    {
    case 16:
        tar = ( U8* )GETPOINT16( dst, x, y );
        break;
    case 24:
    case 32:
        tar = ( U8* )GETPOINT16( dst, x, y );
        break;
    case 8:
        tar = ( U8* )GETPOINT8( dst, x, y );
        break;
    default:
		return FALSE;
    }

    int i;
    int pos = f_tell( fp );
    for( i = 0; i < linecnt; i++ )
    {
        f_read( fp, tar, bytecnt, &nRead );
        tar += dstpitch;
        pos += srcpitch;
        f_lseek( fp, pos );
    }
    if ( surfheader.bpp != 8 )
    {
        f_read( fp, dst->pal->colors, 1024, &nRead );
    }
    return TRUE;
}
