/*****************************************************************************
*
* Copyright (C) 2017   Advanced Digital Chips, Inc. All Rights Reserved.
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
#include "egl_base.h"
#include "egl_private.h"
#include "font/textout.h"

#include <stdlib.h>
#include <string.h>

typedef struct _tagStringInfo
{
	int x;
	int y;
	int len;
	char* str;
} STRING_INFO;

/*
return last Y cordination
*/
static int r_draw_text_in_box(EGL_FONT* font, EGL_RECT* pRect, const char* text, DRAW_TEXT_IN_BOX_OPT* opt)
{
	int strleninpixel;
	int charheight;
	int x, y, w, h, starty;
	int Draw_xPixel = 0;
	int Draw_LineCnt = 0;
	int Draw_FontCnt;
	int eraselen;
	unsigned char temp_u8;
	int temp_s32;
	int i;
	int align = opt->align;
	char* str = 0; //needs free

	STRING_INFO str_info[64];

	x = pRect->x;
	y = pRect->y;
	w = pRect->w;
	h = pRect->h;
	strleninpixel = text_width(font, text);
	charheight = font->h + opt->line_spacing;

	if (strleninpixel == 0 || charheight == 0 || charheight > h)
	{
		return y;
	}
	if (strleninpixel > w)
	{
		char* pch;
		int len;
		str = strdup(text);
		pch = strtok(str, "\n");

		while (pch)
		{
			Draw_FontCnt = 0;
			while (1)
			{
				len = text_count_in_width(font, pch + Draw_FontCnt, w);
				if (len == 0)
					break;
				str_info[Draw_LineCnt].str = pch + Draw_FontCnt;
				if (opt->wordsplit)
				{
					//reverse search space
					if ((pch[Draw_FontCnt + len] != 0x0) && (pch[Draw_FontCnt + len] != 0x20))
					{
						for (i = len - 2; i > 0; i--)
						{
							if (pch[Draw_FontCnt + i] == 0x20)
							{
								pch[Draw_FontCnt + i] = 0;//null terminate
								len = i;
								Draw_FontCnt++;
								break;
							}
						}
					}
				}
				str_info[Draw_LineCnt].len = len;
				Draw_FontCnt += len;
				Draw_LineCnt++;
			}
			pch = strtok(NULL, "\n");
		}
	}
	else
	{
		str_info[Draw_LineCnt].len = text_count_in_width(font, text, w);
		str_info[Draw_LineCnt].str = (char*)text;
		Draw_LineCnt = 1;
	}

	Draw_FontCnt = 0;

	if ((align&EGL_ALIGN_MULTI_LINE) != EGL_ALIGN_MULTI_LINE)
	{
		Draw_LineCnt = 1;
	}

	if (align&EGL_ALIGN_TOP)
	{
		starty = y;
	}
	else if (align&EGL_ALIGN_BOTTOM)
	{
		// BOTTOM
		starty = (y + h) - (Draw_LineCnt*charheight) - opt->line_spacing;
	}
	else
	{
		// CENTER
		starty = y + (h / 2) - (((Draw_LineCnt*charheight) - opt->line_spacing) / 2);
	}
	for (i = 0; i < Draw_LineCnt; i++)
	{
		str_info[i].y = starty + (charheight*i);
	}

	if (align&EGL_ALIGN_RIGHT)
	{
		for (i = 0; i < Draw_LineCnt; i++)
		{
			Draw_xPixel = text_width(font, str_info[i].str);
			if (Draw_xPixel < w)
				str_info[i].x = (x + w) - Draw_xPixel;
			else
				str_info[i].x = x;
		}
	}
	else if (align&EGL_ALIGN_CENTER)
	{
		for (i = 0; i < Draw_LineCnt; i++)
		{
			Draw_xPixel = text_width(font, str_info[i].str);
			if (Draw_xPixel < w)
				str_info[i].x = (x + (w / 2)) - (Draw_xPixel / 2);
			else
				str_info[i].x = x;
		}
	}
	else //LEFT
	{
		for (i = 0; i < Draw_LineCnt; i++)
		{
			str_info[i].x = x;
		}
	}

	if ((Draw_LineCnt == 1) && (text_width(font, str_info[0].str) > w))
	{
		temp_u8 = str_info[0].str[str_info[0].len - 1];
		if (temp_u8 < 0x80)
		{
			eraselen = 1;
			temp_u8 = str_info[0].str[str_info[0].len - 2];
			if (temp_u8 < 0x80)
			{
				eraselen += 1;
				temp_u8 = str_info[0].str[str_info[0].len - 2];
				if (temp_u8 >= 0x80)
				{
					eraselen += 2;
				}
			}
			else
			{
				eraselen += 2;
				temp_u8 = str_info[0].str[str_info[0].len - 2];
				if (temp_u8 < 0x80)
				{
					eraselen += 1;
				}
			}
		}
		else
		{
			eraselen = 2;
			temp_u8 = str_info[0].str[str_info[0].len - 2];
			if (temp_u8 < 0x80)
			{
				eraselen += 1;
			}
			else
			{
				eraselen += 2;
			}
		}
		draw_text_len(font, str_info[0].x, str_info[0].y, str_info[0].str, str_info[0].len - eraselen);
		Draw_xPixel = text_width(font, str_info[0].str);
		temp_s32 = text_width(font, &str_info[0].str[str_info[0].len - eraselen]);
		draw_text_len(font, str_info[0].x + (Draw_xPixel - temp_s32), str_info[0].y, "..", 2);
	}
	else
	{
		for (i = 0; i < Draw_LineCnt; i++)
		{
			draw_text_len(font, str_info[i].x, str_info[i].y, str_info[i].str, str_info[i].len);
		}
	}
	if (str)
		free(str);
	return str_info[Draw_LineCnt - 1].y;
}

void draw_text_in_box(EGL_FONT* font, EGL_RECT* pRect, const char* text, int align)
{
	DRAW_TEXT_IN_BOX_OPT opt;
	opt.align = align;
	opt.line_spacing = 0;
	opt.wordsplit = false;
	r_draw_text_in_box(font, pRect, text, &opt);
}

void draw_text_in_box_ex(EGL_FONT* font, EGL_RECT* pRect, const char* text, DRAW_TEXT_IN_BOX_OPT* opt)
{
	r_draw_text_in_box(font, pRect, text, opt);
}
