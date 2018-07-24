//
// Created by ezequiel on 23-03-2018.
//

#ifndef AS_UTILS_H
#define AS_UTILS_H

/*			|<-width->|<--offset-->|
	0000000011111111110000000000000
	|<----------mask------------->|
*/

#include <limits.h>
#include <stdint.h>

#define MAKE_MASK(width, position) ((~(~0U << static_cast<unsigned>(width)) << static_cast<unsigned>(position)))

#define ABSOLUTE(value) (((value) & (1U << (sizeof(value) * CHAR_BIT - 1))) ? static_cast<unsigned>(-(value)) : (value))

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static inline int get_bits(uint32_t value, unsigned bit_first, unsigned bit_last) {
	return (value >> bit_first) & MAKE_MASK(bit_last - bit_first + 1, 0);
}

static inline int get_bit(uint32_t value, unsigned bit_position) {
	return get_bits(value, bit_position, bit_position);
}

#endif //AS_UTILS_H
