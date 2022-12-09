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
 * \brief BMP Image Driver
 */

#include "sdk.h"

#define BI_RGB        0L
typedef struct
{
    U32   bfSize;
    U32    bfReserved;
    U32   bfOffBits;
    U32  biSize;
    S32   biWidth;
    S32   biHeight;
    U16   biPlanes;
    U16   biBitCount;
    U32  biCompression;
    U32  biSizeImage;
    S32   biXPelsPerMeter;
    S32   biYPelsPerMeter;
    U32  biClrUsed;
    U32  biClrImportant;
} BITMAPFILEHEADER;

typedef struct
{
    U8    rgbBlue;
    U8    rgbGreen;
    U8    rgbRed;
    U8    rgbReserved;
} RGBQUAD;

#define EXTRACT_READ32(startaddr,offset) (U32)(startaddr[offset] + (U32)(startaddr[offset+1]<<8) + (U32)(startaddr[offset+2]<<16) + (U32)(startaddr[offset+3]<<24))

static BITMAPFILEHEADER bmpfh;

/**
 * Load a RGB Data into a SURFACE.
 *
 * \param [in] bmpdata Pointer to BMP Image.
 * \param bpp			   The bits per pixel.
 * \param w				   The width of source rectangles.
 * \param h				   The width of source rectangles.
 * \param bmpdatasize	   The length of data [in bytes].
 * \param [in] pal	   The palette address.
 *
 * \return null if it fails, else the new surface pointer.
 */

static SURFACE* LoadSurfaceInfoFromRGB( U8* bmpdata, U8 bpp, U32 w, U32 h, U32 bmpdatasize, U8* pal )
{
    SURFACE *surface = NULL;
    long i;
    U32  j;
    if( !( ( bpp == 24 ) || ( bpp == 8 ) || ( bpp == 4 ) ) )
    {
        return 0;
    }
    if( ( bpp == 8 ) || ( bpp == 4 ) )
    {
        if( !( surface = create_surface( w, h, bpp ) ) )
        {
            return 0;
        }
        U32 ibpl = ( bmpfh.bfSize - bmpfh.bfOffBits ) / h;
        surface->pitch = ibpl;
        if( bpp == 8 )
        {
            for( i = 0 ; i < h ; i++ )
            {
                memcpy( surface->pixels + i * ibpl, bmpdata + ( h - 1 - i )*ibpl, surface->w );
            }
            surface->pal->nColors = 256;
            memcpy( surface->pal->colors, pal, 256 * 4 );
        }
        else
        {
            for( i = 0 ; i < h ; i++ )
            {
                memcpy( surface->pixels + i * ibpl, bmpdata + ( h - 1 - i )*ibpl, surface->w / 2 );
            }
            surface->pal->nColors = 16;
            memcpy( surface->pal->colors, pal, 16 * 4 );
        }

        return surface;
    }
    else if( bpp == 24 )
    {
        U32 screenbpp = get_screen_bpp();
        if( !( surface = create_surface( w, h, screenbpp ) ) )
        {
            return 0;
        }

        if( screenbpp == 32 ) //screen: 4byte(1byte reserved) per pixel
        {
            U32 ibpl;
            U32* rgb32;
            ibpl = ( ( w * 3 ) + 3 ) & 0xfffffffc;
            rgb32 = ( U32* )surface->pixels;
            bmpdata += ( ( h - 1 ) * ibpl );
            for( i = 0 ; i < h ; i++ )
            {
                for( j = 0 ; j < w ; j++ )
                {
                    rgb32[j] = MAKE_RGB888( bmpdata[j * 3 + 2], bmpdata[j * 3 + 1], bmpdata[j * 3 + 0] );
                }
                bmpdata -= ibpl; //4byte align
                rgb32 += ( surface->pitch / 4 );
            }
            surface->pixtype = PIX_FMT_RGB888;
        }
        else // convert to rgb888 to rgb565
        {
            U32 ibpl;
            U16* rgb565data;
            ibpl = ( ( w * 3 ) + 3 ) & 0xfffffffc;
            rgb565data = ( U16* )surface->pixels;

            bmpdata += ( ( h - 1 ) * ibpl );
            for( i = 0 ; i < h ; i++ )
            {
                for( j = 0 ; j < w ; j++ )
                {
                    rgb565data[j] = MAKE_RGB565( bmpdata[j * 3 + 2], bmpdata[j * 3 + 1], bmpdata[j * 3 + 0] );
                }
                bmpdata -= ibpl; //4byte align
                rgb565data += ( surface->pitch / 2 );
            }
            surface->pixtype = PIX_FMT_RGB565;
        }

    }
    return surface;

}

/**
 * Load a surface from a named Windows BMP file.
 *
 * \param [in] fname Pointer to a null-terminated string that specifies the path name of the image file(BMP).
 *
 * \return null if it fails, else the new surface pointer.
 * 		   
 * \code{.c}
 * SURFACE* surf = loadbmp("test.bmp");
 * \endcode
 * 						
 * \see release_surface
 */

SURFACE* loadbmp( char* fname )
{
    U32 nRead;
    FIL fp;
    SURFACE* surface;
    U8* bmpdata;
    U8* pal = 0;
    U32 datasize;
    U16 ID;

    /* load windows BMP file */
    FRESULT res = f_open( &fp, fname, FA_READ | FA_OPEN_EXISTING );
    if( res != FR_OK )
    {
        DEBUGPRINTF( "Cannot open : %s\r\n", fname );
        return 0;
    }
    f_read( &fp, &ID, 2, &nRead );
    f_read( &fp, ( void * )( &bmpfh ), sizeof( BITMAPFILEHEADER ), &nRead );
    if( ID != 0x4d42 ) // "BM"
    {
        debugstring( "Invalid BMP-ID\r\n" );
        f_close( &fp );
        return 0;
    }

    if( bmpfh.biBitCount == 8 || bmpfh.biBitCount == 4 )
    {
        U32 coltableoffset = 14 + bmpfh.biSize;
        f_lseek( &fp, coltableoffset );
        if( bmpfh.biBitCount == 4 )
        {
            pal = ( U8* )malloc( 64 );
            f_read( &fp, pal, 64, &nRead );
        }
        else
        {
            U32 coltablesize = bmpfh.bfOffBits - coltableoffset;
            pal = ( U8* ) malloc( coltablesize );
            f_read( &fp, pal, coltablesize, &nRead );
        }
    }
    f_lseek( &fp, bmpfh.bfOffBits );
    datasize = bmpfh.bfSize - bmpfh.bfOffBits;
    bmpdata = ( U8* ) malloc( datasize );
    f_read( &fp, bmpdata, datasize, &nRead );
    f_close( &fp );

    surface = LoadSurfaceInfoFromRGB( bmpdata, bmpfh.biBitCount, bmpfh.biWidth, bmpfh.biHeight, datasize, pal );
    if( pal )
        free( pal );
    free( bmpdata );
    return surface;
}

/**
 * Loads a surface from a pointer
 *
 * \param [in] startaddr Pointer to BMP Image.
 *
 * \return null if it fails, else the new surface pointer.
 */

SURFACE* loadbmpp( U8* startaddr )
{
    U32 datasize;
    U16 bpp;
    U32 w, h;
    U8* bmpdata;
    U32 filesize;
    U32 offset;
    U32 bisize;

    if( ( startaddr[0] != 0x42 ) || ( startaddr[1] != 0x4d ) ) // "BM"
    {
        DEBUGPRINTF( "Invalid BMP-ID\r\n" );
        return 0;
    }
    filesize = EXTRACT_READ32( startaddr, 2 );
    offset = EXTRACT_READ32( startaddr, 10 );
    bisize = EXTRACT_READ32( startaddr, 14 );
    datasize = filesize - offset;
    bmpdata = ( U8* )( offset + ( U32 )( startaddr ) );
    bpp = ( U32 )startaddr[0x1c] + ( U32 )( startaddr[0x1d] << 8 );
    w = EXTRACT_READ32( startaddr, 0x12 );
    h = EXTRACT_READ32( startaddr, 0x16 );
    return LoadSurfaceInfoFromRGB( bmpdata, bpp, w, h, datasize, startaddr + 14 + bisize );
}
