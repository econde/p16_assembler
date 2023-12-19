	.data

	.equ	STACK_SIZE, 1024
	.space	STACK_SIZE
stack_top:

	.equ	MENOS_UM, -1

	.equ	VALOR_MAIOR_QUE_0xFFFF, 0x1ffff

	.space VALOR_MAIOR_QUE_0xFFFF, 0x100
	.space 20, -1
	.space 20, 0xffff
	.space 20, 0xffffffff

	.space	MENOS_UM

	.align
op2:
	.word	1
	.byte	MENOS_UM
	.word	MENOS_UM


	.text
	mov	r0, r0

