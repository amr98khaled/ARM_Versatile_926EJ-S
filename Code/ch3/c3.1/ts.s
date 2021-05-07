    .text
    .code 32
    .global reset_handler, vectors_start, vectors_end
    .global main , copy_vectors , IRQ_handler
    
reset_handler:
    LDR sp, =svc_stack_top      /* set SVC mode stack */
    BL copy_vectors             /* copy vector table to address 0 */
    MSR cpsr, #0x92             /* to IRQ mode *//* CPSR[4:0] = 0x12 (IRQ mode) , CPSR[5] = 0 (Execute in ARM state) , CPSR[7-6] = 10 (Disable IRQ but keep FIQ enabled). */
    LDR sp, =irq_stack_top      /* set IRQ mode stack */
    MSR cpsr, #0x13             /* go back to SVC mode with IRQ on *//* CPSR[4:0] = 0x13 (SVC mode) , CPSR[5] = 0 (Execute in ARM state) , CPSR[7-6] = 00 (Enable IRQ & FIQ). */
    BL main                     /* call main() in C */
    B .                         /* loop if main ever return */
    
irq_handler:
    sub lr, lr, #4
    stmfd sp!, {r0-r12, lr}     /* stack r0-r12 and lr */
    bl IRQ_handler              /* call IRQ_hanler() in C */
    ldmfd sp!, {r0-r12, pc}^    /* return , ^ restores cpsr from spsr */
    
vectors_start:
    LDR PC, reset_handler_addr
    LDR PC, undef_handler_addr
    LDR PC, swi_handler_addr
    LDR PC, prefetch_abort_handler_addr
    LDR PC, data_abort_handler_addr
    B .
    LDR PC, irq_handler_addr
    LDR PC, fiq_handler_addr
    
    reset_handler_addr:           .word reset_handler
    undef_handler_addr:           .word undef_handler
    swi_handler_addr:             .word swi_handler
    prefetch_abort_handler_addr:  .word prefetch_abort_handler
    data_abort_handler_addr:      .word data_abort_handler
    irq_handler_addr:             .word irq_handler
    fiq_handler_addr:             .word fiq_handler
vectors_end:
