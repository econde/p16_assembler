	.syntax unified 
	.thumb

	.section .startup, "ax"

	b	_start
	
_start:
	ldr	sp, addressof_stack_top
	bl	main
	b	.

addressof_stack_top:
	.word	stack_top

	.section .stack
stack:
	.space 64
stack_top:

/*-----------------------------------------------------------------------------
	
*/
	.data
var:
	.word	30
	
	.text
main:
	/* Carregamento de constante a 16 bits */
	
	.equ	N, 0x55aa
	mov	r0, N & 0xff
	movt	r0, N >> 8

	/* Carregamento de endereço para acesso a variável */
	
	ldr	r0, addressof_var
	
//	ldr	r0, [pc, addressof_var - . - 4]

	mov	r0, addressof_var - . - 4
	ldr	r0, [r0, pc]

//	adr	r0, addressof_var
//	ldr	r0, [r0]
	
	mov	pc, lr

addressof_var:
	.word	var


