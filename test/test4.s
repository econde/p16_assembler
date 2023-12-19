	.text
;	str	r0, addressof_top_of_stack
;	ldrb	r0, addressof_top_of_stack
	ldr	r0, addressof_top_of_stack
	.space	126

addressof_top_of_stack:
	.word	top_of_stack

	.section .stack
	.space	128
top_of_stack:

;---------------------------------------------------------------------------
;	ldr	r9, 0x1000

;---------------------------------------------------------------------------

	.text

	ldr	r0, addressof_x + 10
	.space	100

addressof_x:
	.word	top_of_stack


	ldr	r0, addressof_y
;	.space	126	; não deve dar erro
	.space	127
;	.space	128

addressof_y:
	.word	top_of_stack
