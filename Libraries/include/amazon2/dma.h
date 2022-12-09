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

#pragma once

#define AHB_DMA_CHANNEL_MAX 4
#define AHB_DMA_BASE	0xF0001800
#define AXI_DMA_CHANNEL_MAX 2
#define AXI_DMA_BASE	0xF4002c00
/*
	AHB DMA registers
*/
#define R_DMAIS		((volatile U32 *)(AHB_DMA_BASE+0))
#define R_DMATCIS	((volatile U32 *)(AHB_DMA_BASE+4))
#define R_DMATCIC	((volatile U32 *)(AHB_DMA_BASE+8))
#define R_DMAEIS	((volatile U32 *)(AHB_DMA_BASE+0xc))
#define R_DMAEIC	((volatile U32 *)(AHB_DMA_BASE+0x10))
#define R_DMABIS	((volatile U32 *)(AHB_DMA_BASE+0x14))
#define R_DMABIC	((volatile U32 *)(AHB_DMA_BASE+0x18))

#define R_DMARTCIS	((volatile U32 *)(AHB_DMA_BASE+0x1c))
#define R_DMACEIS	((volatile U32 *)(AHB_DMA_BASE+0x20))
#define R_DMAECS	((volatile U32 *)(AHB_DMA_BASE+0x24))
#define R_DMASBR	((volatile U32 *)(AHB_DMA_BASE+0x28))
#define R_DMASSR	((volatile U32 *)(AHB_DMA_BASE+0x2c))
#define R_DMASLBR	((volatile U32 *)(AHB_DMA_BASE+0x30))
#define R_DMASLSR	((volatile U32 *)(AHB_DMA_BASE+0x34))

#define R_DMASRC(ch)	((volatile U32 *)(AHB_DMA_BASE+0x100+(0x20*(ch))))

#define R_DMADST(ch)	((volatile U32 *)(AHB_DMA_BASE+0x104+(0x20*(ch))))

#define R_DMALLI(ch)	((volatile U32 *)(AHB_DMA_BASE+0x108+(0x20*(ch))))

#define R_DMACON(ch)	((volatile U32 *)(AHB_DMA_BASE+0x10c+(0x20*(ch))))

#define R_DMACFG(ch)	((volatile U32 *)(AHB_DMA_BASE+0x110+(0x20*(ch))))

#define R_DMASGA(ch)	((volatile U32 *)(AHB_DMA_BASE+0x114 + (0x20*(ch))))

#define R_DMADSA(ch)	((volatile U32 *)(AHB_DMA_BASE+0x118 + (0x20*(ch))))

#define R_DMAARCNT(ch)	((volatile U32 *)(AHB_DMA_BASE+0x11C+(0x20*(ch))))

#define DMA_CON_SRC_INCR	(1<<28)
#define DMA_CON_DST_INCR	(1<<29)

#define DMA_CON_DST_8BIT	(0)
#define DMA_CON_DST_16BIT	(1<<24)
#define DMA_CON_DST_32BIT	(1<<25)
#define DMA_CON_SRC_8BIT	(0)
#define DMA_CON_SRC_16BIT	(1<<20)
#define DMA_CON_SRC_32BIT	(1<<21)

#define DMA_CON_DST_BST_1	(0)
#define DMA_CON_DST_BST_4	(1<<16)
#define DMA_CON_DST_BST_8	(2<<16)
#define DMA_CON_DST_BST_16	(3<<16)
#define DMA_CON_DST_BST_32	(4<<16)
#define DMA_CON_DST_BST_64	(5<<16)
#define DMA_CON_DST_BST_128	(6<<16)
#define DMA_CON_DST_BST_256	(7<<16)

#define DMA_CON_SRC_BST_1	(0)
#define DMA_CON_SRC_BST_4	(1<<12)
#define DMA_CON_SRC_BST_8	(2<<12)
#define DMA_CON_SRC_BST_16	(3<<12)
#define DMA_CON_SRC_BST_32	(4<<12)
#define DMA_CON_SRC_BST_64	(5<<12)
#define DMA_CON_SRC_BST_128	(6<<12)
#define DMA_CON_SRC_BST_256	(7<<12)


typedef enum {
	DMA_PERI_NAND_TX=0,
	DMA_PERI_SDHC,
	DMA_PERI_NAND_RX,
	DMA_PERI_USB_DEVICE_TX,
	DMA_PERI_USB_DEVICE_RX,
	DMA_PERI_RES1,
	DMA_PERI_RES2,
	DMA_PERI_SNDCH0,
	DMA_PERI_SNDCH1,
	DMA_PERI_SNDREC,
}DMA_DEVICE;

#define _AXI_DMA_BASE_	0xF4002c00
#define R_XDMAEN	((volatile U32 *)_AXI_DMA_BASE_)
#define R_XDMAIEN	((volatile U32 *)(_AXI_DMA_BASE_+4))
#define R_XDMAIS	((volatile U32 *)(_AXI_DMA_BASE_+8))
#define R_XDMAIC	((volatile U32 *)(_AXI_DMA_BASE_+0xc))
#define R_XDMASH	((volatile U32 *)(_AXI_DMA_BASE_+0x10))
#define R_XDMASSBTR	((volatile U32 *)(_AXI_DMA_BASE_+0x14))
#define R_XDMADSBTR	((volatile U32 *)(_AXI_DMA_BASE_+0x18))
#define R_XDMASSSTR	((volatile U32 *)(_AXI_DMA_BASE_+0x1c))
#define R_XDMADSSTR	((volatile U32 *)(_AXI_DMA_BASE_+0x20))
#define R_XDMASSLTR	((volatile U32 *)(_AXI_DMA_BASE_+0x24))
#define R_XDMADSLTR	((volatile U32 *)(_AXI_DMA_BASE_+0x28))
#define R_XDMADSLTR	((volatile U32 *)(_AXI_DMA_BASE_+0x28))

#define R_XDMASRC(ch)	((volatile U32 *)(_AXI_DMA_BASE_+0x80+(ch*40)))
#define R_XDMADST(ch)	((volatile U32 *)(_AXI_DMA_BASE_+0x84+(ch*40)))
#define R_XDMALLP(ch)	((volatile U32 *)(_AXI_DMA_BASE_+0x88+(ch*40)))
#define R_XDMACON0(ch)	((volatile U32 *)(_AXI_DMA_BASE_+0x8c+(ch*40)))
#define R_XDMACON1(ch)	((volatile U32 *)(_AXI_DMA_BASE_+0x90+(ch*40)))
#define R_XDMASCOA(ch)	((volatile U32 *)(_AXI_DMA_BASE_+0x94+(ch*40)))
#define R_XDMADCOA(ch)	((volatile U32 *)(_AXI_DMA_BASE_+0x98+(ch*40)))
#define R_XDMAST(ch)	((volatile U32 *)(_AXI_DMA_BASE_+0x9c+(ch*40)))
#define R_XDMASG(ch)	((volatile U32 *)(_AXI_DMA_BASE_+0xa0+(ch*40)))
#define R_XDMADS(ch)	((volatile U32 *)(_AXI_DMA_BASE_+0xa4+(ch*40)))

typedef struct _tagAHB_DMA_LLI
{
	U32 src;
	U32 dest;
	struct _tagAHB_DMA_LLI *next; // 0 is last item
	U32 ctrl;
}AHB_DMA_LLI;

int get_free_axi_dma();//when DMA is not used anymore, should be call set_free_dma().
void set_free_axi_dma(int ch);

int get_free_ahb_dma();//when DMA is not used anymore, should be call set_free_dma().
void set_free_ahb_dma(int ch);

BOOL dma_memcpy(void* dest,void* src,U32 bytelen);
BOOL dma_blockcpy(void* dest,void* src,U32 destpitch,U32 srcpitch,U32 w,U32 h);

BOOL dma_ahb_memcpy(void* dest,void* src,U32 bytelen);
BOOL dma_ahb_blockcpy(void* dest,void* src,U32 destpitch,U32 srcpitch,U32 w,U32 h);
