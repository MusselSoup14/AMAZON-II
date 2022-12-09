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

#define CACH_ON_VAL 0x00000007

/*
CP0:R5 sub bank address register
	sub-bank base address [31:12]
		must be aligned at boundary of sub-bank size
	sub-bank size [11:0]
		0x000: 4KB		0x001: 8KB
		0x003: 16KB		0x007: 32KB
		0x00F: 64KB		0x01F: 128KB
		0x03F: 256KB	0x07F: 512KB
		0x0FF: 1MB		0x1FF: 2MB
		0x3FF: 4MB		0x7FF: 8MB
		0xFFF: 16MB

CP0:R8 sub bank control register
	sub-bank index [6:4]
		000 : Sub-bank0 001 : Sub-bank1
		010 : Sub-bank2 011 : Sub-bank3
		100 : Sub-bank4 101 : Sub-bank5
		110 : Sub-bank6 111 : Sub-bank7
	sub-bank configuration[3:0]
		bit.3 : Configuration information validity
			0 : Invalid 1 : Valid
		bit.2 : Access right
			0 : Supervisor only access,
			1 : User/Supervisor access
		bit [1:0] : Cache Disable
			00 : Disable cache
			01 : Reserved
			10 : Write-Through cache
			11 : Write-Back cache

CP0:R9 cache configuration register
	bank size : 512MByte ( 0x20000000)

	bank7[31:28] |....| bank0[3:0]
	bit.31: Memory Bank 7 TLB address translation
				0: Disable TLB 1: Enable TLB
	bit.30: Memory Bank 7 access right (TLB disable)
				0: Supervisor only access
				1: User/Supervisor access
			Memory Bank 7 TLB page size (TLB enable)
				0: 4KB 1: Reserved
	bit[29:28]: Memory Bank 7 Cache Configuration
				(valid only if TLB disabled)
				00: Disable cache
				01: Reserved
				10: Write-Through cache
				11: Write-Back cache


CP0:R11 invalidation register
	bit[31:7]: Target Address 
	bit[6:4]: Target Address/Target Way
	bit.3: Operation
			0 : Address Based Invalidation
			1 : Way Based Invalidation
	bit.2: Write-Back enable/disable
			0 : w/o Write-Back
			1 : with Write-Back if needed
	bit.1: reserved
	bit.0: cache type
			0 : Instruction cache
			1 : Data cache

*/
#ifndef __ASSEMBLER__ 

/*
icahche_invalidate_xx don't write-back operation. 
*/
void icache_invalidate_way(void);
void icache_invalidate_address(unsigned int addr, int len);

void dcache_invalidate_way(void); //with write-back
void dcache_invalidate_way_without_writeback();
void dcache_invalidate_address(unsigned int addr, int len);
void dcache_invalidate_address_without_writeback();

void CacheDisable(void);//cache disable
void CacheEnable(void);//cache on wheh cache disabled by CacheDisable() 
void CacheInit(void); //cache on

#endif
