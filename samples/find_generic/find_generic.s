	.section .startup

	b	_start

_start:
	ldr	sp, addr_stack_top
	ldr	r0, addr_main
	mov	r1, pc
	add	lr, r1, 4
	mov	pc, r0
	b	.

addr_stack_top:
	.word	stack_top
	
addr_main:
	.word	main

	.section .stack
	.equ	STACK_SIZE, 1024
	.space	STACK_SIZE
stack_top:

/*==============================================================================
#include <stdio.h>
#include <string.h>

#define ARRAY_SIZE(a)	(sizeof(a)/sizeof(a[0]))

int cmp_name (const void *a, const void *b);

size_t find (void *array, size_t array_size, size_t elem_size,
	int (*cmp)(const void *, const void *), const void *context,
	void **result, size_t result_size);

typedef struct {
	char *name;
	char *surname;
	char height;
	char weight;
	int born;
} Person;

Person people [] = {
	{"antonio", "pinto", 180, 75, 50700},
	{"antonio", "silva", 164, 65, 50700},
	{"luis", "silva", 177, 80, 50099}
};

Person *result [ARRAY_SIZE(people)];

int main () {
	return find(people, ARRAY_SIZE(people), sizeof(people[0]),
		cmp_name, "luis silva", (void **)result, ARRAY_SIZE(result));
}
*/
	.section .rodata
antonio:
	.asciz	"antonio"
pinto:
	.asciz	"pinto"
silva:
	.asciz	"silva"
luis:
	.asciz	"luis"
luis_silva:
	.asciz	"luis silva"
		
	.data
people:
	.word	antonio
	.word	pinto
	.byte	180
	.byte	75, 0, 0, 77
	.align
	.word	50700
people_0:
	.word	antonio
	.word	silva
	.byte	164
	.byte	65, 0, 0
	.word	50700
	
	.word	luis
	.word	silva
	.byte	177
	.byte	80, 0, 0
	.word	50099
people_end:

	.equ	PERSON_SIZE, (people_0 - people)
	.equ	PEOPLE_SIZE, (people_end - people) / PERSON_SIZE

	.equ	RESULT_SIZE, PEOPLE_SIZE

result:
	.space	RESULT_SIZE * 2

	.text
main:
	push	lr
	mov	r0, RESULT_SIZE
	push	r0
	ldr	r0, addr_result
	push	r0
	ldr	r0, addr_luis_silva
	push	r0	
	ldr	r0, addr_people
	mov	r1, PEOPLE_SIZE
	mov	r2, PERSON_SIZE
	ldr	r3, addr_cmp_name
	bl	find
	mov	r1, 3 * 2
	add	sp, r1, sp 
	pop	pc

addr_result:
	.word	result
addr_people:
	.word	people
addr_cmp_name:
	.word	cmp_name
addr_luis_silva:
	.word	luis_silva
	
/*------------------------------------------------------------------------------
(r0) size_t find (void *array (r0), size_t array_size (r1), size_t elem_size (r2) (r8),
		int (*cmp (r3)(r10))(const void *, const void *), const void *context (r7[2]),
		void *result[] (r7[4]), size_t result_size (r7[6]) (r5)) {
	void **result_iter (r6) = result;
	void *last (r9) = array + array_size * elem_size;
	for (void *iter (r4) = array; iter < last ; iter += elem_size) {
		if (cmp(iter, context) == 0) {
			*result_iter++ = iter;
			if (--result_size == 0)
				break;
		}
	}
	return result_iter - result;
}
	---------
	|	|
	---------
	|result_size|		r7[6]
	---------
	|result	|		r7[4]
	---------
	|context|	sp	r7[2]
	---------
	|r7	|	r7
	---------
	|lr	|
	---------
	|r4	|
	---------
	|r5	|
	---------
	|r6	|
	---------
	|r8	|
	---------
	|r9	|
	---------
	|r10	|
	---------
	|	|
*/
	.text
find:
	push	r7
	mov	r7, sp		
	push	lr
	push	r4
	push	r5
	push	r6
	push	r8
	push	r9
	push	r10

	mov	r8, r2
	mov	r10, r3
	ldr	r5, [r7, 6]	/* size_t result_size (r7[6]) (r5) */

	ldr	r6, [r7, 4]	/* void **result_iter (r6) = result; */

	mov	r4, r0		/* for (void *iter (r4) = array; */

	mov	r9, r0
	mov	r0, r1
	mov	r1, r2
	bl	multiply	/* array_size * elem_size */
	add	r9, r0, r9	/* void *last  = array + array_size * elem_size; */
for:
	cmp	r4, r9		/* iter < last ;  iter - last */
	bhs	for_end
	mov	r0, r4		/* cmp(iter, context) */
	ldr	r1, [r7, 2]
	mov	r2, pc
	add	lr, r2, 4
	mov	pc, r10		
	sub	r0, r0, 0		/* if (cmp(iter, context) == 0) { */
	bne	if_end
	str	r4, [r6]	/* *result_iter++ = iter; */
	add	r6, r6, 2
	sub	r5, r5, 1		/* if (--result_size == 0) */
	beq	for_end		/*	break;	*/
if_end:
	add	r4, r4, r8	/* iter += elem_size) { */
	b	for
for_end:
	ldr	r0, [r7, 4]	/* return result_iter - result; */
	sub	r0, r6, r0
	lsr	r0, r0, 1
	
	pop	r10
	pop	r9
	pop	r8
	pop	r6
	pop	r5
	pop	r4
	pop	lr
	pop	r7
	mov	pc, lr

/*------------------------------------------------------------------------------
int cmp_name (const void *a, const void *b) {
	char full_name [strlen (((Person*)a)->name) + 1 + strlen (((Person*)a)->surname) + 1];
	strcpy (full_name, ((Person*)a)->name);
	strcat (full_name, " ");
	strcat (full_name, ((Person*)a)->surname);
	return strcmp (full_name, (char *)b);
}
*/

cmp_name:
	push	lr
	push	r4
	push	r5
	push	r6
	mov	r4, r0
	mov	r5, r1
	ldr	r0, [r4, 0]	/* ((Person*)a)->name) */
	bl	strlen
	add	r6, r0, 1
	ldr	r0, [r4, 2]	/* ((Person*)a)->surname) */
	bl	strlen
	add	r0, r0, 1
	add	r6, r6, r0
	mov	r0, sp
	sub	r0, r0, r6
	mov	sp, r0		/* char full_name [ */
	;mov	r0, sp
	ldr	r1, [r4, 0]
	bl	strcpy		/* strcpy (full_name, ((Person*)a)->name); */
	mov	r0, sp
	ldr	r1, addr_space_string
	bl	strcat		/* strcat (full_name, " "); */
	mov	r0, sp
	ldr	r1, [r4, 2]
	bl	strcat		/* strcat (full_name, ((Person*)a)->surname); */
	
	mov	r0, sp
	mov	r1, r5
	bl	strcmp
	mov	r1, sp
	add	r1, r1, r6
	mov	sp, r1
	pop	r6
	pop	r5
	pop	r4
	pop	pc

addr_space_string:
	.word	space_string

	.section .rodata
space_string:
	.asciz	" "

/*------------------------------------------------------------------------------
int strcmp(const char * str1, const char * str2) {
	for ( ; *str1 && *str1 == *str2; str1++, str2++)
		;
	return *str1 - *str2;
}
*/
	.text
strcmp:
	b	strcmp_for_cond
strcmp_for:
	add	r0, r0, 1
	add	r1, r1, 1
strcmp_for_cond:
	ldrb	r2, [r0]
	ldrb	r3, [r1]
	sub	r2, r2, 0
	beq	strcmp_for_end
	cmp	r2, r3
	beq	strcmp_for
strcmp_for_end:
	sub	r0, r2, r3
	mov	pc, lr
	
/*------------------------------------------------------------------------------
char *strcat(char *dst, const char *src) {
	char *aux = dst;
	while (*dst)
		dst++;
	while (*src)
		*dst++ = *src++;
	*dst = '\0';
	return aux;
}
*/
strcat:
	b	strcat_while1_cond
strcat_while1:
	add	r0, r0, 1
strcat_while1_cond:
	ldrb	r2, [r0]
	sub	r2, r2, 0
	bne	strcat_while1
	b	strcat_while2_cond
strcat_while2:
	strb	r2, [r0]
	add	r0, r0, 1
	add	r1, r1, 1
strcat_while2_cond:
	ldrb	r2, [r1]
	sub	r2, r2, 0
	bne	strcat_while2
	mov	r2, 0
	strb	r2, [r0]
	mov	pc, lr

/*------------------------------------------------------------------------------
char *strcpy(char *dst, const char *src) {
    char *aux = dst;
    while (*src)
        *dst++ = *src++;        
    *dst = *src;
    return aux;
}
*/
strcpy:
	b	strcpy_while_cond
strcpy_while:
	strb	r2, [r0]
	add	r0, r0, 1
	add	r1, r1, 1
strcpy_while_cond:
	ldrb	r2, [r1]
	sub	r2, r2, 0
	bne	strcpy_while
	mov	r2, 0
	strb	r2, [r0]
	mov	pc, lr

/*------------------------------------------------------------------------------
size_t strlen(const char *str) {
	size_t i;
	for (i = 0; *str++; i++)
		;
	return i;
}
*/
strlen:
	mov	r1, 0		/* i = 0 */
	b	strlen_cond
strlen_for:
	add	r1, r1, 1	/* i++ */
strlen_cond:
	ldrb	r2, [r0]
	add	r0, r0, 1
	sub	r2, r2, 0
	bne	strlen_for
	mov	r0, r1		/* return i */
	mov	pc, lr
	
/*------------------------------------------------------------------------------
uint32_t multiply(uint16_t multiplicand, uint16_t multiplier) {
	uint16_t result = 0;
	while (multiplier != 0) {
		if ((multiplier & 1) != 0)
			result += multiplicand;
		multiplier >>= 1;
		multiplicand <<= 1;
	}
	return result;
}
*/
multiply:
	push	r4

	mov	r3, 0	/* result = 0 */
	mov	r2, 0
	mov	r4, 0
	b	multiply_while_cond
multiply_while:
	lsr	r1, r1, 1
	bcc	multiply_if_end
	add	r2, r2, r0
	adc	r3, r3, r4
multiply_if_end:
	lsl	r0, r0, 1
multiply_while_cond:
	sub	r1, r1, 0
	bne	multiply_while
	mov	r1, r3
	mov	r0, r2

	pop	r4
	mov	pc, lr
