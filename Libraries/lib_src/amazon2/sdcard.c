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
#include "sdk.h"

//#define SDHC_USE_DMA

typedef struct
{
    int CSD_STRUCTURE : 2;
    int RESERVED : 6;
    int TAAC : 8;
    int TRAN_SPEED : 8;
    int CCC : 12;
    int READ_BL_LEN : 4;
    int READ_BL_PARTIAL : 1;
    int WRITE_BLK_MISALIGN : 1;
    int READ_BLK_MISALIGN : 1;
    int SDR_IMP : 1;
    int RESERVED2 : 2;
    int C_SIZE : 12;
    int VDD_R_CURR_MIN : 3;
    int VDD_R_CURR_MAX : 3;
    int VDD_W_CURR_MIN : 3;
    int VDD_W_CURR_MAZ : 3;
    int C_SIZE_MULT : 3;
    int ERASE_BLK_EN : 1;
    int SECTOR_SIZE : 7;
    int WP_GRP_SIZE : 7;
    int WP_GRP_ENABLE : 1;
    int RESERVED3 : 5;
    int FILE_FORMAT_GRP : 1;
    int COPY : 1;
    int PERM_WRITE_PROTECT : 1;
    int TMP_WRITE_PROTECT : 1;
    int FILE_FORMAT : 2;
    int RESERVED4 : 2;
    int CRC : 7;
    int ALWAYS1 : 1;

} CSD_STRUCT;

enum SDC_ERROR
{
    SDC_ERR_NONE = 0,
    SDC_ERR_READ_TIMEOUT,
    SDC_ERR_RESPONSE_TIMEOUT,
    SDC_ERR_WRITE_CRC,
    SDC_ERR_READ_CRC,
    SDC_ERR_RESPONSE_CRC,
    SDC_ERR_NO_CARD
};
#define SDC_STAT_DATA_READ_DONE		(1<<8)
#define SDC_STAT_DATA_WRITE_DONE	(1<<9)
#define SDC_STAT_CMD_DONE		(1<<10)
#define SDC_STAT_FIFO_EMPTY		(1<<11)
#define SDC_STAT_FIFO_HALF_FULL	(1<<12)
#define SDC_STAT_FIFO_FULL		(1<<13)
#define SDC_STAT_CARD_REMOVE	(1<<14)
#define SDC_STAT_CARD_INSERT	(1<<15)

#define SDC_NOERROR	0
#define SDC_RESPONSE_TIMEOUT 1
#define SDC_READ_TIMEOUT 2

static U8 g_bHCMemory = 0;

static BOOL sdhc_command( U32 cmd, U32 arg );

//#define SDC_DEBUG_ENABLE
#ifdef SDC_DEBUG_ENABLE
#define SDC_DEBUG debugprintf
#else
#define SDC_DEBUG(...)
#endif


static U16 s_sdc_resposebuf[8];//max 136bit
static U32 s_sdc_lasterrorno = SDC_ERR_NONE;
static U32 s_sdc_laststat = 0;
static U32 s_BlockSize = 512;
static U32 s_MemSize = 0;

#ifdef SDHC_USE_DMA
static BOOL dma_write_sdhc( void* src )
{
    U32 conval = 0;
    dcache_invalidate_way();
    int dmach = get_free_ahb_dma();
    if ( dmach < 0 )
    {
        return FALSE;
    }
    conval |= DMA_CON_DST_32BIT;
    conval |= DMA_CON_DST_BST_8;

    if ( ( ( U32 )src & 0xf ) == 0 )
        conval |= DMA_CON_SRC_BST_256;
    else
        conval |= DMA_CON_SRC_BST_1;

    conval |= DMA_CON_SRC_INCR; //source address increment

    if ( ( ( U32 )src & 0x3 ) == 0 )
        conval |= DMA_CON_SRC_32BIT;
    else if ( ( ( U32 )src & 0x1 ) == 0 )
        conval |= DMA_CON_SRC_16BIT;
    else
        conval |= DMA_CON_SRC_8BIT;

    *R_SDCCON |= 1 << 2; //DMA mode

    *R_DMASRC( dmach ) = ( U32 )src;
    *R_DMADST( dmach ) = ( U32 )R_SDCDFA;
    *R_DMACON( dmach ) = conval;

    // flow control by SDHC
    *R_DMACFG( dmach ) = ( 6 << 12 ) | ( DMA_PERI_SDHC << 4 ) | 1;
    while ( 1 )
    {
        U32 stat = *R_SDCSTAT;
        if ( stat & ( 1 << 9 ) ) //write done
            break;
    }
    set_free_ahb_dma( dmach );
    return TRUE;
}

static BOOL dma_read_sdhc( void* dest )
{
    U32 conval = 0;
    dcache_invalidate_way();
    int dmach = get_free_ahb_dma();
    if ( dmach < 0 )
    {
        return FALSE;
    }
    conval |= DMA_CON_SRC_32BIT;//src 32bit

    if ( ( ( U32 )dest & 0xf ) == 0 )
        conval |= DMA_CON_DST_BST_256;
    else
        conval |= DMA_CON_DST_BST_1;

    conval |= DMA_CON_SRC_BST_8;
    conval |= DMA_CON_DST_INCR; //dest address increment

    if ( ( ( U32 )dest & 0x3 ) == 0 )
        conval |= DMA_CON_DST_32BIT;
    else if ( ( ( U32 )dest & 0x1 ) == 0 )
        conval |= DMA_CON_DST_16BIT;
    else
        conval |= DMA_CON_DST_8BIT;

    *R_DMASRC( dmach ) = ( U32 )R_SDCDFA;
    *R_DMADST( dmach ) = ( U32 )dest;
    *R_DMACON( dmach ) = conval;

    // flow control by SDHC
    *R_DMACFG( dmach ) = ( 6 << 12 ) | ( DMA_PERI_SDHC << 4 ) | 1;
    while ( 1 )
    {
        U32 stat = *R_SDCSTAT;
        if ( stat & ( 1 << 8 ) )
            break;
    }
    set_free_ahb_dma( dmach );

    return TRUE;
}
#endif

BOOL sdhc_erase( U32 addr, U32 endaddr )
{
    sdhc_command( 0x400 | 32, addr );
    sdhc_command( 0x400 | 33, endaddr );
    sdhc_command( 0x500 | 38, 0 );
    return TRUE;
}

BOOL sdhc_writeblock( void* buf, U32 blocknum, U32 nob )
{
    BOOL bok = TRUE;
#ifdef SDHC_USE_DMA
    *R_SDCCON |= ( 1 << 2 );
#endif

    *R_SDCNWR = 0x10;
    *R_SDCNOB = nob;
    U32 addr;
    if ( g_bHCMemory )
        addr = blocknum;
    else
        addr = blocknum * s_BlockSize;

    if ( nob > 1 )
        sdhc_command( 0x400 | ( 1 << 7 ) | ( 1 << 6 ) | ( 25 ), addr ); //multiblock
    else
        sdhc_command( 0x400 | ( 1 << 7 ) | ( 1 << 6 ) | ( 24 ), addr ); //single block
#ifdef SDHC_USE_DMA
    if ( dma_write_sdhc( buf ) == FALSE )
    {
#endif
        register U32 i;
        register U32 stat;

        if ( ( ( U32 )buf & 0x3 ) == 0 ) //32bit align
        {
            U32* databuf = ( U32* )buf;
            for ( i = 0; i < nob * 16; i++ )
            {
                //fill half for speed
                *R_SDCDFA = *databuf;
                *R_SDCDFA = *( databuf + 1 );
                *R_SDCDFA = *( databuf + 2 );
                *R_SDCDFA = *( databuf + 3 );
                *R_SDCDFA = *( databuf + 4 );
                *R_SDCDFA = *( databuf + 5 );
                *R_SDCDFA = *( databuf + 6 );
                *R_SDCDFA = *( databuf + 7 );
                databuf += 8;
                //wait for half empty
                while ( 1 )
                {
                    stat = *R_SDCSTAT;
                    if ( ( stat & ( 1 << 12 ) ) == 0 )
                        break;
                }
                //check error
                if ( stat & ( 1 << 3 ) )
                {
                    bok = FALSE;
                    break;
                }
            }
            if ( bok )
            {
                while ( 1 )
                {
                    stat = *R_SDCSTAT;
                    if ( stat & ( 1 << 9 ) )
                        break;
                }
            }
        }
        else if ( ( ( U32 )buf & 0x1 ) == 0 ) //16it align
        {
            U16* databuf = ( U16* )buf;
            for ( i = 0; i < nob * 32; i++ )
            {
                //fill half for speed
				*(vU16*)R_SDCDFA = *databuf;
				*(vU16*)R_SDCDFA = *(databuf + 1);
				*(vU16*)R_SDCDFA = *(databuf + 2);
				*(vU16*)R_SDCDFA = *(databuf + 3);
				*(vU16*)R_SDCDFA = *(databuf + 4);
				*(vU16*)R_SDCDFA = *(databuf + 5);
				*(vU16*)R_SDCDFA = *(databuf + 6);
				*(vU16*)R_SDCDFA = *(databuf + 7);
                databuf += 8;
                //wait for half empty
                while ( 1 )
                {
                    stat = *R_SDCSTAT;
                    if ( ( stat & ( 1 << 12 ) ) == 0 )
                        break;
                }
                //check error
                if ( stat & ( 1 << 3 ) )
                {
                    bok = FALSE;
                    break;
                }
            }
            if ( bok )
            {
                while ( 1 )
                {
                    stat = *R_SDCSTAT;
                    if ( stat & ( 1 << 9 ) )
                        break;
                }
            }
        }
        else
        {
            U8* databuf = ( U8* )buf;
            for ( i = 0; i < nob * 64; i++ )
            {
                //fill half for speed
				*(vU8*)R_SDCDFA = *databuf;
				*(vU8*)R_SDCDFA = *(databuf + 1);
				*(vU8*)R_SDCDFA = *(databuf + 2);
				*(vU8*)R_SDCDFA = *(databuf + 3);
				*(vU8*)R_SDCDFA = *(databuf + 4);
				*(vU8*)R_SDCDFA = *(databuf + 5);
				*(vU8*)R_SDCDFA = *(databuf + 6);
				*(vU8*)R_SDCDFA = *(databuf + 7);
                databuf += 8;
                //wait for half empty
                while ( 1 )
                {
                    stat = *R_SDCSTAT;
                    if ( ( stat & ( 1 << 12 ) ) == 0 )
                        break;
                }
                //check error
                if ( stat & ( 1 << 3 ) )
                {
                    bok = FALSE;
                    break;
                }
            }
            if ( bok )
            {
                while ( 1 )
                {
                    stat = *R_SDCSTAT;
                    if ( stat & ( 1 << 9 ) )
                        break;
                }
            }
        }
#ifdef SDHC_USE_DMA
    }
#endif
    if ( nob > 1 )
        sdhc_command( 0x50c, 0 ); //stop commmand
    *R_SDCSTAT = 0x200;//write operation done
    return bok;
}

static int sdhc_readblock_r( void* buf, U32 blocknum, U32 nob )
{
    register U32 stat;

#ifdef SDHC_USE_DMA
    *R_SDCCON |= ( 1 << 2 );
#endif

    *R_SDCNWR = 0x10;
    *R_SDCNOB = nob;
    U32 addr;

    if ( g_bHCMemory )
        addr = blocknum;
    else
        addr = blocknum * s_BlockSize;

    if ( nob > 1 )
        sdhc_command( 0x400 | ( 1 << 7 ) | ( 18 ), addr );
    else
        sdhc_command( 0x400 | ( 1 << 7 ) | ( 17 ), addr );
    int re = 0;
#ifdef SDHC_USE_DMA
    if ( dma_read_sdhc( buf ) == FALSE )
    {
#endif
        register U32 i, j;
        U8* databuf8 = ( U8* )buf;
        U16* databuf16 = ( U16* )buf;
        U32* databuf32 = ( U32* )buf;
        U32 cnt = s_BlockSize / 64;
        for ( i = 0; i < nob * cnt; i++ )
        {
            //wait for half full
            while ( 1 )
            {
                stat = *R_SDCSTAT;
                if ( stat & ( 1 << 13 ) ) //full
                {
                    break;
                }
                if ( stat & ( 1 << 1 ) ) //read time out
                {
                    s_sdc_lasterrorno = SDC_ERR_READ_TIMEOUT;
                    SDC_DEBUG( "READ TIME OUT ERROR(0x%x block)\r\n", blocknum );
					sdhc_command(0x500 | 12, 0);//stop commmand
                    re = 1;
                    break;
                }
				if (stat & (1 << SDC_ERR_READ_CRC))
				{
					s_sdc_lasterrorno = SDC_ERR_READ_CRC;
					re = 2;
					break;
				}

            }
            if ( re != 0 )
            {
                break;
            }

            if ( ( ( U32 )buf & 0x3 ) == 0 )
            {
                for ( j = 0; j < 2; j++ )
                {
                    *databuf32 = *R_SDCDFA;
                    *( databuf32 + 1 ) = *R_SDCDFA;
                    *( databuf32 + 2 ) = *R_SDCDFA;
                    *( databuf32 + 3 ) = *R_SDCDFA;
                    *( databuf32 + 4 ) = *R_SDCDFA;
                    *( databuf32 + 5 ) = *R_SDCDFA;
                    *( databuf32 + 6 ) = *R_SDCDFA;
                    *( databuf32 + 7 ) = *R_SDCDFA;
                    databuf32 += 8;
                }
            }
            else if ( ( ( U32 )buf & 0x1 ) == 0 )
            {
                for ( j = 0; j < 4; j++ )
                {
                    *databuf16 = *( volatile U16* )R_SDCDFA;
                    *( databuf16 + 1 ) = *( volatile U16* )R_SDCDFA;
                    *( databuf16 + 2 ) = *( volatile U16* )R_SDCDFA;
                    *( databuf16 + 3 ) = *( volatile U16* )R_SDCDFA;
                    *( databuf16 + 4 ) = *( volatile U16* )R_SDCDFA;
                    *( databuf16 + 5 ) = *( volatile U16* )R_SDCDFA;
                    *( databuf16 + 6 ) = *( volatile U16* )R_SDCDFA;
                    *( databuf16 + 7 ) = *( volatile U16* )R_SDCDFA;
                    databuf16 += 8;
                }
            }
            else
            {
                for ( j = 0; j < 8; j++ )
                {
                    *databuf8 = *( volatile U8* )R_SDCDFA;
                    *( databuf8 + 1 ) = *( volatile U8* )R_SDCDFA;
                    *( databuf8 + 2 ) = *( volatile U8* )R_SDCDFA;
                    *( databuf8 + 3 ) = *( volatile U8* )R_SDCDFA;
                    *( databuf8 + 4 ) = *( volatile U8* )R_SDCDFA;
                    *( databuf8 + 5 ) = *( volatile U8* )R_SDCDFA;
                    *( databuf8 + 6 ) = *( volatile U8* )R_SDCDFA;
                    *( databuf8 + 7 ) = *( volatile U8* )R_SDCDFA;
                    databuf8 += 8;
                }
            }


            if ( stat & ( 1 << 8 ) ) //read operatoin completed
            {
                break;
            }
        }
#ifdef SDHC_USE_DMA
    }
#endif
    if ( re == 0 )
    {
        while ( 1 )
        {
            stat = *R_SDCSTAT;
            if ( stat & ( 1 << 8 ) )
                break;
            if ( stat & ( 1 << 1 ) )
            {
                re = 3;
                break;
            }
        }
    }
    if ( nob > 1 )
    {
        sdhc_command( 0x500 | 12, 0 ); //stop commmand
    }
    *R_SDCSTAT = 0x100;//write operation done
    return re;
}

BOOL sdhc_readblock( void* buf, U32 blocknum, U32 nob )
{
    if ( sdhc_readblock_r( buf, blocknum, nob ) != 0 )
    {
        int i;
        int sysclock = get_ahb_clock();
        for ( i = 0; i < 100; i++ )
        {
            if ( ( sysclock / ( 2 + i ) ) <= ( 1 * 1000 * 1000 ) )
                break;
        }
        U32 orgdiv = *R_SDCCD;
        *R_SDCCD = i;
        int re = sdhc_readblock_r( buf, blocknum, nob );

        *R_SDCCD = orgdiv;
        if ( re == 0 )
            return TRUE;
        else if ( re == 1 )
            DEBUGPRINTF( "READ TIME OUT ERROR(0x%x block)\r\n", blocknum );
        else if ( re == 2 )
            debugstring( "READ CRC ERROR\r\n" );
        return FALSE;
    }
    return TRUE;
}

static BOOL sdhc_command( U32 cmd, U32 arg )
{
    U8 i;

    *R_SDCCMDA = arg;
    *R_SDCCMDCON = cmd;
    U8 cmdnum = cmd & 0x3f;
    //DEBUGPRINTF("CMD%d ,ARG:%#x \r\n", cmdnum,arg);
    while ( 1 )
    {
        s_sdc_laststat = *R_SDCSTAT;
        if ( s_sdc_laststat & 0x400 )
        {
            //	PRINTVAR(s_sdc_laststat);
            break;
        }
        if ( s_sdc_laststat & ( 1 << SDC_ERR_RESPONSE_TIMEOUT ) )
        {
            *R_SDCSTAT = 0x7ff; // do not clear read done / write done
            //PRINTVAR(s_sdc_laststat);
            s_sdc_lasterrorno = SDC_ERR_RESPONSE_TIMEOUT;
            SDC_DEBUG( "SDC_ERR_RESPONSE_TIMEOUT\r\n" );
            return FALSE;
        }
    }

    *R_SDCSTAT = 0x7ff; // do not clear read done / write done

    if ( cmd & ( 1 << 10 ) )
    {

        if ( s_sdc_laststat & ( 1 << SDC_ERR_RESPONSE_TIMEOUT ) )
        {
            //PRINTVAR(s_sdc_laststat);
            s_sdc_lasterrorno = SDC_ERR_RESPONSE_TIMEOUT;
            SDC_DEBUG( "SDC_ERR_RESPONSE_TIMEOUT\r\n" );
            return FALSE;
        }
        if ( !( cmdnum == 41 || cmdnum == 1 ) )
        {
            if ( s_sdc_laststat & ( 1 << 5 ) )
            {
                s_sdc_lasterrorno = SDC_ERR_RESPONSE_CRC;
                //PRINTVAR(s_sdc_lasterrorno );
                return FALSE;
            }
        }

        if ( cmd & ( 1 << 9 ) )
        {
            SDC_DEBUG( "Long Respose\r\n" );
            for ( i = 0; i < 8; i++ )
                s_sdc_resposebuf[i] = *( volatile U16* )R_SDCRFA;
        }
        else
        {
            SDC_DEBUG( "Short Respose\r\n" );
            for ( i = 0; i < 3; i++ )
            {
                s_sdc_resposebuf[i] = *( volatile U16* )R_SDCRFA;
#ifdef SDC_DEBUG_ENABLE
                PRINTVAR( s_sdc_resposebuf[i] );
#endif
            }
        }
    }
    return TRUE;
}

static void sdcard_power_on( BOOL bOn )
{
    if ( bOn )
    {
        *R_SDCCON = ( 1 << 5 ) | 1; //1bit mode,clock enable
        delayms( 10 );
    }
    else
    {
        *R_SDCCON = 0;//clock disable
    }

}

/**
* intialize sdcard.
*
* \return true if it succeeds, false if it fails.
*/
BOOL sdhc_init()
{
    U32 i;
    g_bHCMemory = 0;
    sdcard_power_on( FALSE );
    sdcard_power_on( TRUE );
    // initialize
    *R_SDCCD = 0x200;
    *R_SDCRTO = 0xff;
    *R_SDCDTO = 0xff00;
    *R_SDCBL = 0x200;
    i = 512; //512 times
    do
    {
        if ( !sdhc_command( 0, 0 ) ) //reset
            continue;

        if ( sdhc_command( 0x400 + 8, 0x1aa ) )
            break;//cmd55
    }
    while ( --i );

    i = 1024; //512 times
    do
    {
        sdhc_command( 0x400 + 55, 0 ); //ACMDs shall be preceded with APP_CMD command (CMD55).
        if ( !sdhc_command( 0x500 + 41, 0x40ff8000 ) )
        {
            continue;
        }
        else
        {
            if ( s_sdc_resposebuf[0] & 0x80 )
            {
                if ( s_sdc_resposebuf[0] & ( 1 << 6 ) )
                {
                    debugstring( "HCSD Memory Card\r\n" );
                    g_bHCMemory = 1;
                }
                else
                {
                    debugstring( "SD Memory Card\r\n" );
                }
                break;
            }
        }

    }
    while ( --i );

    if ( i == 0 )
    {
        debugstring( "No response(SD Card), check your card \r\n" );
        return FALSE;
    }


    debugstring( "Identification Mode Start\r\n" );

    i = 512; //512 times
    do
    {
        if ( sdhc_command( 0x600 | 2, 0 ) == TRUE )
        {
            break;
        }
    }
    while ( i-- );

    if ( i == 0 )
    {
        debugstring( "COMMAND SEND ERROR \r\n" );
        return FALSE;
    }


    if ( sdhc_command( 0x400 | 3, 0 ) == FALSE ) //Get RCA
    {

        return FALSE;
    }
    U32 rca;
    rca = ( ( s_sdc_resposebuf[0] & 0x00ff ) << 8 ) | ( s_sdc_resposebuf[1] >> 8 );
    sdhc_command( 0x600 | 9, rca << 16 );

#ifdef SDC_DEBUG_ENABLE
    debugstring( "==== CSD ===== \r\n" );
    for ( i = 0; i < 8; i++ )
    {
        DEBUGPRINTF( "%04X [%d:%d]\r\n", s_sdc_resposebuf[i], 16 * ( 8 - i ) - 1, 16 * ( 8 - i ) - 16 );
        PRINTVAR( s_sdc_resposebuf[i] );
    }
#endif
    U8 sd_ver = 0;//0:1.0, 1:2.0
    U8 TAAC, NSAC, TRAN_SPEED;
    sd_ver = s_sdc_resposebuf[0] >> 14;
    TAAC = s_sdc_resposebuf[0] & 0xff;
    NSAC = s_sdc_resposebuf[1] >> 8;
    TRAN_SPEED = s_sdc_resposebuf[1] & 0xff;
    U32 maxfreq;
    switch ( TRAN_SPEED & 0x7 )
    {
    case 0:
        maxfreq = 1 * 10000;
        break;
    case 1:
        maxfreq = 1 * 100000;
        break;
    case 2:
        maxfreq = 10 * 100000;
        break;
    case 3:
        maxfreq = 100 * 100000;
        break;
    default:
        debugstring( "unsupported operating frequency\r\n" );
        maxfreq = 1 * 10000;
        break;
    }
    U32 freqtype[16] = { 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };
    maxfreq *= freqtype[TRAN_SPEED >> 3];

    DEBUGPRINTF( "the mandatory maximum operating frequency of SD Memory Card : %dMhz \r\n", maxfreq / 1000000 );

    int READ_BL_LEN = s_sdc_resposebuf[2] & 0xf;
    int C_SIZE;
    if ( sd_ver == 0 )
        C_SIZE = ( ( s_sdc_resposebuf[3] & 0x03ff ) << 2 ) + ( ( s_sdc_resposebuf[4] & 0xc000 ) >> 14 );
    else
        C_SIZE = ( ( s_sdc_resposebuf[3] & 0x3f ) << 16 ) + s_sdc_resposebuf[4];
    s_BlockSize = 1 << READ_BL_LEN;;
    if ( sd_ver == 0 )
    {
        int C_SIZE_MULT = ( ( s_sdc_resposebuf[4] & 0x3 ) << 1 ) + ( ( s_sdc_resposebuf[5] & 0x8000 ) >> 15 );
        //U32 BLOCKNR = (C_SIZE+1)*(pow(2,C_SIZE_MULT+2));
        U32 BLOCKNR = ( C_SIZE + 1 ) * ( 1 << ( C_SIZE_MULT + 2 ) );
        s_MemSize = ( BLOCKNR * s_BlockSize ) / ( 1024 * 1024 );
    }
    else
    {
        s_MemSize = ( C_SIZE + 1 ) / 2;
        DEBUGPRINTF( "SD Memory Size : %ldMbyte \r\n", s_MemSize );
    }
    DEBUGPRINTF( "SD Memory Size : %dMbyte ,Blocksize:%dbyte\r\n", s_MemSize, s_BlockSize );

    sdhc_command( 0x600 | 10, rca << 16 );

#ifdef SDC_DEBUG_ENABLE
    debugstring( "==== CID ===== \r\n" );
    for ( i = 0; i < 8; i++ )
    {
        DEBUGPRINTF( "%04X [%d:%d]\r\n", s_sdc_resposebuf[i], 16 * ( 8 - i ) - 1, 16 * ( 8 - i ) - 16 );
        PRINTVAR( s_sdc_resposebuf[i] );
    }
#endif

    sdhc_command( 0x500 | 7, rca << 16 );
    debugstring( "SD CARD Initialize Completed\r\n" );

    //after initialize
    if ( READ_BL_LEN != 9 ) //if not 512
    {
        s_BlockSize = 512;
        sdhc_command( ( 1 << 10 ) | 16, 512 ); //set block size
        debugstring( "set block size as 512byte\r\n" );
    }

    sdhc_command( ( 1 << 10 ) | 55, rca << 16 ); //APPcmd
    sdhc_command( ( 1 << 10 ) | 6, 2 ); //4bit mode

    delayms( 50 );

    *R_SDCCON = ( 1 << 5 ) | 3; //4bit mode

    U32 sysclock = get_ahb_clock();
    for ( i = 0; i < 100; i++ )
    {
        if ( ( sysclock / ( 2 + i ) ) <= maxfreq )
            break;
    }
    DEBUGPRINTF( "SDCard operating frequency:%d \r\n", sysclock / ( 2 + i ) );
    *R_SDCCD = i;
    *R_SDCBL = s_BlockSize;

    return TRUE;
}

U32 sdhc_getsectorsize()
{
    return s_BlockSize;
}

/**
* Gets sector count of SDcard.
*
* \return sector count.
*/
U32 sdhc_getsectorcount()
{
    return ( ( unsigned long long )s_MemSize * 1024 * 1024 / sdhc_getsectorsize() );
}

/**
* return Megabyte Size.
*
* \return Megabyte size.
*/
U32 sdc_memsize()
{
    return s_MemSize;
}

/**
* read sectors
*
* \param buf Pointer to data.
* \param sector	   Sector number.
* \param sectcnt	   count of sector.
*
* \return true if it succeeds, false if it fails.
*/
BOOL sdhc_read_sect( void* buf, U32 sector, U32 sectcnt )
{
    return sdhc_readblock( buf, sector, sectcnt );
}

/**
* write sectors(512byte)
*
* \param buf Pointer to data.
* \param sector	   Sector number.
* \param sectcnt	   Sector count.
*
* \return true if it succeeds, false if it fails.
*/
BOOL sdhc_write_sect( void* buf, U32 sector, U32 sectcnt )
{
    return sdhc_writeblock( buf, sector, sectcnt );
}
