org:0x00
// Linked list
// Element:
// struct linked_element_t {
//	struct linked_element_t *next;
//	int data;
//	struct linked_element_t *prev;
// }
//
// Finds a list element by value
// r0 = pointer to list head
// r1 = data to be found
// r2 = pointer to list tail

	mov r10, r0
loop1:
	ldr r11, [r10, #8]
	mov r12, r10
	ldr r10, [r10]
	bneq r11, r1, loop1
	beq r10, #0, loop_end
	bneq r10, r2, loop1
loop_end:
	mov r3, r12
	nop
