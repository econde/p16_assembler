/*
Copyright 2018 Ezequiel Conde

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

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

#define ABSOLUTE(value) (((value) & (1U << (sizeof(value) * CHAR_BIT - 1))) \
                        ? static_cast<unsigned>(-(value)) : (value))

static inline int get_bits(uint32_t value, unsigned bit_first, unsigned bit_last) {
	return (value >> bit_first) & MAKE_MASK(bit_last - bit_first + 1, 0);
}

static inline int get_bit(uint32_t value, unsigned bit_position) {
	return get_bits(value, bit_position, bit_position);
}

#endif //AS_UTILS_H
