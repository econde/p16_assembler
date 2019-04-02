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
#include "p16.tab.hpp"
#include "error.h"
#include "utils.h"

using namespace ast;
using namespace std;

enum {
    RD_POSITION = 0,
    RN_POSITION = 4,
    RM_POSITION = 7,
    RS_POSITION = 7,
    LDR_RELATIVE_OPCODE = 0x0c00,
    LDR_RELATIVE_CONSTANT_SIZE = 6,
    LDR_REALTIVE_CONSTANT_POSITION = 4,

    POP_OPCODE = 0x0400,
    PUSH_OPCODE = 0x2400,
    PUSH_RS_POSITION = 0,

    LDR_INDIRECT_OPCODE = 0x0000,
    STR_INDIRECT_OPCODE = 0x2000,
    LDR_STR_INDIRECT_BYTE_INDICATION_POSITION = 11,
    LDR_STR_INDIRECT_REG_INDICATION_POSITION = 12,
    LDR_STR_INDIRECT_CONST_INDEX_SIZE = 3,
    LDR_STR_INDIRECT_CONST_INDEX_POSITION = 7,

    BEQ_OPCODE = 0x4000,
    BNE_OPCODE = 0x4400,
    BCS_OPCODE = 0x4800,
    BCC_OPCODE = 0x4c00,
    BGE_OPCODE = 0x5000,
    BLT_OPCODE = 0x5400,
    B_OPCODE = 0x5800,
    BL_OPCODE = 0x5c00,
    BRANCH_OFFSET_SIZE = 10,
    BRANCH_OFFSET_POSITION = 0,

    SHIFT_CONST_SIZE = 4,
    SHIFT_CONST_POSITION = 7,

    LSL_OPCODE = 0xe000,
    LSR_OPCODE = 0xe800,
    ASR_OPCODE = 0xf000,
    ROR_OPCODE = 0xf800,
    RRX_OPCODE = 0xd800,

    ARITH_CONST_SIZE = 4,
    ARITH_CONST_POSITION = 7,

    ADD_REG_OPCODE = 0x8000,
    SUB_REG_OPCODE = 0x8800,
    ADC_REG_OPCODE = 0x9000,
    SBC_REG_OPCODE = 0x9800,
    ADD_CONST_OPCODE = 0xa000,
    SUB_CONST_OPCODE = 0xa800,

    AND_OPCODE = 0xc000,
    OR_OPCODE = 0xc800,
    XOR_OPCODE = 0xd000,

    NOT_OPCODE = 0xb010,

    MOV_REG_OPCODE = 0xb000,
    MOV_RS_POSITION = 7,
    MOV_CONST_OPCODE = 0x6000,
    MOVT_CONST_OPCODE = 0x7000,
    MOV_CONST_SIZE = 8,
    MOV_CONST_POSITION = 4,

    MOVS_OPCODE = 0xb020,

    MSR_OPCODE = 0xb040,
    MRS_OPCODE = 0xb060,
    SPSR_INDICATION_POSITION = 4,

    CMP_OPCODE = 0xb800
};

//	Instruction

void Code_generator::visit(Load_relative *s) {
	unsigned code = LDR_RELATIVE_OPCODE + (s->rd->n << RD_POSITION);
	Value_type direct_type = s->constant->get_type();
	if (direct_type == ABSOLUTE || direct_type == Value_type::LABEL || direct_type == UNDEFINED) {
		auto constant = 0U;
		if (direct_type == ABSOLUTE)
            constant = s->constant->get_value();
		else {   // LABEL ou UNDEFINED
			string symbol = s->constant->get_symbol();
			auto addend = s->constant->get_value();
			auto *reloc = new Relocation{s, &s->constant->location,
										 LDR_REALTIVE_CONSTANT_POSITION, LDR_RELATIVE_CONSTANT_SIZE,
                                         Relocation::Type::RELATIVE_UNSIGNED, symbol, addend};
			Relocations::add(reloc);
		}
		if ((constant & ~MAKE_MASK(LDR_RELATIVE_CONSTANT_SIZE, 0)) != 0) {
			error_report(&s->constant->location,
                         "Address defined by \"" + s->constant->to_string()
                         + string_printf("\" PC + %d (0x%x) is out of range for PC-relative addressing",
                                         constant, constant));
		}
		if ((constant & 1) != 0)
			warning_report(&s->constant->location,
						   string_printf("Word access in odd address: PC + %d (0x%x)", constant, constant));
		code |= constant << LDR_REALTIVE_CONSTANT_POSITION;
	}
	else
		error_report(&s->constant->location, string_printf("Invalid expression"));
	Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(code));
}

void Code_generator::visit(Load_store_indirect *s) {
	if (s->rn->n > 7)
		error_report(&s->rn->location, "Invalid register");
	auto byte = s->ldst & 1;
	auto code = ((s->ldst & 2) ? STR_INDIRECT_OPCODE : LDR_INDIRECT_OPCODE)
			| (byte << LDR_STR_INDIRECT_BYTE_INDICATION_POSITION) | (s->rd->n << RD_POSITION) | (s->rn->n << RN_POSITION);
	if (s->constant == nullptr) {
		code |= (1 << LDR_STR_INDIRECT_REG_INDICATION_POSITION) + (s->rm->n << RM_POSITION);
	} else {
		auto constant_type = s->constant->get_type();
		if (constant_type == ABSOLUTE) {
			auto constant = s->constant->get_value();
			if (byte) {
                if ((constant & ~MAKE_MASK(LDR_STR_INDIRECT_CONST_INDEX_SIZE, 0)) != 0) {
                    warning_report(&s->constant->location,
                                   string_printf("Expression's value = %d (0x%x) not encodable in %d bit,"
                                                 " truncate to %d (0x%x)",
                                                 constant, constant, LDR_STR_INDIRECT_CONST_INDEX_SIZE,
                                                 constant & MAKE_MASK(LDR_STR_INDIRECT_CONST_INDEX_SIZE, 0),
                                                 constant & MAKE_MASK(LDR_STR_INDIRECT_CONST_INDEX_SIZE, 0)));
                }
                code |= ((constant & MAKE_MASK(LDR_STR_INDIRECT_CONST_INDEX_SIZE, 0)) << LDR_STR_INDIRECT_CONST_INDEX_POSITION);
            }
            else {
				if ((constant & 1) != 0)
					warning_report(&s->constant->location, string_printf(
							"Expression's value = %d (0x%x) must be an even value", constant, constant));
                constant /= 2;
				if ((constant & ~MAKE_MASK(LDR_STR_INDIRECT_CONST_INDEX_SIZE, 0)) != 0) {
                    warning_report(&s->constant->location,
                                   string_printf( "Expression's value / 2 = %d (0x%x) not encodable in %d bit,"
                                                  "truncate to %d (0x%x)",
                                                  constant, constant, LDR_STR_INDIRECT_CONST_INDEX_SIZE,
                                                  constant & MAKE_MASK(LDR_STR_INDIRECT_CONST_INDEX_SIZE, 0),
                                                  constant & MAKE_MASK(LDR_STR_INDIRECT_CONST_INDEX_SIZE, 0)));
                }
                code |= ((constant & MAKE_MASK(LDR_STR_INDIRECT_CONST_INDEX_SIZE, 0)) << LDR_STR_INDIRECT_CONST_INDEX_POSITION);
			}
		}
		else if (constant_type == UNDEFINED) {
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
		case EQ:
			code = BEQ_OPCODE;
			break;
		case NE:
			code = BNE_OPCODE;
			break;
		case CS:
        case LO:
			code = BCS_OPCODE;
			break;
		case CC:
        case HS:
			code = BCC_OPCODE;
			break;
		case GE:
			code = BGE_OPCODE;
			break;
		case LT:
			code = BLT_OPCODE;
			break;
		case B:
			code = B_OPCODE;
			break;
		case BL:
			code = BL_OPCODE;
			break;
	}
	auto exp_type = s->expression->get_type();
	auto offset = 0;    //  valor com sinal
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
			warning_report(&s->expression->location, string_printf("Address of %s = 0x%x, must be even!", symbol.c_str(), addend));
		auto *reloc = new Relocation{s, &s->expression->location, BRANCH_OFFSET_POSITION, BRANCH_OFFSET_SIZE,
							   Relocation::Type::RELATIVE, symbol, addend};
		Relocations::add(reloc);
	}
	else if (exp_type == Value_type::LABEL) {   //  Label resolvida, distância definida se pertencer à mesma secção
		string symbol = s->expression->get_symbol();

		if (s->section_index == Symbols::get_section(symbol)) {
			offset = s->expression->get_value() - s->section_offset - 2;    //  O PC está dois endereços à frente
			if ((offset & 1) != 0)
				warning_report(&s->expression->location, string_printf("Address of %s = 0x%x, must be even!", symbol.c_str(), offset));
			offset >>= 1;
			if (offset >= (1 << BRANCH_OFFSET_SIZE) || offset < (~0 << BRANCH_OFFSET_SIZE))
				error_report(&s->expression->location,
                             string_printf( "Interval between PC and target address - %+d (0x%x) words - "
                                            "isn't codable in %d bit two's complement",
                                            offset, offset, BRANCH_OFFSET_SIZE));
		} else {    // A Label pertence a outra secção será resolvida na fase de relocalização
			auto addend = s->expression->get_value() - 2;
			if ((addend & 1) != 0)
				warning_report(&s->expression->location, string_printf("Address of %s = 0x%x, must be even!", symbol.c_str(), addend));
			auto *reloc = new Relocation{s, &s->expression->location, BRANCH_OFFSET_POSITION, BRANCH_OFFSET_SIZE,
                                         Relocation::Type::RELATIVE, symbol, addend};
			Relocations::add(reloc);
		}
	}
	Sections::write16(s->section_index, s->section_offset,
					  static_cast<uint16_t>(code | ((offset & MAKE_MASK(BRANCH_OFFSET_SIZE, 0)) << BRANCH_OFFSET_POSITION)));
}

void Code_generator::visit(Shift *s) {
	if (s->rn->n > 7)
		error_report(&s->rn->location, "Invalid register");

	auto position_type = s->constant->get_type();
	if (position_type == ABSOLUTE) {
		auto constant = s->constant->get_value();
		if ((constant & ~MAKE_MASK(SHIFT_CONST_SIZE, 0)) != 0) {
			warning_report(&s->constant->location,
                           string_printf("Expression's value = %d (0x%x) not encodable in %d bit, truncate to %d (0x%x)",
                                         constant, constant, SHIFT_CONST_SIZE,
                                         constant & MAKE_MASK(SHIFT_CONST_SIZE, 0),
                                         constant & MAKE_MASK(SHIFT_CONST_SIZE, 0)));
		}
		Sections::write16(s->section_index, s->section_offset,
                          static_cast<uint16_t>(((s->operation == LSL
                                                  ? LSL_OPCODE : s->operation == LSR
                                                                 ? LSR_OPCODE : s->operation == ASR
                                                                                ? ASR_OPCODE : s->operation == ROR
                                                                                               ? ROR_OPCODE : 0))
                                                + ((constant & MAKE_MASK(SHIFT_CONST_SIZE, 0)) << SHIFT_CONST_POSITION)
                                                + (s->rn->n << RN_POSITION) + (s->rd->n << RD_POSITION)));
	}
	else if (position_type == UNDEFINED)
		error_report(&s->constant->location, string_printf("Undefined expression"));
	else if (position_type == Value_type::LABEL || position_type == INVALID)
		error_report(&s->constant->location, string_printf("Invalid expression"));
}

void Code_generator::visit(Rrx *s) {
	if (s->rn->n > 7)
		error_report(&s->rn->location, "Invalid register");
	Sections::write16(s->section_index, s->section_offset,
					  static_cast<uint16_t>(RRX_OPCODE + (s->rn->n << RN_POSITION) + (s->rd->n << RD_POSITION)));
}

void Code_generator::visit(Arith *s) {
	if (s->rn->n > 7)
		error_report(&s->rn->location, "Invalid register");

	auto code = (s->rn->n << RN_POSITION) | (s->rd->n << RD_POSITION);
	if (s->expression == nullptr) {
        switch (s->operation) {
            case ADD:
                code |= ADD_REG_OPCODE;
                break;
            case SUB:
                code |= SUB_REG_OPCODE;
                break;
            case ADC:
                code |= ADC_REG_OPCODE;
                break;
            case SBC:
                code |= SBC_REG_OPCODE;
                break;
        }
		code |= s->rm->n << RM_POSITION;
	} else {
	    switch (s->operation) {
            case ADD:
                code |= ADD_CONST_OPCODE;
                break;
            case SUB:
                code |= SUB_CONST_OPCODE;
                break;
            case ADC:
            case SBC:
                error_report(&s->expression->location, "Invalid operand");
	    }
		auto expression_type = s->expression->get_type();
		if (expression_type == ABSOLUTE || expression_type == Value_type::LABEL) {
			auto constant = s->expression->get_value();
			if ((constant & ~MAKE_MASK(ARITH_CONST_SIZE, 0)) != 0) {
				warning_report(&s->expression->location,
                               string_printf( "Expression's value = %d (0x%x) not encodable in %d bit, truncate to %d (0x%x)",
                                              constant, constant, ARITH_CONST_SIZE,
                                              constant & MAKE_MASK(ARITH_CONST_SIZE, 0),
                                              constant & MAKE_MASK(ARITH_CONST_SIZE, 0)));
			}
			code |= (constant & MAKE_MASK(ARITH_CONST_SIZE, 0)) << ARITH_CONST_POSITION;
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
	if (s->rn->n > 7)
		error_report(&s->rn->location, "Invalid register");

	Sections::write16(s->section_index, s->section_offset,
                      static_cast<uint16_t>	((s->operation == AND
                                                 ? AND_OPCODE : s->operation == OR
                                                                ? OR_OPCODE : s->operation == EOR ? XOR_OPCODE : 0)
                                                + (s->rm->n << RM_POSITION)
                                                + (s->rn->n << RN_POSITION)
                                                + (s->rd->n << RD_POSITION)));
}

void Code_generator::visit(Not *s) {
	Sections::write16(s->section_index, s->section_offset,
					  static_cast<uint16_t>(NOT_OPCODE + (s->rn->n << RS_POSITION) + (s->rd->n << RD_POSITION)));
}

void Code_generator::visit(Move *s) {
    auto code = s->rd->n << RD_POSITION;
	if (s->constant == nullptr) {
		code |= MOV_REG_OPCODE | (s->rs->n << MOV_RS_POSITION);
	} else {
        code |= s->high == MOV_LOW ? MOV_CONST_OPCODE : MOVT_CONST_OPCODE;
		auto const_type= s->constant->get_type();
		if (const_type == ABSOLUTE ) {
			auto constant = s->constant->get_value();
			if ((abs(static_cast<int>(constant)) & ~MAKE_MASK(MOV_CONST_SIZE, 0)) != 0) {
				warning_report(&s->constant->location,
					string_printf("Expression's value = %d (0x%x) not encodable in %d bit, truncate to %d (0x%x)",
							constant, constant, MOV_CONST_SIZE,
							constant & MAKE_MASK(MOV_CONST_SIZE, 0), constant & MAKE_MASK(MOV_CONST_SIZE, 0)));
			}
			code |= (constant & MAKE_MASK(MOV_CONST_SIZE, 0)) << MOV_CONST_POSITION;
		}
		else if (const_type == Value_type::LABEL) {
			auto symbol = s->constant->get_symbol();
			auto value = s->constant->get_value();
			auto *reloc = new Relocation {s, &s->constant->location, MOV_CONST_POSITION, MOV_CONST_SIZE,
                                          Relocation::Type::ABSOLUTE, symbol, value};
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
	Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(MOVS_OPCODE));
}

void Code_generator::visit(Compare *s) {
	if (s->rn->n > 7)
		error_report(&s->rn->location, "Invalid register");
	auto code = s->rn->n << RN_POSITION;
	if (s->constant == nullptr) {
		code |= CMP_OPCODE | (s->rm->n << RM_POSITION);
	} else {
		error_report(&s->constant->location, "Invalid operand");
	}
	Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(code));
}

void Code_generator::visit(Msr *s) {
	if (s->rd->n != CPSR && s->rd->n != SPSR)
		error_report(&s->rd->location, "Invalid register");

	Sections::write16(s->section_index, s->section_offset,
                      static_cast<uint16_t>(MSR_OPCODE | ((s->rd->n == SPSR) << SPSR_INDICATION_POSITION)
                                            | (s->rs->n << MOV_RS_POSITION)));
}

void Code_generator::visit(Mrs *s) {
	if (s->rs->n != CPSR && s->rs->n != SPSR)
		error_report(&s->rs->location, "Invalid register");

	Sections::write16(s->section_index, s->section_offset,
					  static_cast<uint16_t>(MRS_OPCODE | ((s->rs->n == SPSR) << SPSR_INDICATION_POSITION)
                                            | (s->rd->n << RD_POSITION)));
}

void Code_generator::visit(Push_pop *s) {
	Sections::write16(s->section_index, s->section_offset,
					  static_cast<uint16_t>((s->push == PUSH ? PUSH_OPCODE : POP_OPCODE)
											| (s->r->n << RD_POSITION)));
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
		warning_report(&s->location,
			string_printf("Misaligned address (0x%x)- multibyte value (%d bytes) not in an address multiple of %d",
					s->section_offset, s->grain_size, s->grain_size));
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
				auto *reloc = new Relocation{s, &e->location, 0, s->grain_size * 8,
							Relocation::Type::ABSOLUTE, symbol, addend};
				Relocations::add(reloc);
			}

			if ((abs(static_cast<int>(value)) & ~mask) != 0)
				warning_report(&e->location,
					string_printf("Expression's value = %d (0x%x) not encodable in %d bit, truncate to %d (0x%x)",
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
