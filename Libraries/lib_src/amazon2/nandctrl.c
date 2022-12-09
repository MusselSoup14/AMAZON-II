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

//#define SECTOR_SIZE_IS_PAGESIZE

//#define NAND_USE_DMA
//#define NAND_DEBUG
#ifdef NAND_DEBUG
#define NAND_DEBUGPRINTF	DEBUGPRINTF
#else
#define NAND_DEBUGPRINTF(...)
#endif


/* Cell info constants */
#define NAND_CI_CHIPNR_MSK      0x03
#define NAND_CI_CELLTYPE_MSK    0x0C

/*
* NAND Flash Manufacturer ID Codes
*/
#define NAND_MFR_TOSHIBA        0x98
#define NAND_MFR_SAMSUNG        0xec
#define NAND_MFR_FUJITSU        0x04
#define NAND_MFR_NATIONAL       0x8f
#define NAND_MFR_RENESAS        0x07
#define NAND_MFR_STMICRO        0x20
#define NAND_MFR_HYNIX          0xad
#define NAND_MFR_MICRON         0x2c
#define NAND_MFR_AMD            0x01
#define NAND_MFR_MACRONIX       0xc2
#define NAND_MFR_EON            0x92
#define NAND_MFR_EON2           0xC8
#define NAND_MFR_ATO            0x9b


struct nand_manufacturers
{
    int id;
    char *name;
};

struct nand_manufacturers nand_manuf_ids[] =
{
    {NAND_MFR_TOSHIBA, "Toshiba"},
    {NAND_MFR_SAMSUNG, "Samsung"},
    {NAND_MFR_FUJITSU, "Fujitsu"},
    {NAND_MFR_NATIONAL, "National"},
    {NAND_MFR_RENESAS, "Renesas"},
    {NAND_MFR_STMICRO, "ST Micro"},
    {NAND_MFR_HYNIX, "Hynix"},
    {NAND_MFR_MICRON, "Micron"},
    {NAND_MFR_AMD, "AMD/Spansion"},
    {NAND_MFR_MACRONIX, "Macronix"},
    {NAND_MFR_EON, "Eon"},
    {NAND_MFR_EON2, "Eon"},
    {NAND_MFR_ATO, "ATO"},
    {0x0, "Unknown"}
};

/**
* struct nand_flash_dev - NAND Flash Device ID Structure
* @name:       Identify the device type
* @id:         device ID code
* @pagesize:   Pagesize in bytes. Either 256 or 512 or 0
*              If the pagesize is 0, then the real pagesize
*              and the eraseize are determined from the
*              extended id bytes in the chip
* @erasesize:  Size of an erase block in the flash device.
* @chipsize:   Total chipsize in Mega Bytes
* @options:    Bitfield to store chip relevant options
*/
struct nand_flash_dev
{
    char *name;
    int id;
    unsigned long pagesize;
    unsigned long chipsize;//Mega-bytes
    unsigned long blocksize;//block size
};


struct nand_flash_dev nand_flash_ids[] =
{

    {"16MiB 1,8V 8-bit",       0x33, 512, 16, 0x4000},
    {"16MiB 3,3V 8-bit",       0x73, 512, 16, 0x4000},
    {"16MiB 1,8V 16-bit",      0x43, 512, 16, 0x4000},
    {"16MiB 3,3V 16-bit",      0x53, 512, 16, 0x4000},

    {"32MiB 1,8V 8-bit",       0x35, 512, 32, 0x4000},
    {"32MiB 3,3V 8-bit",       0x75, 512, 32, 0x4000},
    {"32MiB 1,8V 16-bit",      0x45, 512, 32, 0x4000},
    {"32MiB 3,3V 16-bit",      0x55, 512, 32, 0x4000},

    {"64MiB 1,8V 8-bit",       0x36, 512, 64, 0x4000},
    {"64MiB 3,3V 8-bit",       0x76, 512, 64, 0x4000},
    {"64MiB 1,8V 16-bit",      0x46, 512, 64, 0x4000},
    {"64MiB 3,3V 16-bit",      0x56, 512, 64, 0x4000},

    {"128MiB 1,8V 8-bit",      0x78, 512, 128, 0x4000},
    {"128MiB 1,8V 8-bit",      0x39, 512, 128, 0x4000},
    {"128MiB 3,3V 8-bit",      0x79, 512, 128, 0x4000},
    {"128MiB 1,8V 16-bit",     0x72, 512, 128, 0x4000},
    {"128MiB 1,8V 16-bit",     0x49, 512, 128, 0x4000},
    {"128MiB 3,3V 16-bit",     0x74, 512, 128, 0x4000},
    {"128MiB 3,3V 16-bit",     0x59, 512, 128, 0x4000},

    {"256MiB 3,3V 8-bit",      0x71, 512, 256, 0x4000},

    /*
    * These are the new chips with large page size. The pagesize and the
    * erasesize is determined from the extended id bytes
    */

    /* 512 Megabit */
    {"64MiB 1,8V 8-bit",       0xA2, 0,  64, 0, },
    {"64MiB 1,8V 8-bit",       0xA0, 0,  64, 0, },
    {"64MiB 3,3V 8-bit",       0xF2, 0,  64, 0, },
    {"64MiB 3,3V 8-bit",       0xD0, 0,  64, 0, },
    {"64MiB 3,3V 8-bit",       0xF0, 0,  64, 0, },
    {"64MiB 1,8V 16-bit",      0xB2, 0,  64, 0, },
    {"64MiB 1,8V 16-bit",      0xB0, 0,  64, 0, },
    {"64MiB 3,3V 16-bit",      0xC2, 0,  64, 0, },
    {"64MiB 3,3V 16-bit",      0xC0, 0,  64, 0, },

    /* 1 Gigabit */
    {"128MiB 1,8V 8-bit",      0xA1, 0, 128, 0, },
    {"128MiB 3,3V 8-bit",      0xF1, 0, 128, 0, },
    {"128MiB 3,3V 8-bit",      0xD1, 0, 128, 0, },
    {"128MiB 1,8V 16-bit",     0xB1, 0, 128, 0, },
    {"128MiB 3,3V 16-bit",     0xC1, 0, 128, 0, },
    {"128MiB 1,8V 16-bit",     0xAD, 0, 128, 0, },

    /* 2 Gigabit */
    {"256MiB 1,8V 8-bit",      0xAA, 0, 256, 0, },
    {"256MiB 3,3V 8-bit",      0xDA, 0, 256, 0, },
    {"256MiB 1,8V 16-bit",     0xBA, 0, 256, 0, },
    {"256MiB 3,3V 16-bit",     0xCA, 0, 256, 0, },

    /* 4 Gigabit */
    {"512MiB 1,8V 8-bit",      0xAC, 0, 512, 0, },
    {"512MiB 3,3V 8-bit",      0xDC, 0, 512, 0, },
    {"512MiB 1,8V 16-bit",     0xBC, 0, 512, 0, },
    {"512MiB 3,3V 16-bit",     0xCC, 0, 512, 0, },

    /* 8 Gigabit */
    {"1GiB 1,8V 8-bit",        0xA3, 0, 1024, 0,},
    {"1GiB 3,3V 8-bit",        0xD3, 0, 1024, 0,},
    {"1GiB 1,8V 16-bit",       0xB3, 0, 1024, 0,},
    {"1GiB 3,3V 16-bit",       0xC3, 0, 1024, 0,},

    /* 16 Gigabit */
    {"2GiB 1,8V 8-bit",        0xA5, 0, 2048, 0,},
    {"2GiB 3,3V 8-bit",        0xD5, 0, 2048, 0,},
    {"2GiB 1,8V 16-bit",       0xB5, 0, 2048, 0,},
    {"2GiB 3,3V 16-bit",       0xC5, 0, 2048, 0,},

    /* 32 Gigabit */
    {"4GiB 1,8V 8-bit",        0xA7, 0, 4096, 0,},
    {"4GiB 3,3V 8-bit",        0xD7, 0, 4096, 0,},
    {"4GiB 1,8V 16-bit",       0xB7, 0, 4096, 0,},
    {"4GiB 3,3V 16-bit",       0xC7, 0, 4096, 0,},

    /* 64 Gigabit */
    {"8GiB 1,8V 8-bit",        0xAE, 0, 8192, 0,},
    {"8GiB 3,3V 8-bit",        0xDE, 0, 8192, 0,},
    {"8GiB 1,8V 16-bit",       0xBE, 0, 8192, 0,},
    {"8GiB 3,3V 16-bit",       0xCE, 0, 8192, 0,},

    /* 128 Gigabit */
    {"16GiB 1,8V 8-bit",       0x1A, 0, 16384, 0},
    {"16GiB 3,3V 8-bit",       0x3A, 0, 16384, 0},
    {"16GiB 1,8V 16-bit",      0x2A, 0, 16384, 0},
    {"16GiB 3,3V 16-bit",      0x4A, 0, 16384, 0},

    /* 256 Gigabit */
    {"32GiB 1,8V 8-bit",       0x1C, 0, 32768, 0},
    {"32GiB 3,3V 8-bit",       0x3C, 0, 32768, 0},
    {"32GiB 1,8V 16-bit",      0x2C, 0, 32768, 0},
    {"32GiB 3,3V 16-bit",      0x4C, 0, 32768, 0},

    /* 512 Gigabit */
    {"64GiB 1,8V 8-bit",       0x1E, 0, 65536, 0},
    {"64GiB 3,3V 8-bit",       0x3E, 0, 65536, 0},
    {"64GiB 1,8V 16-bit",      0x2E, 0, 65536, 0},
    {"64GiB 3,3V 16-bit",      0x4E, 0, 65536, 0},
    {NULL,}
};

/*
parity size :
4bit ecc,7byte/512byte
24bit ecc, 336bit(44byte)/1 kbyte

ECC code offset : 8Kpage(spare436):84byte,2kpage(spare 64byte):20byte
*/
#define ECC_OFFSET_4 20
#define ECC_OFFSET_24 84

// ECC offset for H/W default
#define ECC_OFFSET_4_HW_DEFAULT 36
#define ECC_OFFSET_24_HW_DEFAULT 100


#define BLOCK2PAGE(BlockNum) (BlockNum * NandInfo.pageperblock)

volatile U32 g_dummy;//for dummy read

typedef struct _tagNand
{
    U8 maf_id;
    U8 dev_id;
    U8 cellinfo;
    U8 eccbit;
    U32 pagesize;
    U32 blocksize;
    U32 pageperblock;
    U32 blockcnt;
    U32 sparesize;
    U32 maxbadblockcnt;
    U32 rowaddrlen;
    U32 datastartblock; //file system area
    U32 datablockcnt;
} NANDTYPE;

static NANDTYPE NandInfo;


#define BI_GOOD	0xff //not used block
#define BI_USED	0x7f //allocated block
#define BI_NOTUSED	0x0f //need for erase to check bad?
#define BI_BAD 0x00 //can't use

typedef struct _tagSpare
{
    U8 LSN[3];
    U8 paritybit;
    U8 reserve; // reserve[0] is used parity
    U8 BadInfo;
    U8 Ecc[3]; //not used
    U8 SEcc[2]; //not used
    U8 reserve2[5];

} SPAREDATA; //small page


typedef struct _tagSpare2
{
    U8 BadInfo;
    U8 paritybit;
    U8 LSN[3]; // Logical Number
    U8 reserve2[3];
    U8 Ecc[3];//not used
    U8 SEcc[2];//not used
    U8 reserve3[3];
} SPAREDATA2; //large page

static U8* BlockInfoTable;
static U16* PhyBlockTable; // index is logical number, data is Phy num
static U16* LogBlockTable; // index is physical number, data is Log num

__inline__ static U32 nand_get_log_block(U32 phynum)
{
    return LogBlockTable[phynum];
}

static U8 sparebuf_for_lognum[16];

static void make_logical_data(int lognum)
{

    if (NandInfo.pagesize == 512)
    {
        SPAREDATA* sparedata = (SPAREDATA*)sparebuf_for_lognum;
        sparedata->BadInfo = 0xff;
        sparedata->LSN[0] = lognum;
        sparedata->LSN[1] = lognum >> 8;
        sparedata->LSN[2] = lognum >> 16;
        U8 paritybit = 0;
        int k;
        //check parity bit
        for (k = 0; k < 24; k++)
        {
            if (lognum & (1 << k))
            {
                if (paritybit)
                    paritybit = 0;
                else
                    paritybit = 1;
            }
        }
        sparedata->paritybit = paritybit;
    }
    else//2048
    {
        SPAREDATA2* sparedata = (SPAREDATA2*)sparebuf_for_lognum;
        sparedata->BadInfo = 0xff;
        sparedata->LSN[0] = lognum;
        sparedata->LSN[1] = lognum >> 8;
        sparedata->LSN[2] = lognum >> 16;
        U8 paritybit = 0;
        int k;
        //check parity bit
        for (k = 0; k < 24; k++)
        {
            if (lognum & (1 << k))
            {
                if (paritybit)
                    paritybit = 0;
                else
                    paritybit = 1;
            }
        }
        sparedata->paritybit = paritybit;
    }
}




#ifdef NAND_USE_DMA

// supports only 512,1024,2048,8192byte
static BOOL dma_read_nand(void* dest, U32 bytelen)
{
    U32 conval = 0;
    int dmach = get_free_ahb_dma();
    if (dmach < 0)
        return FALSE;

    dcache_invalidate_way();
    *R_NFMCON |= (1 << 8); //dma request

    U32 transfer_size = bytelen >> 2;
    conval |= DMA_CON_SRC_32BIT;//src 32bit

    if (((U32)dest & 0x3) == 0)
        conval |= DMA_CON_DST_32BIT;
    else if (((U32)dest & 0x1) == 0)
        conval |= DMA_CON_DST_16BIT;
    else
        conval |= DMA_CON_DST_8BIT;


    conval |= DMA_CON_SRC_BST_4;
    conval |= DMA_CON_DST_BST_256;

    conval |= DMA_CON_DST_INCR; //dest address increment

    *R_DMASGA(dmach) = 0;
    *R_DMADSA(dmach) = 0;
    *R_DMAARCNT(dmach) = 0;
    *R_DMASRC(dmach) = (U32)R_NFMDATA;
    *R_DMADST(dmach) = (U32)dest;
    conval |= (transfer_size & 0xfff);
    *R_DMACON(dmach) = conval;
    *R_DMACFG(dmach) = (DMA_PERI_NAND_RX << 4) | 1;

    while ((*R_DMACFG(dmach)) & 1);
    set_free_ahb_dma(dmach);
    return TRUE;
}

// supports only 512,1024,2048,8192byte

static BOOL dma_write_nand(void* src, U32 bytelen)
{
    U32 conval = 0;
    int dmach = get_free_ahb_dma();
    if (dmach < 0)
        return FALSE;

    dcache_invalidate_way();
    *R_NFMCON |= (1 << 8); //dma request

    U32 transfer_size;
    U32 loopcnt = 1;
    U32 onetimesize = bytelen;
    conval |= DMA_CON_DST_32BIT;//register

    if (((U32)src & 0x3) == 0)
    {
        conval |= DMA_CON_SRC_32BIT;
        transfer_size = bytelen >> 2;
    }
    else if (((U32)src & 0x1) == 0)
    {
        conval |= DMA_CON_SRC_16BIT;
        if (bytelen == 8192)
        {
            onetimesize = 4096;
            transfer_size = 2048;
            loopcnt = 2;
        }
        else
            transfer_size = bytelen >> 1;
    }
    else
    {
        conval |= DMA_CON_SRC_8BIT;
        if (bytelen == 8192)
        {
            transfer_size = 2048;
            onetimesize = 2048;
            loopcnt = 4;
        }
        else
            transfer_size = bytelen;
    }

    conval |= DMA_CON_SRC_BST_256;
    conval |= DMA_CON_DST_BST_256;


    conval |= DMA_CON_SRC_INCR; //src address increment
    conval |= (transfer_size & 0xfff);

    *R_DMASRC(dmach) = (U32)src;
    *R_DMADST(dmach) = (U32)R_NFMDATA;
    *R_DMASGA(dmach) = 0;
    *R_DMADSA(dmach) = 0;
    *R_DMAARCNT(dmach) = loopcnt - 1;
    *R_DMACON(dmach) = conval;
    *R_DMACFG(dmach) = (DMA_PERI_NAND_TX << 8) | 1;

    while ((*R_DMACFG(dmach)) & 1);
    set_free_ahb_dma(dmach);
    return TRUE;
}

#endif

static void nandbuf_read(U8* buf, int bytelen)
{
#ifdef NAND_USE_DMA
    BOOL re = FALSE;
    re = dma_read_nand(buf, bytelen);
    if (re == TRUE)
        return;
#endif
    int i;
    if (((U32)buf) & 1) //1byte boundary
    {
        for (i = 0; i < bytelen; )
        {
            buf[i] = *(volatile U8*)R_NFMDATA;
            buf[i + 1] = *(volatile U8*)R_NFMDATA;
            buf[i + 2] = *(volatile U8*)R_NFMDATA;
            buf[i + 3] = *(volatile U8*)R_NFMDATA;
            i += 4;
        }
    }
    else if (((U32)buf) & 2) //2byte boundary
    {
        for (i = 0; i < bytelen; )
        {
            *(U16*)(buf + i) = *(volatile U16*)R_NFMDATA;
            *(U16*)(buf + i + 2) = *(volatile U16*)*R_NFMDATA;
            *(U16*)(buf + i + 4) = *(volatile U16*)R_NFMDATA;
            *(U16*)(buf + i + 6) = *(volatile U16*)R_NFMDATA;
            i += 8;
        }
    }
    else
    {
        for (i = 0; i < bytelen; )
        {
            *(U32*)(buf + i) = *R_NFMDATA;
            *(U32*)(buf + i + 4) = *R_NFMDATA;
            *(U32*)(buf + i + 8) = *R_NFMDATA;
            *(U32*)(buf + i + 12) = *R_NFMDATA;
            i += 16;
        }
    }
}

static void nandbuf_write(U8* buf, int bytelen)
{
#ifdef NAND_USE_DMA
    BOOL re = FALSE;
    re = dma_write_nand(buf, bytelen);
    if (re == TRUE)
        return;
#endif
    int i;
    if (((U32)buf) & 1) //1byte boundary
    {
        for (i = 0; i < bytelen; )
        {
            *(volatile U8*)R_NFMDATA = buf[i];
            *(volatile U8*)R_NFMDATA = buf[i + 1];
            *(volatile U8*)R_NFMDATA = buf[i + 2];
            *(volatile U8*)R_NFMDATA = buf[i + 3];
            i += 4;
        }
    }
    else if (((U32)buf) & 2) //2byte boundary
    {
        U16* wbuf = (U16*)buf;
        for (i = 0; i < bytelen / 2; )
        {
            *(volatile U16*)R_NFMDATA = wbuf[i];
            *(volatile U16*)R_NFMDATA = wbuf[i + 1];
            *(volatile U16*)R_NFMDATA = wbuf[i + 2];
            *(volatile U16*)R_NFMDATA = wbuf[i + 3];
            i += 4;
        }
    }
    else
    {
        U32* lbuf = (U32*)buf;
        for (i = 0; i < bytelen / 4; )
        {
            *(volatile U32*)R_NFMDATA = lbuf[i];
            *(volatile U32*)R_NFMDATA = lbuf[i + 1];
            *(volatile U32*)R_NFMDATA = lbuf[i + 2];
            *(volatile U32*)R_NFMDATA = lbuf[i + 3];
            i += 4;
        }
    }
}

static bool wait_write_erase_done()
{
    //check busy level
    while (1)
    {
        U32 stat = *R_NFMSTAT;
        //check busy level or rising edge
        if ((stat & (1 << 1)) || (stat & (1 << 0)))
        {
            *(volatile U8*)R_NFMCMD = NAND_STATUS; // Command
            stat = *R_NFMDATA;
            if (stat & (1 << 6)) //check done
            {
                if (stat & 1)
                    return FALSE;
                else
                    return TRUE;
            }
        }
    }
}

static void WaitBusy()
{
    // Check Busy End
    while (!(*(volatile U8*)R_NFMSTAT & 0x01));// for "trr" timing,see Nand-Flash data sheet
    while (!(*(volatile U8*)R_NFMSTAT & 0x02));
    int i;
    for (i = 0; i < 1; i++);

}

__inline__ void nand_reset()
{
    *(volatile char *)R_NFMCMD = NAND_CMD_RESET; // Read
    WaitBusy();
}

__inline__ static void setpageaddress(U32 pagenum, U32 offset)
{
    int i;
    int k;
    U8 mask;
    U8 validbit;
    *(volatile U8 *)R_NFMADDR = offset & 0xff; // A0-A7
    if (NandInfo.pagesize != 512)
        *(volatile U8 *)R_NFMADDR = offset >> 8;

    for (i = 0; i < NandInfo.rowaddrlen; i += 8)
    {
        mask = 0;
        validbit = (NandInfo.rowaddrlen - i) > 8 ? 8 : (NandInfo.rowaddrlen - i);
        for (k = 0; k < validbit; k++)
            mask |= (1 << k);
        *(volatile U8 *)R_NFMADDR = (U8)(pagenum  & mask); // Row Address
        pagenum = pagenum >> 8;
    }
}
__inline__ static void seteraseblockaddr(U32 blockaddr)
{
    int i;
    int k;
    U8 mask;
    U8 validbit;
    for (i = 0; i < NandInfo.rowaddrlen; i += 8)
    {
        mask = 0;
        validbit = (NandInfo.rowaddrlen - i) > 8 ? 8 : (NandInfo.rowaddrlen - i);
        for (k = 0; k < validbit; k++)
            mask |= (1 << k);
        *(volatile U8 *)R_NFMADDR = (U8)(blockaddr  & mask); // Row Address
        blockaddr = blockaddr >> 8;
    }
}
__inline__ static void setspareaddr(U32 pagenum)
{
    if (NandInfo.pagesize == 512)
        setpageaddress(pagenum, 0);
    else
    {
        int i;
        int k;
        U8 mask;
        U8 validbit;
        *(volatile U8 *)R_NFMADDR = 0;
        if (NandInfo.pagesize != 512)
            *(volatile U8 *)R_NFMADDR = NandInfo.pagesize >> 8;

        for (i = 0; i < NandInfo.rowaddrlen; i += 8)
        {
            mask = 0;
            validbit = (NandInfo.rowaddrlen - i) > 8 ? 8 : (NandInfo.rowaddrlen - i);
            for (k = 0; k < validbit; k++)
                mask |= (1 << k);
            *(volatile U8 *)R_NFMADDR = (U8)(pagenum  & mask); // Row Address
            pagenum = pagenum >> 8;
        }
    }
}


BOOL nand_phy_readpage(U32 pageaddr, void* buffer)
{
    U8* buf = (U8*)buffer;
    if (NandInfo.eccbit == 24) //ECC
    {
        int j;
        for (j = 0; j < NandInfo.pagesize / 1024; j++, buf += 1024)
        {
            g_dummy = *R_NFMSTAT; //clear busy
            *(volatile char *)R_NFMCMD = NAND_CMD_READ0; // Read
            setpageaddress(pageaddr, j * 1024);
            *(volatile char *)R_NFMCMD = NAND_CMD_READSTART; // Read
            g_dummy = *R_NFMMLCECC(10);
            WaitBusy();
            //set  ecc location
            //check 0 block
            if (NandInfo.blocksize > (pageaddr*NandInfo.pagesize))
                *R_NFMSPADDR = NandInfo.pagesize + (j * 42) + ECC_OFFSET_24_HW_DEFAULT;
            else
                *R_NFMSPADDR = NandInfo.pagesize + (j * 4 * 11) + ECC_OFFSET_24;
            *R_NFMCON |= (1 << 16);

            while (1)
            {
                U32 stat = *R_NFMSTAT;
                if (stat & (1 << 2)) //wait for auto decoding
                {
                    if ((stat & (1 << 3)) == 0)
                    {
                        if (stat < (1 << 11))
                            DEBUGPRINTF("ECC Decoding error(NFSTAT:%#x)\r\n", stat);
                    }
#ifdef NAND_DEBUG
                    if ((stat >> 12) & 0x1f)
                    {
                        DEBUGPRINTF("error bit count %d ==> corrected\r\n", (stat >> 12) & 0x1f);
                    }
#endif
                    break;
                }
            }

            U32 i;

            if ((U32)(&buf[0]) & 1) //1byte boundary
            {
                for (i = 0; i < 1024; )
                {
                    buf[i] = *(volatile U8*)R_NFMECD;
                    buf[i + 1] = *(volatile U8*)R_NFMECD;
                    buf[i + 2] = *(volatile U8*)R_NFMECD;
                    buf[i + 3] = *(volatile U8*)R_NFMECD;
                    i += 4;
                }
            }
            else if ((U32)(&buf[0]) & 2) //2byte boundary
            {
                for (i = 0; i < 1024; )
                {
                    *(U16*)(buf + i) = *(volatile U16*)R_NFMECD;
                    *(U16*)(buf + i + 2) = *(volatile U16*)R_NFMECD;
                    *(U16*)(buf + i + 4) = *(volatile U16*)R_NFMECD;
                    *(U16*)(buf + i + 6) = *(volatile U16*)R_NFMECD;
                    i += 8;
                }
            }
            else
            {
                for (i = 0; i < 1024; )
                {
                    *(U32*)(buf + i) = *R_NFMECD;
                    *(U32*)(buf + i + 4) = *R_NFMECD;
                    *(U32*)(buf + i + 8) = *R_NFMECD;
                    *(U32*)(buf + i + 12) = *R_NFMECD;
                    i += 16;
                }
            }
        }
    }
    else if (NandInfo.eccbit == 4) //ECC
    {
        int j;
        for (j = 0; j < NandInfo.pagesize / 512; j++, buf += 512)
        {
            g_dummy = *R_NFMSTAT; //clear busy
            *(volatile char *)R_NFMCMD = NAND_CMD_READ0; // Read
            setpageaddress(pageaddr, j * 512);
            *(volatile char *)R_NFMCMD = NAND_CMD_READSTART; // Read
            //set  ecc location
            //check 0 block
            if (NandInfo.blocksize > (pageaddr*NandInfo.pagesize))
                *R_NFMSPADDR = NandInfo.pagesize + (j * 7) + ECC_OFFSET_4_HW_DEFAULT;
            else
                *R_NFMSPADDR = NandInfo.pagesize + (j * 4 * 2) + ECC_OFFSET_4;
            g_dummy = *R_NFMMLCECC(10); //clear
            WaitBusy();
            *R_NFMCON |= ((1 << 16) | (1 << 15)); //4bit, AUTO ECC

            while (1)
            {
                U32 stat = *R_NFMSTAT;
                if (stat & (1 << 2)) //wait for auto decoding
                {
                    if ((stat & (1 << 3)) == 0)
                    {
                        if (stat & (1 << 11))
                        {
                            DEBUGPRINTF("ECC Decoding error(NFSTAT:%#x)\r\n", stat);
                            return FALSE;
                        }
                    }
#ifdef NAND_DEBUG
                    if ((stat >> 12) & 0x1f)
                    {
                        DEBUGPRINTF("error bit count %d ==> corrected\r\n", (stat >> 12) & 0x1f);
                    }
#endif
                    break;
                }
            }
            U32 i;

            if ((U32)(&buf[0]) & 1) //1byte boundary
            {
                for (i = 0; i < 512; )
                {
                    buf[i] = *(volatile U8*)R_NFMECD;
                    buf[i + 1] = *(volatile U8*)R_NFMECD;
                    buf[i + 2] = *(volatile U8*)R_NFMECD;
                    buf[i + 3] = *(volatile U8*)R_NFMECD;
                    i += 4;
                }
            }
            else if ((U32)(&buf[0]) & 2) //2byte boundary
            {
                for (i = 0; i < 512; )
                {
                    *(U16*)(buf + i) = *(volatile U16*)R_NFMECD;
                    *(U16*)(buf + i + 2) = *(volatile U16*)R_NFMECD;
                    *(U16*)(buf + i + 4) = *(volatile U16*)R_NFMECD;
                    *(U16*)(buf + i + 6) = *(volatile U16*)R_NFMECD;
                    i += 8;
                }
            }
            else
            {
                for (i = 0; i < 512; )
                {
                    *(U32*)(buf + i) = *R_NFMECD;
                    *(U32*)(buf + i + 4) = *R_NFMECD;
                    *(U32*)(buf + i + 8) = *R_NFMECD;
                    *(U32*)(buf + i + 12) = *R_NFMECD;
                    i += 16;
                }
            }
        }
    }
    else
    {
        g_dummy = *R_NFMSTAT; //clear busy

        *(volatile char *)R_NFMCMD = NAND_CMD_READ0; // Read
        setpageaddress(pageaddr, 0);
        if (NandInfo.pagesize != 512)
            *(volatile char *)R_NFMCMD = NAND_CMD_READSTART; // Read

        // Check Busy End
        WaitBusy();
        nandbuf_read(buf, NandInfo.pagesize);
    }
    return TRUE;
}
//read only 512byte for FAT
static U8 *page_cache = 0;
static int page_cache_num = -1;
BOOL nand_phy_readsect512(U32 sector, void* buffer)
{
    U8* buf = (U8*)buffer;
    U32 sectsize = nand_get_sectorsize();
    if (NandInfo.pagesize == sectsize)
    {
        if (nand_phy_readpage(sector, buffer) == FALSE)
            return nand_phy_readpage(sector, buf);
        return TRUE;
    }
    else
    {
        U32 pagenum;
        U32 offset;
        U32 secperpage = NandInfo.pagesize / sectsize;
        pagenum = sector / secperpage;
        offset = (sector % secperpage) * sectsize;
        if (pagenum != page_cache_num)
        {
            if (nand_phy_readpage(pagenum, page_cache) == FALSE)
                if (nand_phy_readpage(pagenum, page_cache) == FALSE)
                    return FALSE;
        }
        page_cache_num = pagenum;
        memcpy(buffer, page_cache + offset, 512);
        return TRUE;
    }
}

BOOL nand_phy_readspare(U32 pageaddr, void *buffer, U32 len)
{
    int i;
    char* buf;
    g_dummy = *R_NFMSTAT; //clear busy
    buf = (U8*)buffer;
    if (NandInfo.pagesize == 512)
    {
        *(volatile U8 *)R_NFMCMD = NAND_CMD_READSPARE; // Read
        setspareaddr(pageaddr);
    }
    else
    {
        *(volatile U8 *)R_NFMCMD = NAND_CMD_READ0;
        setspareaddr(pageaddr);
        *(volatile U8 *)R_NFMCMD = NAND_CMD_READSTART;
    }

    // Check Busy End
    WaitBusy();
    if (len > NandInfo.sparesize)
        len = NandInfo.sparesize;
    for (i = 0; i < len; i++)
        buf[i] = *(volatile U8*)R_NFMDATA;
    // Check Busy End
    while (!(*(volatile U8*)R_NFMSTAT & 0x02)); // for "trr" timing,see Nand-Flash data sheet
    return TRUE;
}

BOOL nand_phy_readblock(U32 blocknum, void* buffer)
{
    int i;
    U32 startpage = BLOCK2PAGE(blocknum);
    for (i = 0; i < NandInfo.pageperblock; i++)
    {
        if (nand_phy_readpage(startpage, buffer) == FALSE)
        {
            if (nand_phy_readpage(startpage, buffer) == FALSE)
                return FALSE;
        }
        startpage++;
        buffer += NandInfo.pagesize;
    }
    return TRUE;
}

extern BOOL b_use_usbhost;
extern void pause_usb_host();
extern void resume_usb_host();

BOOL nand_phy_writepage_spare(U32 pageaddr, void* buffer, BOOL bspare_wirte, BYTE* spare_buffer, int spare_buffer_len)
{
    U8* buf = (U8*)buffer;

#if CONFIG_USBHOST == 1
    if (b_use_usbhost)
        pause_usb_host();
#endif

    U32	mlc_ecc[8][11];//8K
    int i;
    int j;
    g_dummy = *R_NFMSTAT; //clear busy
    if (NandInfo.pagesize == 512)
        *(volatile U8*)R_NFMCMD = 0x0;
    *(volatile U8*)R_NFMCMD = NAND_CMD_PPRO1;

    setpageaddress(pageaddr, 0);

    if (NandInfo.eccbit == 24)
    {
        if ((U32)(&buf[0]) & 1) //1byte align
        {
            for (j = 0; j < NandInfo.pagesize / 1024; j++)
            {
                g_dummy = *R_NFMMLCECC(10); //ecc clear
                *R_NFMCON = (1 << 12); //ecc gen enable
                for (i = 0; i < 1024; )
                {
                    *(volatile U8*)R_NFMDATA = buf[i];
                    *(volatile U8*)R_NFMDATA = buf[i + 1];
                    *(volatile U8*)R_NFMDATA = buf[i + 2];
                    *(volatile U8*)R_NFMDATA = buf[i + 3];
                    i += 4;
                }
                buf += i;
                for (i = 0; i < 11; i++)
                    mlc_ecc[j][i] = *(R_NFMMLCECC(0) + i);
            }
        }
        else if ((U32)(&buf[0]) & 2) //2byte align
        {
            U16* lbuf = (U16*)&buf[0];
            for (j = 0; j < NandInfo.pagesize / 1024; j++)
            {
                g_dummy = *R_NFMMLCECC(10); //ecc clear
                *R_NFMCON |= (1 << 12); //ecc gen enable
                for (i = 0; i < 512; )
                {
                    *(volatile U16*)R_NFMDATA = lbuf[i];
                    *(volatile U16*)R_NFMDATA = lbuf[i + 1];
                    *(volatile U16*)R_NFMDATA = lbuf[i + 2];
                    *(volatile U16*)R_NFMDATA = lbuf[i + 3];
                    i += 4;
                }
                lbuf += i;
                for (i = 0; i < 11; i++)
                    mlc_ecc[j][i] = *(R_NFMMLCECC(0) + i);
            }
        }
        else //4byte align
        {
            U32* lbuf = (U32*)&buf[0];
            for (j = 0; j < NandInfo.pagesize / 1024; j++)
            {
                g_dummy = *R_NFMMLCECC(10); //ecc clear
                *R_NFMCON = (1 << 12); //ecc gen enable
                for (i = 0; i < 256; )
                {
                    *R_NFMDATA = lbuf[i];
                    *R_NFMDATA = lbuf[i + 1];
                    *R_NFMDATA = lbuf[i + 2];
                    *R_NFMDATA = lbuf[i + 3];
                    i += 4;
                }
                lbuf += i;
                for (i = 0; i < 11; i++)
                    mlc_ecc[j][i] = *(R_NFMMLCECC(0) + i);
            }
        }

    }
    else if (NandInfo.eccbit == 4) // 2048/4096
    {
        if ((U32)(&buf[0]) & 1) //1byte align
        {
            for (j = 0; j < NandInfo.pagesize / 512; j++)
            {
                g_dummy = *R_NFMMLCECC(10); //ecc clear
                *R_NFMCON = ((1 << 15) | (1 << 12)); //4bit ecc gen enable
                for (i = 0; i < 512; )
                {
                    *(volatile U8*)R_NFMDATA = buf[i];
                    *(volatile U8*)R_NFMDATA = buf[i + 1];
                    *(volatile U8*)R_NFMDATA = buf[i + 2];
                    *(volatile U8*)R_NFMDATA = buf[i + 3];
                    i += 4;
                }
                buf += i;
                mlc_ecc[j][0] = *R_NFMMLCECC(0);
                mlc_ecc[j][1] = *R_NFMMLCECC(1);
            }
        }
        else if ((U32)(&buf[0]) & 2) //2byte align
        {
            U16* lbuf = (U16*)&buf[0];
            for (j = 0; j < NandInfo.pagesize / 512; j++)
            {
                g_dummy = *R_NFMMLCECC(10); //ecc clear
                *R_NFMCON = ((1 << 15) | (1 << 12)); //4bit ecc gen enable
                for (i = 0; i < 256; )
                {
                    *(volatile U16*)R_NFMDATA = lbuf[i];
                    *(volatile U16*)R_NFMDATA = lbuf[i + 1];
                    *(volatile U16*)R_NFMDATA = lbuf[i + 2];
                    *(volatile U16*)R_NFMDATA = lbuf[i + 3];
                    i += 4;
                }
                lbuf += i;
                mlc_ecc[j][0] = *R_NFMMLCECC(0);
                mlc_ecc[j][1] = *R_NFMMLCECC(1);
            }
        }
        else //4byte align
        {
            U32* lbuf = (U32*)&buf[0];
            for (j = 0; j < NandInfo.pagesize / 512; j++)
            {
                g_dummy = *R_NFMMLCECC(10); //ecc clear
                *R_NFMCON = ((1 << 15) | (1 << 12)); //4bit ecc gen enable

                for (i = 0; i < 128; )
                {
                    *R_NFMDATA = lbuf[i];
                    *R_NFMDATA = lbuf[i + 1];
                    *R_NFMDATA = lbuf[i + 2];
                    *R_NFMDATA = lbuf[i + 3];
                    i += 4;
                }
                lbuf += i;
                mlc_ecc[j][0] = *R_NFMMLCECC(0);
                mlc_ecc[j][1] = *R_NFMMLCECC(1);
            }
        }

    }
    else
    {
        nandbuf_write(buf, NandInfo.pagesize);
    }

    if (bspare_wirte) //write logical number
    {

        /*
        *( volatile U8* )R_NFMCMD  = NAND_CMD_RANDI ;
        *( volatile U8* )R_NFMADDR = ( U8 )NandInfo.pagesize;
        *( volatile U8* )R_NFMADDR = ( U8 )( NandInfo.pagesize >> 8 );
        */
        for (i = 0; i < spare_buffer_len; i++)
            *(vU8*)R_NFMDATA = spare_buffer[i];
    }
    if (NandInfo.eccbit == 24)
    {
        //write ecc at spare
        *(volatile U8*)R_NFMCMD = NAND_CMD_RANDI;
        //check 0 block
        if (NandInfo.blocksize > (pageaddr*NandInfo.pagesize))
        {
            *(volatile U8*)R_NFMADDR = (NandInfo.pagesize + ECC_OFFSET_24_HW_DEFAULT);
            *(volatile U8*)R_NFMADDR = (NandInfo.pagesize + ECC_OFFSET_24_HW_DEFAULT) >> 8;
            for (j = 0; j < NandInfo.pagesize / 1024; j++)
            {
                for (i = 0; i < 10; i++)
                    *R_NFMDATA = mlc_ecc[j][i];
                BYTE lastecc1 = (BYTE)(mlc_ecc[j][i]);
                BYTE lastecc2 = (BYTE)(mlc_ecc[j][i] >> 8);
                *(vU8*)R_NFMDATA = lastecc1;
                *(vU8*)R_NFMDATA = lastecc2;
            }
        }
        else
        {
            *(volatile U8*)R_NFMADDR = (NandInfo.pagesize + ECC_OFFSET_24);
            *(volatile U8*)R_NFMADDR = (NandInfo.pagesize + ECC_OFFSET_24) >> 8;
            for (j = 0; j < NandInfo.pagesize / 1024; j++)
            {
                for (i = 0; i < 11; i++)
                    *R_NFMDATA = mlc_ecc[j][i];
            }
        }
    }
    else if (NandInfo.eccbit == 4)
    {
        //write ecc at spare
        *(volatile U8*)R_NFMCMD = NAND_CMD_RANDI;
        //check 0 block
        if (NandInfo.blocksize > (pageaddr*NandInfo.pagesize))
        {
            *(volatile U8*)R_NFMADDR = (NandInfo.pagesize + ECC_OFFSET_4_HW_DEFAULT);
            *(volatile U8*)R_NFMADDR = (NandInfo.pagesize + ECC_OFFSET_4_HW_DEFAULT) >> 8;
            for (j = 0; j < NandInfo.pagesize / 512; j++)
            {
                *R_NFMDATA = mlc_ecc[j][0];
                *(vU8*)R_NFMDATA = (BYTE)(mlc_ecc[j][1]);
                *(vU8*)R_NFMDATA = (BYTE)(mlc_ecc[j][1] >> 8);
                *(vU8*)R_NFMDATA = (BYTE)(mlc_ecc[j][1] >> 16);
            }
        }
        else
        {
            *(volatile U8*)R_NFMADDR = (NandInfo.pagesize + ECC_OFFSET_4);
            *(volatile U8*)R_NFMADDR = (NandInfo.pagesize + ECC_OFFSET_4) >> 8;
            for (j = 0; j < NandInfo.pagesize / 512; j++)
            {
                *R_NFMDATA = mlc_ecc[j][0];
                *R_NFMDATA = mlc_ecc[j][1];
            }
        }
    }

    *(volatile U8*)R_NFMCMD = NAND_CMD_PPRO2; // Confirm

    if (wait_write_erase_done() == FALSE)
    {
#if CONFIG_USBHOST == 1
        if (b_use_usbhost)
            resume_usb_host();
#endif
        return FALSE;
    }

#if CONFIG_USBHOST == 1
    if (b_use_usbhost)
        resume_usb_host();
#endif

    //for nand_phy_readsect
    if (page_cache_num == pageaddr)
        memcpy(page_cache, buffer, NandInfo.pagesize);
    return TRUE;
}
BOOL nand_phy_writepage(U32 pageaddr, void* buffer)
{

    //check start page of block in FAT area
    int phynum = pageaddr / NandInfo.pageperblock;
    if ((phynum >= NandInfo.datastartblock) && ((pageaddr % NandInfo.pageperblock) == 0))
    {
        int lognum = nand_get_log_block(phynum);
        if (lognum == 0xffff)//not allocated
        {
            DEBUGPRINTF("ERRORR ================ logical number is not allocated block yet\r\n");
            return nand_phy_writepage_spare(pageaddr, buffer, FALSE, NULL, 0);
        }
        make_logical_data(lognum);

        return nand_phy_writepage_spare(pageaddr, buffer, TRUE, sparebuf_for_lognum, 16);
    }
    return nand_phy_writepage_spare(pageaddr, buffer, FALSE, NULL, 0);
}

BOOL nand_phy_writespare(U32 pageaddr, void* buffer, U32 len)
{
    int i;
    U8* buf = (U8*)buffer;
    g_dummy = *R_NFMSTAT; //clear busy

#if CONFIG_USBHOST == 1
    if (b_use_usbhost)
        pause_usb_host();
#endif

    if (NandInfo.pagesize == 512)
    {
        *(volatile U8 *)R_NFMCMD = NAND_CMD_READSPARE;
        *(volatile U8*)R_NFMCMD = NAND_CMD_PPRO1;
        setspareaddr(pageaddr);
    }
    else
    {
        *(volatile U8 *)R_NFMCMD = NAND_CMD_PPRO1;
        setspareaddr(pageaddr);
    }

    if (len > NandInfo.sparesize)
        len = NandInfo.sparesize;
    for (i = 0; i < len; i++)
    {
        *(volatile U8*)R_NFMDATA = buf[i];
    }
    *(volatile U8*)R_NFMCMD = NAND_CMD_PPRO2; // Confirm
    if (wait_write_erase_done() == FALSE)
    {
#if CONFIG_USBHOST == 1
        if (b_use_usbhost)
            resume_usb_host();
#endif
        return FALSE;
    }
#if CONFIG_USBHOST == 1
    if (b_use_usbhost)
        resume_usb_host();
#endif
    return TRUE;
}

BOOL nand_phy_writeblock(U32 blocknum, void* buffer)
{
    int i;
    U32 startpage = BLOCK2PAGE(blocknum);
    U8* buf = (U8*)buffer;
    for (i = 0; i < NandInfo.pageperblock; i++)
    {
        if (nand_phy_writepage(startpage + i, buf) == FALSE)
            return FALSE;
        buf += NandInfo.pagesize;
    }
    return TRUE;
}
BOOL nand_phy_eraseblock(U32 blocknum)
{
    U32 pagenum = BLOCK2PAGE(blocknum);
    g_dummy = *R_NFMSTAT; //clear busy
    *(volatile char *)R_NFMCMD = NAND_CMD_BERS1;
    seteraseblockaddr(pagenum);
    *(volatile U8*)R_NFMCMD = NAND_CMD_BERS2; // Command
    if (wait_write_erase_done() == FALSE)
        return FALSE;
    return TRUE;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	make bad block table. </summary>
///
/// <remarks>	</remarks>
///
/// <returns>	bad block count </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
static int nand_make_bbt()
{
    int badblockcnt = 0;
    int i;
    int pageperblock = NandInfo.pageperblock;
    U32 logicalBlocknum;
    U8 sparedatabuf[16];
    U8 sparedatabuf2[16];
    for (i = NandInfo.datastartblock; i < NandInfo.blockcnt; i++)
    {
        nand_phy_readspare(i * pageperblock, sparedatabuf, 16);
        if (NandInfo.eccbit == 24)
            nand_phy_readspare(i * pageperblock + pageperblock - 1, sparedatabuf2, 16);
        else
            nand_phy_readspare(i * pageperblock + 1, sparedatabuf2, 16);

        if (NandInfo.pagesize == 512)
        {
            SPAREDATA* sparedata = (SPAREDATA*)sparedatabuf;
            SPAREDATA* sparedata2 = (SPAREDATA*)sparedatabuf2;
            if ((sparedata->BadInfo != 0xff) || (sparedata2->BadInfo != 0xff))
            {
                BlockInfoTable[i] = BI_BAD;
                DEBUGPRINTF("%dblock is bad block\r\n", i);
                badblockcnt++;
                continue;
            }
            //make logical block number
            logicalBlocknum = (U32)sparedata->LSN[0] + ((U32)sparedata->LSN[1] << 8) + ((U32)sparedata->LSN[2] << 16);
            if (logicalBlocknum == 0xffffff)
            {
                BlockInfoTable[i] = BI_GOOD;
            }
            else
            {
                U8 paritybit = 0;
                int k;
                //check parity bit
                for (k = 0; k < 24; k++)
                {
                    if (logicalBlocknum & (1 << k))
                    {
                        if (paritybit)
                            paritybit = 0;
                        else
                            paritybit = 1;
                    }
                }
                if (sparedata->paritybit != paritybit)
                {
                    BlockInfoTable[i] = BI_NOTUSED;
                }
                else
                {
                    if (PhyBlockTable[logicalBlocknum] != 0xffff)
                    {
                        DEBUGPRINTF("Error : logical number was duplicated, it needs Erase all]\r\n");
                    }
                    PhyBlockTable[logicalBlocknum] = i;
                    LogBlockTable[i] = logicalBlocknum;
                    BlockInfoTable[i] = BI_USED;
                    //NAND_DEBUGPRINTF("P-%d : L-%d\r\n",i,logicalBlocknum);
                }
            }
        }
        else //large block
        {
            SPAREDATA2* sparedata = (SPAREDATA2*)sparedatabuf;
            SPAREDATA2* sparedata2 = (SPAREDATA2*)sparedatabuf2;
            if ((sparedata->BadInfo != 0xff) || (sparedata2->BadInfo != 0xff))
            {
                BlockInfoTable[i] = BI_BAD;
                badblockcnt++;
                continue;
            }
            //make logical block number
            logicalBlocknum = (U32)sparedata->LSN[0] + ((U32)sparedata->LSN[1] << 8) + ((U32)sparedata->LSN[2] << 16);
            if (logicalBlocknum == 0xffffff)
            {
                BlockInfoTable[i] = BI_GOOD;
            }
            else
            {
                U8 paritybit = 0;
                int k;
                //check parity bit
                for (k = 0; k < 24; k++)
                {
                    if (logicalBlocknum & (1 << k))
                    {
                        if (paritybit)
                            paritybit = 0;
                        else
                            paritybit = 1;
                    }
                }
                if (sparedata->paritybit != paritybit)
                {
                    BlockInfoTable[i] = BI_NOTUSED;
                    DEBUGPRINTF("%d block paritybit error\r\n", i);
                }
                else
                {
                    if (PhyBlockTable[logicalBlocknum] != 0xffff)
                    {
                        DEBUGPRINTF("Error : logical number was duplicated, it needs Erase all]\r\n");
                    }
                    PhyBlockTable[logicalBlocknum] = i;
                    LogBlockTable[i] = logicalBlocknum;
                    BlockInfoTable[i] = BI_USED;
                    //NAND_DEBUGPRINTF("P-%d : L-%d\r\n",i,logicalBlocknum);
                }
            }
        }
    }
    DEBUGPRINTF("make bad block inforamation done(bad-block(%d))\r\n", badblockcnt);
    return badblockcnt;
}
/*
* find empty block and erase it
**/
static int nand_get_phy_freeblock(U32 logicalblocknum)
{
    int i;
    int k = logicalblocknum;
    for (i = NandInfo.datastartblock; i < NandInfo.blockcnt; i++)
    {
        if (BlockInfoTable[k] == BI_GOOD)
        {
            //if(nand_phy_eraseblock(k)==TRUE)
            return k;
        }
        else if (BlockInfoTable[i] == BI_NOTUSED)
        {
            if (nand_phy_eraseblock(k) == TRUE)
            {
                BlockInfoTable[i] = BI_GOOD;
                return k;
            }
            BlockInfoTable[k] = BI_BAD;
        }
        k++;
        if (k >= NandInfo.blockcnt)
            k = NandInfo.datastartblock;
    }
    return -1;
}

static BOOL nand_allocate_block(U32 phynum, U32 lognum)
{
    if (phynum > NandInfo.blockcnt || lognum > NandInfo.blockcnt)
        return FALSE;

    U16 oldphynum = PhyBlockTable[lognum];
    if (oldphynum != phynum && oldphynum < NandInfo.blockcnt)
    {
        BlockInfoTable[oldphynum] = BI_NOTUSED;
        //nand_phy_eraseblock(oldphynum);
    }
    BlockInfoTable[phynum] = BI_USED;
    PhyBlockTable[lognum] = phynum;
    LogBlockTable[phynum] = lognum;

    return TRUE;
}
/*
static void nand_set_badblock(U32 lognum)
{
U32 phyblocknum = PhyBlockTable[lognum];
BlockInfoTable[phyblocknum]=BI_BAD;
nand_phy_eraseblock(phyblocknum);
PhyBlockTable[lognum]=0xffff;//not allocate
}
*/

U32 nand_get_phy_block(U32 lognum)
{
    U32 phyblocknum = PhyBlockTable[lognum];
    if (phyblocknum == 0xffff) //not allocate
    {
        phyblocknum = nand_get_phy_freeblock(lognum);
        if (phyblocknum == -1)
        {
            NAND_DEBUGPRINTF("failed to get free block\r\n");
            return -1;
        }
        NAND_DEBUGPRINTF("new block allocated P-%d:L-%d\r\n", phyblocknum, lognum);
        nand_allocate_block(phyblocknum, lognum);
    }
    return phyblocknum;
}

U32 nand_get_pagesize()
{
    return NandInfo.pagesize;
}

U32 nand_get_blocksize()
{
    return NandInfo.blocksize;
}
U32 nand_get_pageperblock()
{
    return NandInfo.blocksize / NandInfo.pagesize;
}
U32 nand_get_blockcnt()
{
    return NandInfo.blockcnt;
}
U32 nand_get_memsize_kbyte()
{
    return NandInfo.blocksize / 1024 * NandInfo.datablockcnt;
}

U32 nand_get_sectorcount()
{
    return (NandInfo.blocksize * NandInfo.datablockcnt) / nand_get_sectorsize();
}


// Flash Translation Layer(FTL)

U32 nand_get_phy_page(U32 pagenum)
{
    U32 blocknum = pagenum / NandInfo.pageperblock;
    blocknum = nand_get_phy_block(blocknum);
    return (blocknum * NandInfo.pageperblock + (pagenum % NandInfo.pageperblock));
}

BOOL nand_eraseblock(U32 blocknum)
{
    U32 phyblocknum = nand_get_phy_block(blocknum);
    if (nand_phy_eraseblock(phyblocknum) == FALSE)
        return FALSE;
    return TRUE;
}

BOOL nand_writepage(U32 pagenum, void* buf)
{
    pagenum = nand_get_phy_page(pagenum);
    return nand_phy_writepage(pagenum, buf);
}

BOOL nand_readpage(U32 pagenum, void* buf)
{
    pagenum = nand_get_phy_page(pagenum);
    return nand_phy_readpage(pagenum, buf);
}

DWORD nand_get_sectorsize()
{
#ifdef SECTOR_SIZE_IS_PAGESIZE
    return NandInfo.pagesize;
#else
    return 512;
#endif
}

typedef struct
{
    int blocknum;
    BOOL dirty; // if true, data is updated so it should be written at storage
    U8* buf;
} BLOCKBUF;
BLOCKBUF blockbuf;
/*
{
.blocknum=-1,
.dirty=0,
.buf={0,}
};
*/
static void init_blockbuf()
{
    blockbuf.blocknum = -1;
    blockbuf.dirty = FALSE;
    blockbuf.buf = malloc(nand_get_blocksize());
}
static void flushblockbuf()
{
    if (blockbuf.dirty == 0)
        return;

    U32 pagesize = nand_get_pagesize();
    U32 pageperblock = nand_get_pageperblock();
    U32 startpage = blockbuf.blocknum * pageperblock;

    nand_eraseblock(blockbuf.blocknum);
    int i;
    for (i = 0; i < pageperblock; i++)
    {
        nand_writepage(startpage + i, blockbuf.buf + (i * pagesize));
    }
    blockbuf.dirty = 0;
    blockbuf.blocknum = -1;
}

static void fillblockbuf(U32 blocknum)
{
    nand_phy_readblock(nand_get_phy_block(blocknum), blockbuf.buf);
    blockbuf.blocknum = blocknum;
}

void nand_flushdata()
{
    flushblockbuf();
}

static BOOL nand_updatesect(U32 sector, void* buffer)
{
    U8* buf = (U8*)buffer;
    U32 pagesize = nand_get_pagesize();
    int sectorsize = nand_get_sectorsize();
    U32 pagenum = sector * sectorsize / pagesize;
    U32 nandblocknum = pagenum / nand_get_pageperblock();

    if (blockbuf.blocknum != nandblocknum)
    {
        flushblockbuf();
        fillblockbuf(nandblocknum);
    }
    U32 sectorperblock = NandInfo.blocksize / sectorsize;
    sector = sector % sectorperblock;
    U8* pbuf = blockbuf.buf + (sector * sectorsize);
    memcpy(pbuf, buf, sectorsize);
    blockbuf.dirty = 1;
    return TRUE;
}


BOOL nand_updatesects(U32 startsector, void* buf, U32 cnt)
{
    int i;
    int sectorsize = nand_get_sectorsize();
    U32 fatsector = NandInfo.datastartblock * nand_get_pageperblock();
    fatsector *= nand_get_pagesize() / sectorsize;
    startsector += fatsector;
    for (i = 0; i < cnt; i++)
    {
        if (nand_updatesect(startsector + i, (void*)(((U32)buf) + (i * sectorsize))) == FALSE)
            return FALSE;
    }
    return TRUE;
}
static BOOL nand_readsect(U32 sector, void* buffer)
{
    U8* buf = (U8*)buffer;
    U32 sectsize = nand_get_sectorsize();
    U32 pagesize = nand_get_pagesize();
    U32 pagenum = sector * sectsize / pagesize;
    U32 nandblocknum = pagenum / nand_get_pageperblock();
    if (blockbuf.blocknum == nandblocknum)
    {
        U32 offset = (sector * sectsize) % nand_get_blocksize();
        memcpy(buf, blockbuf.buf + offset, sectsize);
        return TRUE;
    }
    else
    {
        if (NandInfo.pagesize == sectsize)
        {
            sector = nand_get_phy_page(sector);
        }
        else
        {
            pagenum = nand_get_phy_page(pagenum);
            sector = (pagenum * (pagesize / sectsize)) + (sector % (pagesize / sectsize));
        }
#ifdef SECTOR_SIZE_IS_PAGESIZE
        BOOL re = nand_readpage(sector, buf);
#else
        BOOL re = nand_phy_readsect512(sector, buf);
#endif
        return re;
    }
}


BOOL nand_readsects(U32 startsector, void* buf, U32 cnt)
{
    int i;
    U32 fatsector = NandInfo.datastartblock * nand_get_pageperblock();
    U32 sectsize = nand_get_sectorsize();
    fatsector *= nand_get_pagesize() / sectsize;
    startsector += fatsector;
    U8* u8buf = (U8*)buf;

    for (i = 0; i < cnt; i++)
    {
        if (nand_readsect(startsector + i, u8buf + (i * sectsize)) == FALSE)
            return FALSE;
    }
    return TRUE;
}

static U32 nandcfg = 0x2221;
void nand_set_cfg(U32 cfg)
{
    nandcfg = cfg;
}

void nand_eraseall()
{
    int i;
    for (i = 0; i < NandInfo.blockcnt; i++)
    {
        //debugprintf("\r%04d/%04d",i,NandInfo.blockcnt);
        if (nand_phy_eraseblock(i) == FALSE)
        {
            //DEBUGPRINTF("%d block erase error\r\n",i);
        }
    }
    //debugstring("\r\n");
}


static int nand_id_len(U8 *id_data, int arrlen)
{
    //find id
    int i;
    U8 mafid = id_data[0];
    for (i = 3; i < arrlen; i++)
    {
        if (mafid == id_data[i])
        {
            if (i < arrlen - 1)
            {
                if (id_data[i + 1] == id_data[1])
                    return i;
            }
            return i;
        }
    }
    return arrlen;
}

static bool isSomeSS(U8 id[8], int idlen)
{
    int extid = id[3];
    if (idlen == 6 && (NandInfo.cellinfo
                       & NAND_CI_CELLTYPE_MSK) && id[5] != 0x00)
    {
        U32 temp;
        int sparesizes[6] = { 640,128,218,400,436,512 };
        NandInfo.pagesize = 2048 << (extid & 0x03);
        extid = extid >> 2;
        temp = ((extid >> 2) & 0x04) | (extid & 0x03);
        if (temp < 6)
        {
            NandInfo.sparesize = sparesizes[temp];
        }
        else
            NandInfo.sparesize = sparesizes[0];

        extid = extid >> 2;

        NandInfo.blocksize = (128 * 1024) << (((extid >> 1) & 0x04) | (extid & 0x03));
        return true;
    }
    return false;
}
static bool isSomeHynix(U8 id[8], int idlen)
{
    if (idlen == 6 && (NandInfo.cellinfo & NAND_CI_CELLTYPE_MSK))
    {
        U32 temp;
        int extid = id[3] >> 2;
        int sparesize[] = { 128,224,448,64,32,16 };

        NandInfo.pagesize = 2048 << (extid & 0x03);
        temp = ((extid >> 2) & 0x04) | (extid & 0x03);
        if (temp < 6)
            NandInfo.sparesize = sparesize[temp];
        else
            NandInfo.sparesize = 640;

        extid = extid >> 2;
        temp = ((extid >> 1) & 0x04) | (extid & 0x03);
        if (temp < 0x03)
            NandInfo.blocksize = (128 * 1024) << temp;
        else if (temp == 0x03)
            NandInfo.blocksize = 768 * 1024;
        else
            NandInfo.blocksize = (64 * 1024) << temp;
        return true;
    }
    return false;
}


static void decode_ext_id(U8 id_data[8])
{
    int extid, id_len;

    id_len = nand_id_len(id_data, 8);

    if (id_data[0] == NAND_MFR_SAMSUNG)
    {
        if (isSomeSS(id_data, id_len))
            return;
    }
    else if (id_data[0] == NAND_MFR_HYNIX)
    {
        if (isSomeHynix(id_data, id_len))
            return;
    }
    extid = id_data[3];
    NandInfo.pagesize = 1024 << (extid & 0x03);
    extid >>= 2;
    NandInfo.sparesize = (8 << (extid & 0x01)) *
                         (NandInfo.pagesize >> 9);
    extid >>= 2;
    NandInfo.blocksize = (64 * 1024) << (extid & 0x03);
}

void nand_rebuild_bbt()
{
    memset(PhyBlockTable, 0xff, NandInfo.blockcnt * 2);
    memset(LogBlockTable, 0xff, NandInfo.blockcnt * 2);
    int badcnt = nand_make_bbt();
    NandInfo.datablockcnt -= badcnt;
}
/// <summary>
/// initialize NAND Flash  and FTL
/// 1. Read NAND Flash ID.
/// 2. Initialize a bad-block table.
/// </summary>
/// <returns>true if it succeeds, false if it fails.</returns>
BOOL nand_init()
{
    int i;
    BOOL re = FALSE;
    U8 id_buf[8];
    static BOOL binit = FALSE;
    if (binit)
        return TRUE;

    //disable port pull-up
    *R_GPPUDIS(0) = 0xff; //data
    *R_GPPUDIS(1) |= 0x3f; //control

    if (get_ahb_clock() > 40000000)
        *R_NFMCFG = nandcfg | (1 << 20);
    else
        *R_NFMCFG = nandcfg;

    nand_reset();
    //read id
    *(volatile char *)R_NFMCMD = NAND_CMD_ID; // Read
    *(volatile char *)R_NFMADDR = 0x00; // ADDRESS

    for (i = 0; i < 8; i++)
    {
        id_buf[i] = *(volatile char *)R_NFMDATA;
    }

    //find manufacturer
    i = 0;
    while (nand_manuf_ids[i].id != 0)
    {
        if (id_buf[0] == nand_manuf_ids[i].id)
        {
            debugprintf("NAND Manufacturer : %s", nand_manuf_ids[i].name);
            re = TRUE;
            break;
        }
        i++;
    }
    if (re == FALSE)
    {
        debugprintf("NAND Manufacturer unknown: 0x%02x\r\n", id_buf[0]);
        return FALSE;
    }

    re = FALSE;
    i = 0;
    while (nand_flash_ids[i].name != NULL)
    {
        if (id_buf[1] == nand_flash_ids[i].id)
        {
            debugprintf(", %s\r\n", nand_flash_ids[i].name);
            re = TRUE;
            break;
        }
        i++;
    }
    if (re == FALSE)
    {
        debugprintf("NAND Device unknown: 0x%02x\r\n", id_buf[1]);
        return FALSE;
    }
    NandInfo.maf_id = id_buf[0];
    NandInfo.dev_id = id_buf[1];
    /* The 3rd id byte holds MLC / multichip data */
    NandInfo.cellinfo = id_buf[2];

    if (nand_flash_ids[i].pagesize == 0)
    {
        decode_ext_id(id_buf);
    }
    else
    {
        NandInfo.blocksize = nand_flash_ids[i].blocksize;
        NandInfo.pagesize = nand_flash_ids[i].pagesize;
        NandInfo.sparesize = NandInfo.pagesize / 32;
    }

    NandInfo.pageperblock = NandInfo.blocksize / NandInfo.pagesize;
    NandInfo.blockcnt = (nand_flash_ids[i].chipsize << 10) / (NandInfo.blocksize >> 10);
    if ((NandInfo.cellinfo & NAND_CI_CELLTYPE_MSK) && (NandInfo.pagesize != 512))
    {
        debugstring("NAND Flash : MLC(Multi Level Cell) type \r\n");
        if (NandInfo.pagesize >= (8 * 1024))
            NandInfo.eccbit = 24;
        else
            NandInfo.eccbit = 4;//1bit per 512byte
    }
    else if (NandInfo.pagesize == 2048) //1bit per 512byte
    {
        //if you want to speed up, set as zero
        NandInfo.eccbit = 4;
    }

    if (NandInfo.eccbit == 24)
        *R_NFMCON = 0;//24bit ECC mode
    else
        *R_NFMCON = (1 << 15);

    U32 temp = (nand_flash_ids[i].chipsize << 12) / (NandInfo.pagesize >> 8);
    //i = __builtin_ffs(temp);//find first set
    i = ffs(temp); //find first set
    i--;
    NandInfo.rowaddrlen = i;
    //reserve 1Mbyte for boot
    i = 1;
    while (1)
    {
        if ((NandInfo.blocksize * i) >= (1024 * 1024))
            break;
        i++;
    }

    NandInfo.datastartblock = i;
    NandInfo.datablockcnt = NandInfo.blockcnt - NandInfo.datastartblock;
    init_blockbuf();
    if (page_cache == 0)
    {
        page_cache = (U8*)malloc(NandInfo.pagesize);
        page_cache_num = -1;
    }

    BlockInfoTable = (U8*)malloc(NandInfo.blockcnt);
    PhyBlockTable = (U16*)malloc(NandInfo.blockcnt * 2);
    LogBlockTable = (U16*)malloc(NandInfo.blockcnt * 2);
    memset(PhyBlockTable, 0xff, NandInfo.blockcnt * 2);
    memset(LogBlockTable, 0xff, NandInfo.blockcnt * 2);

    int badcnt = nand_make_bbt();
    NandInfo.datablockcnt -= badcnt;
    binit = TRUE;
    return TRUE;
}
