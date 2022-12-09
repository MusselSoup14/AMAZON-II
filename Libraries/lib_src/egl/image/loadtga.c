/*****************************************************************************
*
* Copyright (C) 2016     Advanced Digital Chips, Inc. All Rights Reserved.
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

enum
{
	DATA_TYPE_NO_IMAGE = 0,
	DATA_TYPE_INDEXED = 1,
	DATA_TYPE_RGB = 2,
	DATA_TYPE_BW = 3,
	DATA_TYPE_RLE_INDEXED = 9,
	DATA_TYPE_RLE_RGB = 10,
	DATA_TYPE_RLE_BW = 11
};

#define TGA_INTERLEAVE_MASK	0xc0
#define TGA_INTERLEAVE_NONE	0x00

#define TGA_ORIGIN_MASK		0x30
#define TGA_ORIGIN_LEFT		0x00
#define TGA_ORIGIN_RIGHT	0x10
#define TGA_ORIGIN_LOWER	0x00
#define TGA_ORIGIN_UPPER	0x20

/**
* Load surface from a pointer.
*
* \param [in] databuf Pointer to Image.
*
* \return null if it fails, else the new surface pointer.
*/
SURFACE *loadtgap(U8 *databuf, U32 buflen)
{
	int b_rle = 0;
	int indexed = 0;
	int grey = 0;
	int ckey = -1;
	int ncols, w, h;
	U8 *dst;
	int i;
	int bpp; //byte per pixel
	int pal_bits;
	int lpitch;
	int count;
	int repcount;
	SURFACE *surf;
	U8 *src;
	U8 imagetype;
	U8 imagedesc;
	U8 hascmap;

	if (databuf == 0)
		return 0;

	src = databuf + 18 + databuf[0];
	hascmap = databuf[1];
	imagetype = databuf[2];
	ncols = databuf[5] + (databuf[6] << 8);
	pal_bits = databuf[7];
	w = databuf[12] + (databuf[13] << 8);
	h = databuf[14] + (databuf[15] << 8);
	bpp = (databuf[16] + 7) >> 3;
	imagedesc = databuf[17];

	indexed = 0;
	b_rle = 0;

	if (imagetype == DATA_TYPE_RLE_INDEXED)
	{
		b_rle = 1;
		if (!hascmap || bpp != 8 || ncols > 256)
			return 0;
		indexed = 1;
	}
	else if (imagetype == DATA_TYPE_INDEXED)
	{
		if (!hascmap || bpp != 8 || ncols > 256)
			return 0;
		indexed = 1;
	}
	else if (imagetype == DATA_TYPE_RLE_RGB)
	{
		b_rle = 1;
	}
	else if (imagetype == DATA_TYPE_RLE_BW)
	{
		b_rle = 1;
		if (bpp != 1)
			return 0;
		indexed = grey = 1;
	}
	else if (imagetype == DATA_TYPE_BW)
	{
		if (bpp != 1)
			return 0;
		indexed = grey = 1;
	}
	else if (imagetype == DATA_TYPE_RGB)
	{
		//do nothing
	}
	else
	{
		return NULL;
	}

	if (bpp == 1)
	{
		if (!indexed)
			return 0;
	}

	if (((imagedesc & TGA_INTERLEAVE_MASK) != TGA_INTERLEAVE_NONE)
		|| (imagedesc & TGA_ORIGIN_RIGHT))
	{
		return 0;
	}

	surf = create_surface(w, h, bpp * 8);

	if (surf == NULL)
	{
		return 0;
	}
	if (!indexed)
	{
		if (bpp == 4)
		{
			surf->pixtype = PIX_FMT_ARGB;
		}
		else
		{
			surf->pixtype = PIX_FMT_RGB888;
		}
	}
	else
		surf->pixtype = PIX_FMT_PALETTE;


	if (hascmap)
	{
		int pal_size = ncols * ((pal_bits + 7) >> 3);

		if (indexed && !grey)
		{
			U8 *p;
			sARGB *pal_colors = (sARGB*)surf->pal->colors;
			p = src;
			if (pal_bits == 15 || pal_bits == 16)
			{
				for (i = 0; i < ncols; i++)
				{
					U16 c = p[0] + (p[1] << 8);
					p += 2;
					pal_colors[i].r = (BYTE)((c >> 7) & 0xf8);
					pal_colors[i].g = (BYTE)((c >> 2) & 0xf8);
					pal_colors[i].b = (BYTE)(c << 3);
				}
			}
			else if (pal_bits == 24)
			{
				for (i = 0; i < ncols; i++)
				{
					pal_colors[i].b = *p++;
					pal_colors[i].g = *p++;
					pal_colors[i].r = *p++;
				}
			}
			else if (pal_bits == 32)
			{
				for (i = 0; i < ncols; i++)
				{
					pal_colors[i].b = *p++;
					pal_colors[i].g = *p++;
					pal_colors[i].r = *p++;

					if (*p++ < 128)
					{
						ckey = i;
					}
				}
			}

			if (ckey >= 0)
			{
#ifdef HW_2D_ENGINE
				surf->transColor.r = pal_colors[ckey].r;
				surf->transColor.g = pal_colors[ckey].g;
				surf->transColor.b = pal_colors[ckey].b;
				surf->drawmode |= DRAWMODE_TRANSPARENCY;
#else
				surf->pal->colorkey = ckey;
#endif
			}
		}

		src += pal_size;
	}

	if (grey)
	{
		sARGB *pal_colors = (sARGB*)surf->pal->colors;

		for (i = 0; i < 256; i++)
		{
			pal_colors[i].r = pal_colors[i].g = pal_colors[i].b = i;
		}

		//if grey, set colorkey
#ifdef HW_2D_ENGINE
		surf->transColor.r = pal_colors[0].r;
		surf->transColor.g = pal_colors[0].b;
		surf->transColor.b = pal_colors[0].b;
		surf->drawmode |= DRAWMODE_TRANSPARENCY;
#else
		surf->pal->colorkey = 0;
#endif
	}

	if (imagedesc & TGA_ORIGIN_UPPER)
	{
		lpitch = surf->pitch;
		dst = (U8*)surf->pixels;
	}
	else
	{
		lpitch = -(surf->pitch);
		dst = (U8 *)surf->pixels + (h - 1) * surf->pitch;
	}

	//decode and block copy
	count = 0;
	repcount = 0;
	if (b_rle)
	{
		U32 clr;
		for (i = 0; i < h; i++)
		{
			int x = 0;
			while (1)
			{
				U8 c;
				if (count) {
					int n = count;
					if (n > w - x)
						n = w - x;
					memcpy(dst + x * bpp, src, n * bpp);
					src += n * bpp;
					count -= n;
					x += n;
					if (x == w)
						break;
				}
				else if (repcount) {
					int n = repcount;
					if (n > w - x)
						n = w - x;
					repcount -= n;
					if (bpp == 4)
					{
						int k;
						U32*  tdst = (U32*)(dst + x * 4);
						for (k = 0; k < n; k++)
						{
							*tdst = clr;
							tdst++;
						}
						n = 0;
						x += k;
					}
					else
					{
						while (n--) {
							memcpy(dst + x * bpp, &clr, bpp);
							x++;
						}
					}
					if (x == w)
						break;
				}
				c = *(src);
				src++;
				if (c & 0x80) {
					memcpy(&clr, src, bpp);
					src += bpp;
					repcount = (c & 0x7f) + 1;
				}
				else {
					count = c + 1;
				}
			}
			dst += lpitch;
		}
	}
	else
	{
		if (lpitch < 0)
		{
			for (i = 0; i < h; i++)
			{
				memcpy(dst, src, w * bpp);
				src += w * bpp;
				dst += lpitch;
			}
		}
		else
		{
			dcache_invalidate_way();
			dma_blockcpy(dst, src, lpitch, w*bpp, w*bpp, h);
		}
	}

	return surf;
}


/**
* Load surface from a named TGA file.
*
* \param [in] fname Pointer to a null-terminated string .
*
* \return null if it fails, else the new surface pointer.
*/
SURFACE *loadtga(char *fname)
{
	U32 nRead;
	FIL fp;
	U32 datasize;
	U8 *buf;
	FRESULT res = f_open(&fp, fname, FA_READ | FA_OPEN_EXISTING);

	if (res != FR_OK)
	{
		DEBUGPRINTF("Cannot open : %s\r\n", fname);
		return 0;
	}

	datasize = f_size(&fp);
	buf = (U8 *)malloc(datasize);

	if (buf == 0)
	{
		debugstring("memory is insufficient to load file\n");
		return 0;
	}

	f_read(&fp, buf, datasize, &nRead);
	f_close(&fp);
	SURFACE *surf = loadtgap(buf, datasize);
	free(buf);
	return surf;
}
