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

#include "relocations.h"
#include "utils.h"
#include "error.h"

namespace ast {

std::list<Relocation*> Relocations::list;

void Relocations::deallocate()
{
	for (auto s: list) {
		delete s;
	}
}

void Relocations::print(ostream & os)
{
	ostream_printf(os, "File       Line Section Offset Pos Mask Type    Addend Symbol\n");
	for (auto reloc: list) {
		ostream_printf(os, "%-10s %4d %3d %7d %4d  %04x %s %6d %-10s\n",
				reloc->location->unit, reloc->location->line,
				reloc->section_index, reloc->section_offset,
				reloc->position, MAKE_MASK(reloc->width, 0),
				reloc->type == Relocation::Relocation_type::ABSOLUTE ? "ABSOLUTE" : "RELATIVE",
				reloc->addend, reloc->symbol.c_str());
	}
}

void Relocations::relocate()
{
	for (auto reloc: list) {
		if ( ! reloc->symbol.empty()) {
			if (Symbols::get_type(reloc->symbol) == UNDEFINED) {
				error_report(reloc->exp_location, "Undefined symbol: " + reloc->symbol);
				continue;
			}
		}
		auto section = reloc->section_index;
		auto offset = reloc->section_offset;
		auto content = Sections::read16(section, offset);
		auto mask = MAKE_MASK(reloc->width, 0U);
		//  Saltos para endereços absolutos geram relocations relativas sem simbolo associado
		auto value = 0;	// Valor com sinal
		value = (reloc->symbol.empty() ? 0 : Sections::get_address(Symbols::get_section(reloc->symbol)))
				+ reloc->addend;
		if (reloc->type == Relocation::Relocation_type::RELATIVE) {
			// Nas instruções branch os offsets são codificados em número de instruções (value / 2)
			if (value & 1)
				warning_report(reloc->exp_location,
					string_printf("Target address is odd: %+d (0x%x)", value, value));
			value -= Sections::get_address(section) + offset;
			if (value >= (1 << reloc->width) || value < (~0 << reloc->width))	// signed
				error_report(reloc->exp_location,
						 string_printf("Interval between PC and target address: %+d (0x%x), "
								"isn't codable in %d bit two's complementxx",
								value, value, reloc->width + 1));
			value >>= 1;
		}
		else if (reloc->type == Relocation::Relocation_type::RELATIVE_UNSIGNED) {	// usado no ldr rd, label
			if (value & 1)
				warning_report(reloc->exp_location,
					string_printf("Target address is odd: %+d (0x%x)", value, value));
			value -= Sections::get_address(section) + offset;
			if (value < 0)
				error_report(reloc->exp_location,
							"Indicated address must be higher than current location");
			if (value >= (1 << (reloc->width + 1)))
				error_report(reloc->exp_location,
					string_printf("Interval between PC and target address: %+d (0x%x), "
							"isn't codable with %d bit",
							value, value, reloc->width + 1));
			value >>= 1;
		}
		else if (value > (1 << reloc->width)) { // Relocation::Type::ABSOLUTE
			error_report(reloc->exp_location,
				string_printf("Expression's value: %d (0x%x) isn't codable in %d bit",
					value, value, reloc->width));
		}
		content &= ~(mask << reloc->position);
		content |= (value & mask) << reloc->position;
		Sections::write16(section, offset, content);
	}
}

}

