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

#ifndef SECTION_H
#define SECTION_H

#include <cstdint>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <fstream>

#include "properties.h"

namespace ast {

struct Section {
	std::string name;
	unsigned content_capacity, content_size;
	uint8_t *content;

	void enlarge(unsigned new_capacity);

    unsigned base_address;

	Section(std::string name) : name {name}, content_capacity {0}, content_size {0}, content(nullptr), base_address {0} { }
	
	void write8(unsigned offset, uint8_t value);
	void write16(unsigned offset, uint16_t value);
	void write32(unsigned offset, uint32_t b);
	void write_block(unsigned offset, const uint8_t *data, unsigned size);
	void fill(unsigned offset, uint8_t b, unsigned size);
	uint8_t read8(unsigned offset);
	uint16_t read16(unsigned offset);
	uint32_t read32(unsigned offset);
	void read_block(unsigned offset, uint8_t *buffer, unsigned size);
};

class Sections {
	static std::list<Section*> list;
public:
	static int align(unsigned address, unsigned alignment) {
		return ((address + (1 << alignment) - 1)
					/ (1 << alignment)) * (1 << alignment);
	}

	static std::vector<Section*> table;
	
	static void deallocate();
	
	static unsigned current_section;
	static unsigned current_offset;
	static void set_section(std::string name);

	static void set_address(unsigned section, unsigned base_address) {
		table.at(section)->base_address = base_address;
	}
	
	static unsigned get_address(unsigned section) {
		return table.at(section)->base_address;
	}

	//	Definir conteúdos de uma dada secção.

	static void write8(unsigned section, unsigned offset, uint8_t b) {
		table.at(section)->write8(offset, b);
	}
	
	static void write16(unsigned section, unsigned offset, uint16_t b) {
		table.at(section)->write16(offset, b);
	}

	static void write32(unsigned section, unsigned offset, uint32_t b) {
		table.at(section)->write32(offset, b);
	}

	static void append8(unsigned section, uint8_t b) {
		table.at(section)->write8(current_offset, b);
		current_offset += 1;
	}

	static void append16(unsigned section, uint16_t b) {
		table.at(section)->write16(current_offset, b);
		current_offset += 2;
	}

	static void append_block(unsigned section, const uint8_t *data, unsigned size) {
		table.at(section)->write_block(current_offset, data, size);
		current_offset += size;
	}
	
	static void fill(unsigned section, unsigned offset, uint8_t data, unsigned size) {
		table.at(section)->fill(offset, data, size);
		current_offset += size;
	}
	
	static void increase(unsigned size) {
		fill(current_section, current_offset, 0x55, size);
	}
	
	//	Ler conteúdos de uma dada secção.
	
	static uint8_t read8(unsigned section, unsigned offset) {
		return table.at(section)->read8(offset);
	}
	
	static uint16_t read16(unsigned section, unsigned offset) {
		return table.at(section)->read16(offset);
	}

	static void read_block(unsigned section, unsigned offset, uint8_t *buffer, unsigned size) {
		return table.at(section)->read_block(offset, buffer, size);
	}

	static void listing(std::ofstream& lst_file);

	//	Coloca as secções em lista ordenada por endereços.
	static bool address_is_free(Section *s);
	
	//	Localiza as secções.
	static void locate(Properties<std::string, unsigned> *section_addresses);
	
	//	Produz um ficheiro em formato Hex Intel com o conteúdo das secções.
	static void binary_hex_intel(const char *file_name);
	
	//	Produz um ficheiro em formato Logisim com o conteúdo das secções.
	static void binary_logisim(const char *file_name, unsigned word_size, unsigned byte_order);
	static void binary_raw(const char *file_name);
};

}

#endif
