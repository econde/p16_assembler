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
#include "directive.h"

using namespace std;
using namespace ast;

string Directive::listing() {
	return string_printf("%4d %04X %02X%2c\t", location.line, 
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

string Byte::listing() {
	if (grain_size == 2)
		return string_printf("%4d %04X %02X%02X\t", location.line, 
			Sections::get_address(section_index) + section_offset,
			Sections::read8(section_index, section_offset + 1),
			Sections::read8(section_index, section_offset));
	else
		return string_printf("%4d %04X %02X%2c\t", location.line, 
			Sections::get_address(section_index) + section_offset,
			Sections::read8(section_index, section_offset), ' ');
}

string Byte::more_listing() {
	string tmp = string();
	if (grain_size == 2)
		for (unsigned i = 2; i < size_in_memory; i += 2)
			tmp += string_printf("%4d %04X %02X%02X", location.line, 
			Sections::get_address(section_index) + section_offset,
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


