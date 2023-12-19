	.text
y:
	.space	511 * 2
	b	y
	b	x
	.space	511 * 2
x:
	b	forward_label

	.space	511 * 2
forward_label:

backward_label:
	.space 511 * 2

	b	backward_label
