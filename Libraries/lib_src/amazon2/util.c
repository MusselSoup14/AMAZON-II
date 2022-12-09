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
#include "typedef.h"
#include "amazon2/util.h"
asm("\
.section .text \n\
	.global _get_cur_sp \n\
	.type _get_cur_sp, @function \n\
	\n\
_get_cur_sp: \n\
mov %sp, %r8 \n\
	jplr \n\
	.size _get_cur_sp, .-_get_cur_sp \n\
	\n\
	.section .text \n\
	.global _get_cur_heapend \n\
	.type _get_cur_heapend, @function \n\
_get_cur_heapend: \n\
push %lr	\n\
	ldi 0,%r8	\n\
	jal __sbrk	\n\
	pop %pc	\n\
	.size _get_cur_heapend, .-_get_cur_heapend ");
/**
 * Determines if heap and stack are conflict.
 *
 * \return true if it conflict, false if it fails.
 */
BOOL heap_stack_collision()
{
	if(get_cur_sp()<get_cur_heapend())
		return TRUE;
	return FALSE;
}
