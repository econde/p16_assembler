	.syntax unified
	.thumb

	.section .startup, "ax"

	b	_start

_start:
	ldr	sp, addressof_stack_top
	mov	r0, pc
	add	lr, r0, #4
	ldr	pc, addressof_main
	b	.

addressof_stack_top:
	.word	stack_top

addressof_main:
	.word	main

	.section	.stack
	.space	1024
stack_top:

/*-----------------------------------------------------------------------------
*/

/*	#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(a[0]))
	uint16 table1[] = {10, 20, 5, 6, 34, 9};
	uint16 table2[] = {11, 22, 33};
	int16 p, q;

	int main() {
		p = search(table1, ARRAY_SIZE(table1), 20);
		q = search(table2, ARRAY_SIZE(table2), 31);
	}
*/

	.data

	.equ	TABLE1_SIZE, (table1_end - table1) / 2
table1:
	.word	10, 20, 5, 6, 34, 9
table1_end:

table2:
	.word	11, 22, 33
table2_end:

p:
	.word	0
q:
	.word	0

/*-----------------------------------------------------------------------------
	int main();
*/
	.text
main:
	push	lr
	ldr	r0, addressof_table1
	mov	r1, #TABLE1_SIZE
	mov	r2, #20
	bl	search
	ldr	r1, addressof_p
	str	r0, [r1]

	ldr	r0, addressof_table2
	mov	r1, #(table2_end - table2) / 2
	mov	r2, #44
	bl	search
	ldr	r1, addressof_q
	str	r0, [r1]
	mov	r0, #0
	pop	pc

addressof_table1:
	.word	table1
addressof_table2:
	.word	table2
addressof_p:
	.word	p
addressof_q:
	.word	q

/*-----------------------------------------------------------------------------

(r0)int16 search(uint16 array[] (r0), uint8 array_size (r1), uint16 value (r2)) {
	for( uint8 i (r3)= 0; i < array_size && array[i] != value; ++i)
		;
	if( i < array_size)
		return i;
	return -1;
}
*/
	.text
search:
	push	r3
	push	r4
	mov	r3, #0		/* r3 - i	*/
for:
	cmp	r3, r1		/* i - array_size	*/
	bhs	for_end
	ldr	r4, [r0, r3]	/* array[i] != value	*/
	cmp	r4, r2
	beq	for_end
	add	r3, r3, #2 	/* ++i	*/
	b	for
for_end:
	cmp	r3, r1		/* if (i < array_size)	*/
	bhs	if_end

	lsr	r0, r3, #1	/* return i	*/
	b	search_end
if_end:
	mov	r0, #0		/* return -1	*/
	sub	r0, r0, #1
search_end:
	pop	r4
	pop	r3
	mov	pc, lr
