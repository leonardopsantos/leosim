org: 0x00000000

    mov r0, #0
    mov r1, #1000
    mov r30, #2000
    mov r2, #2
    mov r3, #2
    mov r4, #3
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
    ldr r12, [r10]

