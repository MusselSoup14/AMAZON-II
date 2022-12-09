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
/**
 * \file
 *
 * \brief JPEG Image H/W Driver
 */
#include "sdk.h"
#include <setjmp.h>
#include <libjpg/jpeglib.h>
#include "image/loadjpg_hw.h"

#define STARTCMD_422 1
#define STARTCMD_420 3


/*MJPEG Decoder registers*/
#define R_JPEG_YADDR		((volatile unsigned int *) 0xf0010228)
#define R_JPEG_UADDR		((volatile unsigned int *) 0xf001022C)
#define R_JPEG_VADDR		((volatile unsigned int *) 0xf0010230)
#define R_JPEG_WIDTH		((volatile unsigned int *) 0xf0010234)
#define R_JPEG_HEIGTH		((volatile unsigned int *) 0xf0010238)
#define R_JPEG_QSC			((volatile unsigned int *) 0xf001023C)
#define R_JPEG_CMDCON		((volatile unsigned int *) 0xf0010240)
#define R_JPEG_JDSTAT		((volatile unsigned int *) 0xf0018000)
#define R_JPEG_IRQSTAT		((volatile unsigned int *) 0xf0018004)
#define R_JPEG_JDDFSTAT		((volatile unsigned int *) 0xf0018008)
#define R_JPEG_JDENA		((volatile unsigned int *) 0xf0018010)
#define R_JPEG_JDFCLR		((volatile unsigned int *) 0xf0018014)
#define R_JPEG_JDFCON		((volatile unsigned int *) 0xf0018018)
#define R_JPEG_JDWCON		((volatile unsigned int *) 0xf001801c)
#define R_JPEG_JDYOUT		((volatile unsigned int *) 0xf0018020)
#define R_JPEG_JDSRST		((volatile unsigned int *) 0xf0018024)
#define R_JPEG_JDVER		((volatile unsigned int *) 0xf0018028)
#define R_JPEG_JDRGBOUT		((volatile unsigned int *) 0xf001802c)
#define R_JPEG_JDLPITCH		((volatile unsigned int *) 0xf0018030)
#define R_JPEG_RGB565		((volatile unsigned int *) 0xf0018034)
#define R_JPEG_TIMEOUT_CNT	((volatile unsigned int *) 0xf0018038)
#define R_JPEG_TIMEOUT_CLR	((volatile unsigned int *) 0xf001803C)
#define R_JPEG_JDIDF		((volatile unsigned int *) 0xf0019000)

#define JPEG_RESET()	do{*R_JPEG_JDSRST=0;*R_JPEG_JDSRST=1;}while(0)

static const int yac_node[] = {     
	0x00000401 , 0x00020300 , 0x00000a03 , 0x00000902 , 0x00020903 , 0x00001006 , 0x00000f05 , 0x00020f06 , 0x00001609 , 0x00001508 ,
	0x00021509 , 0x00001e0c , 0x00001c0d , 0x0002190b , 0x00021d0d , 0x00002810 , 0x00002411 , 0x0002210f , 0x00002711 , 0x00022712 ,
	0x00003215 , 0x00002e16 , 0x00022b14 , 0x00003116 , 0x00023117 , 0x00003c1a , 0x0000381b , 0x00023519 , 0x00003b1b , 0x00023b1c , 
	0x0000481f , 0x00004220 , 0x00023f1e , 0x00004622 , 0x00024320 , 0x00024722 , 0x0000c625 , 0x00008826 , 0x00006a27 , 0x00005c28 , 
	0x00005629 , 0x00005524 , 0x00024d25 , 0x00005a2c , 0x00025127 , 0x00025529 , 0x0000642f , 0x00006230 , 0x0002592b , 0x00025d2d , 
	0x00006833 , 0x0002612f , 0x00026531 , 0x00007a36 , 0x00007437 , 0x00007238 , 0x00026933 , 0x00026d35 , 0x0000783b , 0x00027137 , 
	0x00027539 , 0x0000823e , 0x0000803f , 0x0002793b , 0x00027d3d , 0x00008642 , 0x0002813f , 0x00028541 , 0x0000a845 , 0x00009a46 , 
	0x00009447 , 0x00009248 , 0x00028943 , 0x00028d45 , 0x0000984b , 0x00029147 , 0x00029549 , 0x0000a24e , 0x0000a04f , 0x0002994b , 
	0x00029d4d , 0x0000a652 , 0x0002a14f , 0x0002a551 , 0x0000b855 , 0x0000b256 , 0x0000b057 , 0x0002a953 , 0x0002ad55 , 0x0000b65a , 
	0x0002b157 , 0x0002b559 , 0x0000c05d , 0x0000be5e , 0x0002b95b , 0x0002bd5d , 0x0000c461 , 0x0002c15f , 0x0002c561 , 0x00010664 ,
	0x0000e865 , 0x0000da66 , 0x0000d467 , 0x0000d268 , 0x0002c963 , 0x0002cd65 , 0x0000d86b , 0x0002d167 , 0x0002d569 , 0x0000e26e ,
	0x0000e06f , 0x0002d96b , 0x0002dd6d , 0x0000e672 , 0x0002e16f , 0x0002e571 , 0x0000f875 , 0x0000f276 , 0x0000f077 , 0x0002e973 , 
	0x0002ed75 , 0x0000f67a , 0x0002f177 , 0x0002f579 , 0x0001007d , 0x0000fe7e , 0x0002f97b , 0x0002fd7d , 0x00010481 , 0x0003017f , 
	0x00030581 , 0x00012684 , 0x00011885 , 0x00011286 , 0x00011087 , 0x00030983 , 0x00030d85 , 0x0001168a , 0x00031187 , 0x00031589 , 
	0x0001208d , 0x00011e8e , 0x0003198b , 0x00031d8d , 0x00012491 , 0x0003218f , 0x00032591 , 0x00013694 , 0x00013095 , 0x00012e96 , 
	0x00032993 , 0x00032d95 , 0x00013499 , 0x00033197 , 0x00033599 , 0x00013e9c , 0x00013c9d , 0x0003399b , 0x00033d9d , 0x000142a0 ,
	0x0003419f , 0x000001a1 };

static const int uvac_node[] = {    
	0x00000401 , 0x00020300 , 0x00000a03 , 0x00000902 , 0x00020903 , 0x00001206 , 0x00001007 , 0x00020d05 , 0x00021107 , 0x00001a0a , 
	0x0000180b , 0x00021509 , 0x0002190b , 0x0000240e , 0x0000200f , 0x00021d0d , 0x0000230f , 0x00022310 , 0x00003013 , 0x00002a14 , 
	0x00022712 , 0x00002e16 , 0x00022b14 , 0x00022f16 , 0x00003a19 , 0x0000361a , 0x00023318 , 0x0000391a , 0x0002391b , 0x0000441e , 
	0x0000401f , 0x00023d1d , 0x0000431f , 0x00024320 , 0x00005023 , 0x00004a24 , 0x00024722 , 0x00004e26 , 0x00024b24 , 0x00024f26 ,
	0x0000c629 , 0x0000882a , 0x00006a2b , 0x00005c2c , 0x00005b28 , 0x00025529 , 0x0000642f , 0x00006230 , 0x0002592b , 0x00025d2d ,
	0x00006833 , 0x0002612f , 0x00026531 , 0x00007a36 , 0x00007437 , 0x00007238 , 0x00026933 , 0x00026d35 , 0x0000783b , 0x00027137 , 
	0x00027539 , 0x0000823e , 0x0000803f , 0x0002793b , 0x00027d3d , 0x00008642 , 0x0002813f , 0x00028541 , 0x0000a845 , 0x00009a46 ,
	0x00009447 , 0x00009248 , 0x00028943 , 0x00028d45 , 0x0000984b , 0x00029147 , 0x00029549 , 0x0000a24e , 0x0000a04f , 0x0002994b , 
	0x00029d4d , 0x0000a652 , 0x0002a14f , 0x0002a551 , 0x0000b855 , 0x0000b256 , 0x0000b057 , 0x0002a953 , 0x0002ad55 , 0x0000b65a , 
	0x0002b157 , 0x0002b559 , 0x0000c05d , 0x0000be5e , 0x0002b95b , 0x0002bd5d , 0x0000c461 , 0x0002c15f , 0x0002c561 , 0x00010664 , 
	0x0000e865 , 0x0000da66 , 0x0000d467 , 0x0000d268 , 0x0002c963 , 0x0002cd65 , 0x0000d86b , 0x0002d167 , 0x0002d569 , 0x0000e26e , 
	0x0000e06f , 0x0002d96b , 0x0002dd6d , 0x0000e672 , 0x0002e16f , 0x0002e571 , 0x0000f875 , 0x0000f276 , 0x0000f077 , 0x0002e973 , 
	0x0002ed75 , 0x0000f67a , 0x0002f177 , 0x0002f579 , 0x0001007d , 0x0000fe7e , 0x0002f97b , 0x0002fd7d , 0x00010481 , 0x0003017f , 
	0x00030581 , 0x00012684 , 0x00011885 , 0x00011286 , 0x00011087 , 0x00030983 , 0x00030d85 , 0x0001168a , 0x00031187 , 0x00031589 , 
	0x0001208d , 0x00011e8e , 0x0003198b , 0x00031d8d , 0x00012491 , 0x0003218f , 0x00032591 , 0x00013694 , 0x00013095 , 0x00012e96 , 
	0x00032993 , 0x00032d95 , 0x00013499 , 0x00033197 , 0x00033599 , 0x00013e9c , 0x00013c9d , 0x0003399b , 0x00033d9d , 0x000142a0 , 
	0x0003419f , 0x000001a1 };

static const int ydc_node[] = { 0x00000601 , 0x00000500 , 0x00020501 , 0x00000a04 , 0x00020903 , 0x00000d05 ,
								0x00000f06 , 0x00001107 , 0x00001308 , 0x00001509 , 0x0000170a , 0x0000010b };

static const int uvdc_node[] = {    0x00000401 , 0x00020300 , 0x00000702 , 0x00000903 , 0x00000b04 , 0x00000d05
							, 0x00000f06 , 0x00001107 , 0x00001308 , 0x00001509 , 0x0000170a, 0x0000010b };

static const int yac_leaf[] = {     
	0x00000010 , 0x00000020 , 0x00000030 , 0x00000000 , 0x00000040 , 0x00000011 , 0x00000050 , 0x00000021 , 0x00000012 , 0x00000013 , 
	0x00000014 , 0x00000060 , 0x00000031 , 0x00000015 , 0x00000016 , 0x00000070 , 0x00000022 , 0x00000017 , 0x00000041 , 0x00000023 ,
	0x00000018 , 0x00000019 , 0x0000001a , 0x00000080 , 0x00000032 , 0x00000024 , 0x0000001b , 0x0000001c , 0x00000051 , 0x00000025 ,
	0x0000001d , 0x0000000f , 0x00000042 , 0x00000033 , 0x00000026 , 0x00000027 , 0x00000028 , 0x00000090 , 0x000000a0 , 0x00000061 , 
	0x00000071 , 0x00000081 , 0x00000091 , 0x000000a1 , 0x00000052 , 0x00000062 , 0x00000072 , 0x00000082 , 0x00000092 , 0x000000a2 , 
	0x00000043 , 0x00000053 , 0x00000063 , 0x00000073 , 0x00000083 , 0x00000093 , 0x000000a3 , 0x00000034 , 0x00000044 , 0x00000054 , 
	0x00000064 , 0x00000074 , 0x00000084 , 0x00000094 , 0x000000a4 , 0x00000035 , 0x00000045 , 0x00000055 , 0x00000065 , 0x00000075 , 
	0x00000085 , 0x00000095 , 0x000000a5 , 0x00000036 , 0x00000046 , 0x00000056 , 0x00000066 , 0x00000076 , 0x00000086 , 0x00000096 , 
	0x000000a6 , 0x00000037 , 0x00000047 , 0x00000057 , 0x00000067 , 0x00000077 , 0x00000087 , 0x00000097 , 0x000000a7 , 0x00000038 , 
	0x00000048 , 0x00000058 , 0x00000068 , 0x00000078 , 0x00000088 , 0x00000098 , 0x000000a8 , 0x00000029 , 0x00000039 , 0x00000049 , 
	0x00000059 , 0x00000069 , 0x00000079 , 0x00000089 , 0x00000099 , 0x000000a9 , 0x0000002a , 0x0000003a , 0x0000004a , 0x0000005a ,
	0x0000006a , 0x0000007a , 0x0000008a , 0x0000009a , 0x000000aa , 0x0000002b , 0x0000003b , 0x0000004b , 0x0000005b , 0x0000006b ,
	0x0000007b , 0x0000008b , 0x0000009b , 0x000000ab , 0x0000002c , 0x0000003c , 0x0000004c , 0x0000005c , 0x0000006c , 0x0000007c , 
	0x0000008c , 0x0000009c , 0x000000ac , 0x0000002d , 0x0000003d , 0x0000004d , 0x0000005d , 0x0000006d , 0x0000007d , 0x0000008d , 
	0x0000009d , 0x000000ad , 0x0000001e , 0x0000002e , 0x0000003e , 0x0000004e , 0x0000005e , 0x0000006e , 0x0000007e , 0x0000008e ,
	0x0000009e , 0x000000ae , 0x0000001f , 0x0000002f , 0x0000003f , 0x0000004f , 0x0000005f , 0x0000006f , 0x0000007f , 0x0000008f ,
	0x0000009f , 0x000000af};

static const int uvac_leaf[] = {    
	0x00000000 , 0x00000010 , 0x00000020 , 0x00000030 , 0x00000011 , 0x00000040 , 0x00000050 , 0x00000012 , 0x00000013 , 0x00000060 ,
	0x00000021 , 0x00000014 , 0x00000015 , 0x00000070 , 0x00000016 , 0x00000017 , 0x00000031 , 0x00000022 , 0x00000023 , 0x00000018 ,
	0x00000080 , 0x00000041 , 0x00000024 , 0x00000019 , 0x0000001a , 0x0000001b , 0x0000001c , 0x00000090 , 0x00000032 , 0x00000033 , 
	0x00000025 , 0x0000000f , 0x00000051 , 0x00000026 , 0x00000027 , 0x0000001d , 0x000000a0 , 0x00000061 , 0x00000042 , 0x00000043 , 
	0x0000001e , 0x00000052 , 0x0000001f , 0x00000071 , 0x00000081 , 0x00000091 , 0x000000a1 , 0x00000062 , 0x00000072 , 0x00000082 , 
	0x00000092 , 0x000000a2 , 0x00000053 , 0x00000063 , 0x00000073 , 0x00000083 , 0x00000093 , 0x000000a3 , 0x00000034 , 0x00000044 ,
	0x00000054 , 0x00000064 , 0x00000074 , 0x00000084 , 0x00000094 , 0x000000a4 , 0x00000035 , 0x00000045 , 0x00000055 , 0x00000065 ,
	0x00000075 , 0x00000085 , 0x00000095 , 0x000000a5 , 0x00000036 , 0x00000046 , 0x00000056 , 0x00000066 , 0x00000076 , 0x00000086 , 
	0x00000096 , 0x000000a6 , 0x00000037 , 0x00000047 , 0x00000057 , 0x00000067 , 0x00000077 , 0x00000087 , 0x00000097 , 0x000000a7 , 
	0x00000028 , 0x00000038 , 0x00000048 , 0x00000058 , 0x00000068 , 0x00000078 , 0x00000088 , 0x00000098 , 0x000000a8 , 0x00000029 ,
	0x00000039 , 0x00000049 , 0x00000059 , 0x00000069 , 0x00000079 , 0x00000089 , 0x00000099 , 0x000000a9 , 0x0000002a , 0x0000003a , 
	0x0000004a , 0x0000005a , 0x0000006a , 0x0000007a , 0x0000008a , 0x0000009a , 0x000000aa , 0x0000002b , 0x0000003b , 0x0000004b , 
	0x0000005b , 0x0000006b , 0x0000007b , 0x0000008b , 0x0000009b , 0x000000ab , 0x0000002c , 0x0000003c , 0x0000004c , 0x0000005c ,
	0x0000006c , 0x0000007c , 0x0000008c , 0x0000009c , 0x000000ac , 0x0000002d , 0x0000003d , 0x0000004d , 0x0000005d , 0x0000006d , 
	0x0000007d , 0x0000008d , 0x0000009d , 0x000000ad , 0x0000002e , 0x0000003e , 0x0000004e , 0x0000005e , 0x0000006e , 0x0000007e ,
	0x0000008e , 0x0000009e , 0x000000ae , 0x0000002f , 0x0000003f , 0x0000004f , 0x0000005f , 0x0000006f , 0x0000007f , 0x0000008f ,
	0x0000009f , 0x000000af };

static const int ydc_leaf[] = {     0x00000000 , 0x00000010 , 0x00000020 , 0x00000030 , 0x00000040 , 0x00000050 , 0x00000060 , 0x00000070 , 0x00000080 , 0x00000090 , 0x000000a0
                                    , 0x000000b0 };

static const int uvdc_leaf[] = {    0x00000000 , 0x00000010 , 0x00000020 , 0x00000030 , 0x00000040 , 0x00000050 , 0x00000060 , 0x00000070 , 0x00000080 , 0x00000090 , 0x000000a0
                                    , 0x000000b0  };



#define ENDIRQ_CLEAR 4

#define HALF_LEVEL	128 //bytes , FIFO depth 256byte


static void JPEGDec_Init()
{
	
    //MJPEG Core Control Setting
    *R_JPEG_JDENA  = 1;
    *R_JPEG_JDFCLR = 8;
    *R_JPEG_JDFCLR = 0;
    *R_JPEG_JDFCON = HALF_LEVEL / 4;
#if 1
    *R_JPEG_JDWCON = 8;
#else

    while ( 1 )
    {
        if ( *R_JPEG_JDWCON < 8 ); //wait fifo level setting

        break;
    }

#endif
  
    //MJPEG Core Huffman Coding Table Setting
    //DC Table setting
	static bool bTableInit = false;
	if (bTableInit == false)
	{
		memcpy((void *)0xf0010800, ydc_node, 3 * 4 * 4);
		memcpy((void *)0xf0011000, ydc_leaf, 3 * 4 * 4);
		memcpy((void *)0xf0014800, uvdc_node, 3 * 4 * 4);
		memcpy((void *)0xf0015000, uvdc_leaf, 3 * 4 * 4);

		//AC Table Setting
		memcpy((void *)0xf0012800, yac_node, 10 * 4 * 16);
		*(volatile unsigned int *)0xf0012A84 = yac_node[160];
		*(volatile unsigned int *)0xf0012A88 = yac_node[161];
		memcpy((void *)0xf0013000, yac_leaf, 10 * 4 * 16);
		*(volatile unsigned int *)0xf0013284 = yac_leaf[160];
		*(volatile unsigned int *)0xf0013288 = yac_leaf[161];
		memcpy((void *)0xf0016800, uvac_node, 10 * 4 * 16);
		*(volatile unsigned int *)0xf0016a84 = uvac_node[160];
		*(volatile unsigned int *)0xf0016a88 = uvac_node[161];
		memcpy((void *)0xf0017000, uvac_leaf, 10 * 4 * 16);
		*(volatile unsigned int *)0xf0017284 = uvac_leaf[160];
		*(volatile unsigned int *)0xf0017288 = uvac_leaf[161];
		bTableInit = true;
	}

}

//#define DMA_FIFO_INPUT
#ifdef DMA_FIFO_INPUT
static void dmatransfer( void* src, int len )
{
    int dmach = get_free_ahb_dma();
    *R_DMASRC( dmach ) = ( U32 )src;
    *R_DMADST( dmach ) = ( U32 )0xf0019000;
    U32 conval = 0;
    conval |= DMA_CON_SRC_32BIT;//dest 32bit
    conval |= DMA_CON_DST_32BIT;//src 32bit

    //conval |= DMA_CON_SRC_BST_256;
    conval |= DMA_CON_SRC_BST_1;
    conval |= DMA_CON_DST_BST_1;//NO burst , APB do not supports fixed burst mode
    conval |= DMA_CON_SRC_INCR;

    //must be 32
    U32 transfersize = 32;

    if ( ( len >> 2 ) < 0xfff )
    {
        conval |= ( len >> 2 );
    }
    else
    {
        U32 reloadcnt = ( ( len >> 2 ) / transfersize ) - 1;
        conval |= transfersize;

        *R_DMASGA( dmach ) = 0;
        *R_DMADSA( dmach ) = 0;
        *R_DMAARCNT( dmach ) = reloadcnt;
    }
    *R_DMACON( dmach ) = conval;
    *R_DMACFG( dmach ) = 1;
    while ( *R_DMACFG( dmach ) & 1 );
    set_free_ahb_dma( dmach );
}
#endif

#define JPEG_TIMEOUT_CNT 0x400000
/**
 * Convert a JPEG to RGB565 or RGB888 with H/W Engine
 *
 * \param imageaddr The address of a JPEG data.
 * \param len	    The length of data.
 * \param outputbuf Pointer to store [RGB565 or RGB888].
 * \param lpitch    The length of line.
 * \param bpp	    The bits per pixel.
 *
 * \return true if it succeeds, false if it fails.
 */
BOOL jpeg_hw_decode_raw ( U32 imageaddr, U32 len, U32 outputbuf, U32 lpitch, U32 bpp )
{
    U32 endaddr = imageaddr + len;
    JPEGDec_Init();
    // set output address
    *R_JPEG_JDRGBOUT = outputbuf;
    *R_JPEG_JDLPITCH = lpitch;
    *R_JPEG_QSC      =   64; //50%
    if ( bpp == 32 )
        *R_JPEG_RGB565 = 0 | ( 1 << 1 ); // (1<<1), timeout
    else
        *R_JPEG_RGB565 = 1 | ( 1 << 1 );

    *R_JPEG_TIMEOUT_CNT = JPEG_TIMEOUT_CNT;
    *R_JPEG_CMDCON       =   STARTCMD_420;

#ifdef DMA_FIFO_INPUT
    dcache_invalidate_way();
#endif
    BOOL re;

    while ( 1 )
    {
        U32 stat = *R_JPEG_IRQSTAT;
        if ( stat & ( 1 << 1 ) )
        {
            if ( imageaddr < endaddr )
            {
                int remain = endaddr - imageaddr;
                int k;
                if( remain >= HALF_LEVEL )
                {
#ifdef DMA_FIFO_INPUT
                    dmatransfer( ( void* )imageaddr, HALF_LEVEL );
                    imageaddr += HALF_LEVEL;
#else
                    if( imageaddr & 3 )
                    {
                        for ( k = 0; k < HALF_LEVEL ; k += 16 )
                        {
                            * ( volatile U16 * ) 0xf0019000 = * ( U16 * ) imageaddr;
                            * ( volatile U16 * ) 0xf0019000 = * ( U16 * ) ( imageaddr + 2 );
                            * ( volatile U16 * ) 0xf0019000 = * ( U16 * ) ( imageaddr + 4 );
                            * ( volatile U16 * ) 0xf0019000 = * ( U16 * ) ( imageaddr + 6 );
                            * ( volatile U16 * ) 0xf0019000 = * ( U16 * ) ( imageaddr + 8 );
                            * ( volatile U16 * ) 0xf0019000 = * ( U16 * ) ( imageaddr + 10 );
                            * ( volatile U16 * ) 0xf0019000 = * ( U16 * ) ( imageaddr + 12 );
                            * ( volatile U16 * ) 0xf0019000 = * ( U16 * ) ( imageaddr + 14 );
                            imageaddr += 16;
                        }
                    }
                    else
                    {
                        for ( k = 0; k < HALF_LEVEL ; k += 32 )
                        {
                            * ( volatile U32 * ) 0xf0019000 = * ( U32 * ) imageaddr;
                            * ( volatile U32 * ) 0xf0019000 = * ( U32 * ) ( imageaddr + 4 );
                            * ( volatile U32 * ) 0xf0019000 = * ( U32 * ) ( imageaddr + 8 );
                            * ( volatile U32 * ) 0xf0019000 = * ( U32 * ) ( imageaddr + 12 );
                            * ( volatile U32 * ) 0xf0019000 = * ( U32 * ) ( imageaddr + 16 );
                            * ( volatile U32 * ) 0xf0019000 = * ( U32 * ) ( imageaddr + 20 );
                            * ( volatile U32 * ) 0xf0019000 = * ( U32 * ) ( imageaddr + 24 );
                            * ( volatile U32 * ) 0xf0019000 = * ( U32 * ) ( imageaddr + 28 );
                            imageaddr += 32;
                        }
                    }
#endif
                }
                else
                {
                    if( imageaddr & 3 )
                    {
                        for ( k = 0; k < remain; k += 2 )
                        {
                            * ( volatile U32 * ) 0xf0019000 = * ( U16 * ) imageaddr;
                            imageaddr += 4;
                        }
                    }
                    else
                    {
                        for ( k = 0; k < remain; k += 4 )
                        {
                            * ( volatile U32 * ) 0xf0019000 = * ( U32 * ) imageaddr;
                            imageaddr += 4;
                        }
                    }
                }
                *R_JPEG_TIMEOUT_CNT = JPEG_TIMEOUT_CNT;
                continue;
            }
        }
        if ( stat & 1 ) // decode end
        {
            //debugstring("JPEG : decode end\r\n");
            re = TRUE;
            break;
        }
        if ( stat & ( 1 << 2 ) ) // timeout
        {
			*R_JPEG_TIMEOUT_CLR = 1; //timeout clear
			*R_JPEG_TIMEOUT_CLR = 0;
			if (endaddr <= imageaddr)
            {
                debugstring( "JPEG error : time out\r\n" );
                re = FALSE;
                break;
            }
            else //timeout count is too small
            {
                //debugstring("JPEG warning: time out, fifo full, retry\r\n");
                continue;
            }
        }
    }

    *R_JPEG_CMDCON = ENDIRQ_CLEAR;//clear
    *R_JPEG_JDFCLR = 8;
    *R_JPEG_JDFCLR = 0;
    *R_JPEG_CMDCON = 0;//clear
    *R_JPEG_JDENA = 0;
    *R_JPEG_JDSRST = 0;
    *R_JPEG_JDSRST = 1;

    return re;
}

// MARKER
#define SOI 0xD8
#define EOI 0xD9
#define APP0 0xE0
#define SOF 0xC0
#define DQT 0xDB
#define DHT 0xC4
#define SOS 0xDA
#define DRI 0xDD
#define COM 0xFE
#define JIFF	0x4649464a // JIFF little endian
/*
SOF0: Start Of Frame 0:
~~~~~~~~~~~~~~~~~~~~~~~

  - $ff, $c0 (SOF0)
  - length (high byte, low byte), 8+components*3
  - data precision (1 byte) in bits/sample, usually 8 (12 and 16 not
	supported by most software)
  - image height (2 bytes, Hi-Lo), must be >0 if DNL not supported
  - image width (2 bytes, Hi-Lo), must be >0 if DNL not supported
  - number of components (1 byte), usually 1 = grey scaled, 3 = color YCbCr
	or YIQ, 4 = color CMYK)
  - for each component: 3 bytes
	 - component id (1 = Y, 2 = Cb, 3 = Cr, 4 = I, 5 = Q)
	 - sampling factors (bit 0-3 vert., 4-7 hor.)
	 - quantization table number

 Remarks:
  - JFIF uses either 1 component (Y, greyscaled) or 3 components (YCbCr,
	sometimes called YUV, colour).

APP0: JFIF segment marker:
~~~~~~~~~~~~~~~~~~~~~~~~~~

  - $ff, $e0 (APP0)
  - length (high byte, low byte), must be >= 16
  - 'JFIF'#0 ($4a, $46, $49, $46, $00), identifies JFIF
  - major revision number, should be 1 (otherwise error)
  - minor revision number, should be 0..2 (otherwise try to decode anyway)
  - units for x/y densities:
	 0 = no units, x/y-density specify the aspect ratio instead
	 1 = x/y-density are dots/inch
	 2 = x/y-density are dots/cm
  - x-density (high byte, low byte), should be <> 0
  - y-density (high byte, low byte), should be <> 0
  - thumbnail width (1 byte)
  - thumbnail height (1 byte)
  - n bytes for thumbnail (RGB 24 bit), n = width*height*3

 Remarks:
  - If there's no 'JFIF'#0, or the length is < 16, then it is probably not
	a JFIF segment and should be ignored.
  - Normally units=0, x-dens=1, y-dens=1, meaning that the aspect ratio is
	1:1 (evenly scaled).
  - JFIF files including thumbnails are very rare, the thumbnail can usually
	be ignored.  If there's no thumbnail, then width=0 and height=0.
  - If the length doesn't match the thumbnail size, a warning may be
	printed, then continue decoding.

*/

/**
 * Load a surface from JPEG
 *
 * \param imageaddr The address of a JPEG data.
 * \param filesize  The length of data.
 *
 * \return null if it fails, else the new surface pointer.
 * \see release_surface
 */
SURFACE *loadjpgp_hw ( U32 imageaddr, U32 filesize )
{
    int i;
    U16 w = 0;
    U16 h = 0;
    U16 soflength;
    U8  ncomp;
    U8 bps;//data precision (1 byte) in bits/sample, usually 8
    U8 yc = 0;
    U8 startcmd;
    SURFACE *psurf = 0;
    U32 lpitch = 0;

    if ( ( ( * ( U8 * ) imageaddr ) != 0xFF ) || ( * ( U8 * ) ( imageaddr + 1 ) != 0xD8 ) )
    {
        DEBUGPRINTF ( "invalid or unsupported JPEG Format(0x%X%X)\r\n", * ( U8 * ) ( imageaddr + 1 ), * ( U8 * ) imageaddr );
        return 0;
    }

    for ( i = 2; i < filesize; i += 1 )
    {
        if ( * ( U8 * ) ( imageaddr + i ) == 0xff )
        {
            i++;

            //find SOF
            if ( * ( U8 * ) ( imageaddr + i ) == SOF )
            {
                soflength = ( ( U16 ) * ( U8 * ) ( imageaddr + i + 1 ) << 8 ) + ( U16 ) * ( U8 * ) ( imageaddr + i + 2 );
                bps = * ( U8 * ) ( imageaddr + i + 3 );
                h = ( ( U16 ) * ( U8 * ) ( imageaddr + i + 4 ) << 8 ) + ( U16 ) * ( U8 * ) ( imageaddr + i + 5 );
                w = ( ( U16 ) * ( U8 * ) ( imageaddr + i + 6 ) << 8 ) + ( U16 ) * ( U8 * ) ( imageaddr + i + 7 );
                ncomp = * ( U8 * ) ( imageaddr + i + 8 ) ;
                yc = * ( U8 * ) ( imageaddr + i + 10 ) ;
                //				DEBUGPRINTF("width : %d, height : %d \r\n",w,h);
                break;
            }
            /*
            //find EOI, end of image
            if ( * ( U8 * ) ( imageaddr + i ) == EOI )
            {
            	endaddr = imageaddr + i;
            	break;
            }
            */

        }
    }

    if ( i >= filesize )
        return 0;

    if ( w < 8 )
        return 0;

    if ( w == 0 || h == 0 )
    {
        DEBUGPRINTF ( "image width or height is 0\r\n" );
        return 0;
    }

    if ( yc == 0x22 )
    {
        startcmd = STARTCMD_420;
    }
    else if ( yc == 0x20 || yc == 0x21 )
    {
        startcmd = STARTCMD_422;
    }
    else
    {
        DEBUGPRINTF ( "invalid or unsupported JPEG Format(%#X)\r\n", * ( U16 * ) imageaddr );
        return 0;
    }

	//if you want more quality , set 32
	int bpp = get_screen_bpp();

    lpitch = ( w + 15 ) & 0xfffffff0; //in pixel
	int alignh = (h + 15) & 0xfffffff0; // align
	psurf = create_surface(lpitch, alignh, bpp);
	psurf->h = h;
	psurf->w = w;


    if ( jpeg_hw_decode_raw ( imageaddr, filesize, ( U32 ) psurf->pixels, psurf->pitch / (bpp/8), bpp ) == FALSE )
    {
        release_surface ( psurf );
        return NULL;
    }

    return psurf;
}

/**
 * Load a surface from JPEG
 *
 * \param [in] filename Pointer to a null-terminated string that specifies the path name of the image file(JPEG).
 *
 * \return null if it fails, else the new surface pointer.
 *
 * \code{.c}
 * SURFACE* surf = loadjpg_hw("test.jpg");
 * \endcode
 *
 * \see release_surface
 */
SURFACE *loadjpg_hw ( char *filename )
{
    U32 nRead;
    U32 filesize ;
    char *databuf;
    SURFACE *p;
    FIL fp;
    FRESULT res = f_open ( &fp, filename, FA_READ | FA_OPEN_EXISTING );

    if ( res != FR_OK )
    {
        DEBUGPRINTF ( "Cannot open : %s\r\n", filename );
        return 0;
    }

    filesize = f_size ( &fp );
    databuf = ( char * ) malloc ( filesize );
    f_read ( &fp, databuf, filesize, &nRead );
    f_close ( &fp );
    p = loadjpgp_hw ( ( U32 ) databuf, filesize );
    free ( databuf );
    return p;
}
