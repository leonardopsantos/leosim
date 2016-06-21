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
// r3 = pointer to result
//
// Accelerator registers:
// r32 = pointer to first element
// r33 = pointer to tail
// r34 = data to be found
// r35 = offset from pointer to data (8)
// r37 = pointer to result
// r41 = status register

	mov r3, #0
	mov r32, r0
	mov r34, r1
	mov r33, r2
	mov r35, #8
	mov r41, #1
	nop
	nop
	nop
	mov r3, r37
