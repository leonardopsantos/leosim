org: 0x00000000
//	r0 = ponteiro para matriz A
//	r1 = ponteiro para matriz B
//	r30 = ponteiro para matriz resultante
//	r2 = # de linhas A
//	r3 = # de colunas B
//	r4 = # de colunas A

	mul r5, r3, #4
	mov r20, #0
loop1:
	mov r21, #0
loop2:
	mov r19, #0
	mul r10, r4, #4
	mul r10, r10, r20
	add r10, r10, r0
	mov r11, r1
	mla r11, r11, r21, #4
	mov r22, #0
loop3:
	ldr r12, [r10], #4
	ldr r13, [r11], r5
	add r22, r22, #1
	mla r19, r19, r12, r13
	bneq r22, r4, loop3
	str r19, [r30], #4
	add r21, r21, #1
	bneq r21, r3, loop2
	add r20, r20, #1
	bneq r20, r2, loop1
