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

static bool ahb_dma_used[AHB_DMA_CHANNEL_MAX] = {false, false, false, false};
int get_free_ahb_dma()
{
    int i;
    CRITICAL_ENTER();
    for( i = 0; i < AHB_DMA_CHANNEL_MAX; i++ )
    {
        if( ahb_dma_used[i] == false )
        {
            ahb_dma_used[i] = true;
            CRITICAL_EXIT();
            return i;
        }
    }
    CRITICAL_EXIT();
    return -1;
}
void set_free_ahb_dma( int ch )
{
    CRITICAL_ENTER();
    if( ( ch >= 0 ) && ( ch < AHB_DMA_CHANNEL_MAX ) )
    {
        *R_DMACFG( ch ) = 0xff << 4; //set dest, src as reserved
        ahb_dma_used[ch] = false;
    }
    CRITICAL_EXIT();
}

static bool axi_dma_used[AXI_DMA_CHANNEL_MAX] = {false, false};
int get_free_axi_dma()
{
    int i;
    CRITICAL_ENTER();
    for( i = 0; i < AXI_DMA_CHANNEL_MAX; i++ )
    {
        if( axi_dma_used[i] == false )
        {
            axi_dma_used[i] = true;
            CRITICAL_EXIT();
            return i;
        }
    }
    CRITICAL_EXIT();
    return -1;
}
void set_free_axi_dma( int ch )
{
    CRITICAL_ENTER();
    if( ch < AXI_DMA_CHANNEL_MAX )
        axi_dma_used[ch] = false;
    CRITICAL_EXIT();
    return ;
}

/*
source 나 destination 이 cache 영역이면 이 함수를 호출 하기 전에 반드시 cache invalidation 을 해 주어야 한다.
*/

static BOOL dma_axi_memcpy( U32* dest, U32* src, U32 bytelen )
{
    U32 conval = 0;
    int dmach = get_free_axi_dma();

    if( dmach < 0 )
        return FALSE;
    U32 bitwidth = 8;
    U32 transfer_size = bytelen;

    if( bytelen > 4 )
    {
        if( ( ( U32 )dest & 0x3 ) == 0 ) //32bit
        {
            conval |= 1 << 3;
        }
        else if( ( ( U32 )dest & 0x1 ) == 0 ) //16bit
        {
            conval |= 1 << 2;
        }

        if( ( ( U32 )src & 0x3 ) == 0 ) //32bit
        {
            conval |= 1 << 11;
            transfer_size = bytelen >> 2;
            bitwidth = 32;
        }
        else if( ( ( U32 )src & 0x1 ) == 0 ) //16bit
        {
            conval |= 1 << 10;
            transfer_size = bytelen >> 1;
            bitwidth = 16;
        }
    }
    conval |= 0x1 << 8 | 0x1; //address increment

    *R_XDMASRC( dmach ) = ( U32 )src;
    *R_XDMADST( dmach ) = ( U32 )dest;

    U32 reloadcnt = transfer_size >> 11; // devide by 0x800

    if( reloadcnt )
    {
        *R_XDMACON0( dmach ) = 0x800 << 20 | conval;

        *R_XDMASCOA( dmach ) = 0x0;
        *R_XDMADCOA( dmach ) = 0x0;

        *R_XDMAST( dmach ) = reloadcnt - 1;
        *R_XDMACON1( dmach ) = 1 << 18 | 1 << 12 | 1 << 4;

        *R_XDMAEN = 0x1;
        *R_XDMAIEN |= 0x1 << dmach;

        while( ( *R_XDMAIEN ) & ( 1 << dmach ) );

        U32 totaltx = ( bitwidth / 8 ) * 0x800 * reloadcnt;
        U32 remain = bytelen - totaltx;

        if( remain )
        {
            memcpy( ( U8* )dest + totaltx, ( U8* )src + totaltx, remain );
        }
    }
    else
    {
        conval |= transfer_size << 20;
        *R_XDMACON0( dmach ) = conval;
        *R_XDMACON1( dmach ) = 0;

        *R_XDMAEN = 0x1;
        *R_XDMAIEN = 0x1 << dmach;
        while( ( ( *R_XDMAIEN ) & ( 1 << dmach ) ) );
        U32 totaltx = ( bitwidth / 8 ) * transfer_size;
        U32 remain = bytelen - totaltx;
        if( remain )
        {
            memcpy( ( U8* )dest + totaltx, ( U8* )src + totaltx, remain );
        }
    }

    set_free_axi_dma( dmach );
    return TRUE;

}

static BOOL dma_axi_blockcpy( void* dest, void* src, U32 destpitch, U32 srcpitch, U32 w, U32 h )
{
    U32 conval = 0;
    int dmach = get_free_axi_dma();
    if( dmach < 0 )
        return FALSE;
    U32 transfer_size = w;
    U32 srcoffset = srcpitch - w;
    U32 destoffset = destpitch - w;

    *R_XDMASRC( dmach ) = ( U32 )src;
    *R_XDMADST( dmach ) = ( U32 )dest;
    *R_XDMASCOA( dmach ) = srcoffset;
    *R_XDMADCOA( dmach ) = destoffset;

    // block count
    *R_XDMAST( dmach ) = h - 1;

    //increment mode
    conval |= ( 1 | ( 1 << 8 ) );
    conval |= transfer_size << 20;
    *R_XDMACON0( dmach ) = conval;
    *R_XDMACON1( dmach ) = ( 1 << 18 ) | ( 1 << 12 ) | ( 1 << 4 );

    *R_XDMAEN = 0x1;
    *R_XDMAIEN |= 0x1 << dmach;

    while( ( *R_XDMAIEN ) & ( 1 << dmach ) );

    set_free_axi_dma( dmach );
    return TRUE;
}

BOOL dma_ahb_memcpy( void* dest, void* src, U32 bytelen )
{
    U32 conval = 0;
    int dmach = get_free_ahb_dma();
    if( dmach < 0 )
    {
        debugstring( "All DMA Channel is busy\r\n" );
        return FALSE;
    }
    U32 bitwidth = 8;
    U32 transfer_size = bytelen;
    //dcache_invalidate_way();
    if( bytelen > 4 )
    {
        if( ( ( U32 )dest & 0x3 ) == 0 ) //32bit
        {
            conval |= 1 << 25;
        }
        else if( ( ( U32 )dest & 0x1 ) == 0 ) //16bit
        {
            conval |= 1 << 24;
        }

        if( ( ( U32 )src & 0x3 ) == 0 ) //32bit
        {
            conval |= 1 << 21;
            transfer_size = bytelen >> 2;
            bitwidth = 32;
        }
        else if( ( ( U32 )src & 0x1 ) == 0 ) //16bit
        {
            conval |= 1 << 20;
            transfer_size = bytelen >> 1;
            bitwidth = 16;
        }
    }
    conval |= DMA_CON_SRC_BST_256;
    conval |= DMA_CON_DST_BST_256;

    //increment mode
    conval |= ( 1 << 29 );
    conval |= ( 1 << 28 );

    *R_DMASRC( dmach ) = ( U32 )src;
    *R_DMADST( dmach ) = ( U32 )dest;

    U32 reloadcnt = transfer_size >> 11; // devide by 0x800
    if( reloadcnt )
    {
        //use auto reload
        conval |= 0x800;
        *R_DMASGA( dmach ) = 0;
        *R_DMADSA( dmach ) = 0;
        *R_DMAARCNT( dmach ) = reloadcnt - 1;
        *R_DMACON( dmach ) = conval;
        *R_DMACFG( dmach ) = 1;
        while( ( *R_DMACFG( dmach ) ) & 1 );
        set_free_ahb_dma( dmach );

        U32 totaltx = ( bitwidth / 8 ) * 0x800 * reloadcnt;
        U32 remain = bytelen - totaltx;
        if( remain )
        {
            memcpy( dest + totaltx, src + totaltx, remain );
        }
    }
    else
    {
        conval |= transfer_size;
        *R_DMACON( dmach ) = conval;
        *R_DMACFG( dmach * 8 ) = 1;
        while( ( *R_DMACFG( dmach ) ) & 1 );
        set_free_ahb_dma( dmach );

        U32 totaltx = ( bitwidth / 8 ) * transfer_size;
        U32 remain = bytelen - totaltx;
        if( remain )
        {
            memcpy( dest + totaltx, src + totaltx, remain );
        }
    }
    return TRUE;
}
BOOL dma_ahb_blockcpy( void* dest, void* src, U32 destpitch, U32 srcpitch, U32 w, U32 h )
{
    U32 conval = 0;
    U32 transfer_size = w;
    U32 srcoffset;
    U32 destoffset;
    U32 bitwidth = 8;

    int dmach = get_free_ahb_dma();
    if( dmach < 0 )
    {
        debugstring( "All DMA is busy\r\n" );
        return FALSE;
    }
    srcoffset = srcpitch - w;
    destoffset = destpitch - w;
    if( ( w & 3 ) == 0 )
    {
        if( ( ( U32 )dest & 0x3 ) == 0 ) //32bit
        {
            conval |= 1 << 25;
        }
        else if( ( ( U32 )dest & 0x1 ) == 0 ) //16bit
        {
            conval |= 1 << 24;
        }

        if( ( ( U32 )src & 0x3 ) == 0 ) //32bit
        {
            conval |= 1 << 21;
            transfer_size = w >> 2;
            bitwidth = 32;
        }
        else if( ( ( U32 )src & 0x1 ) == 0 ) //16bit
        {
            conval |= 1 << 20;
            transfer_size = w >> 1;
            bitwidth = 16;
        }
    }
    else if( ( w & 1 ) == 0 )
    {
        if( ( ( U32 )dest & 0x1 ) == 0 ) //16bit
        {
            conval |= 1 << 24;
        }
        if( ( ( U32 )src & 0x1 ) == 0 ) //16bit
        {
            conval |= 1 << 20;
            transfer_size = w >> 1;
            bitwidth = 16;
        }
    }
    /*
    if(((U32)src & 0xf ) == 0)
    	conval |= DMA_CON_SRC_BST_256;
    else
    	conval |= DMA_CON_SRC_BST_1;
    	*/
    conval |= DMA_CON_SRC_BST_256;

    if( ( ( U32 )dest & 0xf ) == 0 )
        conval |= DMA_CON_DST_BST_256;
    else
        conval |= DMA_CON_DST_BST_1;

    conval |= ( 3 << 28 );
    conval |= ( transfer_size & 0xfff );

    *R_DMASRC( dmach ) = ( U32 )src;
    *R_DMADST( dmach ) = ( U32 )dest;
    *R_DMASGA( dmach ) = srcoffset;
    *R_DMADSA( dmach ) = destoffset;
    *R_DMAARCNT( dmach ) = h - 1;
    *R_DMACON( dmach ) = conval;
    *R_DMACFG( dmach ) = 1;
    while( ( *R_DMACFG( dmach ) ) & 1 );
    set_free_ahb_dma( dmach );
    return TRUE;
}

BOOL dma_memcpy( void * dest, void* src, U32 bytelen )
{
    dcache_invalidate_way();
    return dma_axi_memcpy( dest, src, bytelen );
}

BOOL dma_blockcpy( void* dest, void* src, U32 destpitch, U32 srcpitch, U32 w, U32 h )
{
    dcache_invalidate_way();
    return dma_axi_blockcpy( dest, src, destpitch, srcpitch, w, h );
}
