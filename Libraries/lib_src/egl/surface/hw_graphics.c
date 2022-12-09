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

#define	GEC_RUN2D			1
#define	GEC_CANCEL2D		0


#define	GEC_DITHERNONE		(0x0<<6)
#define	GEC_DITHER2x2		(0x1<<6)
#define	GEC_DITHER4x4		(0x2<<6)

#define MAX_COMMAND			2048
#define MAX_COMMAND_MASK	(MAX_COMMAND-1)

U32 _ge_CurCommandNum=0;///< 현재 Packet의 넘버 0-2047
U32 g_TMEM_StartAddr = 0xc0000000;///< Texture memory 시작주소
U32 g_TMEM_Size = 0x1000000;


void ge_end_cmd( void )
{		
	_ge_CurCommandNum = (_ge_CurCommandNum+1) & MAX_COMMAND_MASK;
	//wait when command queue is full.
	while( *R_GE_PRP  == _ge_CurCommandNum )
	{
		//DEBUGPRINTF("GE COMMAND FULL : %08x,%08x(%d)\r\n",*R_GE_PRP ,_ge_CurCommandNum,i);
		DEBUGPRINTF("GE COMMAND FULL : %08x,%08x\r\n",*R_GE_PRP ,_ge_CurCommandNum);
	}

	*R_GE_PWP=_ge_CurCommandNum;
}


void ge_wait_empty_cmd_q( void )
{		
	while( *R_GE_PRP  != _ge_CurCommandNum ) ;
}

void ge_wait_cmd_complete()
{
	while(!((*R_GE_CDB) & (1<<24)));
}

void ge_set_texture_mem(U32 addr, U32 size)
{
	*R_TMASA = addr;
	g_TMEM_StartAddr= addr;
	g_TMEM_Size = size;
}

void ge_init()
{
	*R_GE_PWP = 0;
	*R_GE_PRP = 0;
	*R_GE_RC = 0;
	*R_GE_FCC = 0;
	*R_GE_RC = (1<<8)|GEC_RUN2D|GEC_DITHER4x4;
}

extern SURFACE g_FrameSurface[2];
void flip( void )
{
	U8 curbank = (*R_GE_CDB)&3;
	while(*R_GE_FCC != 0 );

	*R_GE_FCC = 0; // reset flip count
 	*R_GE_FCC = 1; // increase flip count
	GE_WriteCmd( 0,(1<<1) );
	GE_WriteCmd( 1,0 );
	ge_end_cmd();
	curbank %=2;
	set_draw_target(&g_FrameSurface[curbank]);
}

void flip_wait( void )
{
	while(*R_GE_FCC != 0 );
	*R_GE_FCC = 0; // reset flip count
	*R_GE_FCC = 1; // increase flip count
	GE_WriteCmd( 0,	(1<<1) );
	GE_WriteCmd( 1,0 );
	ge_end_cmd();
	while(*R_GE_FCC != 0 );
	set_draw_target(get_ge_target_buffer());
}

void async_flip( void )
{
	U8 curbank = (*R_GE_CDB)&3;
	while(*R_GE_FCC  != 0 );

	*(U16*)R_GE_FCC = 0; // reset flip count
	*(U16*)R_GE_FCC = 1; // increase flip count
	GE_WriteCmd( 0,	1 );
	GE_WriteCmd( 1,0 );
	ge_end_cmd();	
	curbank %=2;
	set_draw_target(&g_FrameSurface[curbank]);
}

void async_flip_wait( void )
{
	while((*R_GE_FCC ) != 0 );
	*(U16*)R_GE_FCC = 0; // reset flip count
	*(U16*)R_GE_FCC = 1; // increase flip count
	GE_WriteCmd( 0,	1 );
	GE_WriteCmd( 1,0 );
	ge_end_cmd();	

	while((*R_GE_FCC ) != 0 );
	set_draw_target(get_ge_target_buffer());
}

