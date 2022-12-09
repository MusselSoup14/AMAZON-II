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
#include "etc_driver/AK4183.h"

//#define DEBUG_TC
#ifdef DEBUG_TC
#define TC_PRINTF DEBUGPRINTF
#else
#define TC_PRINTF(...)
#endif

#define AK4183_ADDR		0x92

#define	CMD_SLEEP		0x70
#define CMD_Driver_X	0x80
#define CMD_Driver_Y	0x90
#define CMD_Driver_YP	0xA0
#define CMD_Driver_XM	0xB0
#define	CMD_READ_X		0xC0
#define	CMD_READ_Y		0xD0
#define CMD_Z1			0xE0
#define CMD_Z2			0xF0

/*
* ak4183 control command
* BIT	Name	Function
*  7	 S	Start Bit 0: Sleep mode, 1: Accelerate and Axis
* 6-4	A2-A0	Channel Selection Bits
*  3	X1	Don't care
*  2	PD0	Power down
*  1	Mode	Resolution of A/D converter. 0: 12bit, 1: 8bit
*  0	X2	Don't care
*/

static U8 TwiStop( void )
{
    volatile U32 dtime;
    *R_TWICTRL = 0;
    *R_TWIDATA = 0xfe;
    for ( dtime = 0; dtime < 1000; dtime++ );
    *R_TWICTRL = F_TWICTRL_TWIEN | F_TWICTRL_TWIMOD_MSTART | F_TWICTRL_TWITR_MTRM | F_TWICTRL_TWIAK_NACK;
    for ( dtime = 0; dtime < 1000; dtime++ );

    return 0;
}

static char ak4183_cmd( U8 twi_data )
{
    twi_write2( AK4183_ADDR, twi_data, 0, 0 );
    return 0;
}

static int ak4183_read()
{
    U16 twi_data = 0;
    if( twi_wait_busy() == FALSE )
    {
        return -1;
    }

    *R_TWIDATA = AK4183_ADDR | 0x01;
    *R_TWICTRL = F_TWICTRL_TWIEN | F_TWICTRL_TWIMOD_MSTART;

    if ( twi_trans_complete() == FALSE )
    {
        TwiStop();
        return -1;
    }

    twi_data = *R_TWIDATA;
    if ( twi_trans_complete() == FALSE )
    {
        TwiStop();
        return -1;
    }

    twi_data = *R_TWIDATA;
    if ( twi_trans_complete() == FALSE )
    {
        TwiStop();
        return -1;
    }

    *R_TWICTRL |= F_TWICTRL_TWIAK_NACK;
    twi_data <<= 8;
    twi_data |= *R_TWIDATA;
    if ( twi_trans_complete() == FALSE )
    {
        TwiStop();
        return -1;
    }
    U8 tmp = *R_TWIDATA;    // dummy read

    *R_TWICTRL = F_TWICTRL_TWIEN;
    if ( FALSE == twi_wait_busy() )
    {
        return -1;
    }
    return twi_data >>= 4;

}

#define ADCVAL_X_MIN 120
#define ADCVAL_X_MAX 3950

#define ADCVAL_Y_MIN 150
#define ADCVAL_Y_MAX 3750

#define SKIP_DIF 50

#define SAMPLES 5

static int tc_avr( int* databuf, int cnt )
{
    int i;
    int x = 0;
    int avr;
    for ( i = 0; i < cnt; i++ )
    {
        x += databuf[i];
    }
    avr = x / cnt;
    //평균보다 오차가 큰 좌표는 버리고 다시 계산
    int validcnt = 0;
    x = 0;
    for ( i = 0; i < cnt; i++ )
    {
        if ( ( ( avr + SKIP_DIF ) > databuf[i] ) && ( ( avr - SKIP_DIF ) < databuf[i] ) )
        {
            x += databuf[i];
            validcnt++;
        }
    }
    if ( validcnt < cnt - 2 )
    {
        TC_PRINTF( "invalid avr \r\n" );
#ifdef DEBUG_TC
        for ( i = 0; i < cnt; i++ )
            PRINTVAR( databuf[i] );
#endif
        return -1;
    }
    avr = x / validcnt;
    return avr;
}
static inline int read_xpos( void )
{
    int i, j;
    int x_raw[SAMPLES];
    int tmp;
    memset( x_raw, 0, sizeof( unsigned int )*SAMPLES );

    ak4183_cmd( CMD_Driver_X );
    for ( i = 0, j = 0; i < SAMPLES; i++ )
    {
        tmp = ak4183_read();
        if ( ( tmp < ADCVAL_X_MIN ) || ( tmp > ADCVAL_X_MAX ) )
        {
            TC_PRINTF( "invalid x = %d\r\n", tmp );
            continue;
        }
        x_raw[j++] = tmp;
    }
    if ( j > SAMPLES - 2 )
    {
        return  tc_avr( x_raw, j );
    }
    TC_PRINTF( "invalid xpos \r\n" );
    return -1;
}
static inline int read_ypos( void )
{
    int i, j;
    int y_raw[SAMPLES];
    int tmp = 0;

    memset( y_raw, 0, sizeof( unsigned int )*SAMPLES );

    ak4183_cmd( CMD_Driver_Y );

    for ( i = 0, j = 0; i < SAMPLES; i++ )
    {
        tmp = ak4183_read();
        //debugprintf("max y = %d\r\n",tmp);
        if ( ( tmp < ADCVAL_Y_MIN ) || ( tmp > ADCVAL_Y_MAX ) )
        {
            TC_PRINTF( "invalid y = %d\r\n", tmp );
            continue;
        }
        y_raw[j++] = tmp;
    }
    if ( j > SAMPLES - 2 )
    {
        return  tc_avr( y_raw, j );
    }

    TC_PRINTF( "invalid ypos \r\n" );
    return -1;
}
static inline int TouchPress1()
{
    int z;
    ak4183_cmd( CMD_Z1 );
    z = ak4183_read();
    return z;
}
static inline int TouchPress2()
{
    int z;
    ak4183_cmd( CMD_Z2 );
    z = ak4183_read();
    return z;
}

#define SCREEN_W 800
#define SCREEN_H 480

inline void touch2screen( int touch_x, int touch_y, int* sx, int* sy )
{
    if ( touch_x < ADCVAL_X_MIN )
    {
        touch_x = ADCVAL_X_MIN;
    }
    else if ( touch_x > ADCVAL_X_MAX )
    {
        touch_x = ADCVAL_X_MAX;
    }

    *sx = ( touch_x - ADCVAL_X_MIN ) * SCREEN_W / ( ADCVAL_X_MAX - ADCVAL_X_MIN );

    if ( touch_y < ADCVAL_Y_MIN )
    {
        touch_y = ADCVAL_Y_MIN;
    }
    else if ( touch_y > ADCVAL_Y_MAX )
    {
        touch_y = ADCVAL_Y_MAX;
    }

    *sy = SCREEN_H - ( touch_y - ADCVAL_Y_MIN ) * SCREEN_H / ( ADCVAL_Y_MAX - ADCVAL_Y_MIN );
}


/**< Circular Buffer Types */
typedef unsigned char INT8U;
typedef int KeyType;
typedef struct
{
    int writePointer; /**< write pointer */
    int readPointer;  /**< read pointer */
    int size;         /**< size of circular buffer */
    KeyType keys[0];    /**< Element of circular buffer */
    KeyType lastkey;
} CircularBuffer;
CircularBuffer* KeyQue;

/**< Init Circular Buffer */
static CircularBuffer* CircularBufferInit( CircularBuffer** pQue, int size )
{
    int sz = size * sizeof( KeyType ) + sizeof( CircularBuffer );
    *pQue = ( CircularBuffer* )malloc( sz );
    if ( *pQue )
    {
        ( *pQue )->size = size;
        ( *pQue )->writePointer = 0;
        ( *pQue )->readPointer = 0;
    }
    return *pQue;
}

inline bool CircularBufferIsFull( CircularBuffer* que )
{
    return ( ( que->writePointer + 1 ) % que->size == que->readPointer );
}

inline bool CircularBufferIsEmpty( CircularBuffer* que )
{
    return ( que->readPointer == que->writePointer );
}

inline bool CircularBufferEnque( CircularBuffer* que, KeyType k )
{
    if ( CircularBufferIsFull( que ) == true )
        return false;
    CRITICAL_ENTER();
    que->lastkey = k;
    que->keys[que->writePointer] = k;
    que->writePointer++;
    que->writePointer %= que->size;
    CRITICAL_EXIT();
    return true;
}

inline bool CircularBufferDeque( CircularBuffer* que, KeyType* pK )
{
    if ( CircularBufferIsEmpty( que ) == true )
        return false;
    CRITICAL_ENTER();
    *pK = que->keys[que->readPointer];
    que->readPointer++;
    que->readPointer %= que->size;
    CRITICAL_EXIT();
    return true;
}


inline void CircularBufferClear( CircularBuffer* que )
{
    CRITICAL_ENTER();
    que->writePointer = 0;
    que->readPointer = 0;
    CRITICAL_EXIT();
}

bool get_touchxy_buffered( int* x, int* y )
{
    KeyType key;
    if ( CircularBufferDeque( KeyQue, &key ) == false )
        return false;
    *x = key >> 16;
    *y = key & 0xffff;
    return true;
}

static bool get_touchxy_cur( int* x, int* y )
{
    bool b = false;
    int level = *R_GPILEV(11);
    if ( ( level & ( 1 << 6 ) ) == 0 ) //check low level
    {
        int touch_x, touch_y;
        int z1;
        touch_x = read_xpos();
        touch_y = read_ypos();
        if ( ( touch_x != -1 ) && ( touch_y != -1 ) )
        {
            z1 = TouchPress1();
            if ( z1 > 5 )
            {
                touch2screen( touch_x, touch_y, x, y );
                b = true;
            }            
        }
    }

    return b;
}

static bool bKeyBuffered = false;
bool get_touchxy( int* x, int* y )
{
    if ( bKeyBuffered )
    {
        //only buffered last data
        if ( CircularBufferIsEmpty( KeyQue ) == true )
            return false;
        CRITICAL_ENTER();
        //clear queue
        *x = KeyQue->lastkey >> 16;
        *y = KeyQue->lastkey & 0xffff;
        CRITICAL_EXIT();
        CircularBufferClear( KeyQue );
        return true;
    }
    else
    {
        return get_touchxy_cur( x, y );
    }
}


static void get_tcdata()
{
    int touch_x, touch_y;
    int x, y;
    int z1;
    touch_x = read_xpos();
    touch_y = read_ypos();
    if ( touch_x == -1 )
    {
        return;
    }

    if ( touch_y == -1 )
    {
        return;
    }
    z1 = TouchPress1();
    if ( z1 < 10 )
    {
        return;
    }
    touch2screen( touch_x, touch_y, &x, &y );
    CircularBufferEnque( KeyQue, x << 16 | y );
}

//--------------------------------------------------------------
static void irq_isr( void )
{
#if 0
    while ( 1 )
    {
        int level = *R_GPILEV(11);
        if ( level & ( 1 << 6 ) )
            break;
        get_tcdata();
    }
#else
    int level = *R_GPILEV(11);
    if ( ( level & ( 1 << 6 ) ) == 0 ) //check low level
    {
        get_tcdata();
    }
#endif
	*R_GPEDS(11) = 1<<6;  // PIO 11.6 EDGE Detect status clear.
}

void touchinit()
{
    *R_PAF( 5 ) &= ~( 0xf << 8 );    	
    set_interrupt( INTNUM_GPIO11, irq_isr );	
    CircularBufferInit( &KeyQue, KEY_BUFFER_SIZE );
}

bool enable_touch_bufferd_mode( bool b )
{
    bool oldmode = bKeyBuffered;
    if ( b )
    {
        bKeyBuffered = true;
		*R_GPFED(11) |= 1<<6;  // PIO 11.6 Falling Edge Detect enable.
        enable_interrupt( INTNUM_GPIO11, TRUE );
    }
    else
    {
        bKeyBuffered = false;
        if ( KeyQue )
            CircularBufferClear( KeyQue );
        enable_interrupt( INTNUM_GPIO11, FALSE );
    }
    return oldmode;
}

