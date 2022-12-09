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
 * \brief PNG Image Driver
 */
#include "sdk.h"
#include "lpng/png.h"
static png_bytep startbuf=0;
static png_size_t curp=0;
static png_size_t data_len=0;
static void setdata(png_bytep buf, png_size_t len)
{
	startbuf = buf;
	data_len = len;
	curp=0;
}

static void PNGCBAPI readfunc2(png_structp png_ptr,png_bytep data,png_size_t length)
{
	U32 remain=data_len - curp;
	if(length>remain)
	{
		length = remain;
	}
	memcpy(data,startbuf+curp,length);
	curp+=length;
}

static SURFACE* png_loadmem(char* pngbuf,int datalen,int* w,int* h,int* bpp)
{
	
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 y, width, height;
	int bit_depth, color_type;
	png_bytepp ppbRowPointers=0;
	SURFACE* surf =0;
//	int  interlace_type;
	int row;
	setdata(pngbuf,datalen);
	
	if(!png_check_sig(pngbuf,8))
		return 0;
	
	/* Create and initialize the png_struct with the desired error handler
	* functions.  If you want to use the default stderr and longjump method,
	* you can supply NULL for the last three parameters.  We also supply the
	* the compiler header file version, so that we know if the application
	* was compiled with a compatible version of the library.  REQUIRED
	*/
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,NULL);
	
	if (png_ptr == NULL)
	{
		return (0);
	}

	/* Allocate/initialize the memory for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return (0);
	}

	/* Set error handling if you are using the setjmp/longjmp method (this is
	* the normal method of doing things with libpng).  REQUIRED unless you
	* set up your own error handlers in the png_create_read_struct() earlier.
	*/

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		debugstring("Long Jump\r\n");
		if(surf)
			release_surface(surf);
		if(ppbRowPointers)
			free(ppbRowPointers);
		/* Free all of the memory associated with the png_ptr and info_ptr */
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		/* If we get here, we had a problem reading the file */
		return (0);
	}

	png_set_read_fn(png_ptr,NULL,readfunc2);
	//png_init_io(png_ptr, fp);
	
	/* If we have already read some of the signature */
	png_set_sig_bytes(png_ptr, sig_read);


	/* OK, you're doing it the hard way, with the lower-level functions */

	/* The call to png_read_info() gives us all of the information from the
	* PNG file before the first IDAT (image data chunk).  REQUIRED
	*/
	png_read_info(png_ptr, info_ptr);
	
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		NULL, NULL, NULL);
	*w = width;
	*h = height;
	if(color_type & PNG_COLOR_MASK_ALPHA)
		*bpp = 32;
	else
		*bpp = 24;
	
	//int number_passes = png_set_interlace_handling(png_ptr);
	

	/* Optional call to gamma correct and add the background to the palette
	* and update info structure.  REQUIRED if you are expecting libpng to
	* update the palette for you (ie you selected such a transform above).
	*/
	png_read_update_info(png_ptr, info_ptr);
	
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		NULL, NULL, NULL);

	//png_free_data(png_ptr, info_ptr, PNG_FREE_ROWS, 0);

	/* Allocate the memory to hold the image using the fields of info_ptr. */

	row = png_get_rowbytes(png_ptr,info_ptr);
	surf = create_surface(width,height,*bpp);
	if(surf==0)
		return 0;
	
	ppbRowPointers=(png_bytepp)malloc(height * sizeof(png_bytep));
	
	for (y = 0; y < height; y++)
	{
		ppbRowPointers[y] = (png_bytep )((int)surf->pixels + (y*surf->pitch));
		//DEBUGPRINTF("ppbRowPointers[%d]=%#X\r\n",y,ppbRowPointers[y]);
	}
	png_set_bgr(png_ptr);//ARGB 로 나오도록
	png_read_image(png_ptr, ppbRowPointers);
	
	free(ppbRowPointers);
	/* Read rest of file, and get additional chunks in info_ptr - REQUIRED */
	png_read_end(png_ptr, info_ptr);

	/* At this point you have read the entire image */

	/* Clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	/* That's it */
	return (surf);
}
/**
 * Load surface from a pointer(PNG).
 *
 * \param [in] pngbuf Pointer to PNG.
 * \param datalen  The length of data.
 *
 * \return null if it fails, else the new surface pointer.
 */
SURFACE* loadpngp( U8* pngbuf,U32 datalen)
{
	int w,h,bpp;
	SURFACE* sf =png_loadmem(pngbuf,datalen,&w,&h,&bpp);
	if(sf==0)
	{
		debugstring("PNG load error\r\n");
		return 0;
	}
	sf->pixtype = PIX_FMT_ARGB;
	return sf;
}
/**
 * Load surface from a named PNG file.
 *
 * \param [in] filename Pointer to a null-terminated string .
 *
 * \return null if it fails, else the new surface pointer.
 */
SURFACE* loadpng( char* filename )
{
	U32 fsize;
	unsigned char* pngbuf = readfile(filename,&fsize);
	if(pngbuf==0)
		return 0;
	SURFACE* psurf = loadpngp(pngbuf,fsize);
	free(pngbuf);
	return psurf;
}


