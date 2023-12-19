	.section .startup
	b	_start
	b	.

_start:
	ldr	sp, addressof_stack_top
	ldr	pc, addressof_main
	b	.

addressof_stack_top:
	.word	stack_top

addressof_main:
	.word	main

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


