    .text
	.global start
start:	ldr sp, =stack_top
	    bl  sum
stop:	b   stop
sum:
	mov r0, #0
	ldr r1, =Array
	ldr r2, =N
	ldr r2, [r2]
loop:
	ldr r3, [r1], #4
	add r0, r0, r3
	sub r2, r2, #1
	cmp r2, #0
	bne loop
	ldr r4, =Result
	str r0, [r4]
    mov pc,lr

	.data
N:	.word  10
Array:	.word  1,2,3,4,5,6,7,8,9,10
Result:	.word  0
