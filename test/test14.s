	.equ	X, Y + 1

	.byte	Y + 1

	.space -1, 2

	mov	r1, Y + 1
	mov	r0, label
label:

