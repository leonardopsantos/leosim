//	mul r10, r11, r12
//	mul r13,	r14,	r15

	mla r10, r11, r12, r13
//	mla r13,	r14,	r15, 	r17
//	mul r5, r3, #4
//	mul r5, r3, #-4
//	mul r5, r3, #0xc4
//	mul r5, r3, #-0xc4

	mla r11, r11, r21, #4
	mla r11, r11, r21, #0xFe
	mla r11, r11, r21, #-0x4
