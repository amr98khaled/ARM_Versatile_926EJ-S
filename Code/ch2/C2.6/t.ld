ENTRY(start)
SECTIONS
{
    . = 0x10000; /* loading address, required by QEMU */
    .text : { ts.o *(.text) } /* all text in .text section */
    .data : { *(.data) } /* all data in .data section */
    .bss : { *(.bss) }   /* all bss in .bss section */
    
    /***** include the RAW binary file as a data section ************/
  .data : { panda.o porkcar.o }
  . = ALIGN(8);
  . = . + 0x4000; /* 4kB of stack memory */
  stack_top = .;    /* stack_top is a symbol exported by linker */
}

