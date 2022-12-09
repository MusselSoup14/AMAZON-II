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
/* Includes ------------------------------------------------------------------*/
#include "BSPDefs.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS **************
// ***************************************************************************
//#define APP_LOAD_ADDRESS  0xc0000000 //for APP
#define APP_LOAD_ADDRESS    0xc1000000 // for bootloader
#define COPY_BLOCK_SIZE	    4

#define UART_BAUD           115200

#define NAND_BLOCK_SIZE     (2048*64)
#define NAND_PAGE_SIZE	    2048
#define NAND_PAGE_PER_BLOCK	64
#define NAND_ECC_BIT        4
#define NAND_ROW_ADDR_LEN   19
//#define NAND_ROW_ADDR_LEN 16
#define ECC_OFFSET_4        20
#define ECC_OFFSET_24       84

typedef void ( *fp )();

void Reset_Start();
fp vector_table[] __attribute__( ( section ( ".vects" ) ) ) =
{
    Reset_Start                , /* V00 : Reset Vector          */
};

//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************

//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************

/**
  * @brief  
  * @param  
  * @retval 
  */
static void bootcodeDDRInit(void)
{
	register int addr;
	addr = 0xf4000800;
	*(volatile unsigned int*)(addr + 4) = 0x10102; //ddr2 128M
	*(volatile unsigned int*)(addr + 8) = 0x3;

	*( volatile unsigned int* )( addr + 0x10 ) = 0x20131; //ddr2
	*( volatile unsigned int* )( addr + 0x14 ) = 0x0000001; //delay

	*(volatile unsigned int*)(addr + 0x18) = 0x8;
	*(volatile unsigned int*)(addr + 0x1c) = 0x8;
	*(volatile unsigned int*)(addr) = 0x1; //ddr2
}

/**
  * @brief  
  * @param  
  * @retval 
  */
static void bootcodePLLInit(void)
{

//PLL Set
	*(vU32*)0xf4000000 = 0x1f00; //write enable
	*(vU32*)0xf4000030 = 0; //external clock	
	*(volatile unsigned int*)0xF400080C = 0x6;	// DDR2 Refresh Rate for external clock

	*(vU32*)0xf4000020 = 0x116e01; // PLL0 333Mhz

	*(vU32*)0xf4000024 = 0x138401; // PLL1 99.75Mhz, AHB

	*(vU32*)0xf4000028 = 0x233f00; // PLL2 96Mhz, USB

	register int i asm("%r10") = 5000;
	while (i--)
	{
		asm("nop");
	}

	*(volatile unsigned int*)0xF400080C = 0x85;	// DDR2 Refresh Rate for PLL
	//clock select	
	*(vU32*)0xf4000030 = 0x7; //PLL0,PLL1,PLL2
	*(vU32*)0xf4000000 = 0x0; //write disable
}

/**
  * @brief  
  * @param  
  * @retval 
  */
static void bootcodeDebugString( const char* str )
{
  U32 uart_base = ( U32 )R_UART_BASE( 0 );
  
  while( *str )
  {
    //wait empty
    while( !( ( *( volatile U32* )( uart_base + UART_LSR ) )  & F_ULS_TEMP ) );
    *( volatile u8* )( uart_base + UART_THR ) = *str;
    str++;
  }
}

/**
  * @brief  
  * @param  
  * @retval 
  */
static void bootcodeUartInit(void)
{
  U32 apbclock;
  U32 stopbit = 1;
  U32 databits = 8;
  U32 uart_base = ( U32 )R_UART_BASE( 0 );
  
  apbclock = 41500000;  // 166MHz
  U16 divisor = apbclock / ( UART_BAUD * 16 ) ;
  if( ( apbclock % ( UART_BAUD * 16 ) ) > ( ( UART_BAUD * 16 ) / 2 ) )
    divisor++;
  U32 lcr;//line control register
  lcr = ( ( stopbit - 1 ) << 2 ) + ( databits - 5 );
  *( volatile U32* )( uart_base + UART_FCR ) = F_UFC_FIFOEN | F_UFC_RFTL_1;
  lcr |= F_ULC_DLAB;
  *( volatile U32* )( uart_base + UART_LCR ) = lcr;
  *( volatile U32* )( uart_base + UART_DIV_LO ) = divisor & 0xff;
  *( volatile U32* )( uart_base + UART_DIV_HI ) = ( divisor >> 8 ) & 0xff;
  lcr &= ~F_ULC_DLAB; //divisor latch access disable, fifo control write mode.
  *( volatile U32* )( uart_base + UART_LCR ) = lcr;
  *( volatile U32* )( uart_base + UART_IER ) = 0; //disable rx/tx interrupts
}

/**
  * @brief  
  * @param  
  * @retval 
  */
static void bootcodeWaitBusy(void)
{
  // Check Busy End
  while ( !( *( volatile U8* )R_NFMSTAT & 0x01 ) );// for "trr" timing,see Nand-Flash data sheet
  while ( !( *( volatile U8* )R_NFMSTAT & 0x02 ) ); 
  int i;
  for( i = 0; i < 1; i++ );
}

/**
  * @brief  
  * @param  
  * @retval 
  */
__inline__ static void bootcodeNandReset(void)
{
  *( volatile char * )R_NFMCMD  = NAND_CMD_RESET ; // Read  
  bootcodeWaitBusy();
}


/**
  * @brief  
  * @param  
  * @retval 
  */
__inline__ static void bootcodeSetPageAddress( U32 pagenum, U32 offset )
{
  int i;
  int k;
  U8 mask;
  U8 validbit;
  
  *( volatile U8 * )R_NFMADDR  = offset & 0xff ; // A0-A7
  if( NAND_ROW_ADDR_LEN != 512 )
    *( volatile U8 * )R_NFMADDR  = offset >> 8 ;
  
  for( i = 0; i < NAND_ROW_ADDR_LEN; i += 8 )
  {
    mask = 0;
    validbit = ( NAND_ROW_ADDR_LEN - i ) > 8 ? 8 : ( NAND_ROW_ADDR_LEN - i );
    
    for( k = 0; k < validbit; k++ )
      mask |= ( 1 << k );
    
    *( volatile U8 * )R_NFMADDR  = ( U8 )( pagenum  & mask ) ; // Row Address
    pagenum = pagenum >> 8;
  }
}

/**
  * @brief  
  * @param  
  * @retval 
  */
__inline__ static void bootcodeSetSpareAddress( U32 pagenum )
{
#if (NAND_PAGE_SIZE==512)
  bootcodeSetPageAddress( pagenum, 0 );
#else
  {
    int i;
    int k;
    U8 mask;
    U8 validbit;
    
    *( volatile U8 * )R_NFMADDR  = 0 ;
    if( NAND_PAGE_SIZE != 512 )
      *( volatile U8 * )R_NFMADDR  =  NAND_PAGE_SIZE >> 8;
    
    for( i = 0; i < NAND_ROW_ADDR_LEN; i += 8 )
    {
      mask = 0;
      validbit = ( NAND_ROW_ADDR_LEN - i ) > 8 ? 8 : ( NAND_ROW_ADDR_LEN - i );
      
      for( k = 0; k < validbit; k++ )
        mask |= ( 1 << k );
      
      *( volatile U8 * )R_NFMADDR  = ( U8 )( pagenum  & mask ) ; // Row Address
      pagenum = pagenum >> 8;
    }
  }
#endif
}

/**
  * @brief  
  * @param  
  * @retval 
  */
static BOOL bootcodeNandPhyReadSpare( U32 pageaddr, void *buffer )
{
  int i;
  char* buf;
  ///volatile U8 g_dummy = *R_NFMSTAT; //clear busy
  buf = ( U8* )buffer;
  
#if (NAND_PAGE_SIZE == 512)
  {
    *( volatile U8 * )R_NFMCMD  = NAND_CMD_READSPARE ; // Read
    bootcodeSetSpareAddress( pageaddr );
  }
#else
  {
    *( volatile U8 * )R_NFMCMD  = NAND_CMD_READ0 ;
    bootcodeSetSpareAddress( pageaddr );
    *( volatile U8 * )R_NFMCMD  = NAND_CMD_READSTART ;
  }
#endif
  // Check Busy End
  bootcodeWaitBusy();
  for( i = 0; i < 16; i++ )
    buf[i] = *( volatile U8* )R_NFMDATA;
  
  // Check Busy End
  while ( !( *( volatile U8* )R_NFMSTAT & 0x02 ) ); // for "trr" timing,see Nand-Flash data sheet
  
  return TRUE;
}

/**
  * @brief  
  * @param  
  * @retval 
  */
static BOOL bootcodeNandPhyReadPage( U32 pageaddr, void* buffer )
{
  U8* buf = ( U8* )buffer;
  volatile U32 g_dummy;
#if	(NAND_ECC_BIT==24)
  {
    int j;
    
    for( j = 0; j < NAND_PAGE_SIZE / 1024; j++, buf += 1024 )
    {
        g_dummy = *R_NFMSTAT; //clear busy
        *( volatile char * )R_NFMCMD  = NAND_CMD_READ0 ; // Read
        bootcodeSetPageAddress( pageaddr, j * 1024 );
        *( volatile char * )R_NFMCMD  = NAND_CMD_READSTART ; // Read
        g_dummy = *R_NFMMLCECC( 10 );
        bootcodeWaitBusy();
        //set  ecc location
	*R_NFMSPADDR = NAND_PAGE_SIZE + (j * 4 * 11) + ECC_OFFSET_24;
        *R_NFMCON |= ( 1 << 16 );
        while( 1 )
        {
            U32 stat = *R_NFMSTAT;
            if( stat & ( 1 << 2 ) ) //wait for auto decoding
            {
                if ( ( stat & ( 1 << 3 ) ) == 0 )
                {
                    if( stat < ( 1 << 11 ) )
                        bootcodeDebugString( "ECC Decoding error\r\n" );
                }
                break;
            }
        }
        U32 i;
        if( ( U32 )( &buf[0] ) & 1 ) //1byte boundry
        {
            for( i = 0; i < 1024; )
            {
                buf[i] = *( volatile U8* )R_NFMECD;
                buf[i + 1] = *( volatile U8* )R_NFMECD;
                buf[i + 2] = *( volatile U8* )R_NFMECD;
                buf[i + 3] = *( volatile U8* )R_NFMECD;
                i += 4;
            }
        }
        else if( ( U32 )( &buf[0] ) & 2 ) //2byte boundry
        {
            for( i = 0; i < 1024; )
            {
                *( U16* )( buf + i ) = *( volatile U16* )R_NFMECD;
                *( U16* )( buf + i + 2 ) = *( volatile U16* )R_NFMECD;
                *( U16* )( buf + i + 4 ) = *( volatile U16* )R_NFMECD;
                *( U16* )( buf + i + 6 ) = *( volatile U16* )R_NFMECD;
                i += 8;
            }
        }
        else
        {
            for( i = 0; i < 1024; )
            {
                *( U32* )( buf + i ) = *R_NFMECD;
                *( U32* )( buf + i + 4 ) = *R_NFMECD;
                *( U32* )( buf + i + 8 ) = *R_NFMECD;
                *( U32* )( buf + i + 12 ) = *R_NFMECD;
                i += 16;
            }
        }
        while ( !( *( volatile U8* )R_NFMSTAT & 0x02 ) ); // for "trr" timing,see Nand-Flash data sheet
    }
    
    return TRUE;
  }
#elif (NAND_ECC_BIT==4)
  {
    int j;
    for( j = 0; j < NAND_PAGE_SIZE / 512; j++, buf += 512 )
    {
        g_dummy = *R_NFMSTAT; //clear busy
        *( volatile char * )R_NFMCMD  = NAND_CMD_READ0 ; // Read
        bootcodeSetPageAddress( pageaddr, j * 512 );
        *( volatile char * )R_NFMCMD  = NAND_CMD_READSTART ; // Read
        //set  ecc location
        *R_NFMSPADDR = NAND_PAGE_SIZE + (j * 4 * 2) + ECC_OFFSET_4;
        g_dummy = *R_NFMMLCECC( 10 ); //clear
        bootcodeWaitBusy();
        *R_NFMCON |= ( ( 1 << 16 ) | ( 1 << 15 ) ); //4bit, AUTO ECC
        while( 1 )
        {
            U32 stat = *R_NFMSTAT;
            if( stat & ( 1 << 2 ) ) //wait for auto decoding
            {
                if ( ( stat & ( 1 << 3 ) ) == 0 )
                {
                    if( stat < ( 1 << 11 ) )
					//DEBUGPRINTF( "ECC Decoding error(NFSTAT:%#x)\r\n", stat );
					bootcodeDebugString( "ECC Decoding error\r\n");
                }
#ifdef NAND_DEBUG
                if( ( stat >> 12 ) & 0x1f )
                {
                    DEBUGPRINTF( "error bit count %d ==> corrected\r\n", ( stat >> 12 ) & 0x1f );
                }
#endif
                break;
            }
        }
        U32 i;
        if( ( U32 )( &buf[0] ) & 1 ) //1byte boundry
        {
            for( i = 0; i < 512; )
            {
                buf[i] = *( volatile U8* )R_NFMECD;
                buf[i + 1] = *( volatile U8* )R_NFMECD;
                buf[i + 2] = *( volatile U8* )R_NFMECD;
                buf[i + 3] = *( volatile U8* )R_NFMECD;
                i += 4;
            }
        }
        else if( ( U32 )( &buf[0] ) & 2 ) //2byte boundry
        {
            for( i = 0; i < 512; )
            {
                *( U16* )( buf + i ) = *( volatile U16* )R_NFMECD;
                *( U16* )( buf + i + 2 ) = *( volatile U16* )R_NFMECD;
                *( U16* )( buf + i + 4 ) = *( volatile U16* )R_NFMECD;
                *( U16* )( buf + i + 6 ) = *( volatile U16* )R_NFMECD;
                i += 8;
            }
        }
        else
        {
            for( i = 0; i < 512; )
            {
                *( U32* )( buf + i ) = *R_NFMECD;
                *( U32* )( buf + i + 4 ) = *R_NFMECD;
                *( U32* )( buf + i + 8 ) = *R_NFMECD;
                *( U32* )( buf + i + 12 ) = *R_NFMECD;
                i += 16;
            }
        }
        while ( !( *( volatile U8* )R_NFMSTAT & 0x02 ) ); // for "trr" timing,see Nand-Flash data sheet
    }
    
    return TRUE;
  }
#else
  {
    g_dummy = *R_NFMSTAT; //clear busy
    *( volatile char * )R_NFMCMD  = NAND_CMD_READ0 ; // Read
    bootcodeSetPageAddress( pageaddr, 0 );
    if( NAND_PAGE_SIZE != 512 )
        *( volatile char * )R_NFMCMD  = NAND_CMD_READSTART ; // Read
    // Check Busy End
    bootcodeWaitBusy();
#ifdef NAND_USE_DMA
    dma_read_nand( buf, NandInfo.pagesize );
#else
    U32 i;
    for( i = 0; i < NAND_PAGE_SIZE; )
    {
        *( U32* )( buf + i ) = *R_NFMDATA;
        *( U32* )( buf + i + 4 ) = *R_NFMDATA;
        *( U32* )( buf + i + 8 ) = *R_NFMDATA;
        *( U32* )( buf + i + 12 ) = *R_NFMDATA;
        i += 16;
    }
#endif		//NAND DMA
    while ( !( *( volatile U8* )R_NFMSTAT & 0x02 ) ); // for "trr" timing,see Nand-Flash data sheet
      
    return TRUE;
  }
#endif
}

/**
  * @brief  
  * @param  
  * @retval 
  */
static void bootcodeCopyDataFromNand( U8* dest, int startblock, int blockcnt )
{
  int j, k;
  int badcnt;
  U8	spare[16];
  
  for( badcnt = 0, k = startblock; k < ( startblock + blockcnt ); k++ )
  {
    int pagenum ;
    //bootcodeDebugString("block read");
    while( 1 )
    {
      pagenum = ( badcnt + k ) * NAND_PAGE_PER_BLOCK;
      bootcodeNandPhyReadSpare( pagenum, spare ); //check bad block
      if ( spare[0] == 0xFF )
        break;
      bootcodeDebugString( "bad block\r\n" );
      badcnt++;
    }
    for( j = 0 ; j < NAND_PAGE_PER_BLOCK; j++ )
    {
      bootcodeNandPhyReadPage( pagenum + j, dest );
      dest += NAND_PAGE_SIZE;
      if(0 == j % 3)
        bootcodeDebugString( "\r-" );
      else if(1 == j % 3)
        bootcodeDebugString( "\r\\" );      
      else 
        bootcodeDebugString( "\r/" );
    }
    //bootcodeDebugString("\r\n");
  }
}


/**
  * @brief  bootcode Port Init
  * @param  None
  * @retval None
  */
static void bootcodePortInit(void)
{
  *R_PAF(0) = 0; //NAND
  *R_PAF(1) = 0x0; //NAND , UART0
}

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
  * @brief  NAND bootcode Initializer
  * @param  None
  * @retval None
  */
void amazon2Init(void)
{
	bootcodePLLInit();
	bootcodeDDRInit();
}

/**
  * @brief  NAND bootcode Main Procedure.
  * @param  None
  * @retval None
  */
int main(void)
{
  void ( *startfp )();
  
  bootcodePortInit();
  bootcodeUartInit();
  bootcodeDebugString( "\r\n" );
  
  ///bootcodeDebugString( "Nand Boot Code\r\n" );
  *R_NFMCFG = ( 1 << 20 ) | 0x2222;
  if( NAND_ECC_BIT == 24 )
    *R_NFMCON = 0;//24bit ecc mode
  else
    *R_NFMCON = ( 1 << 15 );
  
  bootcodeNandReset();
  bootcodeCopyDataFromNand( ( U8* )APP_LOAD_ADDRESS, 1, COPY_BLOCK_SIZE );
  bootcodeDebugString( "\r\n" );
  startfp = ( void ( * )() )( *( volatile unsigned int* )APP_LOAD_ADDRESS );
  startfp();
  
  while( 1 );
  
  return 0;
}
