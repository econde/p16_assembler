	.thumb
	.equ	MENOS_UM, -1
	.equ	MENOS_TRES, 0 - 3
	.equ	MENOS_SETE, 0 - 3
	.equ	VALOR_GRANDE, 0x122
	.equ	VALOR_MUITO_GRANDE, 1 << 16

	.text
	mov	r0, #MENOS_UM
	mov	r0, #-1
	mov	r0, #256
	movt	r0, #MENOS_SETE & 0xff
	movt	r0, #-7
	movt	r0, #-7 >> 31
	movt	r0, #(-7 >> 8) & 0xff
	mov	r9, #-1
	movt r10, #-128
	mov	r0, #MENOS_TRES
	mov	r0, #MENOS_TRES >> 7
	mov	r0, #VALOR_GRANDE
	mov	r0, #label

	.equ	VALUE2, 0xfffe
	mov	r0, #VALUE2 >> 8

	mov	r0, # - (1 << 16)
	.word	-(1 << 16)
label:
