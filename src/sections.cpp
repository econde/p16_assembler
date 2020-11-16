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

#include <cstdlib>
#include <cassert>
#include <cstring>
#include <iostream>
#include <fstream>
#include "sections.h"
#include "cpp_printf.h"
#include "error.h"
#include "memory_space.h"

namespace ast {

using namespace std;

#define	CHUNK	1024

void Section::enlarge(unsigned new_capacity) {
	new_capacity = ((new_capacity + CHUNK - 1) / CHUNK) * CHUNK;
	content = (uint8_t *)realloc(content, new_capacity);
	assert(content != nullptr);	//	throw exception
	content_capacity = new_capacity;
}

void Section::write8(unsigned offset, uint8_t value) {
	auto limit_superior = offset + 1;
	if (limit_superior > content_capacity)
		enlarge(limit_superior);
	content[offset] = value;
	if (limit_superior > content_size)
		content_size = offset + 1;
}

void Section::write16(unsigned offset, uint16_t value) {
	auto limit_superior = offset + 2;
	if (limit_superior > content_capacity)
		enlarge(limit_superior);
	content[offset]     = static_cast<uint8_t >(value);
	content[offset + 1] = static_cast<uint8_t >(value >> 8);
	if (limit_superior >= content_size)
		content_size = offset + 2;
}

void Section::write32(unsigned offset, uint32_t value) {
	auto limit_superior = offset + 4;
	if (limit_superior > content_capacity)
		enlarge(limit_superior);
	content[offset]     = static_cast<uint8_t >(value);
	content[offset + 1] = static_cast<uint8_t >(value >> 8);
	content[offset + 2] = static_cast<uint8_t >(value >> 16);
	content[offset + 3] = static_cast<uint8_t >(value >> 24);
	if (limit_superior > content_size)
		content_size = offset + 4;
}

void Section::write_block(unsigned offset, const uint8_t *data, unsigned size) {
	auto limit_superior = offset + size;
	while (limit_superior > content_capacity)
		enlarge(limit_superior);
	memcpy(content + offset, data, size);
	if (limit_superior > content_size)
		content_size += limit_superior - content_size;
}

void Section::fill(unsigned offset, uint8_t b, unsigned size) {
	auto limit_superior = offset + size;
	while (limit_superior > content_capacity)
		enlarge(limit_superior);
	memset(content + offset, b, size);
	if (limit_superior > content_size)
		content_size += limit_superior - content_size;
}

uint8_t Section::read8(unsigned offset) {
	if (offset < content_size)
		return *(content + offset);
	return 0x55;
}

uint16_t Section::read16(unsigned offset) {
	uint16_t byte0 = 0x55, byte1 = 0x55;
	if (offset < content_size)
		byte0 = *(content + offset);
	if (offset + 1 < content_size)
		byte1 = *(content + offset + 1);
	return (byte1 << 8) + byte0;
}

uint32_t Section::read32(unsigned offset) {
	uint32_t byte0 = 0x55, byte1 = 0x55, byte2 = 0x55, byte3 = 0x55;
	if (offset < content_size)
		byte0 = *(content + offset);
	if (offset + 1 < content_size)
		byte1 = *(content + offset + 1);
	if (offset + 2 < content_size)
		byte2 = *(content + offset + 2);
	if (offset + 3 < content_size)
		byte3 = *(content + offset + 3);
	return (byte3 << 24) + (byte2 << 16) + (byte1 << 8) + byte0;
}

void Section::read_block(unsigned offset, uint8_t *buffer, unsigned size) {
	if (offset >= content_capacity)
		memset(buffer, 0x55, size);
	else if (offset + size > content_capacity) {
		size_t fill_offset = content_capacity - offset;
		size_t fill_size = size - fill_offset;
		memset(buffer + fill_offset, 0x55, fill_size);
		memcpy(buffer, content + offset, fill_offset);
	}
	else
		memcpy(buffer, content + offset, size);
}

//-----------------------------------------------------------------------------

vector<Section*> Sections::table;
list<Section*> Sections::list;

Section *Sections::current_section = nullptr;

void Sections::deallocate() {
	for (auto s: table) {
		free(s->content);
		delete s;
	}
}

void Sections::set_section(std::string section_name) {
	unsigned section_flags = 0;
	if (section_name != ".bss" && section_name != ".stack")
		section_flags = Section::LOADABLE;

	/* A secção .text é sempre criada inicialmente.
	 * Se o utilizador não definir uma secção inicial esta é utilizada.
	 * Se o utilizador começar por definir uma secção,
	 * esta vai substituir a secção .text inicialmente criada.
	 */
	if (table.size() == 1 && table.at(0)->content_size == 0) {
		table.at(0)->name = section_name;
		table.at(0)->flags = section_flags;
		return;
	}

	unsigned section_number;
	for (section_number = 0; section_number < table.size(); ++section_number)
		if (section_name == table.at(section_number)->name) {
			current_section = table.at(section_number);
			return;	/*	Já existe */
		}
	/* Nova secção */
	table.push_back(new Section {section_name, section_number, section_flags});
	current_section = table.back();
}

void Sections::listing(std::ostream& lst_file) {
	lst_file << "Sections\n";
	ostream_printf(lst_file, "%-8s%-16s%-16s%s\n", "Index", "Name", "Addresses", "Size");
	for (size_t i = 0; i < table.size(); ++i) {
		ostream_printf(lst_file, "%-8d%-16s%04X - %04X     %04X %d\n", i,
						table[i]->name.c_str(), table[i]->base_address,
						table[i]->base_address + table[i]->content_size - 1,
						table[i]->content_size, table[i]->content_size);
	}
}

bool Sections::address_is_free(Section *s) {
	auto iter = list.begin();
	for (; iter != list.end(); ++iter) {
		Section *r = *iter;
		if (s->base_address + s->content_size > r->base_address
				&& r->base_address + r->content_size > s->base_address)
			return false;
		if (s->base_address + s->content_size <= r->base_address)
			break;
	}
	list.insert(iter, s);
	return true;
}

void Sections::locate(Properties<string, unsigned> *section_addresses) {
	auto current_address = 0;
	for (size_t i = 0; i < table.size(); ++i) {
		Section *section = table[i];
		section->base_address = section_addresses->get_property(section->name, current_address);
		if ( ! address_is_free(section)) {
			error_report("Section \"" + section->name
						 + string_printf("\" with size %d (0x%x) can't be located in address %d (0x%x)"
										 ", this overlap another section.\n",
										 section->content_size, section->content_size,
										 section->base_address, section->base_address));
			exit(1);
		}
					//	alinhar o início da secção em endereço par
		current_address = align(section->base_address + section->content_size, 1);
	}
}

//	(Provisório, se as secções forem implementadas sobre esta estrutura, esta operação é desnecessária)
//	Espaço de memória virtual
//	page_size = 4K
//	address space = 64K
static	Memory_space memory = Memory_space(4 * 1024, 64 * 1024);

void Sections::fill_memory_space() {
	for (auto i = 0U; i < Sections::table.size(); ++i) {
		Section *section = Sections::table.at(i);
		if ((section->flags & Section::LOADABLE) == 0)
			continue;
		memory.write(section->base_address, section->content, section->content_size);
	}

}

void Sections::binary_hex_intel(const char *file_name,
								unsigned word_size, unsigned byte_order,
								unsigned lower_address, unsigned higher_address) {
	try {
		std::ofstream file(file_name);
		for (size_t i = 0; i < table.size(); ++i) {
			Section *section = table.at(i);
			if ((section->flags & Section::LOADABLE) == 0)
				continue;
			if (higher_address < section->base_address)
				break;

			if (lower_address >= section->base_address + section->content_size)
				continue;

			auto section_lower_address = max(lower_address, section->base_address);
			auto section_higher_address = min(higher_address, section->base_address + section->content_size);

			auto address = section_lower_address + byte_order;
			auto size = (section_higher_address - section_lower_address) / word_size;

			do {
				auto rec_len = min(16U, size);
				auto load_address = address / word_size;
				uint8_t cheksum = (rec_len + load_address + (load_address >> 8));
				ostream_printf(file, ":%02X%04X00", rec_len, load_address);
				for (auto j = 0U; j < rec_len; ++j, address += word_size) {
					uint8_t b = memory.read8(address);
					ostream_printf(file, "%02X", b);
					cheksum += b;
				}
				ostream_printf(file, "%02X\n", static_cast<uint8_t >(-cheksum));
				size -= rec_len;
			} while (size > 0);
		}
		ostream_printf(file, ":00000001FF");
		file.close();
	} catch (ios_base::failure &e) {
		cerr << e.what();
	}
}

void Sections::binary_logisim(const char *file_name,
								unsigned word_size, unsigned byte_order,
								unsigned lower_address, unsigned higher_address) {
	try {
		std::ofstream file(file_name);
		file << "v2.0 raw" << endl;
		for (auto address = lower_address + byte_order; address < higher_address; ) {
			uint8_t c = memory.read8(address);
			address += word_size;
			//	Quantos bytes iguais a este?
			if (memory.read8(address) == c) {
				auto j = 2U;
				for (address += word_size;
				     	address < higher_address && c == memory.read8(address);
					 		address += word_size, j++)
					;
				ostream_printf(file, " %d*%02x", j, static_cast<uint8_t >(c));
			}
			else
				ostream_printf(file, " %02x", static_cast<uint8_t >(c));
		}
		file.close();
	} catch (ios_base::failure &e) {
		cerr << e.what();
	}
}

void Sections::binary_raw(const char *file_name,
								unsigned word_size, unsigned byte_order,
								unsigned lower_address, unsigned higher_address) {
	try {
		std::ofstream file(file_name);
		for (auto address = lower_address + byte_order; address < higher_address; ) {
			uint8_t c = memory.read8(address);
			file.put(c);
			address += word_size;
		}
		file.close();
	} catch (ios_base::failure &e) {
		cerr << e.what();
	}
}

}
