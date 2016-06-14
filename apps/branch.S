
//	b loop
//	bl loop2

//	bx r10
//	blx r13

//	beq r11, #77, loop1
//	beq r11, #-99, loop2
//	beq r11, #0xFb, loop3
//	bneq r11, #-0xcA, loop4
//	beq r12, r14, loop5

//	bxeq r11, #77, r18
//	blxeq r11, #-99, r19
//	bxeq r11, #0xFb, r20
//	blxneq r11, #-0xcA, r21
//	bxeq r12, r14, r22

	bneq r22, r4, loop3
