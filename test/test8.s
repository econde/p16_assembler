	.text
	.bss
	.data

	.data

op1:
	.word	20
op2:
	.word	30

	.bss

	.equ	XX, 0x7FFFFFFF

	.byte	1

result:
	.word

	.stack
	.space	128

	.text
	
	
	ldr	r1, addressof_op1
	ldr	r2, addressof_op2

	ldr	r0, addressof_result

addressof_op1:
	.word	op1

addressof_op2:
	.word	op2

addressof_result:
	.word	result

	.byte
	mov	r0, r0
	

