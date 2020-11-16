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
#ifndef MEMORY_SPACE_H
#define MEMORY_SPACE_H

#include <stddef.h>
#include <stdint.h>

class Memory_space {
	typedef struct page_entry {
			uint32_t present: 1;
			uint8_t *data;
	} Page_entry;

	unsigned page_size, page_bits, page_mask,
			 page_table_size, page_table_bits, page_table_mask;
	Page_entry *page_table;
	uint8_t *page_zero;
public:
	explicit Memory_space(size_t page_size, size_t space_size);
	~Memory_space();

	void read(uint32_t address, uint8_t *buffer, size_t size);
	void write(uint32_t address, uint8_t *data, size_t size);

	void write8(uint32_t address, uint8_t data);
	void write16(uint32_t address, uint16_t data);
	void write32(uint32_t address, uint32_t data);

	uint8_t read8(uint32_t address);
	uint16_t read16(uint32_t address);
	uint32_t read32(uint32_t address);
};

#endif
