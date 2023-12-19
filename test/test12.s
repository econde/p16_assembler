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

	.equ	AA, 20
	.equ	BB, 30

	.equ	STACK_SIZE, AA + BB

;	.equ	AAAA, 1

	.equ	XXXX, 1 / AAAA

	mov	r0, XXXX

	mov	r0, 1 / 0

	mov	r3, 1 / X

	.equ	X, 0
