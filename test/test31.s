	add	r0, r0, #-1
	asr	r0, r0, #-1
	ldr	r0, [r0, #-3]
	ldr	r0, [r10, #3 - 6]
	ldrb	r0, [r0, #-3]
	ldrb	r0, [r10, #10]
	.word	(0xffff + 0xffff) >> 1
	.word	-1 + 0xffff
	.word	-65536

