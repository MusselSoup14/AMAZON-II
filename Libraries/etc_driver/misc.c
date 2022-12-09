
/******************************************************************************
 Copyright (C) 2013      Advanced Digital Chips Inc.
						http://www.adc.co.kr
 Author : Software Team.
******************************************************************************/


#include "sdk.h"
#include "ctype.h"


FRESULT print_files ( char* path,BOOL recursive )
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof( lfn );
#endif

    res = f_opendir( &dir, path );
    if ( res == FR_OK )
    {
        i = strlen( path );
        for ( ;; )
        {
            res = f_readdir( &dir, &fno );
            if ( res != FR_OK || fno.fname[0] == 0 ) break;
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if ( fno.fattrib & AM_DIR )
            {
                char dirname[_MAX_LFN + 1];
                if( fn[0] != '.' )
                {
                    sprintf( dirname, "%s/%s", path, fn );
					if (recursive)
						print_files(dirname,recursive);
                }
            }
            else
            {
                debugprintf( "%s/%s\t%dbyte\r\n", path, fn, fno.fsize );
            }
        }
    }
    else
    {
        debugprintf( "path(%s) not found\r\n", path );
    }
    return res;
}

#define TEST_TIMER 3
// use last timer
#define MY_PRESCALE 32768

void StartEstimate()
{
    U32 conval;
#if (MY_PRESCALE==32768)
    conval = 7 << 1;
#elif (MY_PRESCALE==8192)
    conval = 6 << 1;
#elif (MY_PRESCALE==2048)
    conval = 5 << 1;
#elif (MY_PRESCALE==512)
    conval = 4 << 1;
#elif (MY_PRESCALE==128)
    conval = 3 << 1;
#elif (MY_PRESCALE==32)
    conval = 2 << 1;
#elif (MY_PRESCALE==8)
    conval = 1 << 1;
#elif (MY_PRESCALE==2)
    conval = 0;
#else
#error invalid MY_PRESCALE
    debugstring( "Error MAX_TIMER_PRESCALE\r\n" );
    return FALSE;
#endif
    //debugstring( "StartEstimate function use timer channel 3\r\n" );
    //reset
    *( vU32* )R_TPCTRL( TEST_TIMER ) = 1 << 1;
    *( vU32* )R_TPCTRL( TEST_TIMER ) = 0;

    *( vU32* )R_TMCNT( TEST_TIMER ) = 0xffff; //max
    *( vU32* )R_TMCTRL( TEST_TIMER ) = conval | 1;
}

U32 EndEstimate( char* str )
{
    *( vU32* )R_TMCTRL( TEST_TIMER ) = 0;; //stop
    U32 upcnt = *( vU32* )R_TMCNT( TEST_TIMER );
    U32 ms = ( upcnt * 1000 ) / ( get_apb_clock() / MY_PRESCALE );
    if( str != 0 )
        debugprintf( "%s : %d ms\r\n", str, ms );
    return ms;

}

void printmem( void* buf, DWORD len )
{
    U32 i;
    U32 k;
    U32 addr = ( U32 )buf;
    BYTE* pbuf = ( BYTE* )buf;
    debugstring( "         " );
    for( i = 0; i < 0x10; i++ )
        debugprintf( " %X ", i );
    debugstring( "\r\n" );
    for( i = 0; i < len; )
    {
        debugprintf( "%08X:", ( addr + i ) & ( ~0xf ) );
        for( k = 0; k < ( ( addr + i ) % 16 ); k++ )
            debugstring( "   " );
        int orgi = i;
        for( ; k < 16; k++ )
        {
            debugprintf( "%02X ", pbuf[i] );
            i++;
            if( !( i < len ) )
                break;
        }
        k++;
        for( ; k < 16; k++ )
        {
            debugstring( "   " );
        }
        debugstring( ":" );
        for( k = 0; k < ( ( addr + orgi ) % 16 ); k++ )
            debugstring( " " );
        for( ; k < 16; k++ )
        {
            if( isprint( pbuf[orgi] ) )
                debugprintf( "%c", pbuf[orgi] );
            else
                debugstring( "." );
            orgi++;
            if( !( orgi < len ) )
                break;
        }
        debugstring( "\r\n" );
    }
}

