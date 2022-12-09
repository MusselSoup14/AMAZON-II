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

/*******************************************/
/* Define values for Microsoft WAVE format */
/*******************************************/
#define RIFF		0x46464952		/* "RIFF" */
#define WAVEMAGIC		0x45564157		/* "WAVE" */
#define FACT		0x74636166		/* "fact" */
#define LIST		0x5453494c		/* "LIST" */
#define FMT		0x20746D66		/* "fmt " */
#define DATA		0x61746164		/* "data" */
#define PCM_CODE	0x0001
#define MS_ADPCM_CODE	0x0002
#define IMA_ADPCM_CODE	0x0011
#define MP3_CODE	0x0055
#define WAVE_MONO	1
#define WAVE_STEREO	2

typedef struct _tagWaveFMT
{
    /* Not saved in the chunk we read:
    	Uint32	FMTchunk;
    	Uint32	fmtlen;
    */
    U16 encoding;
    U16 channels;            /* 1 = mono, 2 = stereo */
    U32 frequency;           /* One of 11025, 22050, or 44100 Hz */
    U32 byterate;            /* Average bytes per second */
    U16 blockalign;          /* Bytes per sample block */
    U16 bitspersample;       /* One of 8, 12, 16, or 4 for ADPCM */
} WaveFMT;
typedef struct Chunk
{
    U32 magic;
    U32 length;
    U8 *data;
} Chunk;


struct wavhdr_t
{
    //RIFF header
    U32 ID; // 0x46464952 "RIFF"
    U32 chunksize;
    U32 format;//0x45564157 "WAVE"

    U32 fmtID; // 0x20746d66 "fmt "
    U32 subchunk1size;

    U16 wFormatTag; // PCM = 1 (i.e. Linear quantization)	Values other than 1 indicate some form of compression.
    U16 nChannels; // mono = 1, stereo = 2
    U32 nSamplesPerSec; // frequency
    U32 nAvgBytesPerSec;
    U16 nBlockAlign;
    U16 wBitsPerSample;
};

static WAVE* snd_dma_ch[8] = {0,};
static const INTERRUPT_TYPE dma_intnum[] = {INTNUM_DMA0, INTNUM_DMA1, INTNUM_DMA2, INTNUM_DMA3};

static bool s_snd_channel_used[SOUND_MAX_CHANNEL] = {0,};
static int get_snd_channel()
{
    CRITICAL_ENTER();
    int i;
    for( i = 0; i < SOUND_MAX_CHANNEL; i++ )
    {
        U32 stat = *R_SNDCON( i );
        if( ( stat & 1 ) == 0 )
        {
            if( s_snd_channel_used[i] == false )
            {
                s_snd_channel_used[i] = true;
                CRITICAL_EXIT();
                return i;
            }
        }
    }
    CRITICAL_EXIT();
    debugstring( "All channel is busy\r\n" );
    return -1;
}
static void release_snd_channel( int i )
{
    if( ( i < 0 ) || ( i >= SOUND_MAX_CHANNEL ) )
        return;
    s_snd_channel_used[i] = false;
}

void snd_dmatransfer( int dmach, int sndch, void* src, int len )
{		
	dcache_invalidate_way();
    *R_DMASRC( dmach ) = ( U32 ) src;
    *R_DMADST( dmach ) = ( U32 ) R_SNDDAT( sndch );
    U32 conval = 0;	
	if((U32)src & 0x3)
	{		
		conval |= DMA_CON_SRC_8BIT;//src 8bit		
		conval |= DMA_CON_DST_32BIT;//dest 32bit
		
		conval |= DMA_CON_SRC_BST_1;
		conval |= DMA_CON_DST_BST_1;//NO burst , APB do not supports fixed burst mode
		conval |= DMA_CON_SRC_INCR;

		//must be 32
		U32 transfersize = 32;

		if( ( len ) < 0xfff )		
		{
			conval |= ( len );			
		}
		else
		{
			U32 reloadcnt  = ( ( len ) / transfersize ) - 1;			
			conval |= transfersize;

			*R_DMASGA( dmach ) = 0;
			*R_DMADSA( dmach ) = 0;
			*R_DMAARCNT( dmach ) = reloadcnt;
		}
	}
	else
	{				
		conval |= DMA_CON_SRC_32BIT;//src 32bit
		conval |= DMA_CON_DST_32BIT;//dest 32bit

		//conval |= DMA_CON_SRC_BST_256;
		conval |= DMA_CON_SRC_BST_1;
		conval |= DMA_CON_DST_BST_1;//NO burst , APB do not supports fixed burst mode
		conval |= DMA_CON_SRC_INCR;

		//must be 32
		U32 transfersize = 32;

		if( ( len>>2 ) < 0xfff )
		{		
			conval |= ( len>>2 );
		}
		else
		{
			U32 reloadcnt  = ( ( len>>2 ) / transfersize ) - 1;
			conval |= transfersize;

			*R_DMASGA( dmach ) = 0;
			*R_DMADSA( dmach ) = 0;
			*R_DMAARCNT( dmach ) = reloadcnt;
		}
	}
    *R_DMACON( dmach ) = conval;
    *R_DMACFG( dmach ) = ( 1 << 17 ) | ( 1 << 12 ) | ( ( DMA_PERI_SNDCH0 + sndch ) << 8 ) | 1;
}

static void refill_bigfile( int dmach, WAVE* pWave )
{
    UINT nRead;
	UINT read_size;

	if (pWave->cursamplesnum) //samples2 was played
		pWave->transfered_len += pWave->bufsize2;
	else
		pWave->transfered_len += pWave->bufsize;
	
	//check more read?
	read_size = (pWave->headersize + pWave->datasize) - f_tell(&pWave->file);
	if (read_size > 0)
    {
		//time to send a first data?
		if (pWave->bLoop)
		{
			if (f_tell(&pWave->file) == (pWave->headersize + WAVE_RELOAD_BUF_MAX))
			{
				//restart
				pWave->transfered_len = 0;
			}
		}

        if( pWave->cursamplesnum ) //samples2 was played
        {
            snd_dmatransfer( dmach, pWave->channelNumber, pWave->samples, pWave->bufsize );
            pWave->validsamples = false;
            pWave->cursamplesnum = 0;
			read_size = read_size > WAVE_RELOAD_BUF_MAX ? WAVE_RELOAD_BUF_MAX : read_size;
            f_read( &pWave->file, pWave->samples2, read_size, &nRead );
            pWave->bufsize2 = nRead;
			if (nRead < WAVE_RELOAD_BUF_MAX) //zero padding for avoid noise
				memset(pWave->samples2 + nRead, 0, WAVE_RELOAD_BUF_MAX - nRead > 128 ? 128 : WAVE_RELOAD_BUF_MAX - nRead);
            pWave->validsamples2 = true;
        }
        else
        {
            snd_dmatransfer( dmach, pWave->channelNumber, pWave->samples2, pWave->bufsize2 );
            pWave->validsamples2 = 0;
            pWave->cursamplesnum = 1;
			read_size = read_size > WAVE_RELOAD_BUF_MAX ? WAVE_RELOAD_BUF_MAX : read_size;
			f_read(&pWave->file, pWave->samples, read_size, &nRead);
            pWave->bufsize = nRead;
			if (nRead < WAVE_RELOAD_BUF_MAX) //zero padding for avoid noise
				memset(pWave->samples + nRead, 0, WAVE_RELOAD_BUF_MAX - nRead > 128 ? 128 : WAVE_RELOAD_BUF_MAX - nRead);
            pWave->validsamples = 1;
        }
        if( pWave->bLoop )
        {
			if (f_tell(&pWave->file) >= (pWave->headersize + pWave->datasize))
                f_lseek( &pWave->file, pWave->headersize );
        }
    }
    else
    {
        if( pWave->validsamples || pWave->validsamples2 )
        {
            if( pWave->cursamplesnum ) //samples2 was played
            {
                if( pWave->validsamples )
                {
                    snd_dmatransfer( dmach, pWave->channelNumber, pWave->samples, pWave->bufsize );
                    pWave->validsamples = false;
                    pWave->cursamplesnum = 0;
                }
            }
            else //samples was played
            {
                if( pWave->validsamples2 )
                {
                    snd_dmatransfer( dmach, pWave->channelNumber, pWave->samples2, pWave->bufsize2 );
                    pWave->validsamples2 = false;
                    pWave->cursamplesnum = 1;
                }
            }
        }
        else
        {
            //DEBUGPRINTF("%s was stopped(ch:%d)\r\n",pWave->filename,pWave->channelNumber);
            sound_stop( pWave );
            fs_enable_support_interrupt( dma_intnum[dmach], FALSE );
        }
    }
}

static void dma_sound_isr_r( int dmach )
{
    WAVE* pWave = snd_dma_ch[dmach];
    *R_DMATCIC = ( 1 << dmach );
#if SUPPORT_NESTED_INTNUM_DMA0==1
    *R_INTPENDCLR = 0x20 + dma_intnum[dmach];
#endif
    if( pWave->TxIsr )
    {
        pWave->TxIsr( dmach, pWave );
    }
    else //for PCM
    {
        if( pWave->bNeedReload ) //big file
        {
            refill_bigfile( dmach, pWave );
        }
        else
        {
			pWave->transfered_len += pWave->bufsize;
			if (pWave->bLoop)
            {
                //DEBUGPRINTF( "loop\r\n" );
                snd_dmatransfer( dmach, pWave->channelNumber, pWave->samples, pWave->bufsize );
            }
            else
            {
                //DEBUGPRINTF( "%s was stopped(ch:%d, dmach:%d)\r\n", pWave->filename, pWave->channelNumber, dmach );
                sound_stop( pWave );
            }
        }
    }
}

#define DMA_SOUND_ISR(A)	static void dma_sound_isr##A(void)\
    {dma_sound_isr_r(A);}

DMA_SOUND_ISR( 0 )
DMA_SOUND_ISR( 1 )
DMA_SOUND_ISR( 2 )
DMA_SOUND_ISR( 3 )

typedef void ( *ISRFP )( void ) ;
static const ISRFP isrtable[] = {dma_sound_isr0, dma_sound_isr1, dma_sound_isr2, dma_sound_isr3};

BOOL SetPlayDmaEx( WAVE* pWave, void* src, U32 len )
{
    dcache_invalidate_way();
    int dmach = get_free_ahb_dma();
    if( dmach < 0 )
        return FALSE;
    set_interrupt( dma_intnum[dmach], isrtable[dmach] );
    enable_interrupt( dma_intnum[dmach], TRUE );

    if( pWave->bNeedReload )
    {
        if( pWave->file.fs != NULL ) //except for mp3 big file
            fs_enable_support_interrupt( dma_intnum[dmach], TRUE );
    }
    snd_dmatransfer( dmach, pWave->channelNumber, src, len );
    pWave->validsamples = 0;
    pWave->cursamplesnum = 0;
    snd_dma_ch[dmach] = pWave;
    return TRUE;
}

BOOL SetPlayDma( WAVE* pWave )
{
    return SetPlayDmaEx( pWave, pWave->samples, pWave->bufsize );
}

static U32 currnt_time_wav( WAVE* pWav )
{
    U32 ms;
    if ( sound_isplay( pWav ) == FALSE )
        return 0;

    U32 tx_len = 0;
    U32 dma_start_addr = ( U32 )pWav->samples;
    if ( pWav->bNeedReload )
    {
        tx_len = pWav->transfered_len;
        if ( pWav->cursamplesnum )
            dma_start_addr = ( U32 )pWav->samples2;

    }
    tx_len += *R_DMASRC( pWav->channelNumber ) - ( U32 )dma_start_addr;
	if(tx_len >0x418930) //check (tx_len*1000 )
	{
		U32 sec = tx_len/pWav->byterate;	
		ms = (1000 * sec) + ((1000 * (tx_len % pWav->byterate)) / pWav->byterate);
}
	else
	{
		ms = 1000 * tx_len / (pWav->byterate);

	}
	return ms;
}

static WAVE* SetWaveHeader( FIL* fp )
{
    /* WAV magic header */
    U32 RIFFchunk;
    U32 wavelen = 0;
    U32 WAVEmagic;
    Chunk chunk;
    /* FMT chunk */
    WaveFMT format ;
    WAVE* pWave = NULL;
    U32 nRead;

    f_read( fp, ( void* )&RIFFchunk, 4, &nRead );
    f_read( fp, ( void* )&wavelen, 4, &nRead );
    if( nRead != 4 )
        return 0;
    if ( wavelen == WAVEMAGIC )      /* The RIFFchunk has already been read */
    {
        WAVEmagic = wavelen;
        wavelen = RIFFchunk;
        RIFFchunk = RIFF;
    }
    else
    {
        f_read( fp, ( void* )&WAVEmagic , 4, &nRead );
    }
    if ( ( RIFFchunk != RIFF ) || ( WAVEmagic != WAVEMAGIC ) )
    {
        debugstring( "Unrecognized file type (not WAVE)" );
        return 0;
    }

    /* Read the audio data format chunk */
    chunk.data = NULL;
    bool bfoundFMT = false;
    while ( 1 )
    {
        f_read( fp, &chunk.magic, 4, &nRead );
        f_read( fp, &chunk.length, 4, &nRead );
        //found DATA Tag
        if ( chunk.magic == DATA )
            break;
        chunk.data = ( U8* ) malloc( chunk.length );
        if ( chunk.data == NULL )
        {
            debugstring( "insufficient invalid Memory\r\n" );
            return 0;
        }

        f_read( fp, chunk.data, chunk.length, &nRead );

        if ( nRead != chunk.length )
        {
            free( chunk.data );
            return ( 0 );
        }
        if ( chunk.magic == FMT )
        {
            memcpy( &format, chunk.data, sizeof( WaveFMT ) );
            bfoundFMT = true;
        }
        free( chunk.data );
    }

    if ( bfoundFMT == false )
    {
        DEBUGPRINTF( "Complex WAVE files not supported" );
        return 0;
    }

    if( format.encoding != PCM_CODE )
    {
        DEBUGPRINTF( "Unknown WAVE data format: 0x%.4x", format.encoding );
        return 0;
    }
    if( format.frequency > SND_OUTPUT_HZ )
    {
        DEBUGPRINTF( "not supported freqeuncy: 0x%.4x", format.frequency );
        return 0;
    }
    pWave = malloc( sizeof( WAVE ) );
    memset( pWave, 0, sizeof( WAVE ) );
    pWave->volume = DEFAULT_VOLUME; // maximum 255
    pWave->panpot = 8;
    pWave->channelNumber = -1;
    pWave->freq = format.frequency;
    pWave->channels = format.channels;
    int bitspersample = format.bitspersample;
    if( format.bitspersample == 16 )
    {
        if( format.channels == 2 )
            pWave->pcmtype = PCMTYPE_SS16;
        else
            pWave->pcmtype = PCMTYPE_SM16;

    }
    else if( format.bitspersample == 8 )
    {
        if( format.channels == 2 )
            pWave->pcmtype = PCMTYPE_US8;
        else
            pWave->pcmtype = PCMTYPE_UM8;
    }
    else
    {
        DEBUGPRINTF( "not supported PCM TYPE: \r\n" );
        free( pWave );
        return 0;
    }
    pWave->byterate = format.frequency * format.channels * bitspersample / 8;
    pWave->headersize = f_tell( fp );
    pWave->datasize = chunk.length;

    pWave->PlaybackPitch = ( ( pWave->freq * 256 ) / SND_OUTPUT_HZ ) - 1;
    U32 sec = pWave->datasize / pWave->byterate;
    U32 temp = pWave->datasize % pWave->byterate;
    pWave->duration_ms = ( sec * 1000 ) + ( ( temp * 1000 ) / pWave->byterate );
    DEBUGPRINTF( "freq : %d , %d(bits/smape), byterate:%d , channel : %d, data-size:%dkbyte, %d sec\r\n"
                 , pWave->freq, bitspersample, pWave->byterate, pWave->channels, pWave->datasize / 1024, sec );

    pWave->current_time = currnt_time_wav;

    return pWave;
}


inline static U32 fill_buf(WAVE* pWave,int bufnum)
{
	U32 nRead;
	UINT read_size;
	U8* dst;
	read_size = (pWave->headersize + pWave->datasize) - f_tell(&pWave->file);
	read_size = read_size > WAVE_RELOAD_BUF_MAX ? WAVE_RELOAD_BUF_MAX : read_size;
	if (read_size == 0)
		return 0;
	
	if (bufnum == 0)
	{
		dst = pWave->samples;
		f_read(&pWave->file, dst, read_size, &nRead);
		if (nRead == 0)
			return 0;
		pWave->bufsize = nRead;
		pWave->validsamples = 1;
	}
	else
	{
		dst = pWave->samples2;
		f_read(&pWave->file, dst, read_size, &nRead);
		if (nRead == 0)
			return 0;
		pWave->bufsize2 = nRead;
		pWave->validsamples2 = 1;
	}
	if (nRead < WAVE_RELOAD_BUF_MAX) //zero padding for avoid noise, last data?
	{
		
		memset(dst + nRead, 0, WAVE_RELOAD_BUF_MAX - nRead > 128 ? 128 : WAVE_RELOAD_BUF_MAX - nRead);
	}
	return nRead;
}

static BOOL bigfile_load( WAVE* pWave )
{
    U32 nRead;
	if (pWave == NULL)
		return FALSE;
	pWave->validsamples = pWave->validsamples2 = 0;
    if( pWave->samples == 0 )
    {
        pWave->samples = ( U8* )malloc( WAVE_RELOAD_BUF_MAX );
        if( pWave->samples == 0 )
        {
            debugstring( "insufficient invalid Memory\r\n" );
            return FALSE;
        }
		pWave->samples2 = (U8*)malloc(WAVE_RELOAD_BUF_MAX);
        if( pWave->samples2 == 0 )
        {
            debugstring( "insufficient invalid Memory\r\n" );
            return FALSE;
        }
        pWave->bMallocedBuf = true;
    }
	if (pWave->transfered_len < pWave->datasize)
		f_lseek( &pWave->file, pWave->headersize + pWave->transfered_len );
	else
	{
		if (pWave->bLoop == FALSE)
			return FALSE;
		pWave->transfered_len = 0;
		f_lseek(&pWave->file, pWave->headersize);
	}
	nRead = fill_buf(pWave, 0);
	if (nRead == 0)
		return FALSE;
	if (nRead<WAVE_RELOAD_BUF_MAX)
	{
		if (pWave->bLoop)
		{
			f_lseek(&pWave->file, pWave->headersize);
		}
		else
		{
			pWave->cursamplesnum = 0;
			return TRUE;
		}
	}
	
	nRead = fill_buf(pWave, 1);
	if (nRead == 0)
	{
		if (pWave->bLoop == FALSE)
		{
			pWave->cursamplesnum = 0;
			return TRUE;
		}
		else
		{
			f_lseek(&pWave->file, pWave->headersize);
			nRead = fill_buf(pWave, 1);
		}
	}
		
	if (nRead < WAVE_RELOAD_BUF_MAX) //zero padding for avoid noise
	{
		if (pWave->bLoop)
			f_lseek(&pWave->file, pWave->headersize);
	}
	
    pWave->cursamplesnum = 0;
    return TRUE;
}
static inline U32 makeword( U8* buf )
{
    U32 re = buf[0] + ( buf[1] << 8 ) + ( buf[2] << 16 ) + ( buf[3] << 24 );
    return re;
}
WAVE* sound_loadwavp( U8* buf, U32 len )
{

    /* WAV magic header */
    U32 RIFFchunk;
    //chunk point
    U32 Length;
    U32 Magic;
    /* FMT chunk */
    WaveFMT format ;
    WAVE* pWave = NULL;
    U32 nRead = 0;
    RIFFchunk = makeword( buf );
    nRead += 4;
    Magic = makeword( buf + nRead );
    nRead += 4;
    if ( Magic != WAVEMAGIC )      /* The RIFFchunk has already been read */
    {
        Magic = makeword( buf + nRead );
        nRead += 4;
    }
    if ( ( RIFFchunk != RIFF ) || ( Magic != WAVEMAGIC ) )
    {
        debugstring( "Unrecognized file type (not WAVE)" );
        return 0;
    }
    bool bFoundFMT = false;
    /* Read the audio data format chunk */
    while ( 1 )
    {
        Magic = makeword( buf + nRead );
        nRead += 4;
        Length = makeword( buf + nRead );
        nRead += 4;

        if ( Magic == DATA )
        {
            break;
        }
        else if ( Magic == FMT )
        {
            bFoundFMT = true;
            memcpy( &format, ( buf + nRead ), sizeof( WaveFMT ) );
        }
        nRead += Length;
    }

    if ( bFoundFMT == false )
    {
        DEBUGPRINTF( "Complex WAVE files not supported" );
        return 0;
    }

    if( format.encoding != PCM_CODE )
    {
        DEBUGPRINTF( "Unknown WAVE data format: 0x%.4x", format.encoding );
        return 0;
    }
    if( format.frequency > SND_OUTPUT_HZ )
    {
        DEBUGPRINTF( "Wave Frequency(%d) is too bot to play\r\n", format.frequency );
        return 0;
    }
    pWave = malloc( sizeof( WAVE ) );
    memset( pWave, 0, sizeof( WAVE ) );
    pWave->volume = DEFAULT_VOLUME; // maximum 255
    pWave->panpot = 8;
    pWave->channelNumber = -1;
    pWave->freq = format.frequency;
    pWave->channels = format.channels;
    int bitspersample = format.bitspersample;
    if( format.bitspersample == 16 )
    {
        if( format.channels == 2 )
            pWave->pcmtype = PCMTYPE_SS16;
        else
            pWave->pcmtype = PCMTYPE_SM16;

    }
    else if( format.bitspersample == 8 )
    {
        if( format.channels == 2 )
            pWave->pcmtype = PCMTYPE_US8;
        else
            pWave->pcmtype = PCMTYPE_UM8;
    }
    else
    {
        free( pWave );
        return 0;
    }
    pWave->byterate = format.frequency * format.channels * bitspersample / 8;
    pWave->headersize = 0;
    pWave->datasize = Length;

    pWave->samples = buf + nRead;
    pWave->bufsize = Length;

    pWave->PlaybackPitch = ( ( pWave->freq * 256 ) / SND_OUTPUT_HZ ) - 1;
    U32 sec = pWave->datasize / pWave->byterate ;
    U32 temp = pWave->datasize % pWave->byterate;
    pWave->duration_ms = ( sec * 1000 ) + ( ( temp * 1000 ) / pWave->byterate );
    DEBUGPRINTF( "freq : %d , %d(bits/smape), byterate:%d , channel : %d, data-size:%dkbyte, %d sec\r\n"
                 , pWave->freq, bitspersample, pWave->byterate, pWave->channels, pWave->datasize / 1024, sec );

    pWave->current_time = currnt_time_wav;

    return pWave;
}


WAVE* sound_loadwav( char* filename )
{
    WAVE* pWave;
    U32 nRead;
    FIL fp;
    if( f_open( &fp, filename, FA_READ | FA_OPEN_EXISTING ) != FR_OK )
    {
        DEBUGPRINTF( "Failed to open %s\r\n", filename );
        return 0;
    }

    pWave = SetWaveHeader( &fp );
    if( pWave == 0 )
    {
        f_close( &fp );
        return 0;
    }
    strcpy( pWave->filename, filename );

    if( pWave->datasize > WAVE_BUF_MAX )
    {
        // big-file, data will be loaded at play.
        pWave->bNeedReload = true;
        pWave->file = fp;
    }
    else
    {
        pWave->bNeedReload = false;
        pWave->bufsize = ( pWave->datasize + 0x1f ) & 0xffffffe0; //align at 0x20 boundry
        pWave->samples = ( U8* )malloc( pWave->bufsize );
        if( pWave->samples == 0 )
        {
            debugstring( "insufficient invalid Memory\r\n" );
            free( pWave );
            f_close( &fp );
            return 0;
        }
        pWave->bMallocedBuf = true;
        f_lseek( &fp, pWave->headersize );
        f_read( &fp, pWave->samples, pWave->bufsize, &nRead );
        f_close( &fp );
        U32 paddsize = pWave->bufsize - pWave->datasize ;
        if( paddsize > 0 )
        {
            memset( &pWave->samples[pWave->datasize], 0, paddsize );
        }
    }

    return pWave;
}

WAVE* sound_loadrawp( void* sampels, int freq, PCMTYPE type, int bytelen )
{
    WAVE* pWave ;
    pWave = malloc( sizeof( WAVE ) );
    if( pWave == 0 )
    {
        debugstring( "insufficient invalid Memory\r\n" );
        return 0;
    }
    memset( pWave, 0, sizeof( WAVE ) );
    pWave->volume = DEFAULT_VOLUME; // maximum 255
    pWave->panpot = 8;
    pWave->channelNumber = -1;
    pWave->freq = freq;
    pWave->channels = 1;
    pWave->datasize = bytelen;
    pWave->PlaybackPitch = ( ( pWave->freq * 256 ) / SND_OUTPUT_HZ ) - 1;
    pWave->samples = sampels;
    pWave->bufsize = bytelen;
    pWave->pcmtype  = type;
    return pWave;
}

BOOL sound_resume( WAVE* pWave )
{
    if( sound_ispause( pWave ) )
    {
        *R_SNDCON( pWave->channelNumber ) |= 1;
        pWave->state = WAVE_STATE_PLAY;
        return TRUE;
    }
    return FALSE;
}
BOOL sound_pause( WAVE* pWave )
{
    BOOL b = FALSE;
    if( pWave == 0 )
        return FALSE;
    if( pWave->channelNumber < 0 )
        return FALSE;
    CRITICAL_ENTER();
    if( pWave->state == WAVE_STATE_PLAY )
    {
        *R_SNDCON( pWave->channelNumber ) &= ~1; //disable
        pWave->state = WAVE_STATE_PAUSE;
        b = TRUE;
    }
    CRITICAL_EXIT();
    return FALSE;
}

BOOL sound_stop( WAVE* pWave )
{
    BOOL b = FALSE;
    if( pWave == 0 )
        return FALSE;
    if( pWave->channelNumber < 0 )
        return FALSE;
    if( pWave->channelNumber != -1 )
    {
        int i;
        CRITICAL_ENTER();
        for( i = 0; i < 8; i++ )
        {
            if( pWave == snd_dma_ch[i] )
            {
#if 1
                *R_DMAARCNT( i ) = 0;
                *R_DMACFG( i ) |= 1 << 20; //ignore DMA
                while( ( *R_DMACFG( i ) ) & ( 1 << 21 ) ); ///wait for empty
#endif
                *R_DMACFG( i ) = 0;
                set_free_ahb_dma( i );
                break;
            }
        }
        *R_SNDCON( pWave->channelNumber ) = 0;
        pWave->channelNumber = -1;
        pWave->state = WAVE_STATE_STOP;
		pWave->validsamples = 0;
		pWave->validsamples2 = 0;
        b = TRUE;
        release_snd_channel( i );
        CRITICAL_EXIT();
    }
    return b;
}

static BOOL sound_play_r( WAVE* pWave )
{
    int i;
    if( pWave == 0 )
        return FALSE;
    sound_stop( pWave );
    i = get_snd_channel();
    if( i == -1 )
    {
        return FALSE;
    }
    //DEBUGPRINTF("%d channel allocated\r\n",i);
    pWave->channelNumber = i;
    U32 conval = ( 1 << 28 ) | ( pWave->PlaybackPitch << 16 ) | ( 0 << 8 ) | ( pWave->pcmtype << 4 ) | 1;

    switch ( pWave->pcmtype )
    {
    case PCMTYPE_US8:
    case PCMTYPE_UM8:
        *R_SNDDAT( pWave->channelNumber ) = 0x80808080;
        break;
    case PCMTYPE_US16:
    case PCMTYPE_UM16:
        *R_SNDDAT( pWave->channelNumber ) = 0x80008000;
        break;
    default:
        break;
    }
    *R_SNDVOL( pWave->channelNumber ) = ( pWave->volume | pWave->volume << 8 ) | pWave->volume | pWave->volume << 8;
    *R_SNDCON( pWave->channelNumber ) = conval | ( 1 << 3 ); // DMA Mode

    if( pWave->preparetoplay != 0 ) //mp3?
    {
        if( pWave->preparetoplay( pWave ) == FALSE )
            return FALSE;
        if ( SetPlayDma( pWave ) )
        {
            pWave->state = WAVE_STATE_PLAY;
            return TRUE;
        }
    }
    else //wav
    {
        if( pWave->bNeedReload ) // from file system
        {
            if( bigfile_load( pWave ) == FALSE )
                return FALSE;
            if ( SetPlayDma( pWave ) )
            {
                pWave->state = WAVE_STATE_PLAY;
                return TRUE;
            }
        }
        else  // from memory
        {
            if ( pWave->transfered_len == 0 )
            {
                if ( SetPlayDma( pWave ) )
                {
                    pWave->state = WAVE_STATE_PLAY;
                    return TRUE;
                }
            }
            else
            {
                if ( SetPlayDmaEx( pWave
                                   , pWave->samples + pWave->transfered_len
                                   , pWave->bufsize - pWave->transfered_len ) )
                {
                    pWave->state = WAVE_STATE_PLAY;
                    return TRUE;
                }
            }
        }
    }

    *R_SNDCON( pWave->channelNumber ) = 0;
    return FALSE;
}
BOOL sound_play( WAVE* pWave )
{
    pWave->transfered_len = 0;
    return sound_play_r( pWave );
}

BOOL sound_playex( WAVE* pWave, U32 start_ms )
{
    U32 len = ( start_ms * pWave->byterate ) / 1000;
    pWave->transfered_len = len & ( ~0xf );
    return sound_play_r( pWave );
}

BOOL sound_ispause( WAVE* pWave )
{
    if( pWave == 0 )
        return FALSE;
    if( pWave->channelNumber < 0 )
        return FALSE;
    if( pWave->channelNumber > ( SOUND_MAX_CHANNEL - 1 ) ) //max channel
        return FALSE;
    CRITICAL_ENTER();
    if( pWave->state == WAVE_STATE_PAUSE )
    {
        CRITICAL_EXIT();
        return TRUE;
    }
    CRITICAL_EXIT();
    return FALSE;
}
BOOL sound_isplay( WAVE* pWave )
{
    if( pWave == 0 )
    {
        return FALSE;
    }
    if( pWave->channelNumber < 0 )
    {
        return FALSE;
    }
    if( pWave->channelNumber > ( SOUND_MAX_CHANNEL - 1 ) ) //max channel
    {
        return FALSE;
    }
    CRITICAL_ENTER();
    if( pWave->state == WAVE_STATE_PLAY )
    {
        CRITICAL_EXIT();
        return TRUE;
    }
    CRITICAL_EXIT();
    return FALSE;
}

extern void sound_clock_init();
BOOL sound_init()
{
    sound_clock_init();
    //*R_SNDOUT(0)    = 0x0000830a; // prescale_on,div4,512fs,48fs,i2s
    *R_SNDOUT( 0 )    = 0x00008701; // prescale_on,div8,256fs,32fs,i2s, 48Khz

    debugstring( "Sound initialized as I2S mode \r\n" );
    *( vU32* )( _SND_BASE_ + 0x80 + ( 0 * 0x10 ) ) = 1;
    return TRUE;
}

BOOL sound_release( WAVE* pWave )
{
    if( pWave )
    {
        if( pWave->bMallocedBuf )
        {
            if( pWave->samples )
                free( pWave->samples );

            if( pWave->samples2 )
                free( pWave->samples2 );
        }
        if( pWave->release != 0 )
            pWave->release( pWave->user );
        if( pWave->bNeedReload )
        {
            f_close( &pWave->file );
        }

        free( pWave );
        return TRUE;
    }
    return FALSE;
}

void sound_vol( U8 vol )
{
    *R_SNDVOLOUT( 0 ) = ( ( u32 )vol << 8 ) + ( vol );
}

void sound_vol_wav( WAVE* pWav, U8 vol )
{
    pWav->volume = vol;
}

U32 sound_current_time( WAVE* pWav )
{
    if ( pWav == NULL )
        return 0;
    return pWav->current_time( pWav );
}
