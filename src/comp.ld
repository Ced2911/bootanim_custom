OUTPUT_FORMAT("elf32-sh")

SECTIONS
{
    . = 0x06010000;
    .text : { 
        *(.ctr0_text) 
        *(.text.main)
        *(.text)     
    }
    .tors : {
       ___ctors = . ;
       *(.ctors)
       ___ctors_end = . ;
       ___dtors = . ;
       *(.dtors)
       ___dtors_end = . ;
    }
    /* Anything rewritable should be in ram */
    .data : { *(.data) }
    __bss_start = .;
    .bss : { *(.bss) }
    __bss_end = .;
    _end = .;
}