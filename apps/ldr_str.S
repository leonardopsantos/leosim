//	ldr r5, [r1]
//	ldr r7, [r14]
	// offset r5 = mem[r1-16]
//	ldr r5, [r1, #-16]
//	ldr r5, [r1, #16]
//	ldr r5, [r1, #0x3FF]
//	ldr r5, [r1, #-0x1F]
	// pre-indexed r5 = mem[r1+0x3], r1 = r1+0x3
//	ldr r5, [r1, #3]!
//	ldr r5, [r1, #0x7f]!
//	ldr r5, [r1, #-40] !
//	ldr r5, [r1, #-0x100] !
	// post-indexed r5 = mem[r1], r1 = r1+0x3
	ldr r5, [r1], #3
//	ldr r5, [r1], #0x3
//	ldr r5, [r1], #-0x0ff
//	ldr r5, [r1], #-0x0FF
//	ldr r5, [r1], #-7
	// register
//	str r5, [r1]
//	str	r7,	[r15]

	// offset
//	str r5, [r1, #-16]
//	str r5, [r1, #16]
//	str r5, [r1, #0x3FF]
//	str r5, [r1, #-0x1F]
	// pre-indexed
//	str r5, [r1, #3]!
//	str r5, [r1, #0x7f]!
//	str r5, [r1, #-20] !
//	str r5, [r1, #-0x80] !
	// pos-indexed
//	str r5, [r1], #3
//	str r5, [r1], #-3
//	str r5, [r1], #0xff
//	str r5, [r1], #0x7F
//	str r5, [r1], #-0x0D7
