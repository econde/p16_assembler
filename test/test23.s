	.equ	MENOS_TRES, 0 - 3
	.equ	VALOR_GRANDE, 0x122

	.section .startup
	.space	64
	b label2

	.text
	.space	246
	mov	r0, #label
label:
	.space  10
label2:

	.section .text2
	mov	r0, #label2
	.byte	label2
