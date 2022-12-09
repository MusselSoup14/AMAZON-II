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
#include "amazon2/cacheutil.h"
.file "cache_util.S"

/* void icache_invalidate_way(void)*/
	.section .ispm,"ax",@progbits

	.align 1
    .global _icache_invalidate_way
	.type _icache_invalidate_way, @function
_icache_invalidate_way:	
	set		8
	sync
	push	%r0, %r1

	ldi 	0x0,%r1
	mvfc	0,%r9
	sync
	cmp		%r0,%r1
	jz		_icache_invalidate_way_end
	
/* cache off */	
	ldi		0x0,%r0
	mvtc 	0,%r9
	sync

/*waybase invalidation	without writeback*/
	ldi		0x8,%r0
	mvtc	0,%R11
	sync
	
	ldi		0x18,%r0
	mvtc	0,%R11
	sync
	
/*cache on	*/
	ldi		CACH_ON_VAL,%r0
	mvtc 	0,%r9
	sync
_icache_invalidate_way_end	:
	pop		%r1, %r0
	clr		8
	sync
	jplr
.size _icache_invalidate_way, .-_icache_invalidate_way	


/* void dcache_invalidate_way(void)*/

	.align 1
    .global _dcache_invalidate_way
	.type _dcache_invalidate_way, @function
_dcache_invalidate_way:	
	set		8
	sync
	push	%r0, %r1

	ldi 	0x0,%r1
	mvfc	0,%r9
	sync
	cmp		%r0,%r1
	jz		_dcache_invalidate_way_end
	
/* cache off */	
	ldi		0x0,%r0
	mvtc 	0,%r9
	sync

/*waybase invalidation	with writeback*/
	ldi		0xd,%r0
	mvtc	0,%R11
	sync
	
	ldi		0x1d,%r0
	mvtc	0,%R11
	sync
	
/*cache on	*/
	ldi		CACH_ON_VAL,%r0
	mvtc 	0,%r9
	sync
_dcache_invalidate_way_end	:
	pop		%r1, %r0
	clr		8
	sync
	jplr
.size _dcache_invalidate_way, .-_dcache_invalidate_way	

	
/*	void CacheDisable (void)*/
	.align 1
	.global _CacheDisable
	.type _CacheDisable, @function
_CacheDisable:
/*cache disable*/
	push	%r0
	set 	8
	ldi		0x0,%r0
	mvtc 	0,%r9
	sync
/*i-cache waybase invalidation	 without writeback*/
	ldi		0x8,%r0
	mvtc	0,%R11
	sync
	
	ldi		0x18,%r0
	mvtc	0,%R11
	sync

/*d-cache waybase invalidation	with writeback*/
	ldi		0xd,%r0
	mvtc	0,%R11
	sync
	
	ldi		0x1d,%r0
	mvtc	0,%R11
	sync
	
	clr 	8
	pop		%r0
	jplr

.size _CacheDisable, .-_CacheDisable	

/*	void CacheEnable (void)*/
	.align 1
	.global _CacheEnable
	.type _CacheEnable, @function
_CacheEnable:
	push	%r0
	set 	8
	ldi		CACH_ON_VAL,%r0
	mvtc 	0,%r9
	sync
	
	clr 	8
	pop		%r0
	jplr

.size _CacheEnable, .-_CacheEnable	


/* void CacheInit(void)*/
	.align 1
	.section .text
    .global _CacheInit
	.type _CacheInit, @function
_CacheInit:	
	push %r0
	set 	8

/*i-cache waybase invalidation	without write-back*/	
	ldi		0x8,%r0
	mvtc	0,%R11
	sync
	
	ldi		0x18,%r0
	mvtc	0,%R11
	sync
	
	/* if cache on */
	mvfc	0,%r9
	sync
	cmp		0,%r0
	jnz		._dinvalidation_and_writeback

/*d-cache waybase invalidation without write-back*/
	ldi		0x9,%r0
	mvtc	0,%R11
	sync
	
	ldi		0x19,%r0
	mvtc	0,%R11
	sync

/*cache on	*/
	ldi		CACH_ON_VAL,%r0
	mvtc 	0,%r9
	sync
	
	clr 	8
	pop		%r0
	jplr

._dinvalidation_and_writeback:
/* cache off */
	ldi		0x0,%r0
	mvtc 	0,%r9
	sync
/*d-cache waybase invalidation	with writeback*/	
	ldi		0xd,%r0
	mvtc	0,%R11
	sync
	
	ldi		0x1d,%r0
	mvtc	0,%R11
	sync
	
/*cache on	*/
	ldi		CACH_ON_VAL,%r0
	mvtc 	0,%r9
	sync
	
	clr 8
	pop		%r0
	jplr
.size _CacheInit, .-_CacheInit	

