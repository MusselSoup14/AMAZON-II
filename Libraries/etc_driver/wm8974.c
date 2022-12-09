#include "sdk.h"
#include "etc_driver/wm8974.h"

typedef struct _tag_addr_data
{
    U32 addr;
    U32 data;
} WM8974_REG_DATA;


void wm8974_init( void )
{



    wm8974_write( R_WM8974_RESET, 0 );

    // Power On
    wm8974_write( R_WM8974_PM1, F_WM8974_PM1_BUFDCOPEN( 0 ) |
                  F_WM8974_PM1_AUXEN( 0 ) | F_WM8974_PM1_PLLEN( 0 ) |
                  F_WM8974_PM1_MICBEN( 1 ) | F_WM8974_PM1_BIASEN( 1 ) |
                  F_WM8974_PM1_BUFIOEN( 1 ) | F_WM8974_PM1_VMIDSEL( 1 ) );

    // Gain Hold Time
    delayms( 10 );

    wm8974_write( R_WM8974_PM2,
                  F_WM8974_PM2_BOOSTEN( 0 ) |	// boost stage enable
                  F_WM8974_PM2_INPPGAEN( 0 ) |	// input microphone pga enable
                  F_WM8974_PM2_ADCEN( 0 ) );	// adc enable

    wm8974_write( R_WM8974_PM3,
                  F_WM8974_PM3_MONOEN( 0 ) |	// mono disable
                  F_WM8974_PM3_SPKNEN( 0 ) |	// speaker n enable
                  F_WM8974_PM3_SPKPEN( 0 ) |	// speaker p enable
                  F_WM8974_PM3_MONOMIXEN( 0 ) |	// mono mixer disable
                  F_WM8974_PM3_SPKMIXEN( 0 ) |	// speaker mixer disable
                  F_WM8974_PM3_DACEN( 0 ) );	// dac enable

    wm8974_write( R_WM8974_AI,
                  F_WM8974_AI_BCP( 0 ) |
                  F_WM8974_AI_FRAMEP( 0 ) |
                  F_WM8974_AI_WL( 0 ) |	// 16bits
                  F_WM8974_AI_FMT( 2 ) |	// I2S
                  F_WM8974_AI_DACLRSWAP( 0 ) |
                  F_WM8974_AI_ADCLRSWAP( 0 ) );

    wm8974_write( R_WM8974_CPC,
                  F_WM8974_CPC_DAC_COMP( 0 ) |	// companding off
                  F_WM8974_CPC_ADC_COMP( 0 ) |
                  F_WM8974_CPC_LOOPBACK( 0 ) );	// loopback disable

    wm8974_write( R_WM8974_CGC,
                  F_WM8974_CGC_CLKSEL( 0 ) |	// MCLK
                  F_WM8974_CGC_MCLKDIV( 0 ) |	// divide by 1
                  F_WM8974_CGC_BCLKDIV( 0 ) |	// divide by 1
                  F_WM8974_CGC_MS( 0 ) );	// input(slave mode)

    wm8974_write( R_WM8974_ADD, F_WM8974_ADD_SR_8K );	// sample rate 8kHz

    wm8974_write( R_WM8974_GPIO,
                  F_WM8974_GPIO_OPCLKDIV( 0 ) |
                  F_WM8974_GPIO_POL( 0 ) |
                  F_WM8974_GPIO_SEL( 0 ) );

    wm8974_write( R_WM8974_DACC,
                  F_WM8974_DACC_DACMU( 0 ) |
                  F_WM8974_DACC_DEEMPH( 0 ) |
                  F_WM8974_DACC_DACOSR128( 1 ) |
                  F_WM8974_DACC_AMUTE( 0 ) |
                  F_WM8974_DACC_DACPOL( 0 ) );

    wm8974_write( R_WM8974_DACVOL, F_WM8974_DACVOL( 255 ) );

    wm8974_write( R_WM8974_ADCC,
                  F_WM8974_ADCC_HPFEN( 1 ) |
                  F_WM8974_ADCC_HPFAPP( 0 ) |
                  F_WM8974_ADCC_HPFCUT( 0 ) |
                  F_WM8974_ADCC_ADCOSR128( 1 ) |
                  F_WM8974_ADCC_ADCPOL( 0 ) );

    wm8974_write( R_WM8974_ADCVOL, F_WM8974_ADCVOL( 255 ) );

    wm8974_write( R_WM8974_DACLM1,
                  F_WM8974_DACLM1_EN( 0 ) |
                  F_WM8974_DACLM1_DCY( 3 ) |
                  F_WM8974_DACLM1_ATK( 2 ) );

    wm8974_write( R_WM8974_DACLM2,
                  F_WM8974_DACLM2_LVL( 0 ) |
                  F_WM8974_DACLM2_BOOST( 0 ) );

    wm8974_write( R_WM8974_ALC1,
                  F_WM8974_ALC1_SEL( 0 ) |
                  F_WM8974_ALC1_MAX( 7 ) |
                  F_WM8974_ALC1_MIN( 0 ) );

    wm8974_write( R_WM8974_ALC2,
                  F_WM8974_ALC2_ZC( 0 ) |
                  F_WM8974_ALC2_HLD( 0 ) |
                  F_WM8974_ALC2_LVL( 0xB ) );

    wm8974_write( R_WM8974_ALC3,
                  F_WM8974_ALC3_MODE( 0 ) |
                  F_WM8974_ALC3_DCY( 3 ) |
                  F_WM8974_ALC3_ATK( 2 ) );

    wm8974_write( R_WM8974_NGATE,
                  F_WM8974_NGATE_NGEN( 0 ) |
                  F_WM8974_NGATE_NGTH( 0 ) );

    wm8974_write( R_WM8974_PLLN,
                  F_WM8974_PLLN_PRESCALE( 0 ) |
                  F_WM8974_PLLN( 8 ) );

    wm8974_write( R_WM8974_ATTEN,
                  F_WM8974_ATTEN_MONO( 0 ) |
                  F_WM8974_ATTEN_SPK( 0 ) );

    wm8974_write( R_WM8974_IP,
                  F_WM8974_IP_MBVSEL( 1 ) |
                  F_WM8974_IP_AUXMODE( 0 ) |
                  F_WM8974_IP_AUX2( 0 ) |
                  F_WM8974_IP_MICN2( 1 ) |
                  F_WM8974_IP_MICP2( 1 ) );

    wm8974_write( R_WM8974_INPPGA,
                  F_WM8974_INPPGA_AZC( 0 ) |
                  F_WM8974_INPPGA_MUTE( 0 ) |
                  F_WM8974_INPPGA_VOL( 0x20 ) );

    wm8974_write( R_WM8974_ADCBST,
                  F_WM8974_ADCBST_PGA( 1 ) |
                  F_WM8974_ADCBST_MICP2( 0 ) |
                  F_WM8974_ADCBST_AUX2( 0 ) );

    wm8974_write( R_WM8974_OP,
                  F_WM8974_OP_MONOBST( 0 ) |
                  F_WM8974_OP_SPKBST( 1 ) |
                  F_WM8974_OP_TSDEN( 0 ) |
                  F_WM8974_OP_VROI( 0 ) );

    wm8974_write( R_WM8974_SPKMIX,
                  F_WM8974_SPKMIX_AUX2( 0 ) |
                  F_WM8974_SPKMIX_BYP2( 0 ) |
                  F_WM8974_SPKMIX_DAC2( 1 ) );

    wm8974_write( R_WM8974_SPKVOL,
                  F_WM8974_SPKVOL_ZC( 0 ) |
                  F_WM8974_SPKVOL_MUTE( 0 ) |
                  F_WM8974_SPKVOL_VOL( 0x3F ) );

    wm8974_write( R_WM8974_MONOMIX,
                  F_WM8974_MONOMIX_MUTE( 0 ) |
                  F_WM8974_MONOMIX_AUX2( 0 ) |
                  F_WM8974_MONOMIX_BYP2( 0 ) |
                  F_WM8974_MONOMIX_DAC2( 1 ) );
}


void WM8974_init_bypass( void )
{
    int i = 0;

    WM8974_REG_DATA data[] =
    {
        { R_WM8974_RESET, 0 },

        {
            R_WM8974_PM1,  // R1
            F_WM8974_PM1_BUFDCOPEN( 1 )	|
            F_WM8974_PM1_AUXEN( 0 )		|
            F_WM8974_PM1_PLLEN( 0 )		|
            F_WM8974_PM1_MICBEN( 1 )		|
            F_WM8974_PM1_BIASEN( 1 )		|
            F_WM8974_PM1_BUFIOEN( 1 )		|
            F_WM8974_PM1_VMIDSEL( 1 )
        },

        {
            R_WM8974_PM2,  // R2
            F_WM8974_PM2_BOOSTEN( 1 )		|	// boost stage enable
            F_WM8974_PM2_INPPGAEN( 1 )	|	// input microphone pga enable
            F_WM8974_PM2_ADCEN( 1 )
        },	// adc enable

        {
            R_WM8974_PM3,  // R3
            F_WM8974_PM3_MONOEN( 0 )		|	// mono disable
            F_WM8974_PM3_SPKNEN( 1 )		|	// speaker n enable
            F_WM8974_PM3_SPKPEN( 1 )		|	// speaker p enable
            F_WM8974_PM3_MONOMIXEN( 0 )	|	// mono mixer disable
            F_WM8974_PM3_SPKMIXEN( 1 )	|	// speaker mixer disable
            F_WM8974_PM3_DACEN( 0 )
        },	// dac enable

        {
            R_WM8974_PM2, // R2
            F_WM8974_PM2_BOOSTEN( 1 )		|	// boost stage enable
            F_WM8974_PM2_INPPGAEN( 1 )	|	// input microphone pga enable
            F_WM8974_PM2_ADCEN( 1 )
        },	// adc enable


        {
            R_WM8974_AI,  // R4
            F_WM8974_AI_BCP( 0 )			|
            F_WM8974_AI_FRAMEP( 0 )		|
            F_WM8974_AI_WL( 0 )			|	// 16bits
            F_WM8974_AI_FMT( 2 )			|	// I2S
            F_WM8974_AI_DACLRSWAP( 0 )	|
            F_WM8974_AI_ADCLRSWAP( 0 )
        },
        {
            R_WM8974_ADD,	// R7
            F_WM8974_ADD_SR_8K
        },	// sample rate 8kHz

        {
            R_WM8974_CPC,	// R5
            F_WM8974_CPC_DAC_COMP( 0 )	|	// companding off
            F_WM8974_CPC_ADC_COMP( 0 )	|
            F_WM8974_CPC_LOOPBACK( 0 )
        },	// loopback disable

        {
            R_WM8974_CGC,	// R6
            F_WM8974_CGC_CLKSEL( 0 )		|	// MCLK
            F_WM8974_CGC_MCLKDIV( 0 )		|	// divide by 1
            F_WM8974_CGC_BCLKDIV( 0 )		|	// divide by 1
            F_WM8974_CGC_MS( 0 )
        },	// input(slave mode)

// INPUT

        {
            R_WM8974_IP, 	// 44
            F_WM8974_IP_MBVSEL( 0 )		|
            F_WM8974_IP_AUXMODE( 0 )		|
            F_WM8974_IP_AUX2( 0 )			|
            F_WM8974_IP_MICN2( 1 )		|
            F_WM8974_IP_MICP2( 1 )
        },

        {
            R_WM8974_INPPGA,	//45
            F_WM8974_INPPGA_AZC( 0 )		|
            F_WM8974_INPPGA_MUTE( 0 )		|
            F_WM8974_INPPGA_VOL( 0x0 )
        },

        {
            R_WM8974_ADCBST,	// 47
            F_WM8974_ADCBST_PGA( 1 )		|
            F_WM8974_ADCBST_MICP2( 0 )	|
            F_WM8974_ADCBST_AUX2( 0 )
        },

        {
            R_WM8974_ADCC,		//14
            F_WM8974_ADCC_HPFEN( 0 )		|
            F_WM8974_ADCC_HPFAPP( 0 )		|
            F_WM8974_ADCC_HPFCUT( 0 )		|
            F_WM8974_ADCC_ADCOSR128( 0 )	|
            F_WM8974_ADCC_ADCPOL( 0 )
        },

        {R_WM8974_ADCVOL, F_WM8974_ADCVOL( 200 ) }, // 0xF

        {
            R_WM8974_DACC,		// 10
            F_WM8974_DACC_DACMU( 0 )		|
            F_WM8974_DACC_DEEMPH( 0 )		|
            F_WM8974_DACC_DACOSR128( 1 )	|
            F_WM8974_DACC_AMUTE( 0 )		|
            F_WM8974_DACC_DACPOL( 0 )
        },

        {
            R_WM8974_DACVOL,	// 11
            F_WM8974_DACVOL( 210 )
        },

        {
            R_WM8974_OP,	// 49
            F_WM8974_OP_MONOBST( 0 )		|
            F_WM8974_OP_SPKBST( 1 )		|
            F_WM8974_OP_TSDEN( 0 )		|
            F_WM8974_OP_VROI( 0 )
        },

        {
            R_WM8974_SPKMIX,	// 50
            F_WM8974_SPKMIX_AUX2( 0 )		|
            F_WM8974_SPKMIX_BYP2( 0 )		|
            F_WM8974_SPKMIX_DAC2( 1 )
        },

        {
            R_WM8974_SPKVOL,	// 54
            F_WM8974_SPKVOL_ZC( 0 )		|
            F_WM8974_SPKVOL_MUTE( 0 )		|
            F_WM8974_SPKVOL_VOL( 0x3F )
        },
        { -1, -1},
    };
    while ( 1 )
    {
        if ( data[i].addr == -1 )
            break;
        wm8974_write( data[i].addr, data[i].data );
    }

}


void wm8974_mic_on( void )
{
    wm8974_write( R_WM8974_PM2,
                  F_WM8974_PM2_BOOSTEN( 1 ) |	// boost stage enable
                  F_WM8974_PM2_INPPGAEN( 1 ) |	// input microphone pga enable
                  F_WM8974_PM2_ADCEN( 1 ) );	// adc enable
}

void wm8974_mic_off( void )
{
    wm8974_write( R_WM8974_PM2,
                  F_WM8974_PM2_BOOSTEN( 0 ) |	// boost stage enable
                  F_WM8974_PM2_INPPGAEN( 0 ) |	// input microphone pga enable
                  F_WM8974_PM2_ADCEN( 0 ) );	// adc enable
}

void wm8974_speacker_on( void )
{
    wm8974_write( R_WM8974_PM3,
                  F_WM8974_PM3_MONOEN( 0 ) |	// mono disable
                  F_WM8974_PM3_SPKNEN( 1 ) |	// speaker n enable
                  F_WM8974_PM3_SPKPEN( 1 ) |	// speaker p enable
                  F_WM8974_PM3_MONOMIXEN( 0 ) |	// mono mixer disable
                  F_WM8974_PM3_SPKMIXEN( 1 ) |	// speaker mixer disable
                  F_WM8974_PM3_DACEN( 1 ) );	// dac enable
}

void wm8974_speacker_off( void )
{
    wm8974_write( R_WM8974_PM3,
                  F_WM8974_PM3_MONOEN( 0 ) |	// mono disable
                  F_WM8974_PM3_SPKNEN( 0 ) |	// speaker n enable
                  F_WM8974_PM3_SPKPEN( 0 ) |	// speaker p enable
                  F_WM8974_PM3_MONOMIXEN( 0 ) |	// mono mixer disable
                  F_WM8974_PM3_SPKMIXEN( 0 ) |	// speaker mixer disable
                  F_WM8974_PM3_DACEN( 0 ) );	// dac enable
}

void wm8974_mono_on( void )
{
    wm8974_write( R_WM8974_PM3,
                  F_WM8974_PM3_MONOEN( 1 ) |	// mono disable
                  F_WM8974_PM3_SPKNEN( 0 ) |	// speaker n enable
                  F_WM8974_PM3_SPKPEN( 0 ) |	// speaker p enable
                  F_WM8974_PM3_MONOMIXEN( 1 ) |	// mono mixer disable
                  F_WM8974_PM3_SPKMIXEN( 0 ) |	// speaker mixer disable
                  F_WM8974_PM3_DACEN( 1 ) );	// dac enable
}

void wm8974_mono_off( void )
{
    wm8974_write( R_WM8974_PM3,
                  F_WM8974_PM3_MONOEN( 0 ) |	// mono disable
                  F_WM8974_PM3_SPKNEN( 0 ) |	// speaker n enable
                  F_WM8974_PM3_SPKPEN( 0 ) |	// speaker p enable
                  F_WM8974_PM3_MONOMIXEN( 0 ) |	// mono mixer disable
                  F_WM8974_PM3_SPKMIXEN( 0 ) |	// speaker mixer disable
                  F_WM8974_PM3_DACEN( 0 ) );	// dac enable
}


void wm8974_set_spk_vol( U8 vol )
{
    wm8974_write( R_WM8974_DACVOL,
                  F_WM8974_DACVOL( vol ) );
}

#define SPI_CS1_LOW()					*R_GPOLOW(13) = 1<<7;
#define SPI_CS1_HIGH()					*R_GPOHIGH(13) = 1<<7;
#define EVM_SPI_CODEC_CLK_HIGH()		*R_GPOHIGH(13) = 1<<6;
#define EVM_SPI_CODEC_CLK_LOW()		*R_GPOLOW(13) = 1<<6;
#define EVM_SPI_CODEC_DATA_HIGH()	*R_GPOHIGH(13) = 1<<5;
#define EVM_SPI_CODEC_DATA_LOW()	*R_GPOLOW(13) = 1<<5;

void wm8974_write( U32 reg, U32 data )
{
    U16 wdata;
    U32 i;
    volatile U32 dtime;

    wdata = ( U16 )( ( reg << 9 ) | ( data & 0x1FF ) );
#if 0
    debugprintf( "reg = 0x%x reg<<9 = 0x%x \r\n", reg, reg << 9 );
    debugprintf( "data = 0x%x data&0x1FF = 0x%x \r\n", data, data & 0x1FF );
    debugprintf( "wdata = 0x%x \r\n", wdata );
    debugstring( "\r\n\r\n" );
#endif
    // SPI_CS1_HIGH();
    SPI_CS1_LOW();
    EVM_SPI_CODEC_CLK_HIGH();
    delayms( 10 );

    for( i = 0; i < 16; i++ )
    {
        EVM_SPI_CODEC_CLK_LOW();

        if( ( wdata << i ) & 0x8000 )
        {
            EVM_SPI_CODEC_DATA_HIGH();
        }
        else
        {
            EVM_SPI_CODEC_DATA_LOW();
        }
        for( dtime = 0; dtime < 10000; dtime++ );

        EVM_SPI_CODEC_CLK_HIGH();
        for( dtime = 0; dtime < 10000; dtime++ );
    }

    EVM_SPI_CODEC_CLK_LOW();
    for( dtime = 0; dtime < 10000 / 2; dtime++ );

    SPI_CS1_HIGH();
    for( dtime = 0; dtime < 10000 / 4; dtime++ );
    EVM_SPI_CODEC_CLK_HIGH();
    delayms( 10 );

}
