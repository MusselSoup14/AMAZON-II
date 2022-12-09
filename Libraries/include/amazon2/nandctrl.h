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

/***********************************************
* NAND FLASH CONTROLLER
***********************************************/
#define NAND_FLASH_CONTROL_BASE 0xF0000800
#define R_NFMCON		((volatile unsigned int*)(NAND_FLASH_CONTROL_BASE))
#define R_NFMCMD		((volatile unsigned int*)(NAND_FLASH_CONTROL_BASE+4))
#define R_NFMADDR		((volatile unsigned int*)(NAND_FLASH_CONTROL_BASE+8))
#define R_NFMDATA		((volatile unsigned int*)(NAND_FLASH_CONTROL_BASE+0xc))
#define R_NFMSTAT		((volatile unsigned int*)(NAND_FLASH_CONTROL_BASE+0x14))
#define R_NFMECC		((volatile unsigned int*)(NAND_FLASH_CONTROL_BASE+0x18))
#define R_NFMCFG		((volatile unsigned int*)(NAND_FLASH_CONTROL_BASE+0x1c))
#define R_NFMECCLSN		((volatile unsigned int*)(NAND_FLASH_CONTROL_BASE+0x20))
#define R_NFMECD		((volatile unsigned int*)(NAND_FLASH_CONTROL_BASE+0x24))
#define R_NFMSPADDR		((volatile unsigned int*)(NAND_FLASH_CONTROL_BASE+0x28))
#define R_NFMMLCECC(n)		((volatile unsigned int*)((NAND_FLASH_CONTROL_BASE+0x2c)+(n*4)))
#define R_NFMELOC(n)	((volatile unsigned int*)((NAND_FLASH_CONTROL_BASE+0x58)+(n*4)))
#define R_NFMEPTN(n)	((volatile unsigned int*)((NAND_FLASH_CONTROL_BASE+0xb8)+(n*4)))
#define R_NFMID			((volatile unsigned int*)(NAND_FLASH_CONTROL_BASE+0x118))


// NAND Command Define
#define NAND_CMD_READ0	0x00	// NAND Read 1st Cycle
#define NAND_CMD_READ1	0x01	// NAND Read 2nd harf register
#define NAND_CMD_READSTART	0x30	
#define NAND_CMD_READSPARE	0x50 // NAND Read spare area
#define NAND_CMD_ID	0x90	// NAND Read ID
#define NAND_CMD_RESET	0xff	// NAND Reset
#define NAND_CMD_PPRO1	0x80	// NAND Page Program 1st Cycle
#define NAND_CMD_PPRO2 0x10	// NAND Page Program 2st Cycle
#define NAND_CMD_BERS1	0x60	// NAND Block Erase 1st Cycle
#define NAND_CMD_BERS2	0xd0	// NAND Block Erase 2st Cycle
#define NAND_CMD_RANDI	0x85	// NAND Random Data Input
#define NAND_CMD_RANDO1	0x05	// NAND Random Data Output 1st Cycle
#define NAND_CMD_RANDO2	0xe0	// NAND Random Data Output 2st Cycle
#define NAND_STATUS		0x70	// NAND Status Read


//sector means 512 byte memory
BOOL nand_phy_readpage(U32 pagenum, void* buf);
BOOL nand_phy_writepage(U32 pagenum, void* buf);
BOOL nand_phy_eraseblock(U32 blocknum);
BOOL nand_phy_readspare(U32 pagenum,void *buffer,U32 len);
BOOL nand_phy_writespare(U32 pagenum,void* buffer,U32 len);
BOOL nand_phy_readblock(U32 blocknum,void* buffer);
BOOL nand_phy_writeblock(U32 blocknum,void* buffer);
BOOL nand_phy_isbadblock(U32 blocknum);

BOOL nand_set_sparedata(U32 lognum);
void nand_set_cfg(U32 cfg);
BOOL nand_init();
U32 nand_get_pagesize();
U32 nand_get_blocksize();
U32 nand_get_pageperblock();
U32 nand_get_blockcnt();
U32 nand_get_memsize_kbyte();
U32 nand_get_phy_block(U32 lognum);

BOOL nand_eraseblock(U32 logblocknum);
BOOL nand_readpage(U32 pagenum,void* buf);
BOOL nand_writepage(U32 pagenum,void* buf);
void nand_flushdata();

// for file-system
BOOL nand_phy_readsect512(U32 sector,void* buf);//read only 512 byte
U32 nand_get_sectorcount();//sector is 512
DWORD nand_get_sectorsize();
BOOL nand_readsects(U32 sector,void* buf,U32 cnt);
BOOL nand_updatesects(U32 sector,void* buf,U32 cnt);


