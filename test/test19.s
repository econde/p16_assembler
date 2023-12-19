	.section .startup
	b	_start
	b	.

_start:
	ldr	sp, addressof_stack_top
	bl	main
	b	.

addressof_stack_top:
	.word	stack_top

	.text
	.rodata
	.data
	.stack

	.text
main:
	mov	pc, lr

	.stack
	.space	1024
stack_top:


	mov	r3, 1 / X

	.equ	X, 0

	mov	r4, 2 / X

	mov	r3, 1 / Y

	.equ	Y, X

	mov	r4, 1 / (3 - 3)
