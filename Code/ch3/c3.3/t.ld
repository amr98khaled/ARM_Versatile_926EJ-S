ENTRY(reset_handler)
SECTIONS
{
    . = 0x10000;         /* loading address */
    .text : { ts.o *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    .data : { font.o }
    . = ALIGN(8);
    . = . + 0x4000;      /* 4kB of SVC stack space */
    svc_stack_top = .;
    . = . + 0x4000;      /* 4kB of IRQ stack space */
    irq_stack_top = .;
}
