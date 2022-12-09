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
#include "typedef.h"
#include "lib_config.h"
#include "amazon2/twi.h"
#include "amazon2/pmu.h"
#include "amazon2/uart.h"

int twi_set_freq ( U32 freq )
{
	unsigned long long tmp;
	U32 twibr0_cnt, twibr1_cnt;

	tmp = ( unsigned long long )get_apb_clock() * 700 / 1000000000;
	twibr0_cnt = tmp + 3;
	twibr1_cnt = ( get_apb_clock() / ( 2 * freq ) ) - ( ( twibr0_cnt + 7 ) >> 1 );

	*R_TWIBR0 = twibr0_cnt;
	*R_TWIBR1 = twibr1_cnt;
	return 0;
}

BOOL twi_trans_complete( void )
{
	U32 status;
	int cnt = TWI_RESP_TIME_OUT_COUNT;
	register volatile U32* pSts asm( "%r14" )=R_TWISTAT;

	while( cnt-- )
	{
		status = *pSts;
		if (status & F_TWISTAT_TWIDT_FIN)
			return TRUE;

		if (status&F_TWISTAT_TWILOST)      // Lost Arbitration
		{
			*R_TWICTRL = 0;
			*R_TWISTAT |= 0x10;   // Clear Lost Arbitration
			DEBUGPRINTF("TWI Lost Arbitration !\r\n");
			return FALSE;
		}
	}   
	DEBUGPRINTF("twi_trans_complete() time out!\r\n");
	*R_TWICTRL = 0;         // Stop twi
	return FALSE;
}

BOOL twi_wait_busy( void ) 
{
	U32 status;
	int cnt = TWI_RESP_TIME_OUT_COUNT;

	register volatile U32* pSts asm( "%r14" )=R_TWISTAT;

	while( cnt-- )
	{
		status = *pSts ;
		if( ( status & F_TWISTAT_TWIBUSY ) == 0 )
			return TRUE;

		if(status&0x10)      // Lost Arbitration
		{
			*R_TWICTRL = 0;
			*R_TWISTAT |= 0x10;   // Clear Lost Arbitration
			debugstring("twi_trans_complete() time out!\r\n");
			return FALSE;
		}
	}
	debugstring( "twi_wait_busy() time out!\r\n" );
	*R_TWISTAT &= ~(1<<5);  // Clear busy status
	*R_TWICTRL = 0;         // Stop twi
	return FALSE;

}

BOOL twi_start (U8 devaddr, U8 *waddr, int len, int flags)
{
    int i;
    U8 con = 0;
    U16 status;

    if(FALSE == twi_wait_busy())
	{
		return -1;
	}
	
    con = F_TWICTRL_TWIEN | F_TWICTRL_TWIMOD_MSTART | F_TWICTRL_TWITR_MTRM;
	
	*R_TWIDATA = devaddr;
	*R_TWICTRL = con;

	if(FALSE == twi_trans_complete())
	{
		return FALSE;
	}

    for (i = 0; i < len; i++) {
		*R_TWIDATA = waddr[i];
		
        if(FALSE == twi_trans_complete())
		{
			return FALSE;
		}
    } 

    if (flags == TWI_RECEIVE) {
        con = F_TWICTRL_TWIEN | F_TWICTRL_TWIMOD_MSTART | F_TWICTRL_REPST_RSTART;
		
		*R_TWICTRL = con;
		*R_TWIDATA = devaddr+1;
		
		if(FALSE == twi_trans_complete())
		{
			return FALSE;
		}

        status  = *R_TWIDATA;     // Read for SCL Generation
			
        if(FALSE == twi_trans_complete())
		{
			return FALSE;
		}
    }
	
	return TRUE;
}
/**
 * write data.
 *
 * \param devaddr		   Device address.
 * \param waddr  Pointer to address.
 * \param len			  The bytes length of address.
 * \param buffer Storage location to write.
 * \param length		  Number of bytes.
 *
 * \return the number of bytes actually written.
 */
int twi_write(U8 devaddr, U8 *waddr,
        int len, U8 *buffer, int length)
{
    int write_num, i;

    if(length <= 0)
        return -1;

    write_num = 0;

    twi_start (devaddr, waddr, len, TWI_TRANSMIT);

	for (i = 0; i < length; i++) {		
		*R_TWIDATA = buffer[i];// When word is 8bit, use this
			
        if(FALSE == twi_trans_complete())
		{
			return -1;
		}
		
        write_num++;
    }
	*R_TWICTRL = F_TWICTRL_TWIEN;	
    return write_num;
}
/**
 * Writes data.
 *
 * \param devaddr		  Device address.
 * \param waddr			  address to be written.
 * \param buffer Storage location to write.
 * \param length		Number of bytes.
 *
 * \return the number of bytes actually written.
 */
int twi_write2(U8 devaddr, U8 waddr, U8 *buffer, int length)
{
	int write_num, i;

	write_num = 0;

	twi_start (devaddr, &waddr, 1, TWI_TRANSMIT);
	for (i = 0; i < length; i++) {
		*R_TWIDATA = buffer[i];
		
		if(FALSE == twi_trans_complete())
		{
			return -1;
		}
		
		write_num++;
	}

	// stop condition
	*R_TWICTRL &= ~(F_TWICTRL_TWIMOD_MSTART);
	return write_num;
}
/**
 * Read from TWI.
 *
 * \param devaddr		  Device address.
 * \param waddr  Pointer to address.
 * \param len	The bytes length of waddr.
 * \param buffer Storage location for data.
 * \param length Maximum number of bytes.
 *
 * \return  the number of bytes read, return -1 if failed.
 */
int twi_read(U8 devaddr, U8 *waddr,
        int len, U8 *buffer, int length)
{
    U32 i;
	U8	tmp;
    int read_cnt = 0;

    twi_start (devaddr, waddr, len, TWI_RECEIVE);

	for ( i = 0; i < length-1; i++ ) {
		buffer[i] = *R_TWIDATA;
		
        if(FALSE == twi_trans_complete())
		{
			return -1;
		}
		
        read_cnt++;
    }

	*R_TWICTRL = F_TWICTRL_TWIEN | F_TWICTRL_TWIMOD_MSTART | F_TWICTRL_TWIAK_NACK;
    buffer[i] = *R_TWIDATA;
    if(FALSE == twi_trans_complete())
	{
		return -1;
	}
    read_cnt++;

    tmp = *R_TWIDATA;    // dummy read
 

	*R_TWICTRL = F_TWICTRL_TWIEN;    
	return read_cnt;
}
