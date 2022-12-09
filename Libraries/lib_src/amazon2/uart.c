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

#ifdef CONFIG_UART_RX_INTERRUPT
//#warning "UART RX interrupt mode Build"
#endif
#ifdef CONFIG_UART_TX_INTERRUPT
//#warning "UART TX interrupt mode Build"
#endif

#if defined(CONFIG_UART_RX_INTERRUPT)||defined(CONFIG_UART_TX_INTERRUPT)
typedef struct _UART_RING_BUF
{

#ifdef CONFIG_UART_RX_INTERRUPT
    U8    *RingBufRxInPtr;  // Pointer to where next character will be inserted
    U8    *RingBufRxOutPtr; // Pointer from where next character will be extracted
    U8    RingBufRx[UART_BUF_SIZE]; // Ring buffer character storage (Rx)
    U16   RingBufRxCtr;     // Number of characters in the Rx ring buffer
#endif

#ifdef CONFIG_UART_TX_INTERRUPT
    U8    *RingBufTxInPtr;    // Pointer to where next character will be inserted
    U8    *RingBufTxOutPtr; // Pointer from where next character will be extracted
    U8    RingBufTx[UART_BUF_SIZE];  // Ring buffer character storage (Tx)
    U16   RingBufTxCtr;     // Number of characters in the Tx ring buffer
#endif
} UART_RING_BUF;

static UART_RING_BUF *ubuf[UART_CHANNEL_MAX];

static void InitRingBuf ( int ch )
{
    if ( ubuf[ch] == 0 )
    {
        ubuf[ch] = malloc ( sizeof ( UART_RING_BUF ) );
    }

    UART_RING_BUF *pRing = ubuf[ch];
#ifdef CONFIG_UART_RX_INTERRUPT
    pRing->RingBufRxCtr    = 0;
    pRing->RingBufRxInPtr  = &pRing->RingBufRx[0];
    pRing->RingBufRxOutPtr = &pRing->RingBufRx[0];
#endif
#ifdef CONFIG_UART_TX_INTERRUPT
    pRing->RingBufTxCtr    = 0;
    pRing->RingBufTxInPtr  = &pRing->RingBufTx[0];
    pRing->RingBufTxOutPtr = &pRing->RingBufTx[0];
#endif
}

static __inline__ UART_RING_BUF *getringbuf ( int ch )
{
    return ubuf[ch];
}

#endif //end defined(CONFIG_UART_RX_INTERRUPT)||defined(CONFIG_UART_TX_INTERRUPT)

static __inline__ U32 getbaseaddress ( int ch )
{
    return ( U32 ) R_UART_BASE ( ch );
}


void uart_rx_int_enable ( int ch, BOOL b )
{
    U32	uart_base = getbaseaddress ( ch );

    if ( b )
        * ( U32 * ) ( uart_base + UART_IER ) |= F_UIE_RDAIEN;
    else
        * ( U32 * ) ( uart_base + UART_IER ) &= ~F_UIE_RDAIEN;
}

void uart_tx_int_enable ( int ch, BOOL b )
{
    U32	uart_base = getbaseaddress ( ch );

    if ( b )
        * ( U32 * ) ( uart_base + UART_IER ) |= F_UIE_THEIEN;
    else
        * ( U32 * ) ( uart_base + UART_IER ) &= ~F_UIE_THEIEN;
}
void uart_status_int_enable ( int ch, BOOL b )
{
    U32	uart_base = getbaseaddress ( ch );

    if ( b )
        * ( U32 * ) ( uart_base + UART_IER ) |= F_UIE_RLSIEN;
    else
        * ( U32 * ) ( uart_base + UART_IER ) &= ~F_UIE_RLSIEN;
}

#if defined(CONFIG_UART_RX_INTERRUPT)||defined(CONFIG_UART_TX_INTERRUPT)
static void getuartfifo ( int channel, UART_RING_BUF *pbuf )
{
    U32 iir;
    char ch;
    U32 uart_base = getbaseaddress( channel );
    iir = * ( volatile U32 * ) ( uart_base + UART_IIR );
    iir &= 0xf;

    switch ( iir )
    {
#ifdef CONFIG_UART_TX_INTERRUPT

    case 2:
        *( volatile U32* )( uart_base + UART_IER ) &= ~F_UIE_THEIEN; //TX interrupt Disable
        if ( pbuf->RingBufTxCtr > 0 )
        {
            int i;
            int txmax = pbuf->RingBufTxCtr > UART_FIFO_DEPTH ? UART_FIFO_DEPTH : pbuf->RingBufTxCtr;

            for ( i = 0; i < txmax; i++ )
            {
                * ( volatile U8 * ) ( uart_base + UART_THR ) = *pbuf->RingBufTxOutPtr;
                pbuf->RingBufTxOutPtr++;

                if ( pbuf->RingBufTxOutPtr == &pbuf->RingBufTx[UART_BUF_SIZE] )
                {
                    pbuf->RingBufTxOutPtr = &pbuf->RingBufTx[0];
                }
            }

            pbuf->RingBufTxCtr -= i;
        }

        if ( pbuf->RingBufTxCtr > 0 )
            *( volatile U32* )( uart_base + UART_IER ) |= F_UIE_THEIEN; //TX interrupt Enable

        break;
#endif
#ifdef CONFIG_UART_RX_INTERRUPT

    case 6:
        debugstring ( "UART Line Status Error(Overrun,Frame,Parity)\r\n" );

    case 4:
    case 0xc:
        *( volatile U32* )( uart_base + UART_IER ) &= ~F_UIE_RDAIEN; //RX interrupt Disable
        while ( ( * ( volatile U32 * ) ( uart_base + UART_LSR ) ) & F_ULS_DRDY ) //data ready
        {
            ch = * ( volatile U8 * ) ( uart_base + UART_RBR );

            if ( pbuf->RingBufRxCtr < UART_BUF_SIZE )
            {
                pbuf->RingBufRxCtr++;
                *pbuf->RingBufRxInPtr = ch;
                pbuf->RingBufRxInPtr++;

                /* Wrap OUT pointer     */
                if ( pbuf->RingBufRxInPtr == &pbuf->RingBufRx[UART_BUF_SIZE] )
                {
                    pbuf->RingBufRxInPtr = &pbuf->RingBufRx[0];
                }
            }
        }

        *( volatile U32* )( uart_base + UART_IER ) |= F_UIE_RDAIEN;
        break;
#endif
    }
}

static void Uart0ISR()
{
    getuartfifo ( 0, ubuf[0] );
}

static void Uart1ISR()
{
    getuartfifo ( 1, ubuf[1] );
}

static void Uart2ISR()
{
    getuartfifo ( 2, ubuf[2] );
}

static void Uart3ISR()
{
    getuartfifo ( 3, ubuf[3] );
}
#endif
/**
 * UART configuration.
 *
 * \param ch	   Channel number.
 * \param baud	   The baudrate.
 * \param databits The databits.
 * \param stopbit  The stopbit.
 * \param parity   The parity.
 *
 * \return true if it succeeds, false if it fails.
 */
BOOL uart_config ( int ch, int baud, UART_DATABITS databits, UART_STOPBITS stopbit, UART_PARITY parity )
{
    U32 apbclock;
    volatile U32 uart_base;

    if ( ch >= UART_CHANNEL_MAX )
        return FALSE;

    uart_base = getbaseaddress ( ch );
#if defined(CONFIG_UART_RX_INTERRUPT)||defined(CONFIG_UART_TX_INTERRUPT)
    InitRingBuf ( ch );
#endif
    apbclock = get_apb_clock();
    U16 divisor = apbclock / ( baud * 16 ) ;

    if ( ( apbclock % ( baud * 16 ) ) > ( ( baud * 16 ) / 2 ) )
        divisor++;

    U32 lcr;//line control register
    lcr = ( ( stopbit - 1 ) << 2 ) + ( databits - 5 );

    switch ( parity )
    {
    case UART_PARODD:
        lcr |= F_ULC_PEN;
        break;

    case UART_PAREVEN:
        lcr |= F_ULC_PEN | F_ULC_EPS;
        break;

    case UART_PARNONE:
    default:
        break;
    }

    * ( volatile U32 * ) ( uart_base + UART_FCR ) = F_UFC_FIFOEN | F_UFC_RFTL_1;
    lcr |= F_ULC_DLAB;
    * ( volatile U32 * ) ( uart_base + UART_LCR ) = lcr;
    * ( volatile U32 * ) ( uart_base + UART_DIV_LO ) = divisor & 0xff;
    * ( volatile U32 * ) ( uart_base + UART_DIV_HI ) = ( divisor >> 8 ) & 0xff;
    lcr &= ~F_ULC_DLAB; //divisor latch access disable, fifo control write mode.
    * ( volatile U32 * ) ( uart_base + UART_LCR ) = lcr;
    * ( volatile U32 * ) ( uart_base + UART_IER ) = 0; //disable rx/tx interrupts
    uart_rx_flush ( ch );
    uart_tx_flush ( ch );
#if defined(CONFIG_UART_RX_INTERRUPT)||defined(CONFIG_UART_TX_INTERRUPT)
    switch ( ch )
    {
    case 0:
        *R_IINTMOD ( INTNUM_UART0 / 32 ) &= ~ ( 1 << ( INTNUM_UART0 - ( 32 * ( INTNUM_UART0 / 32 ) ) ) ); //for the first empty interrupt
        set_interrupt ( INTNUM_UART0, Uart0ISR );
        enable_interrupt ( INTNUM_UART0, TRUE );
        break;

    case 1:
        *R_IINTMOD ( INTNUM_UART1 / 32 ) &= ~ ( 1 << ( INTNUM_UART1 - ( 32 * ( INTNUM_UART1 / 32 ) ) ) );
        set_interrupt ( INTNUM_UART1, Uart1ISR );
        enable_interrupt ( INTNUM_UART1, TRUE );
        break;

    case 2:
        *R_IINTMOD ( INTNUM_UART2 / 32 ) &= ~ ( 1 << ( INTNUM_UART2 - ( 32 * ( INTNUM_UART2 / 32 ) ) ) );
        set_interrupt ( INTNUM_UART2, Uart2ISR );
        enable_interrupt ( INTNUM_UART2, TRUE );
        break;

    case 3:
        *R_IINTMOD ( INTNUM_UART3 / 32 ) &= ~ ( 1 << ( INTNUM_UART3 - ( 32 * ( INTNUM_UART3 / 32 ) ) ) );
        set_interrupt ( INTNUM_UART3, Uart3ISR );
        enable_interrupt ( INTNUM_UART3, TRUE );
        break;
    }

#endif
#ifdef CONFIG_UART_RX_INTERRUPT
    uart_rx_int_enable ( ch, TRUE );
#endif
    return TRUE;
}
/**
 * Clear UART Rx FIFO register.
 *
 * \param ch UART channel number.
 */
void uart_rx_flush ( int ch )
{
    U32 uart_base = getbaseaddress ( ch );
    CRITICAL_ENTER();
    * ( volatile U32 * ) ( uart_base + UART_LCR ) |= ( 1 << 7 ); //DLAB 1
    U32 val = *( volatile U32 * )( uart_base + UART_FCR );
    *( volatile U32 * )( uart_base + UART_LCR ) &= ~( 1 << 7 ); //DLAB 0
    *( volatile U32 * )( uart_base + UART_FCR ) = val | ( 1 << 1 ); //rx fifo reset
    CRITICAL_EXIT();
}
/**
 * Clear UART Tx FIFO register.
 *
 * \param ch UART channel number.
 */
void uart_tx_flush ( int ch )
{
    U32 uart_base = getbaseaddress ( ch );
    CRITICAL_ENTER();
    *( volatile U32 * ) ( uart_base + UART_LCR ) |= ( 1 << 7 ); //DLAB 1
    U32 val = *( volatile U32 * )( uart_base + UART_FCR );
    *( volatile U32 * )( uart_base + UART_LCR ) &= ~( 1 << 7 ); //DLAB 0
    *( volatile U32 * )( uart_base + UART_FCR ) = val | ( 1 << 2 ); //tx fifo reset
    CRITICAL_EXIT();
}
/**
 * Read a character from UART.
 *
 * \param n			  UART channel number.
 * \param [out] ch Storage location for character.
 *
 * \return true if it succeeds, false if it fails.
 */
BOOL uart_getch ( int n, char *ch )
{
#ifdef CONFIG_UART_RX_INTERRUPT
    UART_RING_BUF *pbuf = getringbuf ( n );

    if ( pbuf == 0 )
        return FALSE;

	CRITICAL_ENTER();
	if (pbuf->RingBufRxCtr > 0) /* No, decrement character count */
    {
        pbuf->RingBufRxCtr--; /* No, decrement character count */
        *ch = *pbuf->RingBufRxOutPtr++; /* Get character from buffer */

        /* Wrap OUT pointer     */
        if ( pbuf->RingBufRxOutPtr == &pbuf->RingBufRx[UART_BUF_SIZE] )
        {
            pbuf->RingBufRxOutPtr = &pbuf->RingBufRx[0];
        }
		CRITICAL_EXIT();
        return TRUE;
    }
	CRITICAL_EXIT();
	return FALSE;

#else
    U32 uart_base = getbaseaddress ( n );

    if ( uart_base == 0 )
        return FALSE;

    U32 lsr = * ( U32 * ) ( uart_base + UART_LSR );

    if ( lsr & F_ULS_DRDY )
    {
        *ch = * ( u8 * ) ( uart_base + UART_RBR );
        return TRUE;
    }
    else
        return FALSE;

#endif
}
/**
 * Reads data from UART
 *
 * \param n			   UART channel number.
 * \param [out] buf Storage location for data.
 * \param bufmax	   Maximum number of bytes.
 *
 * \return the number of bytes read.
 */
int uart_getdata ( int n, U8 *buf, int bufmax )
{
    int i;
#ifdef CONFIG_UART_RX_INTERRUPT
    UART_RING_BUF *pbuf = getringbuf ( n );

    if ( pbuf == 0 )
        return FALSE;

    for ( i = 0; i < bufmax; i++ )
    {
        CRITICAL_ENTER();
        if ( pbuf->RingBufRxCtr > 0 )
        {
            pbuf->RingBufRxCtr--;
            buf[i] = *pbuf->RingBufRxOutPtr++; /* Get character from buffer */

            /* Wrap OUT pointer     */
            if ( pbuf->RingBufRxOutPtr == &pbuf->RingBufRx[UART_BUF_SIZE] )
            {
                pbuf->RingBufRxOutPtr = &pbuf->RingBufRx[0];
            }
        }
        else
        {
            CRITICAL_EXIT();
            break;
        }
        CRITICAL_EXIT();
    }

#else
    U32 uart_base = getbaseaddress ( n );

    if ( uart_base == 0 )
        return FALSE;

    for ( i = 0; i < bufmax; i++ )
    {
        U32 lsr = * ( U32 * ) ( uart_base + UART_LSR );

        if ( lsr & F_ULS_DRDY )
        {
            buf[i] = * ( u8 * ) ( uart_base + UART_RBR );
        }
        else
            break;
    }

#endif
    return i;
}
/**
 * Writes a character.
 *
 * \param n  UART Channel Number.
 * \param ch Character to be output.
 *
 * \return true if it succeeds, false if it fails.
 */
BOOL uart_putch ( int n, char ch )
{
    U32 uart_base = getbaseaddress ( n );

    if ( uart_base == 0 )
        return FALSE;

#ifdef CONFIG_UART_TX_INTERRUPT
    UART_RING_BUF *pbuf = getringbuf ( n );

    if ( pbuf == 0 )
        return FALSE;

    CRITICAL_ENTER();

    if ( pbuf->RingBufTxCtr < UART_BUF_SIZE )
    {
        pbuf->RingBufTxCtr++;    /* No, increment character count*/
        *pbuf->RingBufTxInPtr = ch;    /* Put character into buffer*/
        pbuf->RingBufTxInPtr++;

        if ( pbuf->RingBufTxInPtr == &pbuf->RingBufTx[UART_BUF_SIZE] )
        {
            pbuf->RingBufTxInPtr = &pbuf->RingBufTx[0];
        }

        if ( ( * ( U32 * ) ( uart_base + UART_IER ) & F_UIE_THEIEN ) == 0 )
        {
            uart_tx_int_enable ( n, TRUE );
        }

        CRITICAL_EXIT();
        return TRUE;
    }
    else
    {
        // buffer full, wait until the previous data is out
        *( volatile U32* )( uart_base + UART_IER ) &= ~F_UIE_THEIEN; //TX interrupt Disable
        if ( pbuf->RingBufTxCtr > 0 )
        {
            int i;
            int txmax = pbuf->RingBufTxCtr > UART_FIFO_DEPTH ? UART_FIFO_DEPTH : pbuf->RingBufTxCtr;

            for ( i = 0; i < txmax; i++ )
            {
                *( volatile U8 * )( uart_base + UART_THR ) = *pbuf->RingBufTxOutPtr;
                pbuf->RingBufTxOutPtr++;

                if ( pbuf->RingBufTxOutPtr == &pbuf->RingBufTx[UART_BUF_SIZE] )
                {
                    pbuf->RingBufTxOutPtr = &pbuf->RingBufTx[0];
                }
            }

            pbuf->RingBufTxCtr -= i;
        }

        if ( pbuf->RingBufTxCtr > 0 )
            *( volatile U32* )( uart_base + UART_IER ) |= F_UIE_THEIEN; //TX interrupt Enable
        CRITICAL_EXIT();
        return FALSE;
    }

#else

    //wait empty
    while ( ! ( ( * ( volatile U32 * ) ( uart_base + UART_LSR ) )  & F_ULS_TEMP ) );

    *( volatile u8* )( uart_base + UART_THR ) = ch;
    return TRUE;
#endif
}
/**
 * Writes data to a file UART.
 *
 * \param n			   UART Number.
 * \param [in] buf Data to be written.
 * \param len		   Number of bytes.
 *
 * \return true if it succeeds, false if it fails.
 */
BOOL uart_putdata ( int n, U8 *buf, int len )
{
    int i;

    for ( i = 0; i < len; i++ )
    {
        while ( 1 )
        {
            if ( uart_putch( n, buf[i] ) == TRUE )
                break;
        }
    }
    return TRUE;
}
/**
 * Writes a string to UART.
 *
 * \param n			   UART Number.
 * \param [in] str Pointer to a null-terminated string.
 *
 * \return true if it succeeds, false if it fails.
 */
BOOL uart_putstring ( int n, U8 *str )
{
    while ( *str )
    {
        while ( 1 )
        {
            if ( uart_putch( n, *str ) == TRUE )
                break;
        }

        str++;
    }
    return TRUE;
}

BOOL uart_putstring ( int n, U8 *buf );

static int debugch = DEBUG_CHANNEL;
/**
 * Sets UART Number for debug output.
 *
 * \param ch UART Number(UART).
 */
void set_debug_channel ( int ch )
{
    if ( ch < UART_CHANNEL_MAX )
        debugch = ch;
}

/**
 * Gets debug channel.
 *
 * \return The debug channel.
 */
int get_debug_channel()
{
    return debugch;
}
/**
 * Writes a string to UART(debug channel).
 *
 * \param str Output string.
 *
 * Although CONFIG_UART_TX_INTERRUPT is defined, this function works as the polling mode.
 */
void debugstring ( const char *str )
{
    U32 uart_base = getbaseaddress( debugch );

    if ( uart_base == 0 )
        return ;
    while ( *str )
    {
        //wait empty
        while ( !( ( *( volatile U32 * )( uart_base + UART_LSR ) )  & F_ULS_TEMP ) );
        *( volatile u8* )( uart_base + UART_THR ) = *str;
        str++;
    }
}
/**
 * Prints formatted output to UART(debug channel) with file-name and line number.
 *
 * \param filename Pointer to a null-terminated string.
 * \param linenum  The linenum.
 * \param format	   Format control.
 *
 * Although CONFIG_UART_TX_INTERRUPT is defined, this function works as the polling mode.
 */
void r_debugprintf( const char* filename, int linenum, const char *format,	... )
{
    char print_buf[512];
    va_list args;

    sprintf( print_buf, "(%s:%d) ", filename, linenum );
    debugstring( print_buf );
    if	( format )
    {
        va_start ( args, format );
        vsnprintf ( print_buf, sizeof ( print_buf ), format, args );
        va_end ( args );
    }
    debugstring( print_buf );
}
/**
 * Prints formatted output to UART(debug channel).
 *
 * \param format Format control.
 * \see set_debug_channel
 *
 * Although CONFIG_UART_TX_INTERRUPT is defined, this function works as the polling mode.
 */
void debugprintf ( const char *format,	... )
{
    char print_buf[512];
    va_list args;

    if	( format )
    {
        va_start ( args, format );
        vsnprintf ( print_buf, sizeof ( print_buf ), format, args );
        va_end ( args );
    }

    debugstring ( print_buf );
}

