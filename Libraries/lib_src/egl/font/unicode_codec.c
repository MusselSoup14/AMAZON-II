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
#include "egl_config.h"
#include "egl_typedef.h"

int DecodeUTF8(const char *Buffer, unsigned int *pLen)
{
	int value = 0;
	U8 byte1 = (U8)Buffer[0];
	// 0 to 0x7f
	if( (byte1 & 0x80) == 0 )
	{
		*pLen = 1;
		return byte1;
	}
	//to 0x7ff
	else if ((byte1 & 0xE0) == 0xC0)
	{
		U8 byte2 = (U8)Buffer[1];
		value = ((byte1 & 0x1f) << 6 ) | byte2;
		*pLen = 2;
		return value;
	}
	// to 0xffff
	else if ((byte1 & 0xF0) == 0xE0)
	{
		U8 byte2 = (U8)Buffer[1]&0x3f;
		U8 byte3 = (U8)Buffer[2]&0x3f;
		value = ((byte1 & 0x0f) << 12) | (byte2 << 6) | byte3;
		*pLen = 3;
		return value;
	}
	// to 0x1fffff
	else if( (byte1 & 0xF8) == 0xF0 )
	{
		U8 byte2 = (U8)Buffer[1] & 0x3f;
		U8 byte3 = (U8)Buffer[2] & 0x3f;
		U8 byte4 = (U8)Buffer[3] & 0x3f;
		value = ((byte1 & 0x07) << 18) | (byte2 << 12) | (byte3 << 6) | byte4;
		*pLen = 4;
		return value;
	}
	return -1;
}


int DecodeUTF16(const char *Buffer, unsigned int *pLen)
{
	int re = (int)Buffer[0] + ((int)Buffer[1] << 8);
	
	if( re < 0xd800 || re > 0xdfff )
	{
		*pLen = 2;
		return re;
	}
	else if( re < 0xdc00 )
	{
		int value2 = Buffer[2] + ((unsigned int)Buffer[3] << 8);
		re = ((re & 0x3FF)<<10);
		re = re + (value2 & 0x3ff) + 0x10000;
		*pLen = 4;
		return re;
	}
	return -1;
}

#ifndef _CODE_PAGE
#define _CODE_PAGE	CONFIG_CODE_PAGE
#endif
#if CONFIG_CODE_PAGE == 932
#include "cp/cc932.c"

#elif CONFIG_CODE_PAGE == 936
#include "cp/cc936.c"

#elif CONFIG_CODE_PAGE == 949
#include "cp/cc949.c"

#elif CONFIG_CODE_PAGE == 950
#include "cp/cc950.c"

#else 
#include "cp/ccsbcs.c"
#endif


int oem_to_unicode(unsigned short src)
{
	return ff_convert(src,1);
}
int unicode_to_oem(unsigned short src)
{
	return ff_convert(src,0);
}
