	.syntax unified 
	.thumb
	
/*-----------------------------------------------------------------------------
Usar TAB de 8 espaços
*/

	.section startup

	b	_start
	
_start:
	mov	sp, stack_bottom
	bl	main
	b	.

/*-----------------------------------------------------------------------------
*/
	.section .stack
stack:
	.space 64
stack_bottom:

	.equ	STACK_SIZE, stack_bottom - stack

/*-----------------------------------------------------------------------------
*/
	.data
a:
	.word	30
bb:
	.word	4
c:
	.word	0
	
	.text
main:
	ldr	r0, a
	ldr	r1, bb
	bl	div
	str	r0, c
	mov	pc, lr

/*-----------------------------------------------------------------------------
	
int div(uint16_t d, uint16_t o) {
r0		 r0	     r1			
	int r = 0, q = 0;
	    r3	   r4
	for (int i = 0; i < 16; ++i) {
		 r2
		r <<= 1;
		r |= d >> 15;
		d <<= 1;
		q <<= 1;
		if (r > o) {
			r -= o;
			q |= 1;
		}
	}
	return q;
		r0
}

*/

div:
	push	{r4}
	mov	r4, 0
	mov	r2, 16
div_1:
	lsl	r0, r0, 1
	adc	r3, r3, r3
	lsl	r4, r4, 1
	cmp	r3, r1
	bcs	div_2
	sub	r3, r3, r1
	add	r4, r4, 1
div_2:
	sub	r2, r2, 1
	bzc	div_1
	mov	r1, r3
	mov	r0, r4
	pop	{r4}
	mov	pc, lr

