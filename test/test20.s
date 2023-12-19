	.data
people:
	.word	0
people_0:
	.word	0
people_end:

	.equ	PERSON_SIZE, (people_0 - people)
	.equ	PEOPLE_SIZE, (people_end - people) / PERSON_SIZE

	mov	r0, #PERSON_SIZE
	.space	PEOPLE_SIZE

