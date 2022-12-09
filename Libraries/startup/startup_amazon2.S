## -----------------------------------------
## -----------------------------------------

    .section .text
    .global _Reset_Start
	.type _Reset_Start, @function
_Reset_Start:

/* initialize stack pointer */    
    
    ldi 0x20004000-32,%r8 /*  stack pointer , use internal memory since dram does not works yet */
    mov %r8,%sp
	
	jal __startup
	
.size _Reset_Start, .-_Reset_Start
.section ._stack
_stack:    .long     1
