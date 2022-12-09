## -----------------------------------------
## -----------------------------------------
    .section .text
    .global _Reset_Start
	.type _Reset_Start, @function
_Reset_Start:
	
/* initialize stack pointer */    
    
    ldi 0x20004000-32,%r8 /*  stack pointer , use internal memory since sdram do not work yet */
    mov %r8,%sp
	
	jal _amazon2Init
	
	ldi _stack-12,%r8 /* stack pointer */
	mov %r8,%sp
	
    jal     _main

4:  jmp     4b

.size _Reset_Start, .-_Reset_Start
.section ._stack
_stack:    .long     1
