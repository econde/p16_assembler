	.section Startup
	ldr	pc, addressof_main

addressof_main:
	.word	main

	.bss

	.data

	.text
main:
	mov	r0, #3

