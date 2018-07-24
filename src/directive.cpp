#include <string>
#include "directive.h"

using namespace std;
using namespace ast;

string Directive::listing() {
	string tmp = string_printf("%4d %04x ", location.line, 
		Sections::get_address(section_index) + section_offset);
	unsigned i;
	for (i = 0; i < min(4U, size_in_memory); ++i)
		tmp += string_printf("%02x", Sections::read8(section_index, section_offset + i));
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
			tmp += string_printf("%4d %04x %02x%02x%02x%02x\n", location.line,
				Sections::get_address(section_index) + section_offset + i,
				Sections::read8(section_index, section_offset + i + 0),
				Sections::read8(section_index, section_offset + i + 1),
				Sections::read8(section_index, section_offset + i + 2),
				Sections::read8(section_index, section_offset + i + 3));
		tmp += string_printf("%4d %04x ", location.line,
			Sections::get_address(section_index) + section_offset + i);
		for (; i < count_bytes; ++i)
			tmp += string_printf("%02x",
					Sections::read8(section_index, section_offset + i));
		return tmp + "\n";
	}
	return tmp;
}
