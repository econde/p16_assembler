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

#include "code_generator.h"
#include "relocations.h"
#include "iarm.tab.hpp"
#include "error.h"
#include "utils.h"

using namespace ast;
using namespace std;

//------------------------------------------------------------------------------------------
//	Instruction

void Code_generator::visit(Load_store_direct *s) {
	if (s->rd->n > 7)
		error_report(&s->rd->location, "Invalid register");
	auto byte = s->ldst & 1;
	auto code = ((s->ldst & 2) ? 0x2000 : 0x0000) | (byte << 12) | s->rd->n;
	Value_type direct_type = s->direct->get_type();
	if (direct_type == ABSOLUTE || direct_type == Value_type::LABEL || direct_type == UNDEFINED) {
		auto direct7 = 0U;

		if (direct_type == ABSOLUTE)
			direct7 = s->direct->get_value();
		else {   // LABEL ou UNDEFINED
			string symbol = s->direct->get_symbol();
			auto addend = s->direct->get_value() - Symbols::get_value(symbol);
			auto *reloc = new Relocation{s, &s->direct->location, 3, 7, Relocation::Type::ABSOLUTE, symbol, addend};
			Relocations::add(reloc);
		}

		if ((direct7 & ~MAKE_MASK(7, 0)) != 0) {
			error_report(&s->direct->location, "Address defined by \"" + s->direct->to_string()
				+ string_printf("\" %d (0x%x) is out of range for direct access", direct7, direct7));
		}
		code |= direct7 << 3;
		if ((direct7 & 1) != 0 && byte == 0 )
			warning_report(&s->direct->location, "Word access in odd address");
	}
	else
		error_report(&s->direct->location, string_printf("Invalid expression"));
	Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(code));
}

void Code_generator::visit(Load_store_indirect *s) {
	if (s->rd->n > 7)
		error_report(&s->rd->location, "Invalid register");
	if (s->rn->n > 7)
		error_report(&s->rn->location, "Invalid register");

	auto byte = s->ldst & 1;
	auto code = ((s->ldst & 2) ? 0x2800 : 0x0800) | (byte << 12) | s->rd->n | (s->rn->n << 3);
	if (s->constant == nullptr) {
		if (s->rm->n > 7)
			error_report(&s->rm->location, "Invalid register");
		code |= (1 << 10) + (s->rm->n << 6);
	} else {
		Value_type constante_type = s->constant->get_type();
		if (constante_type == ABSOLUTE || constante_type == Value_type::LABEL) {
			auto imm4 = s->constant->get_value();
			if ((imm4 & ~MAKE_MASK(4, 0)) != 0) {
				warning_report(&s->constant->location, string_printf(
					"Expression's value = %d (0x%x) not encodable in 4 bit, truncate to %d (0x%x)",
					imm4, imm4, imm4 & MAKE_MASK(4, 0), imm4 & MAKE_MASK(4, 0)));
			}
			code |= ((imm4 & MAKE_MASK(4, 0)) << 6);
		}
		else if (constante_type == UNDEFINED) {
			error_report(&s->constant->location, string_printf("Undefined expression"));
		}
		else {
			error_report(&s->constant->location, string_printf("Invalid expression"));
		 }
	}
	Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(code));
}

void Code_generator::visit(Branch *s) {
	auto code = 0U;
	switch (s->condition) {
		case ZS:
			code = 0x4000;
			break;
		case ZC:
			code = 0x4400;
			break;
		case CS:
			code = 0x4800;
			break;
		case CC:
			code = 0x4c00;
			break;
		case GE:
			code = 0x5000;
			break;
		case LT:
			code = 0x5400;
			break;
		case B:
			code = 0x5800;
			break;
		case BL:
			code = 0x5c00;
			break;
	}
	auto exp_type = s->expression->get_type();
	auto imm10 = 0U;
	if (exp_type == ABSOLUTE || exp_type == UNDEFINED) {
		string symbol;
		auto addend = s->expression->get_value() - 2;
		if (exp_type == ABSOLUTE) {     // salto para endereço absoluto?
			;
		}
		else { // exp_type == UNDEFINED
			symbol = s->expression->get_symbol();
		}
		if ((addend & 1) != 0)
			warning_report(&s->expression->location, "Odd target address");
		auto *reloc = new Relocation{s, &s->expression->location, 0, 10, Relocation::Type::RELATIVE, symbol, addend};
		Relocations::add(reloc);
	}
	else if (exp_type == Value_type::LABEL) {   //  Label resolvida, distância definida se pertencer à mesma secção
		string symbol = s->expression->get_symbol();

		if (s->section_index == Symbols::get_section(symbol)) {
			int offset = s->expression->get_value() - s->section_offset - 2;    //  O PC está dois endereços à frente
			if ((offset & 1) != 0)
				warning_report(&s->expression->location, "Odd target address");

			offset >>= 1;

			imm10 = offset & MAKE_MASK(10, 0);
			if (offset >= (1 << 9) || offset < (~0 << 9))
				error_report(&s->expression->location, string_printf(
						"Interval between PC and target address - %+d (0x%x) words - "
						"isn't codable in 10 bit two's complement",
						offset, offset));
		} else {    // A Label pertence a outra secção será resolvida na fase de relocalização
			auto addend = s->expression->get_value() - 2;
			if ((addend & 1) != 0)
				warning_report(&s->expression->location, "Odd target address");
			auto *reloc = new Relocation{s, &s->expression->location, 0, 10, Relocation::Type::RELATIVE, symbol, addend};
			Relocations::add(reloc);
		}
	}
	Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(code | imm10));
}

void Code_generator::visit(Shift *s) {
	if (s->rd->n > 7)
		error_report(&s->rd->location, "Invalid register");
	if (s->rn->n > 7)
		error_report(&s->rn->location, "Invalid register");

	auto position_type = s->position->get_type();
	if (position_type == ABSOLUTE) {
		auto imm4 = s->position->get_value();
		if ((imm4 & ~MAKE_MASK(4, 0)) != 0) {
			warning_report(&s->position->location, string_printf(
				"Expression's value = %d (0x%x) not encodable in 4 bit, truncate to %d (0x%x)",
				imm4, imm4, imm4 & MAKE_MASK(4, 0), imm4 & MAKE_MASK(4, 0)));
		}
		Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(
				((s->operation == LSL ? 0xe000
									  : s->operation == LSR ? 0xe800
															: s->operation == ASR ? 0xf000
																				  : s->operation == ROR ? 0xf800
																										: 0)) +
				 ((imm4 & MAKE_MASK(4, 0)) << 6) + (s->rn->n << 3) + s->rd->n));
	}
	else if (position_type == UNDEFINED)
		error_report(&s->position->location, string_printf("Undefined expression"));
	else if (position_type == Value_type::LABEL || position_type == INVALID)
		error_report(&s->position->location, string_printf("Invalid expression"));
}

void Code_generator::visit(Rrx *s) {
	if (s->rd->n > 7)
		error_report(&s->rd->location, "Invalid register");
	if (s->rn->n > 7)
		error_report(&s->rn->location, "Invalid register");
	Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(0xfc00 + (s->rn->n << 3) + s->rd->n));
}

void Code_generator::visit(Arith *s) {
	if (s->rd->n > 7)
		error_report(&s->rd->location, "Invalid register");
	if (s->rn->n > 7)
		error_report(&s->rn->location, "Invalid register");

	auto code = (s->operation == ADD ? 0x8000 :
				 s->operation == ADC ? 0x9000 :
				 s->operation == SUB ? 0x8800 :
				 s->operation == SBC ? 0x9800 : 0) | (1 << 10) | (s->rn->n << 3) | s->rd->n;
	if (s->expression == nullptr) {
		if (s->rm->n > 7)
			error_report(&s->rm->location, "Invalid register");

		code |= s->rm->n << 6;
	} else {
		auto expression_type = s->expression->get_type();
		if (expression_type == ABSOLUTE || expression_type == Value_type::LABEL) {
			auto imm4 = s->expression->get_value();
			if ((imm4 & ~MAKE_MASK(4, 0)) != 0) {
				warning_report(&s->expression->location, string_printf(
						"Expression's value = %d (0x%x) not encodable in 4 bit, truncate to %d (0x%x)",
						imm4, imm4, imm4 & MAKE_MASK(4, 0), imm4 & MAKE_MASK(4, 0)));
			}
			code |=  (1 << 13) | ((imm4 & MAKE_MASK(4, 0)) << 6);
		}
		else if (expression_type == UNDEFINED) {
			error_report(&s->expression->location, "Undefined expression");
		}
		else {
			error_report(&s->expression->location, "Invalid expression");
		}
	}
	Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(code));
}

void Code_generator::visit(Logic *s) {
	if (s->rd->n > 7)
		error_report(&s->rd->location, "Invalid register");
	if (s->rn->n > 7)
		error_report(&s->rn->location, "Invalid register");
	if (s->rm->n > 7)
		error_report(&s->rm->location, "Invalid register");

	Sections::write16(s->section_index, s->section_offset, (uint16_t)
			((s->operation == AND ? 0xc000 :
			  s->operation == ORR ? 0xc800 :
			  s->operation == EOR ? 0xd000 : 0) + (1 << 10) +
			 (s->rm->n << 6) + (s->rn->n << 3) + s->rd->n));
}

void Code_generator::visit(Not *s) {
	Sections::write16(s->section_index, s->section_offset,
					  static_cast<uint16_t>(0xdc00 + (s->rn->n << 3) + s->rd->n));
}

void Code_generator::visit(Move *s) {
	auto code = 0x6000 | (get_bit(s->rd->n, 3) << 12) | (s->rd->n & 7);
	if (s->constant == nullptr) {
		code |= 0x6b00 | (s->rn->n << 3) | (s->rd->n & 7) | ((s->rd->n >> 3) << 13);
	} else {
		auto const_type= s->constant->get_type();
		if (const_type == ABSOLUTE ) {
			auto imm8 = s->constant->get_value();
			if ((imm8 & ~MAKE_MASK(8, 0)) != 0) {
				warning_report(&s->constant->location, string_printf(
					"Expression's value = %d (0x%x) not encodable in 8 bit, truncate to %d (0x%x)",
					imm8, imm8, imm8 & MAKE_MASK(8, 0), imm8 & MAKE_MASK(8, 0)));
			}
			code |= (imm8 & MAKE_MASK(8, 0)) << 3;
		}
		else if (const_type == Value_type::LABEL) {
			auto symbol = s->constant->get_symbol();
			auto value = s->constant->get_value();
			auto *reloc = new Relocation
					{s, &s->constant->location, 3, 8, Relocation::Type::ABSOLUTE, symbol, value};
			Relocations::add(reloc);
		}
		else if (const_type == UNDEFINED) {
			error_report(&s->constant->location, "Undefined expression");
		}
		else {
			error_report(&s->constant->location, "Invalid expression");
		}
	}
	Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(code));
}

void Code_generator::visit(Moves *s) {
	Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(0x6c00));
}

void Code_generator::visit(Compare *s) {
	if (s->rn->n > 7)
		error_report(&s->rn->location, "Invalid register");
	auto code = s->rn->n << 3;
	if (s->constant == nullptr) {
		if (s->rm->n > 7)
			error_report(&s->rm->location, "Invalid register");
		code |= 0x8800 | (s->rm->n << 6);
	} else {
		auto const_type = s->constant->get_type();
		if (const_type == ABSOLUTE || const_type == Value_type::LABEL) {
			int imm4 = s->constant->get_value();
			if ((imm4 & ~MAKE_MASK(4, 0)) != 0) {
				warning_report(&s->constant->location, string_printf(
						"Expression's value = %d (0x%x) not encodable in 4 bit, truncate to %d (0x%x)",
						imm4, imm4, imm4 & MAKE_MASK(4, 0), imm4 & MAKE_MASK(4, 0)));
			}
			code |= 0xa800 | ((imm4 & MAKE_MASK(4, 0)) << 6);
		}
		else if (const_type == UNDEFINED) {
			error_report(&s->constant->location, "Undefined expression");
		}
		else {
			error_report(&s->constant->location, "Invalid expression");
		}
	}
	Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(code));
}

void Code_generator::visit(Msr *s) {
	if (s->rs->n != CPSR && s->rs->n != SPSR)
		error_report(&s->rs->location, "Invalid register");

	Sections::write16(s->section_index, s->section_offset,
					  static_cast<uint16_t>(0x6a00 | ((s->rs->n == SPSR) << 7) | (get_bit(s->rd->n, 3) << 12) | s->rd->n));
}

void Code_generator::visit(Mrs *s) {
	if (s->rs->n != CPSR && s->rs->n != SPSR)
		error_report(&s->rs->location, "Invalid register");

	Sections::write16(s->section_index, s->section_offset,
					  static_cast<uint16_t>(0x6900 | ((s->rs->n == SPSR) << 7) | (get_bit(s->rd->n, 3) << 12) | s->rd->n));
}

void Code_generator::visit(Push_pop *s) {
	Sections::write16(s->section_index, s->section_offset,
					  static_cast<uint16_t>((s->push == PUSH ? 0x2400 : 0x0400) | (get_bit(s->r->n, 3) << 12) | s->r->n));
}

//------------------------------------------------------------------------------------------
//	Directive

void Code_generator::visit(Space *s) {
	Sections::fill(s->section_index, s->section_offset,
				   static_cast<uint8_t>(s->initial->get_value()), s->size->get_value());
}

void Code_generator::visit(Align *s) {
	auto align = s->size->get_value();
	Sections::fill(s->section_index, s->section_offset, 0,
				   ((s->section_offset + ((1 << align) - 1)) / align) * align);
}

void Code_generator::visit(Byte *s) {
	if ((s->section_offset & (s->grain_size - 1)) != 0)
		warning_report(&s->location, string_printf(
				"Misaligned address - multibyte value (%d bytes) not in an address multiple of %d",
				s->grain_size, s->grain_size));
	auto i = 0U;
	auto mask = MAKE_MASK(s->grain_size * 8, 0);
	for (auto e: *s->value_list) {
		auto value = 0U;
		auto exp_type = e->get_type();
		if (exp_type == ABSOLUTE || exp_type == Value_type::LABEL) {
			if (exp_type == ABSOLUTE) {
				value = e->get_value();
			} else {
				auto symbol = e->get_symbol();
				auto addend = e->get_value();
				auto *reloc = new Relocation
						{s, &e->location, 0, s->grain_size * 8, Relocation::Type::ABSOLUTE, symbol, addend};
				Relocations::add(reloc);
			}

			if ((value & ~mask) != 0)
				warning_report(&e->location, string_printf(
						"Expression's value = %d (0x%x) not encodable in %d bit, truncate to %d (0x%x)",
						value, value, s->grain_size * 8, value & mask, value & mask));
			switch (s->grain_size) {
				case 1:
					Sections::write8(s->section_index, s->section_offset + i++, static_cast<uint8_t>(value));
					break;
				case 2:
					Sections::write16(s->section_index, s->section_offset + i++ * 2, static_cast<uint16_t>(value));
					break;
				case 4:
					Sections::write32(s->section_index, s->section_offset + i++ * 4, static_cast<uint32_t>(value));
			}
		}
		else if (exp_type == UNDEFINED) {
			error_report(&e->location, "Undefined expression");
		}
		else {
			error_report(&e->location, "Invalid expression");
		}
	}
}
