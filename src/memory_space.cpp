/*
Copyright 2020 Ezequiel Conde

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

/*	
	Simulação de sistema de memória paginado de um nível.

	Dimensão da página e dimensão total do espaço de endereçamento parametrizáveis.

*/

#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <cassert>
#include "memory_space.h"

using namespace std;

static unsigned mylog2(size_t n) {
	unsigned bits = 0;
	while (n > 1) {
	  bits++;
	  n >>= 1;
	}
	return bits;
}

Memory_space::Memory_space(size_t pagex_size, size_t space_size) {
	assert((pagex_size & (pagex_size - 1)) == 0);		//	power of 2
	page_size = pagex_size;
	page_bits = mylog2(page_size);
	page_mask = page_size - 1;

	page_table_size = space_size / page_size;
	assert((page_table_size & (page_table_size - 1)) == 0);
	page_table_bits = mylog2(page_table_size);
	page_table_mask = page_table_size - 1;

	page_table = (Page_entry*) malloc(page_table_size * sizeof (Page_entry));
	page_zero = (uint8_t*) calloc(page_size, 1);
	for (unsigned i = 0; i < page_table_size; ++i) {
		page_table[i].present = 0;
		page_table[i].data = page_zero;
	}
}

Memory_space::~Memory_space() {
	for (unsigned i = 0; i < page_table_size; ++i) {
		if (page_table[i].present)
			free(page_table[i].data);
	}
	free(page_zero);
	free(page_table);
}

void Memory_space::read(uint32_t address, uint8_t *buffer, size_t buffer_size) {
	unsigned page_index = (address >> page_bits) & page_table_mask;
	unsigned byte_offset = address & page_mask;
	size_t size = buffer_size;
	while ( size > 0 ) {
		size_t nbytes = min(size, (size_t)(page_size - byte_offset));
		memcpy(buffer, page_table[page_index].data + byte_offset, nbytes);
		size -= nbytes;
		page_index = (page_index + 1) & page_table_mask;
		byte_offset = 0;
	}
}

void Memory_space::write(uint32_t address, uint8_t *data, size_t data_size) {
	unsigned page_index = (address >> page_bits) & page_table_mask;
	unsigned byte_offset = address & page_mask;
	size_t size = data_size;
	while ( size > 0 ) {
		size_t nbytes = min(size, (size_t)(page_size - byte_offset));
		if ( ! page_table[page_index].present) {
			page_table[page_index].data = (uint8_t*)calloc(page_size, 1);
			page_table[page_index].present = 1;
		}
		memcpy(page_table[page_index].data + byte_offset, data, nbytes);
		size -= nbytes;
		page_index = (page_index + 1) & page_table_mask;
		byte_offset = 0;
	}
}

void Memory_space::write8(uint32_t address, uint8_t data) {
	unsigned page_index = (address >> page_bits) & page_table_mask;
	unsigned byte_offset = address & page_mask;
	if ( ! page_table[page_index].present) {
		page_table[page_index].data = (uint8_t*) calloc(page_size, 1);
		page_table[page_index].present = 1;
	}
	*(page_table[page_index].data + byte_offset) = data;
}

void Memory_space::write16(uint32_t address, uint16_t data) {
	if ((address & 1) != 0) {
		write8(address, data);
		write8(address + 1, data >> 8);
	}
	unsigned page_index = (address >> page_bits) & page_table_mask;
	unsigned byte_offset = address & page_mask;
	if ( ! page_table[page_index].present) {
		page_table[page_index].data = (uint8_t*) calloc(page_size, 1);
		page_table[page_index].present = 1;
	}
	*(uint16_t*)(page_table[page_index].data + byte_offset) = data;
}

void Memory_space::write32(uint32_t address, uint32_t data) {
	if ((address & 3) != 0) {
		write8(address, data);
		write8(address + 1, data >> 8);
		write8(address + 2, data >> 16);
		write8(address + 3, data >> 24);
	}
	unsigned page_index = (address >> page_bits) & page_table_mask;
	unsigned byte_offset = address & page_mask;
	if ( ! page_table[page_index].present) {
		page_table[page_index].data = (uint8_t*) calloc(page_size, 1);
		page_table[page_index].present = 1;
	}
	*(uint32_t*)(page_table[page_index].data + byte_offset) = data;
}

uint8_t Memory_space::read8(uint32_t address) {
	unsigned page_index = (address >> page_bits) & page_table_mask;
	unsigned byte_offset = address & page_mask;
	return *(page_table[page_index].data + byte_offset);
}

uint16_t Memory_space::read16(uint32_t address) {
	if ((address & 1) != 0)
		return read8(address) + (read8(address + 1) << 8);
	unsigned page_index = (address >> page_bits) & page_table_mask;
	unsigned byte_offset = address & page_mask;
	return *(uint16_t *)(page_table[page_index].data + byte_offset);
}

uint32_t Memory_space::read32(uint32_t address) {
	if ((address & 3) != 0)
		return read8(address) + (read8(address + 1) << 8) +
			(read8(address + 2) << 16) + (read8(address + 3) << 24);
	unsigned page_index = (address >> page_bits) & page_table_mask;
	unsigned byte_offset = address & page_mask;
	return *(uint32_t *)(page_table[page_index].data + byte_offset);
}
