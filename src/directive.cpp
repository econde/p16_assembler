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
	string tmp = string_printf("%4d %04X ", location.line, 
		Sections::get_address(section_index) + section_offset);
	unsigned i;
	for (i = 0; i < min(4U, size_in_memory); ++i)
		tmp += string_printf("%02X", Sections::read8(section_index, section_offset + i));
	for (; i < 4; ++i)
		tmp += "  ";
	return tmp + "\t";
}

string Directive::more_listing(unsigned size) {
	string tmp;
	if (size_in_memory > 4) {
		int i = 4;
		int count_bytes = size;
		for ( ; i < count_bytes - 4; i += 4)
			tmp += string_printf("%4d %04X %02X%02X%02X%02X\n", location.line,
						 Sections::get_address(section_index) + section_offset + i,
						 Sections::read8(section_index, section_offset + i + 0),
						 Sections::read8(section_index, section_offset + i + 1),
						 Sections::read8(section_index, section_offset + i + 2),
						 Sections::read8(section_index, section_offset + i + 3));
		tmp += string_printf("%4d %04X ", location.line,
					 Sections::get_address(section_index) + section_offset + i);
		for (; i < count_bytes; ++i)
			tmp += string_printf("%02X", Sections::read8(section_index, section_offset + i));
		return tmp + "\n";
	}
	return tmp;
}
