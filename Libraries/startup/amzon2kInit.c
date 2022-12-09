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
#include "BSPDefs.h" // added
#include "CustomDefs.h"

void main();

static void serialflash_init()
{
	U32 id;
	id = *( volatile unsigned int* )( 0xf0000030 );

	if ( ( ( id & 0xff ) == 1 ) || ( ( id & 0xff ) == 0xef ) ) //winbond, spansion
	{
		U16 stat;
		stat = ( U8 ) * R_SFSTS;
		stat |= ( ( U8 ) * R_SFSTS2 ) << 8;
		stat |= ( 1 << 9 ); //quad enable
		*R_SFCMD = 0x6;//write enable
		*R_SFSTSW = stat;
	}
	else if ( ( ( id & 0xff ) == 0xc2 )) // MXIC
	{
		U16 stat;
		stat = ( U8 ) * R_SFSTS;		
		stat |= ( 1 << 6 ); //quad enable
		*R_SFCMD = 0x6;//write enable
		*R_SFSTS = stat;
	}

	*R_SFMOD = 0x182; //Quad
	*R_SFBRT = 0x0; //baudrate
	*R_SFCSH = 0xa;
}

static void ddr_init()
{
	register int addr;
	addr = 0xf4000800;

	*( volatile unsigned int* )( addr + 4 ) = 0x10102; //ddr2 128M
	*( volatile unsigned int* )( addr + 8 ) = 0x3;

	*( volatile unsigned int* )( addr + 0x10 ) = 0x20131; //ddr2
	*( volatile unsigned int* )( addr + 0x14 ) = 0x0000001; //delay

	*( volatile unsigned int* )( addr + 0x18 ) = 0x8;
	*( volatile unsigned int* )( addr + 0x1c ) = 0x8;
	*( volatile unsigned int* )( addr ) = 0x1; //ddr2
}

static U32 dummy_data[128];
static ISPM void pll_init()
{
	register int i asm("%r10") ;
	bool fisrt_set = true;
	if (*(vU32*)0xf4000030 != 0) // whether already using PLL or not
		fisrt_set = false;

	if (fisrt_set == false)
	{
		for (i = 0; i < 128; i++)
		{
			dummy_data[i] = i;
		}
	}
	//PLL Set	
	*( vU32* )0xf4000000 = 0x1f00; //write enable
	*( vU32* )0xf4000030 = 0; //external clock
	
	if (fisrt_set == false)
	{
		// adjust DDR2 refresh rate
		*(volatile unsigned int*)0xF400080C = OSC_CLOCK / 1000000;
	}

	//*( vU32* )0xf4000020 = 0x118401; // PLL0 399Mhz , AXI
	//*(vU32*)0xf4000020 = 0x118801;// PLL0 411Mhz , AXI
	//*(vU32*)0xf4000020 = 0x118b01;// PLL0 420Mhz , AXI
	//*(vU32*)0xf4000020 = 0x119201;// PLL0 441Mhz , AXI
	//*(vU32*)0xf4000020 = 0x117701;// PLL0 360Mhz
	*( vU32* )0xf4000020 = 0x116e01; // PLL0 333Mhz
	//*(vU32*)0xf4000020 = 0x213600;// PLL0 330Mhz
	//*(vU32*)0xf4000020 = 0x115701;// PLL0 266Mhz
	//*(vU32*)0xf4000020 = 0x224f00;// PLL0 240Mhz

	*( vU32* )0xf4000024 = 0x138401; // PLL1 99.75Mhz, AHB
	//*(vU32*)0xf4000024 = 0x139f01; // PLL1 120Mhz, AHB
	//*( vU32* )0xf4000024 = 0x223100; // PLL1 150Mhz, AHB

	*( vU32* )0xf4000028 = 0x233f00; // PLL2 96Mhz, USB
	
	i = 5000;
	while ( i-- )
	{
		asm( "nop" );
	}

	//clock select
	*( vU32* )0xf4000030 = 0x7; //PLL0,PLL1,PLL2
	*( vU32* )0xf4000000 = 0x0; //write disable
	
	if (fisrt_set == false)
	{
		//adjust DDR refresh rate
		*(volatile unsigned int*)0xF400080C = get_axi_clock() / 1000000;
		while (1)
		{
			bool ok = true;
			for (i = 0; i < 128; i++)
			{
				if (dummy_data[i] != i)
					ok = false;
			}
			if (ok == true)
				break;
		}
	}
}

void usb_clock_init()
{
	*( vU32* )0xf4000000 = 0xff00; //write enable
	*( vU32* )0xf4000034 |= ( 1 << 24 );
	*( vU32* )0xf4000038 &= ~( 0x1 << 0 ); // USB dp always pull-up enable.
	*( vU32* )0xf4000000 = 0x0; //write disable
}

void display_clock_init()
{
	*( vU32* )0xf4000000 = 0x1f00; //write enable

	//*(vU32*)0xf400002c = 0x233400;// PLL3 79.5Mhz for 1280*768 @ 60hz
	//*(vU32*)0xf400002c = 0x136201;// PLL3 74.25Mhz for HDTV/1280*720
	//*(vU32*)0xf400002c = 0x232b00;// PLL3 66Mhz for LCD 1024x768
	//*( vU32* )0xf400002c = 0x24cd02; // PLL3 51.4Mhz for LCD 1024x600
	//*( vU32* )0xf400002c = 0x249f02; // PLL3 40Mhz for LCD 800x600
	//*( vU32* )0xf400002c = 0x242b00; // PLL3 33Mhz for LCD 800x480
#if (USE_AMAZON_STK)
	*(vU32*)0xf400002c = 0x259a01; // PLL3 30Mhz for LCD 800x480
#elif (USE_LCD_1024x600)	
	*(vU32*)0xf400002c = 0x24CC02; // PLL3 51.25Mhz for LCD 1024x600 (Typical:51.2MHz)
#elif (USE_LCD_1280x800)
  *(vU32*)0xf400002c = 0x138D02;// PLL3 71Mhz for HDTV/1280*800 (Typical: 71.1MHz)
#else
  *(vU32*)0xf400002c = 0x259a01; // PLL3 30Mhz for LCD 800x480
#endif
	//*(vU32*)0xf400002c = 0x254700;// PLL3 27Mhz for SDTV
	//*(vU32*)0xf400002c = 0x25c802;// PLL3 25.175Mhz for 640x480

	delayms( 1 );
	//clock select
	*( vU32* )0xf4000030 |= ( 1 << 3 );

	*( vU32* )0xf4000000 = 0x0; //write disable
}

void sound_clock_init()
{
	U32 stat;
  
	*( vU32* )0xf4000000 = 0xff00; //write enable

	stat = *( vU32* )0xf4000030;
	stat &= ~( 3 << 12 ); // Clear I2S_CLK_SEL

  /* SND_I2S Clock Select */
#if USE_I2S_EXTERNAL_CLOCK
  stat |= ( 2 << 12 ); // USE CAPIN3 clock
#else  
	stat |= ( 1 << 12 ); // USE PLL2 clock
#endif	

	*( vU32* )0xf4000030 = stat;
	*( vU32* )0xf4000000 = 0x0; //write disable
}

static void set_cache_area()
{
	asm( "push %r0" );

	//Program memory
	asm( "ldi 0x0f,  %r0" ); //enable cache with write-back
	asm( "mvtc 0, %r8" );
	asm( "sync" );
	asm( "ldi 0xc0000fff,  %r0" ); //from 0xc0000000,0xfff : 16Mbyte
	asm( "mvtc 0, %r5" );
	asm( "	sync" );

	asm( "ldi 0x1f,  %r0" ); //enable cache with write-back
	asm( "mvtc 0, %r8" );
	asm( "sync" );
	asm( "ldi 0xc1000fff,  %r0" ); //from 0xc1000000,0xfff : 16Mbyte
	asm( "mvtc 0, %r5" );
	asm( "	sync" );

	asm( "pop %r0" );
}


/*
	defined at linker-script.
	*/
extern unsigned int _rom_data_start[];
extern unsigned int _ram_data_start[];
extern unsigned int _ram_data_end[];

extern unsigned int _rom_dspm_start[];
extern unsigned int _ram_dspm_start[];
extern unsigned int _ram_dspm_end[];

extern unsigned int _rom_ispm_start[];
extern unsigned int _ram_ispm_start[];
extern unsigned int _ram_ispm_end[];

extern unsigned int _bss_start[];
extern unsigned int _bss_end[];

/*!
 * \brief
 * Write brief comment for crt0main here.
 *
 * Data, ISPM, DSPM copy
 * BSS initialize
 * C++ constructor
 */

//void crt0main() __attribute__((weak)) __attribute__((section(".ispm")));

static void crt0main()
{
	register unsigned int* src;
	register unsigned int* dest;
	register unsigned int* destend;
	register int len;

	/* data copy */
	if ( _rom_data_start < ( U32* )0x20000000 )
	{
		src = _rom_data_start;
		dest = _ram_data_start;
		destend = _ram_data_end;
		len = ( unsigned int )destend - ( unsigned int )dest;
		memcpy( ( void* )dest, ( void* )src, ( int )len );
	}

	/* BSS init
	*/
	dest = _bss_start;
	destend = _bss_end;
	len = ( unsigned int )destend - ( unsigned int )dest;
	if ( len > 0 )
		memset( dest, 0, len );


	//for C++

	typedef void ( *pfunc ) ();
	extern pfunc __ctors[];
	extern pfunc __ctors_end[];
	pfunc *p;
	for ( p = __ctors_end; p > __ctors; )
		( *--p ) ();

}

static void spm_load()
{
	register unsigned int* src;
	register unsigned int* dest;
	register unsigned int* destend;
	register int len;
	/* SPM copy */
	src = _rom_dspm_start;
	dest = _ram_dspm_start;
	destend = _ram_dspm_end;
	len = ( unsigned int )destend - ( unsigned int )dest;
	if ( len > 0 )
		memcpy( dest, src, len );

	src = _rom_ispm_start;
	dest = _ram_ispm_start;
	destend = _ram_ispm_end;
	len = ( unsigned int )destend - ( unsigned int )dest;
	if ( len > 0 )
		memcpy( dest, src, len );

}

static void spm_init()
{
	asm( "push %r0" );
	asm( "ldi  0x700, %r0                                   " );
	asm( "mvtc 0x0, %r3									   " );
	asm( "nop											   " );
	/*
	Num of memory bank: Phy 4, Log 1 (H/W 4*4=16Kbyte)
	*/
	asm( "ldi  0x01, %r0 #ON							   " );
	asm( "mvtc 0x0, %r4									   " );
	asm( "sync											   " );

	asm( "#START address								   " );
	asm( "ldi  0x702, %r0								   " );
	asm( "mvtc 0x0, %r3									   " );
	asm( "sync											   " );
	asm( "ldi  0x20000000, %r0							   " );
	asm( "mvtc 0x0, %r4									   " );
	asm( "sync											   " );

	asm( "#END address :								   " );
	asm( "ldi  0x703, %r0								   " );
	asm( "mvtc 0x0, %r3									   " );
	asm( "sync											   " );
	asm( "ldi  0x20003fff, %r0							   " );
	asm( "mvtc 0x0, %r4									   " );
	asm( "sync											   " );
	asm( "#setting config reg :							   " );
	asm( "ldi  0x701, %r0								   " );
	asm( "mvtc 0x0, %r3									   " );
	asm( "ldi  0x111, %r0								   " );
	asm( "mvtc 0x0, %r4									   " );
	asm( "sync											   " );
	asm( "pop %r0" );
}

void _startup()
{
	*( vU32* )0xf4000000 = 0xff00;	// write enable
	*( vU32* )0xf4000038 |= 0xF; 	// USB Dn idle pull-up & dn always pull-up & dp idle pull up & dp always pull-up disable.
	*( vU32* )0xf4000000 = 0x0; 	// write disable
	spm_init();
	spm_load();
	pll_init();
	
	//if spi flash mode?
	U32 status = *( vU32* )0xf4000440;
	if ( ( status & 0xe ) == 0xc )
		serialflash_init();

	ddr_init();
	
	asm( "ldi _stack - 12, %r8 /* stack pointer */" );
	asm( "mov %r8, %sp" );
	
	CacheInit(); //only for bank0 ( 0 ~ 0x1fffffff )
	set_cache_area(); // for DDR
	crt0main();
	init_interrupt();
	main();//let's go

	while ( 1 );
}
