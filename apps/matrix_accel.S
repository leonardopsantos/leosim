org: 0x00000000
//	r0 = ponteiro para matriz A
//	mov r0, #0
//	r1 = ponteiro para matriz B
//	mov r1, #1000
//	r30 = ponteiro para matriz resultante
//	mov r30, #2000
//	r2 = # de linhas A
//	mov r2, #2
//	r3 = # de colunas B
//	mov r3, #2
//	r4 = # de colunas A
//	mov r4, #3
// 	r20 = contador de lacos linhas A
//	r21 = contador de lacos colunas B

	mov r37, #4
	mul r38, r3, #4
	mov r20, #0
loop1:
	mov r21, #0
loop2:
	mov r36, #0
	mul r32, r4, #4
	mul r32, r32, r20
	add r32, r32, r0
	mov r34, r1
	mla r34, r34, r21, #4
	mov r22, #0
	add r39, r4, #1
	//starts accelerator
	mov r40, #1
	nop
	nop
	// stores result
	add r21, r21, #1
	str r36, [r30], #4
	bneq r21, r3, loop2
	add r20, r20, #1
	bneq r20, r2, loop1
