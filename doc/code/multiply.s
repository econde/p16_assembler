	.section .startup
	b	_start
	b	.
_start:
	ldr	sp, addressof_stack_top
	ldr	r0, addressof_main
	mov	r1, pc
	add	lr, r1, 4
	mov	pc, r0
	b	.

addressof_stack_top:
	.word	stack_top
addressof_main:
	.word	main

	.text
	.rodata
	.data
	.bss
	.stack

/*---------------------------------------------------
uint8_t m = 20, n = 3;
*/
	.data
m:
	.byte	20
n:
	.byte	3

/*---------------------------------------------------
uint16_t p, q;
*/

	.bss
p:
	.word
q:
	.word

/*---------------------------------------------------
int main() {
	p = multiply(m, n);
	q = multiply(4, 7);
}
*/
	.text
main:
	push	lr
	ldr	r0, addressof_m
	ldrb	r0, [r0]
	ldr	r1, addressof_n
	ldrb	r1, [r1]
	bl	multiply
	ldr	r1, addressof_p
	str	r0, [r1]
	mov	r0, 4
	mov	r1, 7
	bl	multiply
	ldr	r1, addressof_q
	str	r0, [r1]
	pop	pc

addressof_m:
	.word	m
addressof_n:
	.word	n
addressof_p:
	.word	p
addressof_q:
	.word	q

/*---------------------------------------------------
<r0> int multiply(<r0> int multiplicand, <r1> int multiplier) {
	<r2> int product = 0;
	while (multiplier > 0) {
		product += multiplicand;
		multiplier--;
	}
	<r0> return product;
}
*/
multiply:
	mov	r2, 0
	add	r1, r1, 0
	bzs	while_end
while:
	add	r2, r2, r1
	sub	r1, r1, 1
	bzc	while
while_end:
	mov	r0, r2
	mov	pc, lr

	.section .stack
stack:
	.space	1024
stack_top:
