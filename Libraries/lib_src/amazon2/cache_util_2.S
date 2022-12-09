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

	.file "cache_util_2.S"

	.section .ispm,"ax",@progbits
	
	.align 1
    .global _icache_invalidate_address
	.type _icache_invalidate_address, @function
_icache_invalidate_address:
	set		8
	sync
	push	%R0, %R1
	/* check cache enable */
	ldi 	0x0,%r1
	mvfc	0,%r9
	sync
	cmp		%r0,%r1
	jz		.L9

/* cache off */	
	ldi		0x0,%r0
	mvtc 	0,%r9
	sync
/* main body */
	ldi		0,	%r1
	and		0xFFFFFFF0,	%R8
	
	add		0x10,	%r9
	jmp		.L8
.L7:
	mov		%r8,%r0
	mvtc	0,	%R11	 
	sync
	add		0x10, %R8
	add		0x10, %R1
.L8:
 	cmp		%R9,%r1
	jc		.L7
/*  end of main-body */

/*cache on	*/
	ldi		CACH_ON_VAL,%r0
	mvtc 	0,%r9
	sync

.L9:
   	pop	%R0, %R1
	clr		8
	sync
   	jplr

.size _icache_invalidate_address, .-_icache_invalidate_address	

	.align 1
    .global _dcache_invalidate_address
	.type _dcache_invalidate_address, @function
_dcache_invalidate_address:
	set		8
	sync
	push	%R0, %R1
	/* check cache enable */
	ldi 	0x0,%r1
	mvfc	0,%r9
	sync
	cmp		%r0,%r1
	jz		.L6

/* cache off */	
	ldi		0x0,%r0
	mvtc 	0,%r9
	sync
/* main body */
	
	ldi		0,	%r1
	and		0xFFFFFFF0,	%R8
	addq	0x5,	%R8
	
	add		0x10,	%r9
	jmp		.L5
.L4:
	mov		%r8,%r0
	mvtc	0,	%R11	 
	sync
	add		0x10, %R8
	add		0x10, %R1
.L5:
 	cmp		%R9,%r1
	jc		.L4
/*  end of main-body */

/*cache on	*/
	ldi		CACH_ON_VAL,%r0
	mvtc 	0,%r9
	sync

.L6:
   	pop	%R0, %R1
	clr		8
	sync
   	jplr
.size _dcache_invalidate_address, .-_dcache_invalidate_address	


    .align 1
	.global _dcache_invalidate_address_without_writeback
	.type _dcache_invalidate_address_without_writeback, @function
_dcache_invalidate_address_without_writeback:
	set		8
	sync
	push	%R0, %R1
	/* check cache enable */
	ldi 	0x0,%r1
	mvfc	0,%r9
	sync
	cmp		%r0,%r1
	jz		.L3

/* cache off */	
	ldi		0x0,%r0
	mvtc 	0,%r9
	sync

/* main body */
	
	ldi		0,	%r1
	and		0xFFFFFFF0,	%R8
	addq	0x1,	%R8
	
	add		0x10,	%r9
	jmp		.L2
.L1:
	mov		%r8,%r0
	mvtc	0,	%R11	 
	sync
	add		0x10, %R8
	add		0x10, %R1
.L2:
 	cmp		%R9,%r1
	jc		.L1
/*  end of main-body */

/*cache on	*/
	ldi		CACH_ON_VAL,%r0
	mvtc 	0,%r9
	sync

.L3:
   	pop	%R0, %R1
	clr		8
	sync
   	jplr
	.size _dcache_invalidate_address_without_writeback, .-_dcache_invalidate_address_without_writeback	


	
/* void dcache_invalidate_way_without_writeback(void)*/

	.align 1
    .global _dcache_invalidate_way_without_writeback
	.type _dcache_invalidate_way_without_writeback, @function
_dcache_invalidate_way_without_writeback:	
	set		8
	sync
	push	%R0, %R1
	/* check cache enabled */
	ldi 	0x0,%r1
	mvfc	0,%r9
	sync
	cmp		%r0,%r1
	jz		.L10

/* cache off */	
	ldi		0x0,%r0
	mvtc 	0,%r9
	sync

/* main body */
/*waybase invalidation	without writeback*/
	ldi		0x9,%r0
	mvtc	0,%R11
	sync
	
	ldi		0x19,%r0
	mvtc	0,%R11
	sync
	
/*  end of main-body */

/*cache on	*/
	ldi		CACH_ON_VAL,%r0
	mvtc 	0,%r9
	sync

.L10:
   	pop	%R0, %R1
	clr		8
	sync
   	jplr
	.size _dcache_invalidate_way_without_writeback, .-_dcache_invalidate_way_without_writeback	
