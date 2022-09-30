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

string Directive::listing() {
	return string_printf("%4d %04X %02X%3c\t", location.line,
		Sections::get_address(section_index) + section_offset,
		Sections::read8(section_index, section_offset), ' ');
}

string Directive::more_listing() {
	string tmp = string();
	for (unsigned i = 1; i < size_in_memory; ++i) {
		tmp += string_printf("%4d %04X %02X\n", location.line,
		Sections::get_address(section_index) + section_offset + i,
		Sections::read8(section_index, section_offset + i));
	}
	return tmp;
}

static string hex_dump(int line, unsigned section, unsigned offset, unsigned size, int unit) {
	assert(unit == 1 || (unit == 2 && (size & 1) == 0));
	string tmp = string();
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

string Ascii::listing() {
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
		break;

	}
}

string Ascii::more_listing() {
	if (size_in_memory > 2)
		return hex_dump(location.line, section_index, section_offset, size_in_memory, 1);
	return string();
}

//---------------------------------------------------------------------------------------------
//	.byte .word

string Byte::listing() {
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
		break;
	}
}

string Byte::more_listing() {
	if (size_in_memory > 2)
		return hex_dump(location.line, section_index, section_offset, size_in_memory, 1);
	return string();
}

//------------------------------------------------------------------------------
//	.space

string Space::more_listing() {
	string tmp = string();
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

string Align::listing() {
	if (size_in_memory == 0)
		return string_printf("%4d%10c\t", location.line, ' ');
	else
		return string_printf("%4d %04X %02X%2c\t", location.line,
			Sections::get_address(section_index) + section_offset,
			Sections::read8(section_index, section_offset), ' ');
}
