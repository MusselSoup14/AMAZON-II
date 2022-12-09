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
* \brief JPEG Image H/W Driver
*/
#include "sdk.h"
#include <setjmp.h>
#include <libjpg/jpeglib.h>
#include "image/loadjpg_hw.h"
#include "egl_config.h"

// MARKER
#define SOF 0xC0

BOOL drawjpgp_hw(void* imgbuf, U32 filesize, SURFACE* dst, int x, int y)
{
	int i;
	U16 w = 0;
	U16 h = 0;
	U16 soflength;
	U8  ncomp;
	U8 bps;//data precision (1 byte) in bits/sample, usually 8
	U8 yc = 0;
	U8 *databuf = (U8*)imgbuf;

	if (((*(U8 *)databuf) != 0xFF) || (*(U8 *)(databuf + 1) != 0xD8))
	{
		DEBUGPRINTF("invalid or unsupported JPEG Format(0x%X%X)\r\n", *(U8 *)(databuf + 1), *(U8 *)databuf);
		return 0;
	}

	for (i = 2; i < filesize; i += 1)
	{
		if (*(U8 *)(databuf + i) == 0xff)
		{
			i++;

			//find SOF
			if (*(U8 *)(databuf + i) == SOF)
			{
				soflength = ((U16)* (U8 *)(databuf + i + 1) << 8) + (U16)* (U8 *)(databuf + i + 2);
				bps = *(U8 *)(databuf + i + 3);
				h = ((U16)* (U8 *)(databuf + i + 4) << 8) + (U16)* (U8 *)(databuf + i + 5);
				w = ((U16)* (U8 *)(databuf + i + 6) << 8) + (U16)* (U8 *)(databuf + i + 7);
				ncomp = *(U8 *)(databuf + i + 8);
				yc = *(U8 *)(databuf + i + 10);
				//				DEBUGPRINTF("width : %d, height : %d \r\n",w,h);
				break;
			}
		}
	}

	if (i >= filesize)
		return 0;

	if (w < 8)
		return 0;

	if (w == 0 || h == 0)
	{
		DEBUGPRINTF("image width or height is 0\r\n");
		return 0;
	}

	if ((yc == 0x22 || yc == 0x20 || yc == 0x21) == 0)
	{
		DEBUGPRINTF("invalid or unsupported JPEG Format(%#X)\r\n", *(U16 *)databuf);
		return FALSE;
	}

	void* targetbuf = ((U8*)dst->pixels) + (x * (dst->bpp / 8)) + (y + dst->pitch);
#ifdef HW_2D_ENGINE
	if (dst == EGL_GET_TARGET_SURFACE())
	{
		ge_wait_cmd_complete();
	}
#endif
	if ((x + w) >= dst->w)
		return FALSE;
	if ((y + h) >= dst->h)
		return FALSE;
	return jpeg_hw_decode_raw((U32)databuf, filesize, targetbuf, dst->pitch / (dst->bpp / 8), dst->bpp);
}
BOOL drawjpg_hw(char *filename, SURFACE* dst, int x, int y)
{
	U32 nRead;
	U32 filesize;
	char *databuf;
	FIL fp;
	FRESULT res = f_open(&fp, filename, FA_READ | FA_OPEN_EXISTING);

	if (res != FR_OK)
	{
		DEBUGPRINTF("Cannot open : %s\r\n", filename);
		return FALSE;
	}

	filesize = f_size(&fp);
	databuf = (char *)malloc(filesize);
	if (databuf == NULL)
	{
		DEBUGPRINTF("Error : Not Enough Memory!\r\n");
		return FALSE;
	}
	f_read(&fp, databuf, filesize, &nRead);
	f_close(&fp);
	BOOL re = drawjpgp_hw(databuf, filesize,dst,x,y);
	free(databuf);
	return re;
}
