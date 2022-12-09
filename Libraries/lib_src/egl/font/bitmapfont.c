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

#include "egl.h"
#include <stdlib.h>
#include "font/unicode_codec.h"
#include "font/bitmapfont.h"
#include <string.h>

#define EGL_FONT_TO_BMFONT(A)	((BMFont*)(A->ext))

typedef struct
{
	int id;
	short srcX;
	short srcY;
	short srcW;
	short srcH;
	short xOff;
	short yOff;
	short xAdv;
	short page;
	unsigned int chnl;
} SCharDescr;



typedef struct  _tagFont
{
	short fontHeight; // total height of the font
	short base;       // y of base line
	short scaleW;
	short scaleH;
	SCharDescr defChar;
	int Outlinesize;
	int pagecnt;

	int alphaChnl;
	int redChnl;
	int greenChnl;
	int blueChnl;

	EFontTextEncoding encoding;

	EGL_COLOR font_clr;
	srRGB outline_clr;//not used yet

	int charcount;
	SCharDescr* chars;
	SURFACE** surfs;

	int kerning;
	short fontsize;
	int bkmode;
	EGL_COLOR bkcolor;
	BOOL	bAutoKerning;
} BMFont;
static BMFont* bmfont_init( const char *fontFile );
static int bmfont_close( BMFont* pFont );
static int bmfont_gettextwidth( const char *text, int count ); //return text width in pixels

static int bmfont_write( int x, int y, const char *text ); //return text width in pixels

static BMFont *cur_pFont;
static	const char *cur_fontdir;

static int FontLoaderTextFormat_Load();
static int FontLoaderTextFormat_SkipWhiteSpace ( char *str, int start );
static int FontLoaderTextFormat_FindEndOfToken ( char *str, int start );
static void FontLoaderTextFormat_InterpretInfo ( char *str, int start );
static void FontLoaderTextFormat_InterpretCommon ( char *str, int start );
static void FontLoaderTextFormat_InterpretChar ( char *str, int start, int charindex );
//static void FontLoaderTextFormat_InterpretSpacing(char* str, int start);
static void FontLoaderTextFormat_InterpretKerning ( char *str, int start );
static bool FontLoaderTextFormat_InterpretPage ( char *str, int start);
static void FontLoaderTextFormat_InterpretChars ( char *str, int start );


int FontLoaderBinaryFormat_Load ( BYTE *buffer, int bufsize );
void FontLoaderBinaryFormat_ReadInfoBlock ( BYTE *buffer );
void FontLoaderBinaryFormat_ReadCommonBlock ( BYTE *buffer );
bool FontLoaderBinaryFormat_ReadPagesBlock ( BYTE *buffer, int size );
void FontLoaderBinaryFormat_ReadCharsBlock ( BYTE *buffer, int size );

static bool FontLoader_LoadPage ( int id, const char *pageFile );
static void FontLoader_SetFontInfo_outlineThickness ( int outlineThickness );
static void FontLoader_SetCommonInfo ( int fontHeight, int base, int scaleW, int scaleH, int pages, bool isPacked, int alphaChnl, int redChnl, int greenChnl, int blueChnl );
static void FontLoader_AddChar ( int charindex, int id, int x, int y, int w, int h, int xoffset, int yoffset, int xadvance, int page, int chnl );
static void FontLoader_AddChars ( int amount );


//FontLoader

static bool FontLoader_LoadPage ( int id, const char *pageFile )
{
	SURFACE *psurf;

	if ( id > cur_pFont->pagecnt )
		return false;
	if (cur_fontdir[0] == 0)
	{
		int len = strlen(pageFile);
		if (stricmp("tga", &pageFile[len - 3]) == NULL)
			psurf = loadtga((char *)pageFile);
		else if (stricmp("suf", &pageFile[len - 3]) == NULL)
			psurf = loadsurf(pageFile);
		else if (stricmp("png", &pageFile[len - 3]) == NULL)
			psurf = loadpng(pageFile);
		else
			return false;
	}
	else
	{
		char fullname[1024];
		strcpy(fullname,cur_fontdir);
		strcat(fullname,pageFile);
		int len = strlen(pageFile);
		if (stricmp("tga", &pageFile[len - 3]) == NULL)
			psurf = loadtga(fullname);
		else if (stricmp("suf", &pageFile[len - 3]) == NULL)
			psurf = loadsurf(fullname);
		else if (stricmp("png", &pageFile[len - 3]) == NULL)
			psurf = loadpng(fullname);
		else
			return false;
	}

	if ( psurf == 0 )
	{
		return false;
	}

	cur_pFont->surfs[id] = psurf;

	if ( psurf->bpp == 8 )
	{
		EGL_COLOR rgb=MAKE_COLORREF(0,0,0);

		//black & white(text)
		if ( ( cur_pFont->alphaChnl == 0 ) && ( cur_pFont->redChnl == 4 ) && ( cur_pFont->greenChnl == 4 ) && ( cur_pFont->blueChnl == 4 ) )
		{
			rgb = MAKE_COLORREF ( 0xff, 0xff, 0xff );
		}
		//white & black(text)
		else if ( ( cur_pFont->alphaChnl == 0 ) && ( cur_pFont->redChnl == 3 ) && ( cur_pFont->greenChnl == 3 ) && ( cur_pFont->blueChnl == 3 ) )
		{
			rgb = MAKE_COLORREF ( 0, 0, 0 );
		}
		psurf->pixtype = PIX_FMT_A000;//only alpha
		psurf->pal->colors[0] = rgb; //white
	}

	return true;
}

static void FontLoader_SetFontInfo_outlineThickness ( int outlineThickness )
{
	cur_pFont->Outlinesize = outlineThickness;
	cur_pFont->kerning += outlineThickness;
}
static void FontLoader_SetCommonInfo ( int fontHeight, int base, int scaleW, int scaleH, int pages, bool isPacked, int alphaChnl, int redChnl, int greenChnl, int blueChnl )
{
	cur_pFont->fontHeight = fontHeight;
	cur_pFont->base = base;
	cur_pFont->scaleW = scaleW;
	cur_pFont->scaleH = scaleH;
	cur_pFont->pagecnt = pages;
	cur_pFont->surfs = ( SURFACE ** ) malloc ( sizeof ( SURFACE * ) *pages );
	cur_pFont->alphaChnl = alphaChnl;
	cur_pFont->blueChnl = blueChnl;
	cur_pFont->greenChnl = greenChnl;
	cur_pFont->redChnl = redChnl;
}
static void FontLoader_AddChar ( int index, int id, int x, int y, int w, int h, int xoffset, int yoffset, int xadvance, int page, int chnl )
{
	SCharDescr *chars ;

	//debugprintf("%s(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)\r\n",__FUNCTION__,id,x,y,w,h,xoffset,yoffset,xadvance,page,chnl);
	if ( index > cur_pFont->charcount )
		return;

	chars = &cur_pFont->chars[index];
	chars->id = id;
	chars->srcX = x;
	chars->srcY = y;
	chars->srcW = w;
	chars->srcH = h;
	chars->xOff = xoffset;
	chars->yOff = yoffset;
	chars->xAdv = xadvance;
	chars->chnl = chnl;
	chars->page = page;
}

static void FontLoader_AddChars ( int amount )
{
	cur_pFont->charcount = amount;
	cur_pFont->chars = ( SCharDescr * ) malloc ( sizeof ( SCharDescr ) * amount );
	/*
	int i;
	for(i=0;i<amount;i++)
	{
		cur_pFont->chars[i]=(SCharDescr*)malloc(sizeof(SCharDescr));
	}
	*/
	//debugprintf ( "total character count : %d \r\n", amount );
}

//End FontLoader

//Textformat loader;
static int FontLoaderTextFormat_SkipWhiteSpace ( char *str, int start )
{
	size_t n = start;

	while ( n < strlen ( str ) )
	{
		char ch = str[n];

		if ( ch != ' ' &&
			 ch != '\t' &&
			 ch != '\r' &&
			 ch != '\n' )
			break;

		++n;
	}

	return n;
}

static int FontLoaderTextFormat_FindEndOfToken ( char *str, int start )
{
	size_t n = start;

	if ( str[n] == '"' )
	{
		n++;

		while ( n < strlen ( str ) )
		{
			char ch = str[n];

			if ( ch == '"' )
			{
				// Include the last quote char in the token
				++n;
				break;
			}

			++n;
		}
	}
	else
	{
		while ( n < strlen ( str ) )
		{
			char ch = str[n];

			if ( ch == ' ' ||
				 ch == '\t' ||
				 ch == '\r' ||
				 ch == '\n' ||
				 ch == '=' )
				break;

			++n;
		}
	}

	return n;
}


static void FontLoaderTextFormat_InterpretInfo ( char *str, int start )
{
	int outlineThickness = 0;
	// Read all attributes
	int pos, pos2 = start;
	int len = strlen ( str );

	while ( true )
	{
		char token[32];
		char value[32];
		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos2 );

		if ( pos >= len ) break;

		pos2 = FontLoaderTextFormat_FindEndOfToken ( str, pos );
		strncpy ( token, &str[pos], pos2 - pos );
		token[pos2 - pos] = 0;
		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos2 );

		if ( pos >= len || str[pos] != '=' ) break;

		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos + 1 );
		pos2 = FontLoaderTextFormat_FindEndOfToken ( str, pos );
		strncpy ( value, &str[pos], pos2 - pos );
		value[pos2 - pos] = 0;

		if ( strcmp ( token, "outline" ) == 0 )
			outlineThickness = ( short ) strtol ( value, 0, 10 );
		else if ( strcmp ( token, "size" ) == 0 )
		{
			cur_pFont->fontsize = ( short ) strtol ( value, 0, 10 );
		}

		if ( pos >= len ) break;
	}

	FontLoader_SetFontInfo_outlineThickness ( outlineThickness );
}

static void FontLoaderTextFormat_InterpretChar ( char *str, int start, int charindex )
{
	// Read all attributes
	int id = 0;
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	int xoffset = 0;
	int yoffset = 0;
	int xadvance = 0;
	int page = 0;
	int chnl = 0;
	int pos, pos2 = start;
	int len = strlen ( str );

	while ( true )
	{
		char token[32];
		char value[512];
		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos2 );

		if ( pos >= len ) break;

		pos2 = FontLoaderTextFormat_FindEndOfToken ( str, pos );
		strncpy ( token, &str[pos], pos2 - pos );
		token[pos2 - pos] = 0;
		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos2 );

		if ( pos >= len || str[pos] != '=' ) break;

		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos + 1 );

		if ( pos >= len ) break;

		pos2 = FontLoaderTextFormat_FindEndOfToken ( str, pos );

		if ( pos2 >= len ) break;

		strncpy ( value, &str[pos], pos2 - pos );
		value[pos2 - pos] = 0;

		if ( strcmp ( token, "id" ) == 0 )
			id = strtol ( value, 0, 10 );
		else if ( strcmp ( token, "x" ) == 0 )
			x = strtol ( value, 0, 10 );
		else if ( strcmp ( token, "y" ) == 0 )
			y = strtol ( value, 0, 10 );
		else if ( strcmp ( token, "width" ) == 0 )
			width = strtol ( value, 0, 10 );
		else if ( strcmp ( token, "height" ) == 0 )
			height = strtol ( value, 0, 10 );
		else if ( strcmp ( token, "xoffset" ) == 0 )
			xoffset = strtol ( value, 0, 10 );
		else if ( strcmp ( token, "yoffset" ) == 0 )
			yoffset = strtol ( value, 0, 10 );
		else if ( strcmp ( token, "xadvance" ) == 0 )
			xadvance = strtol ( value, 0, 10 );
		else if ( strcmp ( token, "page" ) == 0 )
			page = strtol ( value, 0, 10 );
		else if ( strcmp ( token, "chnl" ) == 0 )
			chnl = strtol ( value, 0, 10 );

		if ( pos >= len ) break;
	}

	// Store the attributes
	FontLoader_AddChar ( charindex, id, x, y, width, height, xoffset, yoffset, xadvance, page, chnl );
}


static void FontLoaderTextFormat_InterpretKerning ( char *str, int start )
{
	// Read the attributes
	int first = 0;
	int second = 0;
	int amount = 0;
	int pos, pos2 = start;
	int len = strlen ( str );

	while ( true )
	{
		char value[512];
		char token[32];
		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos2 );

		if ( pos >= len ) break;

		pos2 = FontLoaderTextFormat_FindEndOfToken ( str, pos );
		strncpy ( token, &str[pos], pos2 - pos );
		token[pos2 - pos] = 0;
		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos2 );

		if ( pos >= len || str[pos] != '=' ) break;

		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos + 1 );

		if ( pos >= len ) break;

		pos2 = FontLoaderTextFormat_FindEndOfToken ( str, pos );

		if ( pos2 >= len ) break;

		strncpy ( value, &str[pos], pos2 - pos );
		value[pos2 - pos] = 0;

		if ( strcmp ( token, "first" ) == 0 )
			first = strtol ( value, 0, 10 );
		else if ( strcmp ( token , "second" ) == 0 )
			second = strtol ( value, 0, 10 );
		else if ( strcmp ( token , "amount" ) == 0 )
			amount = strtol ( value, 0, 10 );

		if ( pos >= len ) break;
	}

	// Store the attributes
	//FontLoader_AddKerningPair ( first, second, amount );
}


static void FontLoaderTextFormat_InterpretCommon ( char *str, int start )
{
	int fontHeight = 0;
	int base = 0;
	int scaleW = 0;
	int scaleH = 0;
	int pages = 0;
	int packed = 0;
	int alphaChnl = 0;
	int redChnl = 0;
	int greenChnl = 0;
	int blueChnl = 0;
	// Read all attributes
	int pos, pos2 = start;
	int len = strlen ( str );

	while ( true )
	{
		char token[32];
		char value[256];
		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos2 );

		if ( pos >= len )
			break;

		pos2 = FontLoaderTextFormat_FindEndOfToken ( str, pos );

		if ( pos >= len )
			break;

		strncpy ( token, str + pos, pos2 - pos );
		token[pos2 - pos] = 0;
		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos2 );

		if ( pos == len || str[pos] != '=' ) break;

		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos + 1 );

		if ( pos >= len )
			break;

		pos2 = FontLoaderTextFormat_FindEndOfToken ( str, pos );

		if ( pos >= len )
			break;

		strncpy ( value, str + pos, pos2 - pos );
		value[pos2 - pos] = 0;

		if ( strcmp ( token, "lineHeight" ) == 0 )
			fontHeight = ( short ) strtol ( value, 0, 10 );
		else if ( strcmp ( token, "base" ) == 0 )
			base = ( short ) strtol ( value, 0, 10 );
		else if ( strcmp ( token, "scaleW" ) == 0 )
			scaleW = ( short ) strtol ( value, 0, 10 );
		else if ( strcmp ( token, "scaleH" ) == 0 )
			scaleH = ( short ) strtol ( value, 0, 10 );
		else if ( strcmp ( token, "pages" ) == 0 )
			pages = strtol ( value, 0, 10 );
		else if ( strcmp ( token, "packed" ) == 0 )
			packed = strtol ( value, 0, 10 );
		else if ( strcmp ( token, "alphaChnl" ) == 0 )
			alphaChnl = strtol ( value, 0, 10 );
		else if ( strcmp ( token, "redChnl" ) == 0 )
			redChnl = strtol ( value, 0, 10 );
		else if ( strcmp ( token, "greenChnl" ) == 0 )
			greenChnl = strtol ( value, 0, 10 );
		else if ( strcmp ( token, "blueChnl" ) == 0 )
			blueChnl = strtol ( value, 0, 10 );

		if ( pos == len ) break;
	}

	FontLoader_SetCommonInfo ( fontHeight, base, scaleW, scaleH, pages, packed ? true : false, alphaChnl, redChnl, greenChnl, blueChnl );
}
static bool FontLoaderTextFormat_InterpretPage ( char *str, int start)
{
	int id = 0;
	char file[256];
	// Read all attributes
	int pos, pos2 = start;
	int len = strlen ( str );

	while ( true )
	{
		char token[32];
		char value[64];
		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos2 );
		pos2 = FontLoaderTextFormat_FindEndOfToken ( str, pos );
		strncpy ( token, str + pos, pos2 - pos );
		token[pos2 - pos] = 0;
		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos2 );

		if ( pos == len || str[pos] != '=' ) break;

		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos + 1 );

		if ( pos >= len )
			break;

		pos2 = FontLoaderTextFormat_FindEndOfToken ( str, pos );
		strncpy ( value, str + pos, pos2 - pos );
		value[pos2 - pos] = 0;

		if ( strcmp ( token, "id" ) == 0 )
			id = strtol ( value, 0, 10 );
		else if ( strcmp ( token , "file" ) == 0 )
		{
			strncpy ( file, value + 1, strlen ( value ) - 2 );
			file[strlen ( value ) - 2] = 0;
		}

		if ( pos == len ) break;
	}

	return FontLoader_LoadPage ( id, file );
}
static void FontLoaderTextFormat_InterpretChars ( char *str, int start )
{
	// Read the attributes
	int amount = 0;
	int pos, pos2 = start;
	int len = strlen ( str );

	while ( true )
	{
		char token[32];
		char value[64];
		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos2 );

		if ( pos >= len ) break;

		pos2 = FontLoaderTextFormat_FindEndOfToken ( str, pos );
		strncpy ( token, &str[pos], pos2 - pos );
		token[pos2 - pos] = 0;
		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos2 );

		if ( pos >= len || str[pos] != '=' ) break;

		pos = FontLoaderTextFormat_SkipWhiteSpace ( str, pos + 1 );

		if ( pos >= len ) break;

		pos2 = FontLoaderTextFormat_FindEndOfToken ( str, pos );

		if ( pos2 >= len ) break;

		strncpy ( value, &str[pos], pos2 - pos );
		value[pos2 - pos] = 0;

		if ( strcmp ( token, "count" ) == 0 )
			amount = strtol ( value, 0, 10 );

		if ( pos >= len ) break;
	}

	// Store the attributes
	FontLoader_AddChars ( amount );
}

static int FontLoaderTextFormat_Load ( char *strbuf, int strlen )
{
	char line[512];
	int k = 0;
	int charindex = 0;

	while ( k < strlen )
	{
		// Read until line feed (or EOF)
		int pos , pos2;
		int i = 0;
		int len;
		char token[32];
		line[0] = 0;

		while ( k < strlen )
		{
			char ch = strbuf[k];
			k++;

			if ( ch != '\n' )
			{
				line[i] = ch;
				i++;
			}
			else
			{
				line[i] = 0;
				break;
			}
		}

		len = i;
		// Skip white spaces
		pos = FontLoaderTextFormat_SkipWhiteSpace ( line, 0 );

		if ( pos > len )
			break;

		// Read token
		pos2 = FontLoaderTextFormat_FindEndOfToken ( line, pos );

		if ( pos > len )
			break;

		strncpy ( token, line + pos, pos2 - pos );
		token[pos2 - pos] = 0;

		// Interpret line
		if ( strcmp ( token, "info" ) == 0 )
			FontLoaderTextFormat_InterpretInfo ( line, pos2 );
		else if ( strcmp ( token, "common" ) == 0 )
			FontLoaderTextFormat_InterpretCommon ( line, pos2 );
		else if ( strcmp ( token, "char" ) == 0 )
		{
			FontLoaderTextFormat_InterpretChar ( line, pos2, charindex );
			charindex++;
		}
		else if ( strcmp ( token, "kerning" ) == 0 )
			FontLoaderTextFormat_InterpretKerning ( line, pos2 );
		else if ( strcmp ( token, "page" ) == 0 )
		{
			if ( FontLoaderTextFormat_InterpretPage ( line, pos2 ) == false )
				return false;
		}
		else if ( strcmp ( token, "chars" ) == 0 )
			FontLoaderTextFormat_InterpretChars ( line, pos2 );
	}

	// Success
	return 0;
}
//End Textformat loader


//start Binaryformat loader
int FontLoaderBinaryFormat_Load ( BYTE *buf, int bufsize )
{
	// Read each block
	char blockType;
	int blockSize;
	int cur = 0;

	while ( cur < bufsize )
	{
		blockType = buf[cur];
		cur++;

		if ( cur >= bufsize )
			break;

		blockSize = ( buf[cur + 3] << 24 ) | ( buf[cur + 2] << 16 ) | ( buf[cur + 1] << 8 ) | buf[cur + 0];
		cur += 4;

		if ( cur >= bufsize )
			break;

		switch ( blockType )
		{
		case 1: // info
			FontLoaderBinaryFormat_ReadInfoBlock ( buf + cur );
			break;

		case 2: // common
			FontLoaderBinaryFormat_ReadCommonBlock ( buf + cur );
			break;

		case 3: // pages
			if ( FontLoaderBinaryFormat_ReadPagesBlock ( buf + cur, blockSize ) == false )
				return -1;

			break;

		case 4: // chars
			FontLoaderBinaryFormat_ReadCharsBlock ( buf + cur, blockSize );
			break;

		case 5: // kerning pairs
			//not support
			break;

		default:
			printf ( "Unexpected block type (%d)", blockType );
			return -1;
		}

		cur += blockSize;
	}

	// Success
	return 0;
}
void FontLoaderBinaryFormat_ReadInfoBlock ( BYTE *buffer )
{
	struct infoBlock
	{
		WORD fontSize;
		BYTE reserved_bold_italic_unicode_smooth;
		BYTE charSet;
		WORD stretchH;
		BYTE aa;
		BYTE paddingUp;
		BYTE paddingRight;
		BYTE paddingDown;
		BYTE paddingLeft;
		BYTE spacingHoriz;
		BYTE spacingVert;
		BYTE outline;         // Added with version 2
		char fontName[1];
	};
	struct infoBlock blk;
	memcpy ( &blk, buffer, sizeof ( struct infoBlock ) );
	cur_pFont->fontsize = blk.fontSize;
	FontLoader_SetFontInfo_outlineThickness ( blk.outline );
}
void FontLoaderBinaryFormat_ReadCommonBlock ( BYTE *buffer )
{
	struct commonBlock
	{
		BYTE lineHeight[2];
		BYTE base[2];
		BYTE scaleW[2];
		BYTE scaleH[2];
		BYTE pages[2];
		BYTE packed  ;
		BYTE alphaChnl;
		BYTE redChnl;
		BYTE greenChnl;
		BYTE blueChnl;
	};
	struct commonBlock *blk = ( struct commonBlock * ) buffer;
	WORD lineHeight;
	WORD base;
	WORD scaleW;
	WORD scaleH;
	WORD pages;
	lineHeight = blk->lineHeight[0] + ( blk->lineHeight[1] << 8 );
	base = blk->base[0] + ( blk->base[1] << 8 );
	scaleH = blk->scaleH[0] + ( blk->scaleH[1] << 8 );
	scaleW = blk->scaleW[0] + ( blk->scaleW[1] << 8 );
	pages = blk->pages[0] + ( blk->pages[1] << 8 );
	FontLoader_SetCommonInfo ( lineHeight, base, scaleW, scaleH, pages, blk->packed & 1 ? true : false, blk->alphaChnl, blk->redChnl, blk->greenChnl, blk->blueChnl );
}
bool FontLoaderBinaryFormat_ReadPagesBlock ( BYTE *buffer, int size )
{
	struct pagesBlock
	{
		char pageNames[1];
	};
	struct pagesBlock *blk = ( struct pagesBlock * ) buffer;
	int id;
	int pos;

	for ( id = 0, pos = 0; pos < size; id++ )
	{
		if ( FontLoader_LoadPage ( id, &blk->pageNames[pos] ) == false )
			return false;

		pos += 1 + ( int ) strlen ( &blk->pageNames[pos] );
	}

	return true;
}
void FontLoaderBinaryFormat_ReadCharsBlock ( BYTE *buffer, int size )
{
	struct charsBlock
	{
		BYTE id[4];
		BYTE x[2];
		BYTE  y[2];
		BYTE width[2];
		BYTE height[2];
		BYTE xoffset[2];
		BYTE yoffset[2];
		BYTE xadvance[2];
		BYTE  page;
		BYTE  chnl;
	};
	struct charsBlock *blk;
	int n ;
	int charamount;
	charamount = size / sizeof ( struct charsBlock ) ;
	FontLoader_AddChars ( charamount );
	blk = ( struct charsBlock * ) buffer;

	for ( n = 0; n < charamount; n++ )
	{
		DWORD id;
		WORD  x;
		WORD  y;
		WORD  width;
		WORD  height;
		short xoffset;
		short yoffset;
		short xadvance;
		id = blk[n].id[0] + ( blk[n].id[1] << 8 ) + ( blk[n].id[2] << 16 ) + ( blk[n].id[3] << 24 );
		x = blk[n].x[0] + ( blk[n].x[1] << 8 );
		y = blk[n].y[0] + ( blk[n].y[1] << 8 );
		width = blk[n].width[0] + ( blk[n].width[1] << 8 );
		height = blk[n].height[0] + ( blk[n].height[1] << 8 );
		xoffset = blk[n].xoffset[0] + ( blk[n].xoffset[1] << 8 );
		yoffset = blk[n].yoffset[0] + ( blk[n].yoffset[1] << 8 );
		xadvance = blk[n].xadvance[0] + ( blk[n].xadvance[1] << 8 );
		FontLoader_AddChar ( n, id, x, y, width, height, xoffset, yoffset, xadvance, blk[n].page, blk[n].chnl );
	}
}
///end Binaryformat loader

static int Font_GetTextLength ( const char *text )
{
	EFontTextEncoding encoding = cur_pFont->encoding;
	int textLen = 0;

	if ( encoding == UTF16 )
	{
		for ( ;; )
		{
			unsigned int len;
			int r = DecodeUTF16 ( &text[textLen], &len );

			if ( r > 0 )
				textLen += len;
			else if ( r < 0 )
				textLen++;
			else
				return textLen;
		}
	}

	return strlen ( text );
}

static int Font_GetTextChar ( const char *text, int pos, int *nextPos )
{
	int ch;
	unsigned int len;
	EFontTextEncoding encoding = cur_pFont->encoding;

	if ( encoding == UTF8 )
	{
		ch = DecodeUTF8 ( &text[pos], &len );

		if ( ch == -1 ) len = 1;
	}
	else if ( encoding == UTF16 )
	{
		ch = DecodeUTF16 ( &text[pos], &len );

		if ( ch == -1 ) len = 2;
	}
	else
	{
		len = 1;
		ch = ( unsigned char ) text[pos];

		if ( ch >= 0x80 ) //oem
		{
			U16 oemcode;
			int ch2 = ( unsigned char ) text[pos + 1];
			len = 2;
			oemcode = ( ( U16 ) ch << 8 ) + ch2;
			ch = oem_to_unicode ( oemcode ); //oem to unicode
		}
	}

	if ( nextPos ) *nextPos = pos + len;

	return ch;
}

/*
static int Font_FindTextChar(const char *text, int start, int length, int ch)
{
	int pos = start;
	int nextPos;
	int currChar = -1;
	while( pos < length )
	{
		currChar = Font_GetTextChar(text, pos, &nextPos);
		if( currChar == ch )
			return pos;
		pos = nextPos;
	}

	return -1;
}
*/
static int SCharDescrCompare ( const void *s1, const void *s2 )
{
	SCharDescr *p1 = ( SCharDescr * ) s1;
	SCharDescr *p2 = ( SCharDescr * ) s2;

	if ( p1->id == p2->id )
		return 0;
	else if ( p1->id > p2->id )
		return 1;
	else
		return -1;
}
static SCharDescr *Font_GetChar ( int id )
{
	SCharDescr vchar;
	SCharDescr *pchar ;
	vchar.id = id;
	pchar = bsearch ( ( void * ) &vchar, cur_pFont->chars, cur_pFont->charcount, sizeof ( SCharDescr ), SCharDescrCompare );
	return pchar;
}


static int bmfont_gettextwidth ( const char *text, int count )
{
	int x = 0;
	int n;

	if ( cur_pFont == 0 )
		return 0;

	if ( count <= 0 )
		count = Font_GetTextLength ( text );

	for ( n = 0; n < count; )
	{
		int charId = Font_GetTextChar ( text, n, &n );
		SCharDescr *ch = Font_GetChar ( charId );
		int a;

		if ( ch == 0 )
			a = 0;
		else
		{
			if ( cur_pFont->bAutoKerning )
				a = ( ch->xAdv );
			else
				a = cur_pFont->fontsize;
		}

		x += ( a + cur_pFont->kerning );
		/*
		if( n < count )
			x += Font_AdjustForKerningPairs(charId, Font_GetTextChar(text,n,0));
			*/
	}

	return x;
}

static void drawchar ( int x, int y, SCharDescr *ch )
{
	SURFACE *psurf ;

	if ( ch->page >= cur_pFont->pagecnt )
		return;

	psurf = cur_pFont->surfs[ch->page];

	if ( cur_pFont->bAutoKerning == FALSE )
		x += ( cur_pFont->fontsize - ch->xAdv + ch->xOff ) / 2;

	DRAW_SURFACE_SCALE_RECT( psurf, x + ch->xOff, y + ch->yOff, ch->srcW, ch->srcH, ch->srcX, ch->srcY, ch->srcW, ch->srcH );
}
static void drawchar_vright ( int x, int y, SCharDescr *ch )
{
	SURFACE *psurf ;
	SURFACE *temp;
	int i, k;
	int bpp ;

	if ( ch->page >= cur_pFont->pagecnt )
		return;

	psurf = cur_pFont->surfs[ch->page];
	temp = CREATE_SURFACE( ch->srcH, ch->srcW, psurf->bpp );
	temp->pixtype = psurf->pixtype;

	if ( psurf->pal )
	{
		temp->pal->colorkey = psurf->pal->colorkey;
		temp->pal->nColors = psurf->pal->nColors;
		memcpy ( temp->pal->colors, psurf->pal->colors, psurf->pal->nColors * 4 );
	}

	bpp = psurf->bpp / 8; //byte per pixel

	if ( bpp == 4 )
	{
		U8 *src = ( U8 * ) ( ( U32 ) psurf->pixels + ch->srcX * bpp + ch->srcY * psurf->pitch );

		for ( i = 0; i < ch->srcW; i++ )
		{
			U32 *dest = ( U32 * ) ( ( U32 ) temp->pixels + ( temp->pitch * i ) );

			for ( k = 0; k < temp->w; k++ )
			{
				U32 data = * ( U32 * ) ( src + ( ( temp->w - 1 - k ) * psurf->pitch ) );
				*dest = data;
				dest++;
			}

			src += bpp;
		}
	}
	else if ( bpp == 1 )
	{
		U8 *src = ( U8 * ) ( ( U32 ) psurf->pixels + ch->srcX * bpp + ch->srcY * psurf->pitch );

		for ( i = 0; i < ch->srcW; i++ )
		{
			U8 *dest = ( U8 * ) ( ( U32 ) temp->pixels + ( temp->pitch * i ) );

			for ( k = 0; k < temp->w; k++ )
			{
				U8 data = * ( src + ( ( temp->w - 1 - k ) * psurf->pitch ) );
				*dest = data;
				dest++;
			}

			src ++;
		}
	}

	if ( cur_pFont->bAutoKerning == FALSE )
		y += ( cur_pFont->fontsize - ch->xAdv + ch->xOff ) / 2;

	DRAW_SURFACE_SCALE_RECT ( temp, x + cur_pFont->fontHeight - ch->yOff - ch->srcH , y + ch->xOff, temp->w, temp->h, 0, 0, temp->w, temp->h );
	RELEASE_SURFACE( temp );
}
static void drawchar_vleft ( int x, int y, SCharDescr *ch )
{
	SURFACE *psurf;
	SURFACE *temp;
	int i, k;
	int bpp ; //byte per pixel

	if ( ch->page >= cur_pFont->pagecnt )
		return;

	psurf = cur_pFont->surfs[ch->page];
	temp = CREATE_SURFACE ( ch->srcH, ch->srcW, psurf->bpp );
	temp->pixtype = psurf->pixtype;

	if ( psurf->pal )
	{
		temp->pal->colorkey = psurf->pal->colorkey;
		temp->pal->nColors = psurf->pal->nColors;
		memcpy ( temp->pal->colors, psurf->pal->colors, psurf->pal->nColors * 4 );
	}

	bpp = psurf->bpp / 8; //byte per pixel

	if ( bpp == 4 )
	{
		U8 *src = ( U8 * ) ( ( U32 ) psurf->pixels + ch->srcX * bpp + ch->srcY * psurf->pitch ) + ch->srcW - 1;

		for ( i = 0; i < ch->srcW; i++ )
		{
			U32 *dest = ( U32 * ) ( ( U32 ) temp->pixels + ( temp->pitch * i ) );

			for ( k = 0; k < temp->w; k++ )
			{
				U32 data = * ( U32 * ) ( src + ( k * psurf->pitch ) );
				*dest = data;
				dest++;
			}

			src -= bpp;
		}
	}
	else if ( bpp == 1 )
	{
		U8 *src = ( U8 * ) ( ( U32 ) psurf->pixels + ch->srcX * bpp + ch->srcY * psurf->pitch ) + ch->srcW - 1;

		for ( i = 0; i < ch->srcW; i++ )
		{
			U8 *dest = ( U8 * ) ( ( U32 ) temp->pixels + ( temp->pitch * i ) );

			for ( k = 0; k < temp->w; k++ )
			{
				U8 data = * ( src + ( k * psurf->pitch ) );
				*dest = data;
				dest++;
			}

			src --;
		}
	}

	if ( cur_pFont->bAutoKerning == FALSE )
		y -= ( cur_pFont->fontsize - ch->xAdv + ch->xOff ) / 2;

	DRAW_SURFACE_SCALE_RECT ( temp, x + ch->yOff, y - temp->h - ch->xOff, temp->w, temp->h, 0, 0, temp->w, temp->h );
	RELEASE_SURFACE( temp );
}
static int Font_InternalWrite ( int x, int y, const char *text, int count )
{
	int n;
	int len = 0;
	int a;

	for ( n = 0; n < count; )
	{
		int charId = Font_GetTextChar ( text, n, &n );
		SCharDescr *ch = Font_GetChar ( charId );

		if ( ch == 0 )
		{
			//debugprintf ( "Unknown Character %d\r\n", charId );
			a = 0;
		}
		else
		{
			if ( cur_pFont->bAutoKerning )
				a = ( ch->xAdv );
			else
				a = cur_pFont->fontsize;

			//rendering
			drawchar ( x, y, ch );
		}

		x += a + cur_pFont->kerning;
		len += a + cur_pFont->kerning;
	}

	return len;
}

static int Font_InternalWrite_vright ( int x, int y, const char *text, int count )
{
	int n;
	int len = 0;
	int a;
	for ( n = 0; n < count; )
	{
		int charId = Font_GetTextChar ( text, n, &n );
		SCharDescr *ch = Font_GetChar ( charId );
		if ( ch == 0 )
		{
			//debugprintf( "Unknown Character %d\r\n", charId );
			a = 0;
		}
		else
		{
			if ( cur_pFont->bAutoKerning )
				a = ( ch->xAdv );
			else
				a = cur_pFont->fontsize;
			drawchar_vright ( x, y, ch );
		}
		y += a + cur_pFont->kerning;
		len += a + cur_pFont->kerning;
	}
	return len;
}

static int Font_InternalWrite_vleft ( int x, int y, const char *text, int count )
{
	int n;
	int len = 0;
	int a;

	for ( n = 0; n < count; )
	{
		int charId = Font_GetTextChar ( text, n, &n );
		SCharDescr *ch = Font_GetChar ( charId );

		if ( ch == 0 )
		{
			//debugprintf ( "Unknown Character %d\r\n", charId );
			a = 0;
		}
		else
		{
			if ( cur_pFont->bAutoKerning )
				a = ( ch->xAdv );
			else
				a = cur_pFont->fontsize;

			//rendering
			drawchar_vleft ( x, y, ch );
		}

		y -= a + cur_pFont->kerning;
		len += a + cur_pFont->kerning;
	}

	return len;
}

static int bmfont_write ( int x, int y, const char *text )
{
	int count ;

	if ( cur_pFont == 0 )
		return 0;

	count = Font_GetTextLength ( text );
	return Font_InternalWrite ( x, y, text, count );
}

int bmpfont_draw_vleft ( EGL_FONT* pFont,int x, int y, const char *text )
{
	int count ;
	cur_pFont = EGL_FONT_TO_BMFONT(pFont);
	count = Font_GetTextLength ( text );
	return Font_InternalWrite_vleft ( x, y, text, count );
}

int bmpfont_draw_vright ( EGL_FONT* pFont,int x, int y, const char *text )
{
	int count ;
	cur_pFont = EGL_FONT_TO_BMFONT(pFont);
	count = Font_GetTextLength ( text );
	return Font_InternalWrite_vright ( x, y, text, count );
}

SURFACE *bmpfont_makesurface (EGL_FONT* pFont, char *text )
{
	int w ;
	SURFACE *fontsurf ;
	SURFACE *surf ;
	int h;
	int bpp;
	int n;
	int len = 0;
	int count;
	int x;
	int a;

	cur_pFont = EGL_FONT_TO_BMFONT(pFont);

	w = bmfont_gettextwidth ( text, 0 );

	if ( w == 0 )
		return 0;

	fontsurf = cur_pFont->surfs[0];
	h = cur_pFont->fontHeight;
	bpp = fontsurf->bpp;
	surf = CREATE_SURFACE ( w, h, bpp );

	if ( surf == 0 )
		return 0;

	memset ( surf->pixels, 0, surf->pitch * surf->h );
	surf->pixtype = fontsurf->pixtype;

	if ( surf->bpp == 8 )
		surf->pal->colors[0] = fontsurf->pal->colors[0];

	len = 0;
	count = Font_GetTextLength ( text );
	x = 0;

	for ( n = 0; n < count; )
	{
		SCharDescr *ch ;
		int charId = Font_GetTextChar ( text, n, &n );
		ch = Font_GetChar ( charId );

		if ( ch == 0 )
		{
			//debugprintf ( "Unknown Character %d\r\n", charId );
			a = 0;
		}
		else
		{
			if ( ch->page < cur_pFont->pagecnt )
			{
				int dx, dy, sx, sy, sh, y;
				U8 *dest;
				U8 *src;
				fontsurf = cur_pFont->surfs[ch->page];

				//copy
				if ( cur_pFont->bAutoKerning )
					dx = x + ch->xOff;
				else
					dx = x + ( ( cur_pFont->fontsize - ch->xAdv + ch->xOff ) / 2 ) + ch->xOff;

				dy = ch->yOff;
				sx = ch->srcX;
				sy = ch->srcY;
				sh = ch->srcH;
				dest = ( ( U8 * ) surf->pixels ) + ( dx * ( bpp / 8 ) ) + ( surf->pitch * dy );
				src = ( ( U8 * ) fontsurf->pixels ) + ( sx * ( fontsurf->bpp / 8 ) ) + ( fontsurf->pitch * sy );

				for ( y = 0; y < sh; y++, dy++ )
				{
					memcpy ( dest, src, ch->srcW );
					dest += surf->pitch;
					src += fontsurf->pitch;
				}
			}

			if ( cur_pFont->bAutoKerning )
				a = ( ch->xAdv );
			else
				a = cur_pFont->fontsize;
		}

		x += a + cur_pFont->kerning;
		len += a + cur_pFont->kerning;
	}

	return surf;
}

void bmpfont_settextencoding (EGL_FONT* pFont, EFontTextEncoding encoding )
{
	BMFont* bmfont = EGL_FONT_TO_BMFONT(pFont);
	bmfont->encoding = encoding;
}

static BMFont *bmfont_init ( const char *fontFile )
{
	// Load the font
	int re;
	U32 fsize;
	BMFont *pbmFont;
	U8 *strbuf;
	pbmFont = ( BMFont * ) malloc ( sizeof ( BMFont ) );

	if ( pbmFont == 0 )
		return 0;

	memset ( pbmFont, 0, sizeof ( BMFont ) );
	strbuf = readfile ( (char*)fontFile, &fsize );

	if ( strbuf == NULL )
	{
		free ( pbmFont );
		return NULL;
	}

	cur_pFont = pbmFont;

	if ( memcmp ( strbuf, "BMF\003", 4 ) == 0 )
	{
		re = FontLoaderBinaryFormat_Load ( ( BYTE * ) strbuf + 4, fsize );
	}
	else
		re = FontLoaderTextFormat_Load ( strbuf, fsize );

	free ( strbuf );

	if ( re == -1 )
	{
		bmfont_close ( pbmFont );
		return 0;
	}

	pbmFont->outline_clr.r = 0;
	pbmFont->outline_clr.g = 0;
	pbmFont->outline_clr.b = 0;
	pbmFont->font_clr = MAKE_COLORREF( 255, 255, 255 );
	return pbmFont;
}

static int bmfont_close ( BMFont *pFont )
{
	int i;

	if ( pFont == 0 )
		return 0;

	if ( pFont->chars )
	{
		free ( pFont->chars );
	}

	if ( pFont->surfs )
	{
		for ( i = 0; i < pFont->pagecnt; i++ )
		{
			SURFACE *psurf = pFont->surfs[i];
			RELEASE_SURFACE( psurf ); //malloc by loadtga
		}

		free ( pFont->surfs );
	}

	free ( pFont );
	return 1;
}

bool bmpfont_setkerning ( EGL_FONT *pFont, int k )
{
	BMFont* bmfont = EGL_FONT_TO_BMFONT(pFont);
	if ( bmfont )
	{
		bmfont->kerning = k;
		return true;
	}

	return false;
}
bool bmpfont_setautokerning (EGL_FONT* pFont,bool b )
{
	BMFont* bmfont = EGL_FONT_TO_BMFONT(pFont);
	bmfont->bAutoKerning = b;
	return true;
}

int bmpfont_draw (EGL_FONT* pFont, int x, int y, const char *str )
{
	cur_pFont = (BMFont*)pFont->ext;
	return bmfont_write ( x, y, str );
}
/*
static int bmfont_draw_pivot (EGL_FONT* pFont, int x, int y, const char *str ,int pivot)
{
	if(pivot == PIVOT_RIGHT)
		return bmpfont_draw_vright(pFont, x, y, str );
	else if(pivot == PIVOT_LEFT)
		return bmpfont_draw_vleft(pFont, x, y, str );
}
*/
static void bmpfont_draw_len( EGL_FONT* pFont, int x, int y, const char* text, int len )
{
	int count ;
	cur_pFont =EGL_FONT_TO_BMFONT(pFont);
	count = Font_GetTextLength ( text );
	if( count > len )
		count = len;
	Font_InternalWrite ( x, y, text, count );
}

static int bmpfont_text_width( EGL_FONT* pFont, const char* text )
{
	int n;
	int len = 0;
	int a;
	int count ;
	cur_pFont = EGL_FONT_TO_BMFONT(pFont);
	count = Font_GetTextLength ( text );

	for ( n = 0; n < count; )
	{
		int charId = Font_GetTextChar ( text, n, &n );
		SCharDescr *ch = Font_GetChar ( charId );

		if ( ch == 0 )
		{
			//debugprintf ( "Unknown Character %d\r\n", charId );
			a = 0;
		}
		else
		{
			if ( cur_pFont->bAutoKerning )
				a = ( ch->xAdv );
			else
				a = cur_pFont->fontsize;
		}

		len += a + cur_pFont->kerning;
	}

	return len;
}

static int bmpfont_text_count_in_width( EGL_FONT* pFont, const char* text, int w )
{
	int n;
	int len = 0;
	int a;
	int count ;
	cur_pFont = EGL_FONT_TO_BMFONT(pFont);
		
	count = Font_GetTextLength ( text );

	for ( n = 0; n < count; )
	{
		int charId = Font_GetTextChar ( text, n, &n );
		SCharDescr *ch = Font_GetChar ( charId );

		if ( ch == 0 )
		{
			//debugprintf ( "Unknown Character %d\r\n", charId );
			a = 0;
		}
		else
		{
			if ( cur_pFont->bAutoKerning )
				a = ( ch->xAdv );
			else
				a = cur_pFont->fontsize;
		}

		len += a + cur_pFont->kerning;
		if( len >= w )
			break;
	}
	return n;
}

static EGL_COLOR bmpfont_set_clr( EGL_FONT* eglfont,EGL_COLOR clr )
{
	int id;
	EGL_COLOR oldclr;
	BMFont* pFont = (BMFont*)eglfont->ext;
	if ( pFont->pagecnt == 0 )
		return 0;

	for ( id = 0; id < pFont->pagecnt; id++ )
	{
		SURFACE *psurf = pFont->surfs[id];
		psurf->pal->colors[0] = clr;
	}
	oldclr = pFont->font_clr;
	pFont->font_clr = clr;
	return oldclr;
}

static EGL_COLOR bmpfont_get_clr(EGL_FONT* pFont)
{
	BMFont* bmfont = EGL_FONT_TO_BMFONT(pFont);
	return bmfont->font_clr;
}

static EGL_COLOR bmpfont_set_bkcolor( EGL_FONT* pFont,EGL_COLOR clr )
{
	EGL_COLOR old;
	BMFont* bmfont = EGL_FONT_TO_BMFONT(pFont);
	old = bmfont->bkcolor;
	bmfont->bkcolor = clr;
	return old;
}

static EGL_COLOR bmpfont_get_bkcolor( EGL_FONT* pFont )
{
	BMFont* bmfont = EGL_FONT_TO_BMFONT(pFont);
	return bmfont->bkcolor;
}

static void bmpfont_set_bkmode( EGL_FONT* font, int mode )
{
	BMFont* bmfont  = ( BMFont* )font->ext;
	bmfont->bkmode = mode;
}
/*
static EGL_FONT* clone(EGL_FONT* font)
{
	EGL_FONT* newfont;
	BMFont *pbmFont;
	if(font==NULL)
		return NULL;
	newfont = (EGL_FONT*)malloc(sizeof(EGL_FONT));
	if(newfont=NULL)
		return NULL;
	pbmFont = ( BMFont * ) malloc ( sizeof ( BMFont ) );
	if(pbmFont==NULL)
	{
		free(newfont);
		return NULL;
	}
	memcmp(pbmFont,font->ext,sizeof(BMFont));
	memcmp(newfont,font,sizeof(EGL_FONT));
	newfont->ext = pbmFont;
	return newfont;
}
*/

EGL_FONT *create_bmpfont ( const char *fname )
{
	EGL_FONT* font ;
	char* str;
	char dir[256];
	BMFont *bmfont ;
	str = strrchr(fname,'/');
	if(str!=NULL)
	{
		strncpy(dir,fname,str-fname+1);
		dir[str-fname+1]=0;
	}
	else
	{
		dir[0]=0;
	}
	cur_fontdir = dir;
	bmfont= bmfont_init ( fname );
	if ( bmfont == NULL )
	{
		return NULL;
	}

	font = ( EGL_FONT * ) malloc ( sizeof ( EGL_FONT ) );

	if ( font == NULL )
		return NULL;

	font->draw = bmpfont_draw;
	font->draw_len = bmpfont_draw_len;
	font->text_count_in_width = bmpfont_text_count_in_width;
	font->text_width = bmpfont_text_width;
	font->set_color = bmpfont_set_clr;
	font->set_bkcolor = bmpfont_set_bkcolor;
	font->get_color = bmpfont_get_clr;
	font->get_bkcolor = bmpfont_get_bkcolor;
	font->h = bmfont->fontHeight;
	font->set_bkmode = bmpfont_set_bkmode;
	font->ext = bmfont;
	return font;
}
void release_bmpfont(EGL_FONT* pFont)
{
	bmfont_close( EGL_FONT_TO_BMFONT(pFont) );
	free( pFont );
	pFont = NULL;
}
