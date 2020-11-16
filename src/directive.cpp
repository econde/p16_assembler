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

#if 0

static string hex_dump(int line, unsigned section, unsigned offset, unsigned size, int unit) {
	assert(unit == 1 || (unit == 2 && (size & 1) == 0));
	string tmp = string();
	string interval = string(unit == 1 ? "   " : "     ");
	auto address = Sections::get_address(section) + offset;
	auto remainder_bytes = size;
	//--------------------------------------------------------------------------
	//	Linhas completas
	auto nlines = remainder_bytes / 16;
	for (auto l = 0U; l < nlines; ++l) {
		tmp += string_printf("%4d %04X     \t", line, address);
		if (unit == 1)
			for (auto i = 0U; i < 16U; i++)
				tmp += string_printf(" %02X",
					Sections::read8(section, offset + i));
		else if (unit == 2)
			for (auto i = 0U; i < 16U; i += 2)
				tmp += string_printf(" %02X%02X",
					Sections::read8(section, offset + i + 1),
					Sections::read8(section, offset + i));
		tmp	+= "  ";
		offset += 16;
		address += 16;
		remainder_bytes -= 16;
		tmp += '\n';
	}
	//--------------------------------------------------------------------------
	//	Última linha
	if (remainder_bytes > 0) {
		tmp += string_printf("%4d %04X     \t", line, address);
		if (unit == 1)
			for (auto i = 0U; i < remainder_bytes; i++)
				tmp += string_printf(" %02X",
					Sections::read8(section, offset + i));
		else if (unit == 2)
			for (auto i = 0U; i < remainder_bytes; i += 2)
				tmp += string_printf(" %02X%02X",
					Sections::read8(section, offset + i + 1),
					Sections::read8(section, offset + i));
		tmp += '\n';
	}
	return tmp;
}


static string hex_dump(int line, unsigned section, unsigned offset, unsigned size, int unit) {
	assert(unit == 1 || (unit == 2 && (size & 1) == 0));
	string tmp = string();
	string interval = string(unit == 1 ? "   " : "     ");
	if (unit == 1)
		tmp += string_printf("%4d          \t"
				"  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  0123456789ABCDEF\n", line);
// 93 0050                                            61 61 65 61              aaea
// 93 0060        61 20 65 65 20 69 69 69 20 6F 6F 6F 6F 20 75 75  a ee iii oooo uu
// 93 0070        75 75 75 00                                      uuu.
	else if (unit == 2)
		tmp += string_printf("%4d          \t"
				"    0    2    4    6    8    A    C    E  0123456789ABCDEF\n", line);
// 61                0    2    4    6    8    A    C    E  0123456789ABCDEF
// 61 00A0                                      0001 00D6              ....
// 61 00B0        0002 00D8 0003 00DA                      ........

	auto address = Sections::get_address(section) + offset;
	auto remainder_bytes = size;

	//--------------------------------------------------------------------------
	//	Primeira linha
	tmp += string_printf("%4d %04X     \t", line, address & ~0xf);
	unsigned n = 0;
	for ( ; n < (address & 0xf); n += unit)
		tmp += interval;
	auto nbytes = min(remainder_bytes, 16U - (address & 0xf));
	if (unit == 1)
		for (auto i = 0U; i < nbytes; i++, n++)
			tmp += string_printf(" %02X", Sections::read8(section, offset + i));
	else if (unit == 2)
		for (auto i = 0U; i < nbytes; i += 2, n += 2)
			tmp += string_printf(" %02X%02X",
				Sections::read8(section, offset + i + 1),
				Sections::read8(section, offset + i));
	for ( ; n < 16; n++)
		tmp += interval;

	//	ascii
	tmp	+= "  ";
	n = 0;
	for ( ; n < (address & 0xf); n++)
		tmp += ' ';
	for (auto i = 0U; i < nbytes; i++, n++) {
		auto c = Sections::read8(section, offset + i);
		if (isprint(c) &&
			c != '\t' && c != '\n' && c != '\r' && c != '\v' &&
			c != '\b' && c != '\f' && c != '\a')
			tmp += string_printf("%c", c);
		else
			tmp += '.';
	}
	tmp += '\n';
	//--------------------------------------------------------------------------
	//	Linhas intermédias
	offset += nbytes;
	address += nbytes;
	remainder_bytes -= nbytes;
	auto nlines = remainder_bytes / 16;
	for (auto l = 0U; l < nlines; ++l) {
		tmp += string_printf("%4d %04X     \t", line, address);
		if (unit == 1)
			for (auto i = 0U; i < 16U; i++)
				tmp += string_printf(" %02X",
					Sections::read8(section, offset + i));
		else if (unit == 2)
			for (auto i = 0U; i < 16U; i += 2, n += 2)
				tmp += string_printf(" %02X%02X",
					Sections::read8(section, offset + i + 1),
					Sections::read8(section, offset + i));
		tmp	+= "  ";

		//	ascii
		for (auto i = 0U; i < 16U; i++) {
			auto c = Sections::read8(section, offset + i);
			if (isprint(c) &&
				c != '\t' && c != '\n' && c != '\r' && c != '\v' &&
				c != '\b' && c != '\f' && c != '\a')
				tmp += string_printf("%c", c);
			else
				tmp += '.';
		}
		offset += 16;
		address += 16;
		remainder_bytes -= 16;
		tmp += '\n';
	}
	//--------------------------------------------------------------------------
	//	Última linha
	if (remainder_bytes > 0) {
		tmp += string_printf("%4d %04X     \t", line, address);
		n = 0;
		if (unit == 1)
			for (auto i = 0U; i < remainder_bytes; i++, n++)
				tmp += string_printf(" %02X",
					Sections::read8(section, offset + i));
		else if (unit == 2)
			for (auto i = 0U; i < remainder_bytes; i += 2, n += 2)
				tmp += string_printf(" %02X%02X",
					Sections::read8(section, offset + i + 1),
					Sections::read8(section, offset + i));

		for ( ; n < 16; n += unit)
			tmp += interval;

		//	ascii
		tmp	+= "  ";
		n = 0;
		for (auto i = 0U; i < remainder_bytes; i++, n++) {
			auto c = Sections::read8(section, offset + i);
			if (isprint(c) &&
				c != '\t' && c != '\n' && c != '\r' && c != '\v' &&
				c != '\b' && c != '\f' && c != '\a')
				tmp += string_printf("%c", c);
			else
				tmp += '.';
		}
		tmp += '\n';
	}
	return tmp;
}

#endif

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

#if 0
string Byte::listing() {
	if (grain_size == 2)
		return string_printf("%4d %04X %02X%02X\t", location.line,
			Sections::get_address(section_index) + section_offset,
			Sections::read8(section_index, section_offset + 1),
			Sections::read8(section_index, section_offset));
	else
		return string_printf("%4d %04X %02X  \t", location.line,
			Sections::get_address(section_index) + section_offset,
			Sections::read8(section_index, section_offset));
}

string Byte::more_listing() {
	string tmp = string();
	if (grain_size == 2)
		for (unsigned i = 2; i < size_in_memory; i += 2)
			tmp += string_printf("%4d %04X %02X%02X\n", location.line,
			Sections::get_address(section_index) + section_offset + i,
			Sections::read8(section_index, section_offset + i + 1),
			Sections::read8(section_index, section_offset + i));
	else
		for (unsigned i = 1; i < size_in_memory; ++i) {
			tmp += string_printf("%4d %04X %02X\n", location.line,
			Sections::get_address(section_index) + section_offset + i,
			Sections::read8(section_index, section_offset + i));
		}
	return tmp;
}
#endif


#if 0
string Byte::listing() {
	if (grain_size == 2)
		if (size_in_memory < 8)
			return string_printf("%4d %04X %02X%02X\t", location.line,
				Sections::get_address(section_index) + section_offset,
				Sections::read8(section_index, section_offset + 1),
				Sections::read8(section_index, section_offset));
		else
			return string_printf("%4d          \t", location.line);
	else	//	grain_size == 1
		if (size_in_memory < 4)
			return string_printf("%4d %04X %02X  \t", location.line,
				Sections::get_address(section_index) + section_offset,
				Sections::read8(section_index, section_offset));
		else
			return string_printf("%4d          \t", location.line);
}

string Byte::more_listing() {
	string tmp = string();
	if (grain_size == 2)
		if (size_in_memory < 8)
			for (unsigned i = 2; i < size_in_memory; i += 2)
				tmp += string_printf("%4d %04X %02X%02X\n", location.line,
				Sections::get_address(section_index) + section_offset + i,
				Sections::read8(section_index, section_offset + i + 1),
				Sections::read8(section_index, section_offset + i));
		else
			return hex_dump(location.line, section_index, section_offset, size_in_memory, grain_size);
	else 	//	grain_size == 1
		if (size_in_memory < 4)
			for (unsigned i = 1; i < size_in_memory; ++i) {
				tmp += string_printf("%4d %04X %02X\n", location.line,
				Sections::get_address(section_index) + section_offset + i,
				Sections::read8(section_index, section_offset + i));
			}
		else
			return hex_dump(location.line, section_index, section_offset, size_in_memory, grain_size);
	return tmp;
}
#endif

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
