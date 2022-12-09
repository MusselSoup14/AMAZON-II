/*****************************************************************************
* Copyright (C) 2005 Kim Hyun-Gyu  <babyworm@adc.co.kr>
*
* This program is free software. you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation@ either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY, without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program@ if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*****************************************************************************
*
* Notes:
*  Subbank Synthesis
*
*****************************************************************************

/****************************************************************************
 * void synth_full(struct mad_synth *, struct mad_frame const *,
 *                  unsigned int, unsigned int);
 *
 * %R8 : str mad_synth
 * %R9 : str mad_frame
 ***************************************************************************/

	.section	.rodata
	.align		2
.LC3:
	.string	"p2:%d\n"
.LC2:
	.string	"p1:%d\n"
.LC1:
	.string	"\n>>>pcm1:%d\n"
.LC0:
	.string	"%d "
.LCFO:
	.string	"%d\n"
.LCfilt:
	.string	"----------          %d\n"

	.text
	.align 1
	.global  _synth_full
	.global  synth_full
	.type	 _synth_full,@function
	.extern _dct32
	.extern _D

	
_synth_full:
synth_full:
	
/* Initial Push */
	push    %lr
	push    %R0, %R1, %R2, %R3, %R4, %R6
	push    %R10, %R11, %R12, %R13, %R14, %R15
	lea     (%sp,-120),%sp
	st      %r8,(%sp,176)
	st      %r9,(%sp,180)

/****************************************************************************
 * Stack Allocation 
 *  SP + 120 : %R10
 *  SP + 124 : 
 *     |
 *  SP + 172 : %LR
 *  SP + 176 : synth (pointer)
 *  SP + 180 : frame (pointer)
 *  SP + 184 : nch
 *  SP + 188 : ns
 ***************************************************************************/
	
  /* ch = 0 */	
	ldi 	0,%R11
	jmp _FOR_CHECK



_OUT_FOR_BODY:

  /*    sbsample = &frame->sbsample[ch];  */
  
	ld      (%sp,112),%r14        /* SP + 112: ch */
	ldi     4608,%r8
	mov     %r14, %R0
	mulu    %r8 , %r14            /* sbsample per channel size: 4608 */
	mov     %r14, %R1
  	add     48,%r14               /* sbsample offset in frame: 44 + 4 */
	ld      (%sp,180),%r13        /* SP + 180: frame pointer */
	add     %r14,%r13
	st      %r13,(%sp,84)         /* sbsample */


  /* filter   = &synth->filter[ch];       */
  	ldi     2048, %r8
  	mov     %R0,  %R14
	mulu    %r8 , %r14
	ld      (%sp,176),%r15        /* SP + 176: synth -> R15 */
	mov     %r15,%r11
	add     %r14,%r11
	st      %r11,(%sp,88)         /* filter */
	
	
	/*
TST_FILTER:
	mov   %r11,      %r9
	ldi   .LCfilt, %r8
	jal   _printf
	*/

/***************************************************************************
 * Register usage
 *    0       1       2       3       4       5       6       7
 *--------------------------------------------------------------------------
 *    ch      ch*smp  X[10]   t15     X[16]   t0      FP      t6
 *==========================================================================    
 *    8       9       10      11      12      13      14      15    
 *--------------------------------------------------------------------------
 *    X_ptr   x_ptr   NA      filter  NA      sbsmp   NA      synth 
 ***************************************************************************/
  	/* phase    = synth->phase;             */
  	ld      (%r15,4096),%r14      /* synth + 4096: phase */
	st      %r14,   (%sp,116)     /* phase */
  
  	/* pcm1     = synth->pcm.samples[ch];   */
	add     4108,%r1
	add     %r1,%r15
	st      %r15,(%sp,96)

/*
TEST_PARAM:
	mov   %r11,      %r9
	ldi   .LC0, %r8
	jal   _printf

	mov   %r13,      %r9
	ldi   .LC0, %r8
	jal   _printf

	mov   %r14,      %r9
	ldi   .LC0, %r8
	jal   _printf

	mov   %r15,      %r9
	ldi   .LCFO, %r8
	jal   _printf
	*/


	
	/*  for (s = 0; s < ns; ++s) {    */
	/* s = 0 */
	ldi     0,%r15
	st      %r15,(%sp,108)
	
	/*  ns */
	ld      (%sp,188),%r12
	cmp     %r12, %r15
	jnc     _CH_ADD
	/*
	ldi     4,%r14
	st      %r14,(%sp,80)
	*/
	
	
_FOR_LOOP:

/***************************************************************************
 * Register usage
 *    0       1       2       3       4       5       6       7
 *--------------------------------------------------------------------------
 *    ch      ch*smp  X[10]   t15     X[16]   t0      FP      t6
 *==========================================================================    
 *    8       9       10      11      12      13      14      15    
 *--------------------------------------------------------------------------
 *    X_ptr   x_ptr   NA      filter  ns      sbsmp   NA      s
 ***************************************************************************/
  /* 
  dct32((*sbsample)[s], phase >> 1,
            (*filter)[0][phase & 1], (*filter)[1][phase & 1]);
  */
 	ldi     128,  %r2
	ld      (%sp,108),%r14        /* s: must be reloaded    */
	mulu    %r2 , %r14            /* s * 128  */
	ld      (%sp,84),%r8          /* sbsample */
	add     %r14, %r8             /* sbsample[s] : 1st argu */

	ld      (%sp,116),%r9         /* phase    */
	mov     %r9,  %r0
	lsr     1,    %r9             /* phase >> 1 : 2nd argu */
	
	and     1,    %r0             /* phase & 1            */
	ldi     512,  %r3             /* filter size          */
	mov     %r0,  %r14            
	mulu    %r3 , %r14            /* filter*phase&1       */
	ld      (%sp,88),%r0          /* filter base          */
	mov     %r0,  %r15
	add     %r14, %r0             /* filter[0][phase&1]   */
	st      %r0,  (%sp,12)        /* 3rd                  */
	
	add     1024,%r14             
	add     %r15,%r14             /* filter[1][phase&1]   */
	st      %r14,(%sp,16)         /* 4th                  */

	jal     _dct32
	
	/*****************************/
	/* 	pe = phase & ~1;  	     */
	ld      (%sp,116),  %r10      /* phase                 */
	mov     %r10, %r11
	and     -2,         %r10
	
	/*  po = ((phase - 1) & 0xf) | 1;   */
	mov     %r11,   %r14
	sub     1,      %r14
	and     15,     %r14
	or      1,      %r14
	st      %r14,   (%sp,100)     /* po */
	
	/* fe = &(*filter)[0][ phase & 1][0]; */
	mov     %r0,    %r4         /* r0: filter[0][phase&1] */
	
	/* fx = &(*filter)[0][~phase & 1][0]; */
	mov     %r11,   %r8
	not     %r8
	and     1,      %r8
	mulu    %r3 ,   %r8
	ld      (%sp,88),%r0
	add     %r8,    %r0
	
	/* fo = &(*filter)[1][~phase & 1][0]; */
	mov     %r8,    %r3
	add     1024,   %r3
	add     %r15,   %r3
	/*
TST_POINT_param:
	mov   %r10,      %r9
	ldi   .LC0, %r8
	jal   _printf

	mov   %r14,      %r9
	ldi   .LC0, %r8
	jal   _printf

	mov   %r4,      %r9
	ldi   .LC0, %r8
	jal   _printf

	mov   %r0,      %r9
	ldi   .LC0, %r8
	jal   _printf

	mov   %r3,      %r9
	ldi   .LCFO, %r8
	jal   _printf
	*/
	

	/*  Dptr = &D[0];  */
	ldi     _D,     %r6
	
	/*  ptr = *Dptr + po; */
	mulu    4 ,     %r14        /* po * 4 */
	mov     %r14,   %r12        
	mov     %r6,    %r1
	add     %r12,   %r1


/***************************************************************************
 * Register usage
 *    0       1       2       3       4       5       6       7
 *--------------------------------------------------------------------------
 *    fx      ptr     128     fo      fe      NA      Dptr    NA
 *==========================================================================    
 *    8       9       10      11      12      13      14      15    
 *--------------------------------------------------------------------------
 *    NA      pse>>1  pe      phase   po*4    sbsmp   po*4    filterbase
 ***************************************************************************/
/*
      ML0(hi, lo, (*fx)[0], ptr[ 0]);
      MLA(hi, lo, (*fx)[1], ptr[14]);
      MLA(hi, lo, (*fx)[2], ptr[12]);
      MLA(hi, lo, (*fx)[3], ptr[10]);
      MLA(hi, lo, (*fx)[4], ptr[ 8]);
      MLA(hi, lo, (*fx)[5], ptr[ 6]);
      MLA(hi, lo, (*fx)[6], ptr[ 4]);
      MLA(hi, lo, (*fx)[7], ptr[ 2]);
*/

	ld    (%r0),    %r8
	ld    (%r1),    %r9
	ld    (%r0,4),  %r5
	ld    (%r1,56), %r14
	mul   %r9,      %r8
	mac   %r5,      %r14
	
  ld    (%r0,8),  %r8
	ld    (%r1,48), %r9
	ld    (%r0,12), %r5
	ld    (%r1,40), %r14
	mac   %r9,      %r8
	mac   %r5,      %r14  
	
	ld    (%r0,16), %r8
	ld    (%r1,32), %r9
	ld    (%r0,20), %r5
	ld    (%r1,24), %r14
	mac   %r9,      %r8
	mac   %r5,      %r14
	
	ld    (%r0,24), %r8
	ld    (%r1,16), %r9
	ld    (%r0,28), %r5
	ld    (%r1,8),  %r14
	mac   %r9,      %r8
	mac   %r5,      %r14
	mfml  %r2

/***************************************************************************
 * Register usage
 *    0       1       2       3       4       5       6       7
 *--------------------------------------------------------------------------
 *    fx      ptr     temp    fo      fe      NA      Dptr    NA
 *==========================================================================    
 *    8       9       10      11      12      13      14      15    
 *--------------------------------------------------------------------------
 *    NA      NA      pe      phase   po*4    sbsmp   NA      filterbase
 ***************************************************************************/
	/*
	ld (%sp,80),%r11
	mulu %r11 , %r14
	*/
	
	/* ptr = *Dptr + pe; */
	mov   %r10,     %r14
	mulu  4 ,       %r14
	st    %r14,     (%sp,20)      /* pe * 4 */
	mov   %r6,      %r1
  add   %r14,     %r1

/*
    ML0(hi, lo, (*fe)[0], ptr[ 0]);
    MLA(hi, lo, (*fe)[1], ptr[14]);
    MLA(hi, lo, (*fe)[2], ptr[12]);
    MLA(hi, lo, (*fe)[3], ptr[10]);
    MLA(hi, lo, (*fe)[4], ptr[ 8]);
    MLA(hi, lo, (*fe)[5], ptr[ 6]);
    MLA(hi, lo, (*fe)[6], ptr[ 4]);
    MLA(hi, lo, (*fe)[7], ptr[ 2]);
*/

	ld    (%r4),    %r8
	ld    (%r1),    %r9
	ld    (%r4,4),  %r5  
	ld    (%r1,56), %r14
	mul   %r9 ,     %r8
	mac   %r5 ,     %r14
	
	ld    (%r4,8),  %r8
	ld    (%r1,48), %r9
	ld    (%r4,12), %r5
	ld    (%r1,40), %r14
	mac   %r9 ,     %r8
	mac   %r5 ,     %r14
	
	ld    (%r4,16), %r8
	ld    (%r1,32), %r9
	ld    (%r4,20), %r5
	ld    (%r1,24), %r14
	mac   %r9 ,     %r8
	mac   %r5 ,     %r14
	
	ld    (%r4,24), %r8
	ld    (%r1,16), %r9
	ld    (%r4,28), %r5
	ld    (%r1, 8), %r14
	mac   %r9 ,     %r8
	mac   %r5 ,     %r14


  /* lo = lo -temp */
	mfml  %r8
	sub   %r2,      %r8
	
	/* *pcm1++ = SHIFT(MLZ(hi, lo)); */
	asr   0x2,      %r8
	ld    (%sp,96), %r9
	st    %r8,      (%r9, 0)
	addq  4,        %r9
	st    %r9,      (%sp,96)
	/*
TST_POINT1:
	ld    (%r9, -4), %r9
	ldi   .LC1, %r8
	jal   _printf
	*/
  
/***************************************************************************
 * Register usage
 *    0       1       2       3       4       5       6       7
 *--------------------------------------------------------------------------
 *    fx      ptr     temp    fo      fe      NA      Dptr    NA
 *==========================================================================    
 *    8       9       10      11      12      13      14      15    
 *--------------------------------------------------------------------------
 *    NA      pcm1    pe      phase   po*4    sbsmp   NA      filterbase
 ***************************************************************************/
  /* pcm2 = pcm1 + 30;            */
	add   120,      %r9               /* pcm2       */
	st    %r9,      (%sp,92)
	
	/* for (sb = 1; sb < 16; ++sb)  */
	ldi   1,        %r14
	st    %r12,     (%sp,72)          /* po* 4   없어져도 될듯   */
	st    %r14,     (%sp,104)         /* sb         */
 
  ld    (%sp,20), %r14              /* pe * 4 */
	st    %r3,      (%sp, 60)         /* fo backup -> 확인필요 */
	st    %r4,      (%sp, 28)         /* fe backup -> 확인필요 */
	
_INNER_FOR:
	/* ++fe         */
	/* ++Dptr       */
	add   32,       %r4
	add   0x80,     %r6           /* D[x][32]  */
	
	/* ptr = *Dptr + po */
	mov   %r6,      %r1
	add   %r12,     %r1
	
	/*
	      ML0(hi, lo, (*fo)[0], ptr[ 0]);
        MLA(hi, lo, (*fo)[1], ptr[14]);
        MLA(hi, lo, (*fo)[2], ptr[12]);
        MLA(hi, lo, (*fo)[3], ptr[10]);
        MLA(hi, lo, (*fo)[4], ptr[ 8]);
        MLA(hi, lo, (*fo)[5], ptr[ 6]);
        MLA(hi, lo, (*fo)[6], ptr[ 4]);
        MLA(hi, lo, (*fo)[7], ptr[ 2]);
        MLN(hi, lo);
  */
  
  
	ld    (%r3, 0), %r8
	ld    (%r1, 0), %R10
	ld    (%r3, 4), %r5
	ld    (%r1,56), %r14
	mul   %R10 ,     %r8
	mac   %r14,      %r5
	
	ld    (%r3, 8),   %r8
	ld    (%r1,48),   %R10
	ld    (%r3,12),   %r5
	ld    (%r1,40),   %r14
	mac   %R10 ,       %r8
	mac   %r14,        %r5
	
	ld    (%r3,16),   %r8
	ld    (%r1,32),   %R10
	ld    (%r3,20),   %r5
	ld    (%r1,24),   %r14
	mac   %R10 ,       %r8
	mac   %r14,        %r5
	
	ld    (%r3,24),   %r8
	ld    (%r1,16),   %R10
	ld    (%r3,28),   %r5
	ld    (%r1, 8),   %r14
	mac   %R10 ,       %r8
	mac   %r14,        %r5
	mfml  %r2



/***************************************************************************
 * Register usage
 *    0       1       2       3       4       5       6       7
 *--------------------------------------------------------------------------
 *    fx      ptr     temp    fo      fe      NA      Dptr    NA
 *==========================================================================    
 *    8       9       10      11      12      13      14      15    
 *--------------------------------------------------------------------------
 *    NA      pcm2    pe      phase   po*4    sbsmp   NA      filterbase
 ***************************************************************************/
 /* ptr = *Dptr + pe; */
  ld    (%sp,20), %r14 
  mov   %r6,      %r1
	add   %r14,     %r1

 /*
        MLA(hi, lo, (*fe)[7], ptr[ 2]);
        MLA(hi, lo, (*fe)[6], ptr[ 4]);
        MLA(hi, lo, (*fe)[5], ptr[ 6]);
        MLA(hi, lo, (*fe)[4], ptr[ 8]);
        MLA(hi, lo, (*fe)[3], ptr[10]);
        MLA(hi, lo, (*fe)[2], ptr[12]);
        MLA(hi, lo, (*fe)[1], ptr[14]);
        MLA(hi, lo, (*fe)[0], ptr[ 0]);
  */
 	ld    (%r4,28),   %r8
	ld    (%r1, 8),   %R10
	ld    (%r4,24),   %r5
	ld    (%r1,16),   %r14
	mul   %R10 ,       %r8
	mac   %r14,        %r5
	
 	ld    (%r4,20),   %r8
	ld    (%r1,24),   %R10
	ld    (%r4,16),   %r5
	ld    (%r1,32),   %r14
	mac   %R10 ,       %r8
	mac   %r14,        %r5
	
	ld    (%r4,12),   %r8
	ld    (%r1,40),   %R10
	ld    (%r4, 8),   %r5
	ld    (%r1,48),   %r14
	mac   %R10 ,       %r8
	mac   %r14,        %r5
 	
 	ld    (%r4, 4),   %r8
	ld    (%r1,56),   %R10
	ld    (%r4, 0),   %r5
	ld    (%r1, 0),   %r14
	mac   %R10 ,       %r8
	mac   %r14,        %r5
 	mfml  %r8
 	sub   %r2,        %r8
 	
 	/*  *pcm1++ = SHIFT(MLZ(hi, lo)); */	
 	asr   2,          %r8
  ld    (%sp,96),   %r9
	st    %r8,        (%r9, 0)
	addq  4,          %r9
	st    %r9,        (%sp,96)
	/*
TST_POINT2:
	ld    (%r9, -4), %r9
	ldi   .LC2, %r8
	jal   _printf
	*/
 	
 	
 	/* ptr = *Dptr - pe;      */
 	ld    (%sp,20), %r14 
  	mov   %r6,      %r1
	sub   %r14,     %r1
 	
 	/*
 	      ML0(hi, lo, (*fe)[0], ptr[31 - 16]);  // 15
        MLA(hi, lo, (*fe)[1], ptr[31 - 14]);  // 17
        MLA(hi, lo, (*fe)[2], ptr[31 - 12]);  // 19
        MLA(hi, lo, (*fe)[3], ptr[31 - 10]);  // 21
        MLA(hi, lo, (*fe)[4], ptr[31 -  8]);  // 23
        MLA(hi, lo, (*fe)[5], ptr[31 -  6]);  // 25
        MLA(hi, lo, (*fe)[6], ptr[31 -  4]);  // 27
        MLA(hi, lo, (*fe)[7], ptr[31 -  2]);  // 29
  */
  	ld    (%r4, 0),   %r8
	ld    (%r1,60),   %R10
	ld    (%r4, 4),   %r5
	ld    (%r1,68),   %r14
	mul   %R10 ,       %r8
	mac   %r14,        %r5
	
	ld    (%r4, 8),   %r8
	ld    (%r1,76),   %R10
	ld    (%r4,12),   %r5
	ld    (%r1,84),   %r14
	mac   %R10 ,       %r8
	mac   %r14,        %r5
	
	ld    (%r4,16),   %r8
	ld    (%r1,92),   %R10
	ld    (%r4,20),   %r5
	ld    (%r1,100),  %r14
	mac   %R10 ,       %r8
	mac   %r14,        %r5
	
	ld    (%r4,24),   %r8
	ld    (%r1,108),  %R10
	ld    (%r4,28),   %r5
	ld    (%r1,116),  %r14
	mac   %R10 ,       %r8
	mac   %r14,        %r5
	
	
/***************************************************************************
 * Register usage
 *    0       1       2       3       4       5       6       7
 *--------------------------------------------------------------------------
 *    fx      ptr     temp    fo      fe      NA      Dptr    NA
 *==========================================================================    
 *    8       9       10      11      12      13      14      15    
 *--------------------------------------------------------------------------
 *    NA      pcm2    pe      phase   po*4    sbsmp   NA      filterbase
 ***************************************************************************/
	/* ptr = *Dptr - po;  */
	mov   %r6,        %r1
	sub   %r12,       %r1
	
	/*
	MLA(hi, lo, (*fo)[7], ptr[31 -  2]);
        MLA(hi, lo, (*fo)[6], ptr[31 -  4]);
        MLA(hi, lo, (*fo)[5], ptr[31 -  6]);
        MLA(hi, lo, (*fo)[4], ptr[31 -  8]);
        MLA(hi, lo, (*fo)[3], ptr[31 - 10]);
        MLA(hi, lo, (*fo)[2], ptr[31 - 12]);
        MLA(hi, lo, (*fo)[1], ptr[31 - 14]);
        MLA(hi, lo, (*fo)[0], ptr[31 - 16]);
	*/

	ld    (%r3,28),   %r8
	ld    (%r1,116),  %R10
	ld    (%r3,24),   %r5
	ld    (%r1,108),  %r14
	mac   %R10 ,       %r8
	mac   %r14,        %r5
	
	ld    (%r3,20),   %r8
	ld    (%r1,100),  %R10
	ld    (%r3,16),   %r5
	ld    (%r1,92),   %r14
	mac   %R10 ,       %r8
	mac   %r14,        %r5
	
	ld    (%r3,12),   %r8
	ld    (%r1,84),   %R10
	ld    (%r3, 8),   %r5
	ld    (%r1,76),   %r14
	mac   %R10 ,       %r8
	mac   %r14,        %r5
	
	ld    (%r3, 4),   %r8
	ld    (%r1,68),   %R10
	ld    (%r3, 0),   %r5
	ld    (%r1,60),   %r14
	mac   %R10 ,       %r8
	mac   %r14,        %r5
	mfml  %r8
	
	/*
	 *pcm2-- = SHIFT(MLZ(hi, lo));
    ++fo;
  */
	asr   0x2,        %r8
	ld    (%sp,92),   %r15
	st    %r8,        (%r15, 0)
	sub   4,          %r15
	st    %r15,       (%sp,92)
	/*
TST_POINT3:
	ld    (%r15, 4), %r9
	ldi   .LC3, %r8
	jal   _printf
	*/

  	add   32,         %r3	
	
	/* sb++, if (sb < 15) */
	ld    (%sp,104),  %r11
	addq  1,          %r11
	st    %r11,       (%sp,104)
	cmpq  15,         %r11
	jls   _INNER_FOR
	
	
	
/***************************************************************************
 * Register usage
 *    0       1       2       3       4       5       6       7
 *--------------------------------------------------------------------------
 *    fx      ptr     temp    fo      fe      NA      Dptr    NA
 *==========================================================================    
 *    8       9       10      11      12      13      14      15    
 *--------------------------------------------------------------------------
 *    NA      pcm2    pe      phase   po*4    sbsmp   NA      filterbase
 ***************************************************************************/
  /*   ++Dptr;           */
  add   128,      %r6
	
	/* ptr = *Dptr + po;    */
  mov   %r6,      %r1
  add   %r12,     %r1
  
  /*
      ML0(hi, lo, (*fo)[0], ptr[ 0]);
      MLA(hi, lo, (*fo)[1], ptr[14]);
      MLA(hi, lo, (*fo)[2], ptr[12]);
      MLA(hi, lo, (*fo)[3], ptr[10]);
      MLA(hi, lo, (*fo)[4], ptr[ 8]);
      MLA(hi, lo, (*fo)[5], ptr[ 6]);
      MLA(hi, lo, (*fo)[6], ptr[ 4]);
      MLA(hi, lo, (*fo)[7], ptr[ 2]);
  */
  
  ld    (%r3, 0),   %r8
	ld    (%r1, 0),   %R10
	ld    (%r3, 4),   %r5
	ld    (%r1,56),   %r14
	mul   %R10 ,       %r8
	mac   %r14,        %r5
	
	ld    (%r3, 8),   %r8
	ld    (%r1,48),   %R10
	ld    (%r3,12),   %r5
	ld    (%r1,40),   %r14
	mac   %R10 ,       %r8
	mac   %r14,        %r5
	
	ld    (%r3,16),   %r8
	ld    (%r1,32),   %R10
	ld    (%r3,20),   %r5
	ld    (%r1,24),   %r14
	mac   %R10 ,       %r8
	mac   %r14,        %r5
	
	ld    (%r3,24),   %r8
	ld    (%r1,16),   %R10
	ld    (%r3,28),   %r5
	ld    (%r1, 8),   %r14
	mac   %R10 ,       %r8
	mac   %r14,        %r5
	mfml  %r2
	
	/*
	   *pcm1 = SHIFT(-MLZ(hi, lo));
      pcm1 += 16;
  */
	ldi   0,          %r8
	sub   %r2,        %r8
	asr   2,          %r8
	ld    (%sp,96),   %r15
	st    %r8,        (%r15)
	add   64,         %r15
	st    %r15,       (%sp,96)
	
	/* phase = (phase + 1) % 16;  */
	ld    (%sp,116),  %r11
	addq  1,          %r11
	and   15,         %r11
	st    %r11,       (%sp,116)
	
	
	ld    (%sp,108),  %r14                /* s load */
	addq  1,          %r14                /* s++ */
	st    %r14,       (%sp,108)           
	ld    (%sp,188),  %r15                /* ns load  */
	cmp   %r15,       %r14                /* compare  */
	jc _FOR_LOOP
	
	
/* ch = ch + 1 */
_CH_ADD:
	ld (%sp,112),%r11
	addq 1,%r11



/* End of for (for ch < nch) check */
_FOR_CHECK:
	ld    (%sp,184),%r14         /* nch load */
	st    %r11,(%sp,112)
	cmp   %r14, %r11
	jc    _OUT_FOR_BODY
	

/* restroing */
  lea (%sp,120),%sp	# reg=reg+i
	pop  %R10, %R11, %R12, %R13, %R14, %R15
	pop  %R0, %R1, %R2, %R3, %R4, %R6
	pop %pc



/* End of file */
