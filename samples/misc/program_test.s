	.syntax unified 
	.cpu cortex-m3
	.thumb

/*-----------------------------------------------------------------------------
	Tabela de vetores.
	A localizar no endereço 00000000.
*/
	.section .startup,  "ax"
	.global	_start
_start:
//	ldr	pc, main_address
//	ldr	pc, isr_address

main_address:
	.word	main
	
isr_address:
	.word	isr

/*-----------------------------------------------------------------------------
	Área para stack.
*/
	.section .stack

	.space	1024 * 4, 0x55
stack_end:

/*-----------------------------------------------------------------------------
	Área para variáveis.
	A localizar em memória RAM.
*/
	.data

label_a:
	.hword 	20, 23, 2
	
label_b:

array:
	.byte	89, 92
	
array_address:
	.word	array

m:
	.word	3, 4, 5, 6, 7, 8
n:
	.word	10, 20, 30, 40, 50
	
	.equ	DIM, 0xff34
	.equ	c, 34
	.equ	MASK, 0b1001001
	.equ	OFFSET, 6

	.equ	N, 4

	.equ	ADDRESS1, 0x60

/*------------------------------------------------------------------------------
	Área para código.
	A localizar em ROM.
*/
	.text
main:
	b	long
back:
	.space	1023
long:
	b	back
	/*-------------------------------------------------------------------------
		loads e stores
	*/
//	ldr	r0, m_address
//	ldr	r1, n_address

//	ldr	r5, 78 - label_a + 8
//	ldr	r5, label_a + 8
//	ldr	r5, xxxxx + 8
//	ldr	r5, xxxxx + (label_a - label_b)
//	ldr r0, 4
//	ldr	r1, ADDRESS1
	ldr	r4, [r0, 4]
	ldr	r4, [r0, 5]
	ldr	r4, [r0, r3]
	ldr r4, [r3, N]

//	str	r5, label_b
	str	r4, [r0, 4]
	str	r4, [r0, 5]
	str	r4, [r0, r3]

//	ldrb	r5, label_a
	ldrb	r4, [r0, 4]
	ldrb	r4, [r0, 5]
	ldrb	r4, [r0, label_b - label_a]
	ldrb	r3, [r4, OFFSET]

//	strb	r5, label_b
	strb	r4, [r0, 4]
	strb	r4, [r0, 5]
	strb	r4, [r0, r3]

	/*-------------------------------------------------------------------------
		moves
	*/

//main:
	mov	r0, MASK
	mov	r0, label_b >= label_a

	mov	r0, label_b >= label_b
	mov	r0, label_b == 0x8000
	mov	r0, 0x8000 == 0x8000
	mov	r0, DIM >> 8
	mov	r1, (label_b - label_a) / 2
	mov	r4, 1 << 33
	mov	r0, r1
	mov pc, lr
//	mov	r4, label_a + 2
//	mov r3, 34 - xxxxx + 3
//	mov	r0, label_a >> 8
//	mov	r0, label_a * 8

	/*-------------------------------------------------------------------------
		stack
	*/

//	mov	sp, stack_end
//	movt	sp, stack_end >> 8
//	ldr	sp, stack_end_address
	push	{ r0 }
	pop		{ pc }
	
//label_b:
	/*-------------------------------------------------------------------------
		aritméticas
	*/
	add	r3, r4, r5
	add	r1, r1, 1
	add	r0, r0, 15
	
	add r0, r5, 37 - 34
	add r0, r5, 0

//	add r0, r5, MASK * DIM + 4
//	add	r3, r4, 4 - label_b + 3
	sub	r6, r7, label_a
//	add	r4, r4, zzzzz
	adc r6, r5, 13
	sub	r0, r6, r7
	sub r3, r5, 10

//	cmp	sp, lr
	
	/*-------------------------------------------------------------------------
		lógicas
	*/
	and	r3, r4, r5
	orr	r1, r1, r5

	eor	r0, r6, r7
	mvn r3, r5

	/*-------------------------------------------------------------------------
		Shift
	*/
	lsl	r3, r4, 3 * 4
	lsr	r1, r1, 10

	lsr	r4, r5, 3
	asr	r0, r6, 5
	ror r3, r5, 3

	rrx	r3, r7

	/*-------------------------------------------------------------------------
		Branch
	*/
label_1:
	b	.
//	b	ADDRESS1
	b	label_2
//	b	33 - label_2 - 2
//	bl	sort + 333
//	bl	a
label_2:
//	bcc	main
	bcs	label_1 + OFFSET
//	beq	b
//	bne	label_b
//	blt	label_c
//	ble	label_c
//	bgt	label_c
//	bge	label_c
//	b	label_a
//	b	XXXX + 3
	b	label_1 + (label_a - label_b)
//	b	label_a - XXXX

stack_end_address:
	.word	stack_end
	
m_address:
	.word	m
	
n_address:
	.word	n

isr:
	movs	pc, lr
