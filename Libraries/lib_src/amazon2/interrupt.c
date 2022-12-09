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

#include <sys/syscall.h>
/*
#define	SYS_exit	1
#define	SYS_fork	2

#define	SYS_read	3
#define	SYS_write	4
#define	SYS_open	5
#define	SYS_close	6
#define	SYS_wait4	7
#define	SYS_creat	8
#define	SYS_link	9
#define	SYS_unlink	10
#define	SYS_execv	11
#define	SYS_chdir	12
#define	SYS_mknod	14
#define	SYS_chmod	15
#define	SYS_chown	16
#define	SYS_lseek	19
#define	SYS_getpid	20
#define SYS_isatty      21
#define SYS_fstat       22
#define SYS_time 	23


#define SYS_ARG		24
#define	SYS_stat	38


#define	SYS_pipe	42
#define	SYS_execve	59
#define   SYS_times       43
#define SYS_gettimeofday 116

*/
#include "sdk.h"
DSPM int critical_ref_counter = 0;
typedef enum
{
    EXCEPTION_NMI = 0,
    EXCEPTION_DOUBLE_FAULT,
    EXCEPTION_BUS_ERROR,
    EXCEPTION_CP0,
    EXCEPTION_UNDEFINED,
    EXCEPTION_UNIMPLEMTED,
    EXCEPTION_NOTUSED_ISR,
    EXCEPTION_MAX,
} EXCEPTION_TYPE;

static char* error_to_msg[EXCEPTION_MAX] = {"NMI", "Double Fault", "Bus Error", "Coprocessor 0", "Undefined", "unimplemented instruction", "Not registered Interrupt"};


/*!
 *
 * \param[in] addr
 *
 */
static void DumpAllReg( U32* addr )
{

    char* sprname[8] = {"CR0", "CR1", "ML", "MH", "ER", "LR", "PC", "SR"};
    int i;
    debugstring( "\r\n ======== DUMP ALL REGISTER ========\r\n" );

    for ( i = 0; i < 16; )
    {
        DEBUGPRINTF( "R%d = 0x%08x \t", i, addr[i] );
        i++;
        if ( i % 4 == 0 )
        {
            debugstring( "\r\n" );
        }
    }
    for ( ; i < 24; )
    {
        DEBUGPRINTF( "%s = 0x%08x \t", sprname[i - 16], addr[i] );
        i++;
        if ( i % 4 == 0 )
        {
            debugstring( "\r\n" );
        }
    }
    debugstring( " ===================================\r\n" );
}

static void unrecoverable_error( EXCEPTION_TYPE type, U32* registers )
{
    while( 1 )
    {
        if( type < EXCEPTION_MAX )
        {
            debugstring( "======== unrecoverable error ==========\r\n" );
            debugstring( error_to_msg[type] );
            debugstring( "\r\n" );
            DumpAllReg( registers );
            debugstring( "=======================================\r\n" );
        }
        if( type == EXCEPTION_CP0 )
        {
            int register cp0status;
            asm( "push %r0" );
            asm( "mvfc 0,%r15" ); // coprocessor 15
            asm( "sync" );
            asm( "lea (%%r0,0), %0" : "=r"( cp0status ) : );
            asm( "pop %r0" );

            cp0status = ( cp0status >> 2 ) & 0xf;
            switch ( cp0status )
            {
            case 0:
                debugstring( "Access violation exception(Instruction)\r\n" );
                break;
            case 1:
                debugstring( "TLB miss exception(Instruction)\r\n" );
                break;
            case 2:
                debugstring( "Privilege violation exception\r\n" );
                break;
            case 3:
                debugstring( "Address alignment error exception(Data)\r\n" );
                break;
            case 4:
                debugstring( "Access violation exception(Data)\r\n" );
                break;
            case 5:
                debugstring( "TLB miss exception(Data)\r\n" );
                break;
            case 6:
                debugstring( "Write to Read-Only region(Data)\r\n" );
                break;
            case 7:
                debugstring( "Address alignment error exceptioin(Instruction)\r\n" );
                break;
            }
        }
        else if( type == EXCEPTION_NOTUSED_ISR )
        {
            DEBUGPRINTF( " unregistered ISR : regst(0):0x%x\r\n system halted\r\n", *R_INTPEND( 0 ) );
            DEBUGPRINTF( " unregistered ISR : regst(1):0x%x\r\n system halted\r\n", *R_INTPEND( 1 ) );
        }
        int i = 0;
        for( i = 0; i < 0xfffff; i++ )
        {
            asm( "nop" );
            asm( "nop" );
            asm( "nop" );
        }
//         DEBUGPRINTF( "Stop\r\n" );
//         while( 1 );
    }
}

#pragma interrupt_handler
static void NOTUSEDISR( void )
{
    U32* registers = __builtin_frame_address( 0 );
    unrecoverable_error( EXCEPTION_NOTUSED_ISR, registers );
}


#pragma interrupt_handler
static void nmi_autovector( void )
{
    U32* registers = __builtin_frame_address( 0 );
    unrecoverable_error( EXCEPTION_NMI, registers );
}

#pragma interrupt_handler
static void double_fault_exception( void )
{
    U32* registers = __builtin_frame_address( 0 );
    unrecoverable_error( EXCEPTION_DOUBLE_FAULT, registers );
}
#pragma interrupt_handler
static void bus_error_exception( void )
{
    U32* registers = __builtin_frame_address( 0 );
    unrecoverable_error( EXCEPTION_DOUBLE_FAULT, registers );
}

#pragma interrupt_handler
static void coprocessor0_exception( void )
{

    U32* registers = __builtin_frame_address( 0 );
    unrecoverable_error( EXCEPTION_CP0, registers );
}

#pragma interrupt_handler
static void undefined_exception( void )
{
    U32* registers = __builtin_frame_address( 0 );
    unrecoverable_error( EXCEPTION_UNDEFINED, registers );
}

#pragma interrupt_handler
static void unimplemented_exception( void )
{
    U32* registers = __builtin_frame_address( 0 );
    unrecoverable_error( EXCEPTION_UNIMPLEMTED, registers );
}


/**
 * 사용자가 system call에 해당 하는 함수룰 호출 했을 경우 실행된다.
 * 시스템 콜을 호출 하는 함수는 write, read 와 같은 low-level 함수를 호출 했을 경우인데
 * 예를 들어서 fopen, fwrite, fread, printf 와 같이 OS 의존적인 함수를 사용자가 호출 했을 경우이다.
 * printf 만 구현 되어 있어서 사용자가 사용 할 수 있다.
 */
U32 systemcallback( U32 func, U32 p1, U32 p2, U32 p3 )
{
    //	int i;
    U32 ret = 0; // 0 is OK

    switch ( func )
    {
    case SYS_write:
        if ( p1 == 1 || p1 == 2 ) //stdout, stderr.. p2 is string, p3 is length
        {
            char str[1024] ;
            strncpy( str, ( char* )p2, p3 );
            str[p3] = 0;
            debugstring( str );
            ret = p3;

        }
        else
        {
            debugstring( "SYS_write function\r\n" );
            DEBUGPRINTF( "not support parameter : 0x%x\r\n", p1 );
        }
        break;

    default :
        DEBUGPRINTF( "not supported systemcall parameter : func=0x%x, p1=0x%x, p2=0x%x, p3=0x%x,\r\n", func, p1, p2, p3 );
        break;
    }
    return ret;
}
/* 1st arg : %r8
* 2st arg : %r9
* 3st arg : %sp + 0xc
* 4st arg : %sp + 0x10
All regsiter 16+6(cr0,cr1,ml,mh,%er,%lr) + 2(SR,PC)=24
*/

#pragma interrupt_handler
static void SystemCall()
{

    U32 ret;
    volatile U32 *registers;
    int func, p1, p2, p3;
    registers = __builtin_frame_address( 0 );

    //	_vgDumpAllReg(registers);
    func = registers[8];
    p1 = registers[9];
#if GCC_VERSION >= 40701
	p2 = registers[10];
	p3 = registers[11];
#else
    p2 = registers[24 + 3];
    p3 = registers[24 + 4];
#endif
    ret = systemcallback( func, p1, p2, p3 );

    registers[8] = ret;

}
static void unregistered_isr()
{
    U32* registers = __builtin_frame_address( 0 );
    unrecoverable_error( EXCEPTION_NOTUSED_ISR, registers );
}

static void ( *UserVector_table[64] )( void ) =
{
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,//32

    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr,
    unregistered_isr//64
};

#if GCC_VERSION >= 40701
#define ARGUMENT_REGISTER_NANE "%r8-%r11"
#else
#define ARGUMENT_REGISTER_NANE "%r8-%r9"
#endif

#define PRE_DEF_ISR(A)	void predef_isr##A(void) __attribute__((weak)) ;\
    asm(".section .ispm,\"ax\",@progbits");\
    asm(".align 1");\
    asm(".type _predef_isr"#A", @function");\
    asm("_predef_isr"#A":");\
    asm("push	%cr0,%cr1,%ml,%mh,%er,%lr");\
    asm("push	%r7");\
    asm("push	"ARGUMENT_REGISTER_NANE);\
    asm("ldi	_UserVector_table +(4 * "#A") ,%r8");\
    asm("ld		(%r8,0),%r9");\
    asm("jalr	%r9");\
    asm("ldi	0x20 + "#A",%r8");\
    asm("ldi	0xf4004800,%r9");\
    asm("st		%r8,(%r9,0)");\
    asm("pop	"ARGUMENT_REGISTER_NANE);\
    asm("pop	%r7");\
    asm("pop	%cr0,%cr1,%ml,%mh,%er,%lr,%pc,%sr");\
    asm(".size _predef_isr"#A", .-_predef_isr"#A);

//for nested interrupt
#define PRE_DEF_ISR_NESTED_L(A)	void predef_isr##A(void) __attribute__((weak)) ;\
    asm(".section .ispm,\"ax\",@progbits");\
    asm(".align 1");\
    asm(".type _predef_isr"#A", @function");\
    asm("_predef_isr"#A":");\
    asm("push	%cr0,%cr1,%ml,%mh,%er,%lr");\
    asm("push	%r7");\
    asm("push	"ARGUMENT_REGISTER_NANE);\
    asm("sync");\
    \
    asm("ldi	1<<"#A",%r8");\
    asm("ldi	0xf4004818,%r9");\
    asm("st		%r8,(%r9,0)");\
    \
    asm("ldi	0x20 + "#A",%r8");\
    asm("ldi	0xf4004800,%r9");\
    asm("st		%r8,(%r9,0)");\
    asm("set	13");\
    asm("nop");\
    \
    asm("ldi	_UserVector_table +(4 * "#A") ,%r8");\
    asm("ld		(%r8,0),%r9");\
    asm("jalr	%r9");\
    \
    asm("clr 13");\
    asm("nop");\
    asm("ldi	1<<"#A",%r8");\
    asm("ldi	0xf4004814,%r9");\
    asm("st		%r8, (%r9,0)");\
    \
    asm("pop	"ARGUMENT_REGISTER_NANE);\
    asm("pop	%r7");\
    asm("pop	%cr0,%cr1,%ml,%mh,%er,%lr,%pc,%sr");\
    asm(".size _predef_isr"#A", .-_predef_isr"#A);

#define PRE_DEF_ISR_NESTED_H(A)	void predef_isr##A(void) __attribute__((weak)) ;\
    asm(".section .text,\"ax\",@progbits");\
    asm(".align 1");\
    asm(".type _predef_isr"#A", @function");\
    asm("_predef_isr"#A":");\
    asm("push	%cr0,%cr1,%ml,%mh,%er,%lr");\
    asm("push	%r7");\
    asm("push	"ARGUMENT_REGISTER_NANE);\
    asm("sync");\
    \
    asm("ldi	1<<("#A"-32),%r8");\
    asm("ldi	0xf4004858,%r9");\
    asm("st		%r8,(%r9,0)");\
    \
    asm("ldi	0x20 + "#A",%r8");\
    asm("ldi	0xf4004800,%r9");\
    asm("st		%r8,(%r9,0)");\
    asm("set	13");\
    asm("nop");\
    \
    asm("ldi	_UserVector_table +(4 * "#A") ,%r8");\
    asm("ld		(%r8,0),%r9");\
    asm("jalr	%r9");\
    \
    asm("clr	13");\
    asm("sync");\
    \
    asm("ldi	1<<("#A"-32),%r8");\
    asm("ldi	0xf4004854,%r9");\
    asm("st		%r8, (%r9,0)");\
    \
    asm("pop	"ARGUMENT_REGISTER_NANE);\
    asm("pop	%r7");\
    asm("pop	%cr0,%cr1,%ml,%mh,%er,%lr,%pc,%sr");\
    asm(".size _predef_isr"#A", .-_predef_isr"#A);

#if SUPPORT_NESTED_INTNUM_EIRQ0==1
PRE_DEF_ISR_NESTED_L( 0 )
#else
PRE_DEF_ISR( 0 )
#endif
#if SUPPORT_NESTED_INTNUM_TIMER0==1
PRE_DEF_ISR_NESTED_L( 1 )
#else
PRE_DEF_ISR( 1 )
#endif
#if SUPPORT_NESTED_INTNUM_SEIP==1
PRE_DEF_ISR_NESTED_L( 2 )
#else
PRE_DEF_ISR( 2 )
#endif
#if SUPPORT_NESTED_INTNUM_DMA0==1
PRE_DEF_ISR_NESTED_L( 3 )
#else
PRE_DEF_ISR( 3 )
#endif

#if SUPPORT_NESTED_INTNUM_FRAMESYNC==1
PRE_DEF_ISR_NESTED_L( 4 )
#else
PRE_DEF_ISR( 4 )
#endif
#if SUPPORT_NESTED_INTNUM_GPIO0==1
PRE_DEF_ISR_NESTED_L( 5 )
#else
PRE_DEF_ISR( 5 )
#endif
#if SUPPORT_NESTED_INTNUM_UART0==1
PRE_DEF_ISR_NESTED_L( 6 )
#else
PRE_DEF_ISR( 6 )
#endif
#if SUPPORT_NESTED_INTNUM_XDMA0==1
PRE_DEF_ISR_NESTED_L( 7 )
#else
PRE_DEF_ISR( 7 )
#endif

#if SUPPORT_NESTED_INTNUM_EIRQ1==1
PRE_DEF_ISR_NESTED_L( 8 )
#else
PRE_DEF_ISR( 8 )
#endif
#if SUPPORT_NESTED_INTNUM_TIMER1==1
PRE_DEF_ISR_NESTED_L( 9 )
#else
PRE_DEF_ISR( 9 )
#endif
#if SUPPORT_NESTED_INTNUM_PMU==1
PRE_DEF_ISR_NESTED_L( 10 )
#else
PRE_DEF_ISR( 10 )
#endif
#if SUPPORT_NESTED_INTNUM_DMA1==1
PRE_DEF_ISR_NESTED_L( 11 )
#else
PRE_DEF_ISR( 11 )
#endif

#if SUPPORT_NESTED_INTNUM_ICE==1
PRE_DEF_ISR_NESTED_L( 12 )
#else
PRE_DEF_ISR( 12 )
#endif
#if SUPPORT_NESTED_INTNUM_GPIO1==1
PRE_DEF_ISR_NESTED_L( 13 )
#else
PRE_DEF_ISR( 13 )
#endif
#if SUPPORT_NESTED_INTNUM_UART1==1
PRE_DEF_ISR_NESTED_L( 14 )
#else
PRE_DEF_ISR( 14 )
#endif
#if SUPPORT_NESTED_INTNUM_XDMA1==1
PRE_DEF_ISR_NESTED_L( 15 )
#else
PRE_DEF_ISR( 15 )
#endif

#if SUPPORT_NESTED_INTNUM_RESV16==1
PRE_DEF_ISR_NESTED_L( 16 )
#else
PRE_DEF_ISR( 16 )
#endif
#if SUPPORT_NESTED_INTNUM_TIMER2==1
PRE_DEF_ISR_NESTED_L( 17 )
#else
PRE_DEF_ISR( 17 )
#endif
#if SUPPORT_NESTED_INTNUM_USBDEV==1
PRE_DEF_ISR_NESTED_L( 18 )
#else
PRE_DEF_ISR( 18 )
#endif
#if SUPPORT_NESTED_INTNUM_TWI==1
PRE_DEF_ISR_NESTED_L( 19 )
#else
PRE_DEF_ISR( 19 )
#endif

#if SUPPORT_NESTED_INTNUM_USBHOST==1
PRE_DEF_ISR_NESTED_L( 20 )
#else
PRE_DEF_ISR( 20 )
#endif
#if SUPPORT_NESTED_INTNUM_I2SRX1==1
PRE_DEF_ISR_NESTED_L( 21 )
#else
PRE_DEF_ISR( 21 )
#endif
#if SUPPORT_NESTED_INTNUM_UART2==1
PRE_DEF_ISR_NESTED_L( 22 )
#else
PRE_DEF_ISR( 22 )
#endif
#if SUPPORT_NESTED_INTNUM_SEIPRX==1
PRE_DEF_ISR_NESTED_L( 23 )
#else
PRE_DEF_ISR( 23 )
#endif

#if SUPPORT_NESTED_INTNUM_RESV24==1
PRE_DEF_ISR_NESTED_L( 24 )
#else
PRE_DEF_ISR( 24 )
#endif
#if SUPPORT_NESTED_INTNUM_WATCHDOG==1
PRE_DEF_ISR_NESTED_L( 25 )
#else
PRE_DEF_ISR( 25 )
#endif
#if SUPPORT_NESTED_INTNUM_NAND==1
PRE_DEF_ISR_NESTED_L( 26 )
#else
PRE_DEF_ISR( 26 )
#endif
#if SUPPORT_NESTED_INTNUM_DMA2==1
PRE_DEF_ISR_NESTED_L( 27 )
#else
PRE_DEF_ISR( 27 )
#endif

#if SUPPORT_NESTED_INTNUM_SDHC==1
PRE_DEF_ISR_NESTED_L( 28 )
#else
PRE_DEF_ISR( 28 )
#endif
#if SUPPORT_NESTED_INTNUM_I2STX1==1
PRE_DEF_ISR_NESTED_L( 29 )
#else
PRE_DEF_ISR( 29 )
#endif
#if SUPPORT_NESTED_INTNUM_SPI0==1
PRE_DEF_ISR_NESTED_L( 30 )
#else
PRE_DEF_ISR( 30 )
#endif
#if SUPPORT_NESTED_INTNUM_SOUNDMIXER==1
PRE_DEF_ISR_NESTED_L( 31 )
#else
PRE_DEF_ISR( 31 )
#endif

#if SUPPORT_NESTED_INTNUM_RESV32==1
PRE_DEF_ISR_NESTED_H( 32 )
#else
PRE_DEF_ISR( 32 )
#endif
#if SUPPORT_NESTED_INTNUM_GPIO2==1
PRE_DEF_ISR_NESTED_H(33)
#else
PRE_DEF_ISR( 33 )
#endif
#if SUPPORT_NESTED_INTNUM_SPI1==1
PRE_DEF_ISR_NESTED_H(34)
#else
PRE_DEF_ISR( 34 )
#endif
#if SUPPORT_NESTED_INTNUM_TIMER3==1
PRE_DEF_ISR_NESTED_H(35)
#else
PRE_DEF_ISR( 35 )
#endif

#if SUPPORT_NESTED_INTNUM_UART3==1
PRE_DEF_ISR_NESTED_H(36)
#else
PRE_DEF_ISR( 36 )
#endif
#if SUPPORT_NESTED_INTNUM_GPIO3==1
PRE_DEF_ISR_NESTED_H(37)
#else
PRE_DEF_ISR( 37 )
#endif
#if SUPPORT_NESTED_INTNUM_MJPEG_FULL==1
PRE_DEF_ISR_NESTED_H(38)
#else
PRE_DEF_ISR( 38 )
#endif
#if SUPPORT_NESTED_INTNUM_DMA3==1
PRE_DEF_ISR_NESTED_H(39)
#else
PRE_DEF_ISR( 39 )
#endif

#if SUPPORT_NESTED_INTNUM_RESV40==1
PRE_DEF_ISR_NESTED_H(40)
#else
PRE_DEF_ISR( 40 )
#endif
#if SUPPORT_NESTED_INTNUM_XDMA2==1
PRE_DEF_ISR_NESTED_L( 41 )
#else
PRE_DEF_ISR( 41 )
#endif
#if SUPPORT_NESTED_INTNUM_GPIO4==1
PRE_DEF_ISR_NESTED_H(42)
#else
PRE_DEF_ISR( 42 )
#endif
#if SUPPORT_NESTED_INTNUM_MJPEG_DEC_END==1
PRE_DEF_ISR_NESTED_H(43)
#else
PRE_DEF_ISR( 43 )
#endif

#if SUPPORT_NESTED_INTNUM_GPIO5==1
PRE_DEF_ISR_NESTED_H(44)
#else
PRE_DEF_ISR( 44 )
#endif
#if SUPPORT_NESTED_INTNUM_XDMA3==1
PRE_DEF_ISR_NESTED_H(45)
#else
PRE_DEF_ISR( 45 )
#endif
#if SUPPORT_NESTED_INTNUM_GPIO6==1
PRE_DEF_ISR_NESTED_H(46)
#else
PRE_DEF_ISR( 46 )
#endif
#if SUPPORT_NESTED_INTNUM_XDMA4==1
PRE_DEF_ISR_NESTED_H(47)
#else
PRE_DEF_ISR( 47 )
#endif

#if SUPPORT_NESTED_INTNUM_RESV48==1
PRE_DEF_ISR_NESTED_H(48)
#else
PRE_DEF_ISR( 48 )
#endif
#if SUPPORT_NESTED_INTNUM_XDMA5==1
PRE_DEF_ISR_NESTED_H(49)
#else
PRE_DEF_ISR( 49 )
#endif
#if SUPPORT_NESTED_INTNUM_GPIO7==1
PRE_DEF_ISR_NESTED_H(50)
#else
PRE_DEF_ISR( 50 )
#endif
#if SUPPORT_NESTED_INTNUM_XDMA6==1
PRE_DEF_ISR_NESTED_H(51)
#else
PRE_DEF_ISR( 51 )
#endif

#if SUPPORT_NESTED_INTNUM_TIMER_CAP_OVER==1
PRE_DEF_ISR_NESTED_H(52)
#else
PRE_DEF_ISR( 52 )
#endif
#if SUPPORT_NESTED_INTNUM_GPIO8==1
PRE_DEF_ISR_NESTED_H(53)
#else
PRE_DEF_ISR( 53 )
#endif
#if SUPPORT_NESTED_INTNUM_RESV54==1
PRE_DEF_ISR_NESTED_H(54)
#else
PRE_DEF_ISR( 54 )
#endif
#if SUPPORT_NESTED_INTNUM_XDMA7==1
PRE_DEF_ISR_NESTED_H(55)
#else
PRE_DEF_ISR( 55 )
#endif

#if SUPPORT_NESTED_INTNUM_RESV56==1
PRE_DEF_ISR_NESTED_H(56)
#else
PRE_DEF_ISR( 56 )
#endif
#if SUPPORT_NESTED_INTNUM_GPIO9==1
PRE_DEF_ISR_NESTED_H(57)
#else
PRE_DEF_ISR( 57 )
#endif
#if SUPPORT_NESTED_INTNUM_GPIO10==1
PRE_DEF_ISR_NESTED_H(58)
#else
PRE_DEF_ISR( 58 )
#endif
#if SUPPORT_NESTED_INTNUM_GPIO11==1
PRE_DEF_ISR_NESTED_H(59)
#else
PRE_DEF_ISR( 59 )
#endif

#if SUPPORT_NESTED_INTNUM_GPIO12==1
PRE_DEF_ISR_NESTED_H(60)
#else
PRE_DEF_ISR( 60 )
#endif
#if SUPPORT_NESTED_INTNUM_GPIO13==1
PRE_DEF_ISR_NESTED_H(61)
#else
PRE_DEF_ISR( 61 )
#endif
#if SUPPORT_NESTED_INTNUM_GPIO14==1
PRE_DEF_ISR_NESTED_H(62)
#else
PRE_DEF_ISR( 62 )
#endif
#if SUPPORT_NESTED_INTNUM_GPIO15==1
PRE_DEF_ISR_NESTED_H(63)
#else
PRE_DEF_ISR( 63 )
#endif


extern void Reset_Start();
typedef void ( *fp )( void ) ;

/*
	for FreeRTOS
*/
void  __attribute__( ( weak ) ) swi1isr();
#pragma interrupt_handler
void  swi1isr()
{
    DEBUGPRINTF( "SWI 1 ISR\r\n" );
    asm( "halt 1" );
}

fp vector_table[] __attribute__( ( section ( ".vects" ) ) ) =
{
    Reset_Start                , /* V00 : Reset Vector          */
    nmi_autovector              , /* V01 : NMI Vector            */
    NOTUSEDISR        , /* V02 : Interrupt Auto Vector */
    double_fault_exception      , /* V03 : Double fault Vector   */
    bus_error_exception         , /* V04 : Bus Error Exception   */
    NOTUSEDISR                  , /* V05 : Reserved              */
    NOTUSEDISR                  , /* V06: Reserved              */
    NOTUSEDISR                  , /* V07 : Reserved              */
    coprocessor0_exception      , /* V08 : Coprocessor0 Exception*/
    NOTUSEDISR      , /* V09 : Coprocessor1 Exception*/
    NOTUSEDISR     , /* V0A : Coprocessor2 Exception*/
    NOTUSEDISR      , /* V0B : Coprocessor3 Exception*/
    NOTUSEDISR            , /* V0C : OSI reset vector      */
    NOTUSEDISR         , /* V0D : OSI break exception   */
    undefined_exception         , /* V0E : Reserved              */
    unimplemented_exception     , /* V0F : Reserved              */
    NOTUSEDISR          , /* V10 : SWI 00 Vector         */
    swi1isr          , /* V11 : SWI 01 Vector         */
    NOTUSEDISR          , /* V12 : SWI 02 Vector         */
    NOTUSEDISR          , /* V13 : SWI 03 Vector         */
    NOTUSEDISR          , /* V14 : SWI 04 Vector         */
    NOTUSEDISR          , /* V15 : SWI 05 Vector         */
    SystemCall          , /* V16 : SWI 06 Vector         */
    NOTUSEDISR          , /* V17 : SWI 07 Vector         */
    NOTUSEDISR         , /* V18 : SWI 08 Vector         */
    NOTUSEDISR         , /* V19 : SWI 09 Vector         */
    NOTUSEDISR          , /* V1A : SWI 0A Vector         */
    NOTUSEDISR          , /* V1B : SWI 0B Vector         */
    NOTUSEDISR          , /* V1C : SWI 0C Vector         */
    NOTUSEDISR          , /* V1D : SWI 0D Vector         */
    NOTUSEDISR          , /* V1E : SWI 0E Vector         */
    NOTUSEDISR         , /* V1F : SWI 0F Vector         */
    predef_isr0          , /* V20 : User Vector 0         */
    predef_isr1          , /* V21 : User Vector 1         */
    predef_isr2          , /* V22 : User Vector 2         */
    predef_isr3          , /* V23 : User Vector 3         */
    predef_isr4          , /* V24 : User Vector 4         */
    predef_isr5          , /* V25 : User Vector 5         */
    predef_isr6          , /* V26 : User Vector 6         */
    predef_isr7         , /* V27 : User Vector 7         */
    predef_isr8          , /* V28 : User Vector 8         */
    predef_isr9          , /* V29 : User Vector 9         */
    predef_isr10          , /* V2A : User Vector 10        */
    predef_isr11          , /* V2B : User Vector 11        */
    predef_isr12          , /* V2C : User Vector 12        */
    predef_isr13         , /* V2D : User Vector 13        */
    predef_isr14          , /* V2E : User Vector 14        */
    predef_isr15         , /* V2F : User Vector 15        */
    predef_isr16          , /* V30 : User Vector 16            */
    predef_isr17          , /* V31 : User Vector 17           */
    predef_isr18          , /* V32 : User Vector 18           */
    predef_isr19          , /* V33 : User Vector 19           */
    predef_isr20          , /* V34 : User Vector 20           */
    predef_isr21          , /* V35 : User Vector 21           */
    predef_isr22          , /* V36 : User Vector 22           */
    predef_isr23          , /* V37 : User Vector 23           */
    predef_isr24          , /* V38 : User Vector 24           */
    predef_isr25          , /* V39 : User Vector 25           */
    predef_isr26          , /* V3A : User Vector 26           */
    predef_isr27          , /* V3B : User Vector 27           */
    predef_isr28          , /* V3C : User Vector 28           */
    predef_isr29          , /* V3D : User Vector 29           */
    predef_isr30          , /* V3E : User Vector 30           */
    predef_isr31          , /* V3F : User Vector 31           */
    predef_isr32          , /* V3F : User Vector 32           */
    predef_isr33          , /* V3F : User Vector 33           */
    predef_isr34          , /* V3F : User Vector 34           */
    predef_isr35          , /* V3F : User Vector 35           */
    predef_isr36          , /* V3F : User Vector 36           */
    predef_isr37          , /* V3F : User Vector 37           */
    predef_isr38          , /* V3F : User Vector 38           */
    predef_isr39          , /* V3F : User Vector 39           */
    predef_isr40          , /* V3F : User Vector 40           */
    predef_isr41          , /* V3F : User Vector 41           */
    predef_isr42          , /* V3F : User Vector 42           */
    predef_isr43          , /* V3F : User Vector 43           */
    predef_isr44          , /* V3F : User Vector 44           */
    predef_isr45          , /* V3F : User Vector 45           */
    predef_isr46          , /* V3F : User Vector 46           */
    predef_isr47          , /* V3F : User Vector 47           */
    predef_isr48          , /* V3F : User Vector 48           */
    predef_isr49          , /* V3F : User Vector 49           */
    predef_isr50          , /* V3F : User Vector 50           */
    predef_isr51          , /* V3F : User Vector 51           */
    predef_isr52          , /* V3F : User Vector 52           */
    predef_isr53          , /* V3F : User Vector 53           */
    predef_isr54          , /* V3F : User Vector 54           */
    predef_isr55          , /* V3F : User Vector 55           */
    predef_isr56          , /* V3F : User Vector 56           */
    predef_isr57          , /* V3F : User Vector 57           */
    predef_isr58          , /* V3F : User Vector 58           */
    predef_isr59          , /* V3F : User Vector 59           */
    predef_isr60          , /* V3F : User Vector 60           */
    predef_isr61          , /* V3F : User Vector 61           */
    predef_isr62          , /* V3F : User Vector 62           */
    predef_isr63          , /* V3F : User Vector 63           */
};

/**
 * Initialises the interrupt.
 * 1. set vectored mode  
 * 2. Enable Interrupt (CPU)  
 * 3. disable all interrupt ( Interrupt Controller)  
 * 4. copy vector table to SPM area
 */
void init_interrupt()
{
    SETVECTORED;

    *R_INTEN( 0 ) = 0; //all disable
    *R_INTEN( 1 ) = 0; //all disable
    *R_INTMASKCLR( 0 ) = 0xffffffff; // all interrupt disable
    *R_INTMASKCLR( 1 ) = 0xffffffff; // all interrupt disable
    //copy vector table to SPM
    memcpy( ( void* )0x20000000, vector_table, sizeof( vector_table ) );
    //set vector base register

    asm( "push %r0" );
	asm("ldi	0x20000000 ,%r0");
    asm( "mvtc	0,%r12" );
    asm( "sync" );
    asm( "pop %r0" );
    
    INTERRUPT_ENABLE;
}
/**
 * Register function as an interrupt service routine.
 *     1. Interrupt occurs  
 *     2. Interrupt Hook function  
 *     3. User Function which be registered  
 *     4. Interrupt Hook function (clear vector)
 *
 * \param intnum	  Interrupt number.
 * \param fp Interrupt Service Routine.
 *
 * \return true if it succeeds, false if it fails.
 * \code
 * void timer0isr()
 * {
 * ........
 * }
 * void main()
 * {
 *	   set_interrupt(INTNUM_TIMER0,timer0isr);
 *	   .....
 * }
 * \endcode
 */
BOOL set_interrupt( INTERRUPT_TYPE intnum, void ( *fp )() )
{
    if ( intnum >= INTNUM_MAX )
        return FALSE;

    UserVector_table[intnum] = fp;
    return TRUE;
}
/**
 * Register function as an interrupt service routine.
 *     1. Interrupt occurs  
 *     2. User Function   
 * ISR must write vector number to Interrupt Pending Register and ISR must be defined as "#pragma interrupt_handler".
 * 
 * \param intnum	  Interrupt number.
 * \param fp Interrupt Service Routine.
 *
 * \return true if it succeeds, false if it fails.
 * \code
 * #pragma interrupt_handler
 * void timer0isr()
 * {
 *   ..............		
 *     *R_INTPENDCLR=INTNUM_TIMER0;
 * }
 * void main()
 * {
 *	   set_interrupt_direct(INTNUM_TIMER0,timer0isr);
 *	   .....
 * }
 * \endcode
 */
BOOL set_interrupt_direct( int intnum, void ( *fp )() )
{
    if ( intnum >= INTNUM_MAX )
        return FALSE;
    U32* vt = ( U32* )(0x20000000 + (4*0x20));
    vt[intnum] = ( U32 )fp;
    return TRUE;
}
/**
 * Get an ISR
 *
 * \param intnum Interrupt number.
 *
 * \return ISR(interrupt service routine).
 */
void* get_interrupt( INTERRUPT_TYPE intnum )
{
    return UserVector_table[intnum];
}
/**
 * Enables the interrupt.
 *
 * \param num Interrupt number.
 * \param b   true or flase.
 */
void enable_interrupt( INTERRUPT_TYPE num, BOOL b )
{
    CRITICAL_ENTER();
    int n = 0 ;
    if( num > 31 )
    {
        n = 1;
        num -= 32;
    }
    if ( !b ) //disable
    {
        *R_INTEN( n ) &= ( ~( 1 << num ) );
        *R_INTMASKCLR( n ) = ( 1 << num );
    }
    else
    {
        *R_INTEN( n ) |= ( 1 << num );
        *R_INTMASKSET( n ) = ( 1 << num );
    }
    CRITICAL_EXIT();
}
