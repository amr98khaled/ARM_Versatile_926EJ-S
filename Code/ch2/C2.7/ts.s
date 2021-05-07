/* when the ARM CPU starts, it is in the Supervisor or SVC mode. The ts.s file sets the SVC mode stack pointer and calls main() in t.c */
        .text
        .global main
        .global start, stack_top
         
start:
        ldr sp, =stack_top
        bl main
        b .             /* if main return just loop */
