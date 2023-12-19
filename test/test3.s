	.space	2
	.byte	0
	.BYTE	10

	.word	200
	.WORD	100
	msr	CpSr, r0

	adD	r0, R2, #3
	add	r0, R2, #3
	ADD	R10, r7, R15

b:
	.byte	7


	.BSS
xx:
	.byte	0

