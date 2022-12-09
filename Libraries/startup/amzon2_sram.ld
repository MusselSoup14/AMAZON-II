OUTPUT_FORMAT("elf32-ae32000-little", "elf32-ae32000-big",
			"elf32-ae32000-little")
OUTPUT_ARCH(ae32000)
ENTRY(_Reset_Start)
/*GROUP(-lc -lgcc -lm )*/
/*SEARCH_DIR(.);*/

MEMORY
{
	rom (rx)	: ORIGIN = 0x00000000,	LENGTH = 512K
	/* vector table will stored at 0x20000000 by InitInterrupt()*/
	spm(rwx)	: ORIGIN = 0x20000200,	LENGTH = (16*1024-0x200)
	ram (!rx)	: ORIGIN = 0x20001000,	LENGTH = (16*1024-0x1000)
}

SECTIONS
{
  .text : {
	*(.vects)
	*(.text)
    *(.text.*)
	. = ALIGN(4);
	*(.rodata)
	*(.rodata.*)
	*(.rodata.str1.*)
	*(.rodata1)
	. = ALIGN(4);
	___ctors = . ;
	*(.ctors)
	___ctors_end = . ;
	___dtors = . ;
	*(.dtors)
	___dtors_end = . ;
	*(.gnu.linkonce.*) 
	*(.gcc_except_table) 
  } > rom
  _etext = . ;
  PROVIDE (etext = .);
    
  .spm : AT ( (LOADADDR (.text) + SIZEOF(.text) + 3) & ~ (4 - 1)) {
    . = ALIGN(4);
    __ram_spm_start = . ;
    *(.spm)
    . = ALIGN(4);
    __ram_spm_end = .;
  } > spm
  __rom_spm_start  =  LOADADDR (.spm);

  .data : AT ( (LOADADDR (.spm) + SIZEOF(.spm) + 3) & ~ (4 - 1)) {
    . = ALIGN(4);
    __ram_data_start = . ;
    *(.data)
    *(.sdata)
    /*CONSTRUCTORS*/
    . = ALIGN(4);
    __ram_data_end = .;
  } > ram
  __rom_data_start  =  LOADADDR (.data);


  .bss ALIGN(0x4) : AT ((LOADADDR (.data) + SIZEOF(.data) + 3) & ~ (4 - 1) ) {
	  . = ALIGN(4);
    __bss_start = .;
    *(.dynbss)
    *(.bss)
    *(COMMON)
    . = ALIGN(4);
    __bss_end = .;
  } > ram

  _end = . ;
  PROVIDE (end = .);


  ._stack 0x20003fd0 : 
  {
    _stack = .;
    *(._stack)
  } 
}
PROVIDE(___data_size = SIZEOF(.data));
PROVIDE(___bss_size = SIZEOF(.bss));


