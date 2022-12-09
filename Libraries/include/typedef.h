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


#ifdef TRUE	
#undef TRUE 
#endif
#define TRUE 1

#ifdef FALSE 
#undef FALSE
#endif
#define FALSE 0

#ifdef true
#undef true 
#endif
#define true 1

#ifdef false 
#undef false
#endif
#define false 0

/* Define NULL pointer value */
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#if defined(BOOL)
#else 
typedef int BOOL;
#endif
#if defined(bool)
#else
#ifndef __GNUG__ 
typedef int bool;
#endif//__GNUG___
#endif

typedef signed char		S8;
typedef signed short	S16;
typedef signed int		S32;

typedef signed char		s8;
typedef signed short	s16;
typedef signed int		s32;

typedef unsigned char	U8;
typedef unsigned short	U16;
typedef unsigned int	U32;
typedef unsigned long long	U64;

typedef volatile unsigned char	vU8;
typedef volatile unsigned short	vU16;
typedef volatile unsigned int	vU32;
typedef volatile unsigned long long	vU64;

typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;
typedef unsigned long long	u64;

typedef unsigned char	__u8;
typedef unsigned short	__u16;
typedef unsigned int	__u32;
typedef unsigned long long	__u64;


typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;

typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned long	ulong;

typedef struct _tagRect
{
	int x;
	int y;
	int w;
	int h;
}RECT;


#define ALIGN4  __attribute__((__aligned__(4)))
#define __always_inline   inline __attribute__((__always_inline__)) 


