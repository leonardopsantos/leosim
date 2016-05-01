// this is a comment
	// this is also comment

//org: 0x00000000
//	org: 0x00000001
// org: 0x00000002
//	org:	0x00000003
//	 org:	 0x00000f04
//	 org:	 0x00000E05
//    mov r1, #0
//    mov r1, #0xFF
//    mov r1, #0xff
//    mov r1, r0
//    mov r1, r16
//    mov r31, r47
    // register
//	ldr r5, [r1]
	// offset
//    ldr r5, [r1, #-16]
//    ldr r5, [r1, #16]
//    ldr r5, [r1, #0x3FF]
//    ldr r5, [r1, #-0x1F]
    // pre-indexed r5 = mem[r1+0x3], r1 = r1+0x3
//    ldr r5, [r1, #3]!
//    ldr r5, [r1, #0x3]!
//    ldr r5, [r1, #3] !
//    ldr r5, [r1, #-0x3] !
    // post-indexed r5 = mem[r1], r1 = r1+0x3
//    ldr r5, [r1], #0x3
//    ldr r5, [r1], #3
//    ldr r5, [r1], #-0x007
//    ldr r5, [r1], #-7
    // register
//	str r5, [r1]
	// offset
//    str r5, [r1, #-16]
//    str r5, [r1, #16]
//    str r5, [r1, #0x3FF]
//    str r5, [r1, #-0x1F]
    // pre-indexed
//    str r5, [r1, #3]!
//    str r5, [r1, #0x3]!
//    str r5, [r1, #3] !
//    str r5, [r1, #-0x3] !
//    str r5, [r1], #0x3
//    str r5, [r1], #3
//    str r5, [r1], #-0x007
//    str r5, [r1], #-7



