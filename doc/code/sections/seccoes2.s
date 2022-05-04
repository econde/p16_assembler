/*--------------------------------------------
	Function strtok
*/
	.data
ptr:
	.word	0

	.text
strtok:
	ldr	r0, addr_ptr
	ldr	r0, [r0]
	; ...
	mov	pc, lr

addr_ptr:
	.word	ptr

/*--------------------------------------------
	Function accumulate;
*/
	.data
counter:
	.byte	0
	
	.text
accumulate:
	ldr	r0, addr_counter
	ldrb	r0, [r0]
	; ...
	mov	pc, lr

addr_counter:
	.word	counter
