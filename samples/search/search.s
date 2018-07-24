	.section startup
	b	_start

_start:
	mov	r0, pc
	mov	r1, stack_end_address - .
	ldr	r1, [r0, r1]
	mov	sp, r1

	mov	r0, pc
	ldr	r1, [r0, main_address - .]
	add	r0, r0, 8
	mov	lr, r0
	mov	pc, r1
	
	b	.
	
stack_end_address:
	.word	stack_end
main_address:
	.word	main

/*-----------------------------------------------------------------------------
*/
	.section directdata

/*	#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(a[0]))
	uint16 table1[] = {10, 20, 5, 6, 34, 9};
	uint16 table2[] = {11, 22, 33};
	int16 p, q;

	void main() {
		p = search(table1, ARRAY_SIZE(table1), 20);
		q = search(table2, ARRAY_SIZE(table2), 31);
	}
*/

/*-----------------------------------------------------------------------------
	Variáveis
*/
	.data
	.byte	22
	.align	2
	
	.equ	TABLE1_SIZE, (table1_end - table1) / 2
table1:
	.word	10, 20, 5, 6, 34, 9
table1_end:

table2:
	.word	11, 22, 33
table2_end:
	
	.section directdata
p:
	.word	0
q:
	.word	0

/*-----------------------------------------------------------------------------
	Stack
*/
	.section	.stack
	.space	1024
stack_end:

/*-----------------------------------------------------------------------------
	Zona de código
*/
	.text

/*-----------------------------------------------------------------------------
	int main();
*/
main:
	push	lr
	mov	r0, pc
	mov	r1, table1_address - .
	ldr	r0, [r0, r1]
	mov	r1, TABLE1_SIZE
	mov	r2, 20
	bl	search
	mov	r1, pc
	mov	r2, p_address - .
	ldr	r1, [r1, r2]
	str	r0, [r1]
	
	mov	r0, pc
	mov	r1, table2_address - .
	ldr	r0, [r0, r1]
	mov	r1, (table2_end - table2) / 2
	mov	r2, 44
	bl	search
	mov	r1, pc
	mov	r2, q_address - .
	ldr	r1, [r1, r2]
	str	r0, [r1]
	
	mov	r0, 0
	pop	pc

table1_address:
	.word	table1
table2_address:
	.word	table2
p_address:
	.word	p
q_address:
	.word	q

/*-----------------------------------------------------------------------------
	r0				r0			r1		r2
	int16 search(uint16 array[], uint8 array_size, uint16 value) {
		for( uint8 i = 0; i < array_size && array[i] != value; ++i)
			;
		if( i < array_size)
			return i;
		return -1;
	}
*/
	.text
search:
	mov	r3, 0		; r3 - i
for:
	sub	r6, r3, r1	; i - array_size
	bcc	for_end
	ldr	r4, [r0, r3]	; array[i] != value
	cmp	r4, r2
	bzs	for_end
	add	r3, r3,2 	; ++i
	b	for
for_end:
	cmp	r3, r1		; if (i < array_size)
	bcc	if_end
	
	lsr	r0, r3, 1	; return i
	b	search_end
if_end:
	mov	r0, 0		; return -1
	sub	r0, r0, 1
search_end:
	mov	pc, lr
