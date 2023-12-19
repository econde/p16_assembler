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

#include <string>
#include <cassert>
#include "directive.h"

using namespace std;
using namespace ast;

string Directive::listing()
{
	return string_printf("%4d %04X %02X%3c\t", location.line,
		Sections::get_address(section_index) + section_offset,
		Sections::read8(section_index, section_offset), ' ');
}

string Directive::more_listing()
{
	string tmp = {};
	for (unsigned i = 1; i < size_in_memory; ++i) {
		tmp += string_printf("%4d %04X %02X\n", location.line,
		Sections::get_address(section_index) + section_offset + i,
		Sections::read8(section_index, section_offset + i));
	}
	return tmp;
}

//------------------------------------------------------------------------------
//	.section

Dir_section::Dir_section(string name, Location left) :
	Directive {left}, name {name}
{
	Sections::set_section(name);
	section_index = Sections::current_section->number;
	section_offset = Sections::current_section->content_size;
	size_in_memory = 0;
}

static string hex_dump(int line, unsigned section, unsigned offset, unsigned size)
{
	string tmp = {};
	auto address = Sections::get_address(section) + offset;
	auto remainder_bytes = size;
	//--------------------------------------------------------------------------
	//	Linhas completas
	auto nlines = remainder_bytes / 16;
	for (auto l = 0U; l < nlines; ++l) {
		tmp += string_printf("%4d %04X", line, address);
		for (auto i = 0U; i < 16U; i++)
			tmp += string_printf(" %02X",
				Sections::read8(section, offset + i));
		offset += 16;
		address += 16;
		remainder_bytes -= 16;
		tmp += '\n';
	}
	//--------------------------------------------------------------------------
	//	Última linha
	if (remainder_bytes > 0) {
		tmp += string_printf("%4d %04X", line, address);
		for (auto i = 0U; i < remainder_bytes; i++)
			tmp += string_printf(" %02X",
					Sections::read8(section, offset + i));
		tmp += '\n';
	}
	return tmp;
}

//------------------------------------------------------------------------------
//	.ascii

Dir_ascii::Dir_ascii(unsigned asciz, list<string> *sl, Location left) :
	Directive {left}, string_list {sl}
{
	section_index = Sections::current_section->number;
	section_offset = Sections::current_section->content_size;
	for (auto s: *string_list) {
		Sections::append_block(Sections::current_section->number, (const uint8_t *)s.data(), s.size());
		if (asciz)
			Sections::append8(Sections::current_section->number, 0);
	}
	size_in_memory = Sections::current_section->content_size - section_offset;
}

string Dir_ascii::listing()
{
	switch (size_in_memory) {
	case 1:
		return string_printf("%4d %04X %02X\t", location.line,
			Sections::get_address(section_index) + section_offset,
			Sections::read8(section_index, section_offset));
	case 2:
		return string_printf("%4d %04X %02X %02X\t", location.line,
			Sections::get_address(section_index) + section_offset,
			Sections::read8(section_index, section_offset),
			Sections::read8(section_index, section_offset + 1));
	default:
		return string_printf("%4d          \t", location.line);
	}
}

string Dir_ascii::more_listing()
{
	if (size_in_memory > 2)
		return hex_dump(location.line, section_index, section_offset, size_in_memory);
	return {};
}

//---------------------------------------------------------------------------------------------
//	.byte .word

Dir_byte::Dir_byte(unsigned s, list<Expression*> *vl, Location left) :
	Directive {left}, value_list {vl}, grain_size {s}
{
	section_index = Sections::current_section->number;
	section_offset = Sections::current_section->content_size;
	size_in_memory = grain_size * value_list->size();
	Sections::increase(section_index, size_in_memory);
}

Dir_byte::~Dir_byte()
{
	for (auto s: *value_list)
		delete s;
	delete value_list;
}

string Dir_byte::listing()
{
	switch (size_in_memory) {
	case 1:
		return string_printf("%4d %04X %02X\t", location.line,
			Sections::get_address(section_index) + section_offset,
			Sections::read8(section_index, section_offset));
	case 2:
		return string_printf("%4d %04X %02X %02X\t", location.line,
			Sections::get_address(section_index) + section_offset,
			Sections::read8(section_index, section_offset),
			Sections::read8(section_index, section_offset + 1));
	default:
		return string_printf("%4d          \t", location.line);
	}
}

string Dir_byte::more_listing()
{
	if (size_in_memory > 2)
		return hex_dump(location.line, section_index, section_offset, size_in_memory);
	return {};
}

//------------------------------------------------------------------------------
//	.space

Dir_space::Dir_space (Expression *s, Expression *i, Location left) :
	Directive {left}, size {s}, initial {i}
{
	section_index = Sections::current_section->number;
	section_offset = Sections::current_section->content_size;
	if (size->evaluate())
		size_in_memory = size->get_value();
	else
		error_report(&size->location, "Size parameter for .space directive must be defined before");
	Sections::increase(section_index, size_in_memory & 0xffff);
}

string Dir_space::more_listing()
{
	string tmp = {};
	if (size_in_memory <= 3)
		for (unsigned i = 1; i < size_in_memory; ++i) {
			tmp += string_printf("%4d %04X %02X\n", location.line,
				Sections::get_address(section_index) + section_offset + i,
				Sections::read8(section_index, section_offset + i));
		}
	else {
		tmp += string_printf("%4d .... ..\n", location.line);
		tmp += string_printf("%4d %04X %02X\n", location.line,
			Sections::get_address(section_index) + section_offset + size_in_memory - 1,
			Sections::read8(section_index, section_offset + size_in_memory - 1));
	}
	return tmp;
}

//------------------------------------------------------------------------------
//	.equ

Dir_equ::Dir_equ(Symbol *symbol, Expression *value, Location left) :
	Directive {left}, symbol {symbol}
{
	section_index = Sections::current_section->number;	/* Um simbolo equ não pertence a uma secção ... */
	section_offset = Sections::current_section->content_size;
	size_in_memory = 0;
	symbol->set_properties(UNDEFINED, Sections::current_section->number, value);
	Symbol *s = Symbols::search(symbol->name);
	if (s == nullptr) {
		Symbols::add(symbol);
	}
	else if (s->value_expression == nullptr) {
		s->set_properties(UNDEFINED, Sections::current_section->number, value);
		delete symbol;
	}
	else
		error_report(&symbol->location, "Symbol \"" + symbol->name + "\" is already defined");
}

//------------------------------------------------------------------------------
//	.align

Dir_align::Dir_align (Expression *size, Location left) :
	Directive {left}, size {size} 
{
	section_index = Sections::current_section->number;
	section_offset = Sections::current_section->content_size;
	size_in_memory = Sections::align(section_offset, size->get_value()) - section_offset;
	Sections::increase(section_index, size_in_memory);
}

string Dir_align::listing()
{
	if (size_in_memory == 0)
		return string_printf("%4d%10c\t", location.line, ' ');
	else
		return string_printf("%4d %04X %02X%2c\t", location.line,
			Sections::get_address(section_index) + section_offset,
			Sections::read8(section_index, section_offset), ' ');
}
