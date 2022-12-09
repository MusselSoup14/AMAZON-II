
/******************************************************************************
 Copyright (C) 2013      Advanced Digital Chips Inc.
						http://www.adc.co.kr
 Author : Software Team.
******************************************************************************/

#include "sdk.h"

#define TW9900_WR  0x88	// Device AD 
#define TW9900_RR  0x89

extern

U8 tw9900_data_read( U8 addr )
{
    U8 data = 0;
    twi_read( TW9900_WR, &addr, 1, &data, 1 );

    return data;
}

void tw9900_data_write( U8 addr, U8 data )
{
    twi_write2( TW9900_WR, addr, &data, 1 );
}

const unsigned char TW9900array[] =
{
    0x88, 12,
    0x03, 0x82,
    0x05, 0x01,
    0x08, 0x13,
    0x09, 0xf2,
    0x19, 0x57,
    0x1a, 0x0f,
    0x29, 0x03,	// vertical sync start(line unit)
    0x2d, 0x07,
    0x6b, 0x09,
    0x6c, 0x19,
    0x6d, 0x0a, //0x9f,
//	0x6e, 0x28, //0x6e, 0x41,
    0x06, 0x80,
    0x55, 0x10,

    0xff, 0xff
};

bool tw9900_Init( void )
{
	int j ;

	debugprintf( "\r\nSetting TW9900\r\n" );

	U8 status;
	U32 timeout = 0xff;
	
	while(1)
	{
		status = tw9900_data_read(0x1);
		if(!(status & (1<<7)))
			break;
		
		if(timeout == 0)
		{
			debugprintf("There is no video input.\r\n");
			return false;
		}
		
		timeout--;
	}
	
	for ( j = 0; j < 30  ; j += 2 )
	{
		tw9900_data_write( TW9900array[j], TW9900array[j + 1] );
	}    
	
	delayms(200);
	
	return true;
}
