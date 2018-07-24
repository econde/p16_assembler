#include "relocations.h"
#include "utils.h"
#include "error.h"

namespace ast {

	std::list<Relocation*> Relocations::list;

void Relocations::deallocate() {
    for (auto s: list) {
        delete s;
    }
}

void Relocations::print(ostream & os) {
	ostream_printf(os, "File       Line Section Offset Pos Mask Type    Addend Symbol\n");
	for (auto reloc: list) {
		ostream_printf(os, "%-10s %4d %3d %7d %4d  %04x %s %6d %-10s\n",
					   reloc->statement->location.unit, reloc->statement->location.line,
					   reloc->statement->section_index, reloc->statement->section_offset,
					   reloc->position, MAKE_MASK(reloc->width, 0),
					   reloc->type == Relocation::Type::ABSOLUTE ? "ABSOLUTE" : "RELATIVE",
					   reloc->addend, reloc->symbol.c_str());
	}		
}

void Relocations::relocate() {
	for (auto reloc: list) {
		auto section = reloc->statement->section_index;
		auto offset = reloc->statement->section_offset;
		auto content = Sections::read16(section, offset);
        auto mask = MAKE_MASK(reloc->width, 0U);
        auto symbol_section = Symbols::get_section(reloc->symbol);
        //  Saltos para endereços absolutos geram relocations relativas sem simbolo associado
        auto value = (reloc->symbol.empty() ? 0 : Sections::get_address(symbol_section)) + reloc->addend;
        if (reloc->type == Relocation::Type::RELATIVE) {
			value -= Sections::get_address(section) + offset;
			//	Nas instruções branch os offsets são codificados em número de instruções (value / 2)
			value >>= 1;
			if (ABSOLUTE(value) > (mask >> 1))
				error_report(reloc->exp_location,
							 string_printf("Interval between PC and target address: %+d (0x%x) "
											"isn't codable in 10 bit two's complement",
                                           value, value, reloc->width, value & mask, value & mask));
		}
		else if (value > mask) { //  Relocation::Type::ABSOLUTE
            error_report(reloc->exp_location, string_printf(
                    "Expression's value = %d (0x%x) not encodable in %d bit, truncate to %d (0x%x)",
                    value, value, reloc->width, value & mask, value & mask));
        }
    	content += (value & mask) << reloc->position;
	    Sections::write16(section, offset, content);
	}		
}

}

