	.equ	MAX, +64
	.equ	MENOS_UM, -1
	.equ	MAX_UINT16, 0xffff

	mov	r0, #MAX
	mov	r0, #+64
	mov	r0, #+63
	mov	r0, #-0
	mov	r0, #255
	mov	r0, #+255
	mov	r0, #0xffff >> 8
	mov	r0, #-1 >> 8

	.equ	VALUE, -2
	mov	r0, #(VALUE >> 8) & 0xff

	.word	-1
	.word	-1 >> 1
	.word	0xffff
	.word	0xffff >> 1
	.word	MENOS_UM
	.word	MENOS_UM >> 1
	.word	MAX_UINT16
	.word	MAX_UINT16 >> 1
