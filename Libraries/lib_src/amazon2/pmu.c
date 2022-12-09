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

#define PLL_OUT(OSC,DIVF,DIVR,DIVQ)	(OSC*(DIVF+1)/((DIVR+1)*(1<<DIVQ)))

static U32 calc_pll( U32* reg )
{
    U32 val;
    U32 DivQ, DivF, DivR;
    val = *( vU32* )reg;
    DivQ = ( val >> 16 ) & 0x7;
    DivF = ( val >> 8 ) & 0xff;
    DivR = val & 0x1f;
    return PLL_OUT( OSC_CLOCK, DivF, DivR, DivQ );
}
U32 get_ahb_clock()
{
    return calc_pll( ( U32* )0xf4000024 );
}

U32 get_axi_clock()
{
    return calc_pll( ( U32* )0xf4000020 ) / 2;
}

U32 get_apb_clock()
{
	if ((*(U32*)0xf4000030) & (1 << 7))
		return get_axi_clock() / 2;
	else
		return get_axi_clock() / 4;
}

U32 get_pll( int num )
{
    return calc_pll( ( U32* )( 0xf4000020 + ( num * 4 ) ) ) / 2;
}
