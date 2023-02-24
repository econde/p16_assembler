	.syntax unified
	.thumb

	.section .startup, "ax"

	b	_start
	b	.

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

	.section .stack
stack:
	.space 64
stack_top:

/*-----------------------------------------------------------------------------
int x = 30, y = 4, z;

int main() {
	z = div(x, y);
}
*/
	.data
x:
	.word	30
y:
	.word	4
z:
	.word	0

	.text
main:
	ldr	r0, addressof_x
	ldr	r0, [r0]
	ldr	r1, addressof_y
	ldr	r1, [r1]
	bl	div
	ldr	r1, addressof_z
	str	r0, [r1]

	mov	pc, lr

addressof_x:
	.word	x
addressof_y:
	.word	y
addressof_z:
	.word	z

/*-----------------------------------------------------------------------------

(r0) uint16 int div(uint16 dividend (r0), uint16 divisor (r1)) {
	uint16 rest (r3) = 0, quocient (r4) = 0;
	for ( int i (r2) = 0; i < 16; ++i) {
		uint16 dividend_msb = dividend >> 15;
		dividend <<= 1;
		rest = (rest << 1) | dividend_msb;
		quotient <<= 1;
		if (rest >= divisor) {
			rest -= divisor;
			quotient += 1;
		}
	}
	return quotient (r0);
}
*/

div:
	push	lr
	push	r3
	push	r4
	mov	r4, #0
	mov	r2, #16
div_1:
	lsl	r0, r0, #1
	adc	r3, r3, r3
	lsl	r4, r4, #1
	cmp	r3, r1
	blo	div_2
	sub	r3, r3, r1
	add	r4, r4, #1
div_2:
	sub	r2, r2, #1
	bne	div_1
	mov	r1, r3
	mov	r0, r4
	pop	r4
	pop	r3
	pop	lr
	mov	pc, lr
