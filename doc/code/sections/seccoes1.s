	.data
x:
	.word	30
y:
	.word	4
z:
	.word	0

	.text
main:
	ldr	r0, addressof_x
	ldr	r0, [r0]
	ldr	r1, addressof_y
	ldr	r1, [r1]
	add	r0, r0, r1
	ldr	r1, addressof_z
	str	r0, [r1]
	mov	pc, lr

addressof_x:
	.word	x
addressof_y:
	.word	y
addressof_z:
	.word	z
