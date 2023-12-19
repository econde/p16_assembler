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

	.equ	STACK_SIZE, stack_top - stack_begin

	.equ	ADDRESS_MAIN, main - 20

	.equ	MENOS_UM, -1

	.text
main:
	mov	r0, stack_top - stack_begin

	.stack
stack_begin:
	.space	256
stack_top:

addressof_main:
	.word ADDRESS_MAIN
