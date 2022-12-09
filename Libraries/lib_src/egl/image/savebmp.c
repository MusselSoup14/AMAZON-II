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
 * \brief Save data as a BMP file into the file-system
 */
#include "sdk.h"
#define MEMZERO(target,size)	memset((void *)(target), 0, (size_t)(size))
#define MEMCOPY(dest,src,size)	memcpy((void *)(dest), (const void *)(src), (size_t)(size))

#define PUT_2B(array,offset,value)  \
    (array[offset] = (char) ((value) & 0xFF), \
     array[offset+1] = (char) (((value) >> 8) & 0xFF))
#define PUT_4B(array,offset,value)  \
    (array[offset] = (char) ((value) & 0xFF), \
     array[offset+1] = (char) (((value) >> 8) & 0xFF), \
     array[offset+2] = (char) (((value) >> 16) & 0xFF), \
     array[offset+3] = (char) (((value) >> 24) & 0xFF))

static BOOL write_bmp_header( FIL* fp, SURFACE* src )
{
    char bmpfileheader[14];
    char bmpinfoheader[40];
    int headersize, bfSize;
    int bits_per_pixel, cmap_entries;

    bits_per_pixel = 24;
    cmap_entries = 0;

    /* 4-byte boundary */
    int bmplpitch = ( ( src->w * bits_per_pixel / 8 ) + 3 ) & 0xfffffffc;
    /* File size */
    headersize = 14 + 40 + cmap_entries * 4; /* Header and colormap */
    bfSize = headersize + bmplpitch * src->h;

    /* Set unused fields of header to 0 */
    MEMZERO( bmpfileheader, sizeof( bmpfileheader ) );
    MEMZERO( bmpinfoheader, sizeof( bmpinfoheader ) );

    /* Fill the file header */
    bmpfileheader[0] = 0x42;	/* first 2 bytes are ASCII 'B', 'M' */
    bmpfileheader[1] = 0x4D;
    PUT_4B( bmpfileheader, 2, bfSize ); /* bfSize */
    /* we leave bfReserved1 & bfReserved2 = 0 */
    PUT_4B( bmpfileheader, 10, headersize ); /* bfOffBits */

    /* Fill the info header (Microsoft calls this a BITMAPINFOHEADER) */
    PUT_2B( bmpinfoheader, 0, 40 );	/* biSize */
    PUT_4B( bmpinfoheader, 4, src->w ); /* biWidth */
    PUT_4B( bmpinfoheader, 8, src->h ); /* biHeight */
    PUT_2B( bmpinfoheader, 12, 1 );	/* biPlanes - must be 1 */
    PUT_2B( bmpinfoheader, 14, bits_per_pixel ); /* biBitCount */
    /* we leave biCompression = 0, for none */
    /* we leave biSizeImage = 0; this is correct for uncompressed data */

    PUT_2B( bmpinfoheader, 32, cmap_entries ); /* biClrUsed */
    /* we leave biClrImportant = 0 */
    unsigned int nWrite;
    if( f_write( fp, bmpfileheader, 14, &nWrite ) != FR_OK )
        return FALSE;
    if( f_write( fp, bmpinfoheader, 40, &nWrite ) != FR_OK )
        return FALSE;
    return TRUE;
}
/**
 * Save data as a BMP file into the file-system.
 *
 * \param [in] savefname  Pointer to a null-terminated string.
 * \param [in] src		 surface to save.
 *
 * \return true if it succeeds, false if it fails.
 */
BOOL savebmp( char* savefname, SURFACE* src )
{
    int i;
	int bmplpitch;
	UINT nWrite;
	BYTE* bgrbuf;
	FIL f;
	FIL* fp = &f;
    if( src->bpp < 16 )
        return FALSE;
    if( FR_OK != f_open(fp, savefname, FA_WRITE | FA_CREATE_ALWAYS ) )
        return FALSE;
    if( write_bmp_header(fp, src ) == FALSE )
    {
        f_close( fp );
        return FALSE;
    }

	bmplpitch = ( ( src->w * 3 ) + 3 ) & 0xfffffffc;
	bgrbuf = (BYTE*)malloc(bmplpitch*src->h);

    //write data, B/G/R
	BYTE* dest = bgrbuf;
	if(src->bpp==16)
	{
		//convet rgb565 to rgb888;
		U16* rgb565buf;
		rgb565buf=(U16*)src->pixels;
		rgb565buf += ((src->h-1)*(src->pitch/2));
		for( i = 0; i < src->h; i++ )
		{
			int k;
			int j=0;
			U8 r,g,b;
			for(k=0;k<src->w;k++)
			{
				U16 rgb565 = rgb565buf[k];
				r = (rgb565>>11)<<3;
				g = (rgb565>>5)<<2;
				b = rgb565<<3;
				dest[j]=b;
				dest[j+1]=g;
				dest[j+2]=r;
				j+=3;
			}
			dest += bmplpitch;
			rgb565buf -= (src->pitch/2);
		}
	}
	else
	{
		BYTE* rgbbuf;
		rgbbuf=(BYTE*)src->pixels;
		rgbbuf+= ((src->h-1)*src->pitch);
		for( i = 0; i < src->h; i++ )
		{
		   int k;
		   int j=0;
		   int n=0;
		   for(k=0;k<src->w;k++)
		   {
			   dest[j]=rgbbuf[n+2];
			   dest[j+1]=rgbbuf[n+1];
			   dest[j+2]=rgbbuf[n];
			   j+=3;
			   n+=4;
		   }
		   dest += bmplpitch;
		   rgbbuf-= src->pitch;
	   }
    }
	if( f_write( fp, bgrbuf, bmplpitch*src->h, &nWrite ) != FR_OK )
	{
		free(bgrbuf);
		f_close( fp );
		return FALSE;
	}
	
	free(bgrbuf);
    f_close( fp );

    return TRUE;
}
