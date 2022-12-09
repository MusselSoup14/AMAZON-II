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
#pragma once
#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
extern "C" {
#endif

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#define strdup	_strdup
#endif


#include "typedef.h"
#include "egl_config.h"

typedef struct _tag_RECT
{
    int x;
    int y;
    int w;
    int h;
} EGL_RECT;


typedef unsigned int EGL_COLOR;

#define MAKE_COLORREF(r,g,b)	(0xff<<24 | (EGL_COLOR)((((r << 8) | g) << 8) | b))//ARGB
#define MAKE_RGB888(r,g,b)	((EGL_COLOR)((((r << 8) | g) << 8) | b))//RGB
#define MAKE_RGB565(r,g,b)	((U16)((((U16)r&0xf8)<<8)|(((U16)g&0xfc)<<3)|(((U16)b&0xf8)>>3)))  ///< make RGB565 from r,g,b
#define MAKE_COLORARGB(a,r,g,b)	((EGL_COLOR)((((((a<<8) | r) << 8) | g) << 8) | b))//ARGB
#define EXTRACT_ARGB(c,a,r,g,b)	do{a=(BYTE)(c>>24);r=(BYTE)(c>>16);g=(BYTE)(c>>8);b=(BYTE)(c>>0);}while(0);
#define EXTRACT_RGB(c,r,g,b)	do{r=(BYTE)(c>>16);g=(BYTE)(c>>8);b=(BYTE)(c);}while(0);
#define EXTRACT_RGB565(c,r,g,b)	do{r=(BYTE)(c>>11);g=(BYTE)((c>>5)&0x3f);b=(BYTE)(c & 0x1f);}while(0);
#define MAKE_RGB565FROM888(c)		((U16)((((U16)(c>>16)&0xf8)<<8)|(((U16)(c>>8)&0xfc)<<3)|(((U16)c&0xf8)>>3)))
#define GetRedValue(C)	((C>>16)&0xff)
#define GetGreenValue(C)	((C>>8)&0xff)
#define GetBlueValue(C)	(C&0xff)

/*!
\brief Blend the RGB values of two pixels based on a source alpha value.
*/
#define ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB)	\
    do {						\
        dR = (((sR-dR)*(A))>>8)+dR;		\
        dG = (((sG-dG)*(A))>>8)+dG;		\
        dB = (((sB-dB)*(A))>>8)+dB;		\
    } while(0)



typedef struct _tagPOINT
{
    int  x;
    int y;
} EGL_POINT;


typedef struct _tagARGB
{
    U8 b;
    U8 g;
    U8 r;
    U8 a;
} sARGB;
typedef struct _tagABGR
{
    U8 r;
    U8 g;
    U8 b;
    U8 a;
} sABGR;

typedef struct _tagrRGB
{
    U8 b;
    U8 g;
    U8 r;
    U8 resv; //reserved
} srRGB;

typedef struct _tagRGB565
{
    int b: 5;
    int g: 6;
    int r: 5;
} RGB565;

typedef union
{
    U32 data32;
    sARGB argb;
} ARGB;

typedef union
{
    U32 data32;
    sABGR abgr;
} ABGR;

typedef union
{
    U32 data32;
    srRGB rrgb;
} rRGB;

typedef enum
{
    PIX_FMT_RGB565 = 0,
    PIX_FMT_A1RGB555,
    PIX_FMT_PALETTE,
    PIX_FMT_RGB888,
    PIX_FMT_ARGB,//8888
    PIX_FMT_ABGR,//8888
    PIX_FMT_A1RGB,//1888
    PIX_FMT_A4RGB,//4888
    PIX_FMT_A000,//special 8bit mode, only alpha channel, palette 0  is color
    PIX_FMT_A1PALETTE,//A1 + 8 or4 BPP
} PIXFMT;

#ifdef HW_2D_ENGINE
typedef struct _tag_tmemmanager
{
    int offset;
    int size;

    struct _tag_tmemmanager	*prev;
    struct _tag_tmemmanager	*next;
} TMemManager;

#endif //HW_2D_ENGINE

typedef struct
{
    int bpp; //
    int nColors;
    int colorkey;
    EGL_COLOR* colors;

#ifdef HW_2D_ENGINE
    TMemManager tm;
    U8 LUT4Bank;///< only for 4bpp Texel mode
#endif //HW_2D_ENGINE
} PALETTE;

typedef struct _tagcliprect
{
    int x;
    int y;
    int endx;
    int endy;
} CLIP_RECT;
typedef struct
{
    int	w; ///< width of surface
    int	h; ///< height of surface
    int bpp; ///< Bits Per Pixel
    void*	pixels; ///< pointer to th actual pixel data
    int pitch; ///< Length of a surface scanline in bytes
    PIXFMT pixtype;
    PALETTE* pal;
    CLIP_RECT clip;
    void* reserv; ///<for extend
#ifdef HW_2D_ENGINE
    TMemManager tm;
    int drawmode;///<for H/W Graphic engine
    int flipmode; ///<Vertical or horizontal
    sARGB ShadeColor;
    sARGB transColor;
    sARGB alphaConstColor;///<for alphablend
    int sblendmode;
    int dblendmode;
    int TileXSize;///< real width of surface = 1<<(TileXSize+3)
    int TileYSize;///< real height of surface = 1<<(TileYSize+3)
#endif //HW_2D_ENGINE
} SURFACE;

#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
