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
 * \brief Save data as a JPG file into the file-system
 */
#include "sdk.h"
#include <setjmp.h>
#include <libjpg/jpeglib.h>

struct my_error_mgr
{
    struct jpeg_error_mgr errmgr;
    jmp_buf escape;
};

static void my_error_exit( j_common_ptr cinfo )
{
    struct my_error_mgr *err = ( struct my_error_mgr * )cinfo->err;
    longjmp( err->escape, 1 );
}

static BOOL r_saveJpeg( JSAMPLE* image_buffer, int image_width, int image_height, int row_stride /* physical row width in image buffer */
                        , unsigned char** poutputbuf, unsigned long* poutputbufsize )
{
    struct jpeg_compress_struct cinfo;
    struct my_error_mgr jerr;
    JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */

    /* Step 1: allocate and initialize JPEG compression object */

    /* We have to set up the error handler first, in case the initialization
    * step fails.  (Unlikely, but it could happen if you are out of memory.)
    * This routine fills in the contents of struct jerr, and returns jerr's
    * address which we place into the link field in cinfo.
    */
    cinfo.err = jpeg_std_error( &jerr.errmgr );
    jerr.errmgr.error_exit = my_error_exit;

    if( setjmp( jerr.escape ) )
    {
        /* If we get here, libjpeg found an error */
        jpeg_destroy_compress( &cinfo );
        return 0;
    }
    /* Now we can initialize the JPEG compression object. */
    jpeg_create_compress( &cinfo );

    /* Step 2: specify data destination (eg, a file) */
    /* Note: steps 2 and 3 can be done in either order. */

    jpeg_mem_dest( &cinfo, poutputbuf, poutputbufsize );


    /* Step 3: set parameters for compression */

    /* First we supply a description of the input image.
    * Four fields of the cinfo struct must be filled in:
    */
    cinfo.image_width = image_width; 	/* image width and height, in pixels */
    cinfo.image_height = image_height;
    cinfo.input_components = 3;		/* # of color components per pixel */
    cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
    /* Now use the library's routine to set default compression parameters.
    * (You must set at least cinfo.in_color_space before calling this,
    * since the defaults depend on the source color space.)
    */
    jpeg_set_defaults( &cinfo );
    /* Now you can set any non-default parameters you wish to.
    * Here we just illustrate the use of quality (quantization table) scaling:
    */
    //jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

    /* Step 4: Start compressor */

    /* TRUE ensures that we will write a complete interchange-JPEG file.
    * Pass TRUE unless you are very sure of what you're doing.
    */
    jpeg_start_compress( &cinfo, TRUE );

    /* Step 5: while (scan lines remain to be written) */
    /*           jpeg_write_scanlines(...); */

    while ( cinfo.next_scanline < cinfo.image_height )
    {
        /* jpeg_write_scanlines expects an array of pointers to scanlines.
        * Here the array is only one element long, but you could pass
        * more than one scanline at a time if that's more convenient.
        	*/
        row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
        ( void ) jpeg_write_scanlines( &cinfo, row_pointer, 1 );
    }

    /* Step 6: Finish compression */

    jpeg_finish_compress( &cinfo );

    /* Step 7: release JPEG compression object */

    /* This is an important step since it will release a good deal of memory. */
    jpeg_destroy_compress( &cinfo );

    return TRUE;
}


/**
 * Save data as a JPG file into the file-system.
 *
 * \param [in] savefname  Pointer to a null-terminated string.
 * \param [in] src		 surface to save.
 *
 * \return true if it succeeds, false if it fails.
 */
BOOL savejpg( char* savefname, SURFACE* src )
{
    if( src->bpp < 16 )
        return FALSE;
    //convert to 24bit
    int bmplpitch = src->w * 3;
    U8* rgbbuf = ( U8* )malloc( src->w * src->h * 3 );
    if( rgbbuf == NULL )
        return FALSE;
    int i;
    if( src->bpp == 16 )
    {
        //convet rgb565 to rgb888;
        U16* rgb565buf;
        rgb565buf = ( U16* )src->pixels;
        U8* dest = rgbbuf;
        for( i = 0; i < src->h; i++ )
        {
            int k;
            int j = 0;
            U8 r, g, b;
            for( k = 0; k < src->w; k++ )
            {
                U16 rgb565 = rgb565buf[k];
                r = ( rgb565 >> 11 ) << 3;
                g = ( rgb565 >> 5 ) << 2;
                b = rgb565 << 3;
                dest[j] = r;
                dest[j + 1] = g;
                dest[j + 2] = b;
                j += 3;
            }
            dest += bmplpitch;
            rgb565buf += ( src->pitch / 2 );
        }

    }
    else//32 to 24
    {
        U8* rgb8buf;
        rgb8buf = ( U8* )src->pixels;
        U8* dest = rgbbuf;
        for( i = 0; i < src->h; i++ )
        {
            int k;
            int j = 0;
            int n = 0;
            for( k = 0; k < src->w; k++ )
            {
                dest[j]  = rgb8buf[n];
                dest[j + 1]  = rgb8buf[n + 1];
                dest[j + 2]  = rgb8buf[n + 2];
                j += 3;
                n += 4;
            }
            dest += bmplpitch;
            rgb8buf +=  src->pitch ;
        }
    }
    unsigned char* outputbuf;
    unsigned long outputbufsize;
    outputbufsize = src->w * src->h / 2;
    outputbuf = ( unsigned char* )malloc( outputbufsize );

    if( r_saveJpeg( rgbbuf, src->w, src->h, bmplpitch, &outputbuf, &outputbufsize ) == FALSE )
    {
        free( rgbbuf );
        return FALSE;
    }

    FIL f;
    FIL* fp;
    fp = &f;
    if( FR_OK != f_open( fp, savefname, FA_CREATE_ALWAYS | FA_WRITE ) )
    {
        free( outputbuf );
        return FALSE;
    }

    UINT nWrite;
    if( f_write( fp, outputbuf, outputbufsize, &nWrite ) != FR_OK )
    {
        f_close( fp );
        free( outputbuf );
        return FALSE;
    }
    free( outputbuf );
    f_close( fp );
    return TRUE;
}
