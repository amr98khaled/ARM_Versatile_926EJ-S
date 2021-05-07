    .text
    .global start, sum
start:
    ldr sp, =stack_top
    ldr r2, =a
    ldr r0, [r2]
    ldr r2, =b
    ldr r1, [r2]
    bl sum
    ldr r2, =c
    str r0, [r2]
stop: b stop

    .data
a: .word 1
b: .word 2
c: .word 0

