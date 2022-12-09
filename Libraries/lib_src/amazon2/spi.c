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

static void inline spi_wait_finish ( int ch )
{
    U8 stat;
    do
    {
        stat = *R_SPISTAT( ch );
    }
    while ( !( stat & SPISTAT_SPIF ) );
}

void spi_master_init( int ch )
{
    *R_SPICON( ch ) = SPICON_MSTR | SPICON_CPOL | SPICON_CPHA | SPICON_MSBF | SPICON_EN;
}

/*
* mode: SPI_MASTER, SPI_SLAVE
* freq: Frequency
*/
int spi_set_freq ( int ch, int mode, U32 freq )
{
    U8 baudrate;

    if ( ( mode == SPI_MASTER ) && ( freq > SPI_MASTER_MAXFREQ ) )
        return -1;
    else if ( ( mode == SPI_SLAVE ) && ( freq > SPI_SLAVE_MAXFREQ ) )
        return -1;

    baudrate = ( get_apb_clock() / ( freq << 1 ) ) - 1;

    *R_SPIBAUD( ch ) = baudrate;

    return baudrate;
}

void spi_master_xfer( int ch, U8 *wbuf, int wlength, U8 *rbuf,
                      int rlength, int continue_xfer )
{
    int i;
    int rcnt = 0, rlen;
    int wcnt = 0, wlen;
    U8 flush;

    if ( wlength > SPI_MAX_WFIFO )
        wlen = SPI_MAX_WFIFO;
    else
        wlen = wlength;

    if ( rlength > SPI_MAX_RFIFO )
        rlen = SPI_MAX_RFIFO;
    else
        rlen = rlength;

    SPI_CS_LOW();

    while ( wcnt < wlength )
    {
        for ( i = 0; i < wlen; )
        {
            *R_SPIDATA( ch ) = wbuf[i];
            i++;
        }
        spi_wait_finish( ch );

        for ( i = 0; i < wlen; i++ )
            flush = *R_SPIDATA( ch ); // flush garbage

        wbuf += wlen;
        wcnt += wlen;
        if ( ( wlength - wcnt ) < SPI_MAX_WFIFO )
            wlen = wlength - wcnt;
    }

    while ( rcnt < rlength )
    {
        for ( i = 0; i < rlen; )
        {
            *R_SPIDATA( ch ) = 0xaa; //write dummy for clock generation
            spi_wait_finish( ch );
            flush = *R_SPIDATA( ch );
            rbuf[i] = flush;
            i++;
        }

        rbuf += rlen;
        rcnt += rlen;
        if ( ( rlength - rcnt ) < SPI_MAX_RFIFO )
            rlen = rlength - rcnt;
    }

    if ( continue_xfer == 0 )
        SPI_CS_HIGH();
}
