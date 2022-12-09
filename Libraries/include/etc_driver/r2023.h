#pragma once

#include "sdk.h"

// R2023.h

#define R2023_DEVICE_ADDR		0x64
#define R2023_SEC			0x0
#define R2023_MIN			0x1
#define R2023_HOUR			0x2
#define R2023_DAY			0x3
#define R2023_DATE			0x4
#define R2023_MONTH			0x5
#define R2023_YEAR			0x6
#define R2023_OSC_ADJ		0x7
#define R2023_ALARM_W_M		0x8
#define R2023_ALARM_W_H		0x9
#define R2023_ALARM_W_D		0xa
#define R2023_ALARM_D_M		0xb
#define R2023_ALARM_D_H		0xc
#define R2023_CON1			0xe
#define R2023_CON2			0xf

//#define R2023_SET_HEX

typedef struct
{
	U8 sec;
	U8 min;
	U8 hour;
	U8 day;
	U8 date;
	U8 month;
	U8 year;
}R2023_Time;

S32 r2023_write(U8 addr,U8 *buffer, U8 length);
S32 r2023_read(U8 addr,U8 *buffer, U8 length);

S32 r2023_settime(R2023_Time* pDT);
S32 r2023_gettime(R2023_Time* pDT);
