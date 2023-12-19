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

#include <climits>

#include "code_generator.h"
#include "relocations.h"
#include "p16_parser.hpp"
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
    LDR_RELATIVE_CONSTANT_POSITION = 4,

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

    CMP_OPCODE = 0xb800,

    SPACE_SIZE = 16,
    SPACE_INITIAL_VALUE_SIZE = 8
};

//	Instruction

void Code_generator::visit(Inst_load_relative *s)
{
	s->check_alignment();
	if ((s->ldst & 2) != 0) // is a STR
		error_report(&s->location, "str instructions with PC relative address doesn't exist, only ldr exists");
	if (s->ldst == 1)       // is a LDRB
		error_report(&s->location, "ldrb instruction with PC relative address doesn't exist, only ldr exists");
	unsigned code = LDR_RELATIVE_OPCODE + (s->rd->n << RD_POSITION);
	if (s->target->evaluate()) {
		auto target_type = s->target->get_type();
		if (target_type == Value_type::LABEL) {
			string symbol = s->target->get_symbol();
			auto addend = s->target->get_value() - 2;
			auto *reloc = new Relocation{&s->location, &s->target->location, s->section_index, s->section_offset,
							LDR_RELATIVE_CONSTANT_POSITION, LDR_RELATIVE_CONSTANT_SIZE,
							Relocation::Relocation_type::RELATIVE_UNSIGNED, symbol, addend};
			Relocations::add(reloc);
		}
		else
			error_report(&s->target->location, "Invalid expression, must be a label");
	}
	Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(code));
}

void Code_generator::visit(Inst_load_store_indirect *s)
{
	s->check_alignment();
	if (s->rn->n > 7)
		error_report(&s->rn->location, "Invalid register");
	auto byte = s->ldst & 1;
	auto code = ((s->ldst & 2) ? STR_INDIRECT_OPCODE : LDR_INDIRECT_OPCODE)
			| (byte << LDR_STR_INDIRECT_BYTE_INDICATION_POSITION)
			| (s->rd->n << RD_POSITION) | (s->rn->n << RN_POSITION);
	if (s->constant == nullptr) {
		code |= (1 << LDR_STR_INDIRECT_REG_INDICATION_POSITION) + (s->rm->n << RM_POSITION);
	} else if (s->constant->evaluate()) {
		auto constant_type = s->constant->get_type();
		if (constant_type == ABSOLUTE) {
			auto constant_value = s->constant->get_value();
			if (constant_value < 0) {
				error_report(&s->constant->location,"Expression's value must a positive number");
			}
			else {
				if (byte) {
					if ((constant_value & ~MAKE_MASK(LDR_STR_INDIRECT_CONST_INDEX_SIZE, 0)) != 0) {
						warning_report(&s->constant->location,
							string_printf("Expression's value = %d (0x%x) not encodable in %d bit,"
									" truncate to %d (0x%x)",
							constant_value, constant_value, LDR_STR_INDIRECT_CONST_INDEX_SIZE,
							constant_value & MAKE_MASK(LDR_STR_INDIRECT_CONST_INDEX_SIZE, 0),
							constant_value & MAKE_MASK(LDR_STR_INDIRECT_CONST_INDEX_SIZE, 0)));
					}
					code |= ((constant_value & MAKE_MASK(LDR_STR_INDIRECT_CONST_INDEX_SIZE, 0))
										<< LDR_STR_INDIRECT_CONST_INDEX_POSITION);
				}
				else {
					if (((constant_value / 2) & ~MAKE_MASK(LDR_STR_INDIRECT_CONST_INDEX_SIZE, 0)) != 0) {
						warning_report(&s->constant->location,
							string_printf( "Expression's value / 2 = %d (0x%x) not encodable in %d bit,"
									"truncate to %d (0x%x)",
							constant_value, constant_value, LDR_STR_INDIRECT_CONST_INDEX_SIZE,
							constant_value & MAKE_MASK(LDR_STR_INDIRECT_CONST_INDEX_SIZE, 0),
							constant_value & MAKE_MASK(LDR_STR_INDIRECT_CONST_INDEX_SIZE, 0)));
					}
					if ((constant_value & 1) != 0)
						warning_report(&s->constant->location, string_printf(
							"Expression's value = %d (0x%x) must be an even value", constant_value, constant_value));
					constant_value /= 2;
					code |= ((constant_value & MAKE_MASK(LDR_STR_INDIRECT_CONST_INDEX_SIZE, 0))
										<< LDR_STR_INDIRECT_CONST_INDEX_POSITION);
				}
			}
		}
		else if (constant_type == Value_type::LABEL)
			error_report(&s->constant->location,"Can't be a label, must be a constant");
	}
	Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(code));
}

void Code_generator::visit(Inst_branch *s)
{
	s->check_alignment();
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
		code = BCC_OPCODE;
		break;
	case CC:
	case HS:
		code = BCS_OPCODE;
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
	auto offset = 0;    //  o offset do branch é um valor com sinal
	if (s->expression->evaluate()) {
		auto exp_type = s->expression->get_type();
		if (exp_type == ABSOLUTE) {
			auto expression_value = s->expression->get_value();
			if ((expression_value & 1) != 0)
				warning_report(&s->expression->location, "Must be an even value!");
			offset = expression_value >> 1;
		}
		else if (exp_type == Value_type::LABEL) {
			string symbol = s->expression->get_symbol();
			if (s->section_index == Symbols::get_section(symbol)) { //  Label resolvida se pertencer à mesma secção
				offset = s->expression->get_value() - s->section_offset - 2;    //  O PC está dois endereços à frente
				if ((offset & 1) != 0)
					warning_report(&s->expression->location,
						string_printf("Address of %s = 0x%x, must be even!", symbol.c_str(), offset));
				if (offset >= (1 << BRANCH_OFFSET_SIZE) || offset < (~0 << BRANCH_OFFSET_SIZE))
					error_report(&s->expression->location,
						string_printf("Interval between PC and target address: %+d (0x%x) - "
								"isn't codable in %d bit two's complement",
								offset, offset, BRANCH_OFFSET_SIZE + 1));
				offset >>= 1;
			} else {    // A Label pertence a outra secção. Será resolvida na fase de relocalização
				error_report(&s->expression->location,
					string_printf( "Label \"%s\" is defined in another section", symbol.c_str()));
			}
		}
	}
	Sections::write16(s->section_index, s->section_offset,
		static_cast<uint16_t>(code | ((offset & MAKE_MASK(BRANCH_OFFSET_SIZE, 0)) << BRANCH_OFFSET_POSITION)));
}

void Code_generator::visit(Inst_shift *s)
{
	s->check_alignment();
	if (s->rn->n > 7)
		error_report(&s->rn->location, "Invalid register");

	if (s->constant->evaluate()) {
		auto position_type = s->constant->get_type();
		if (position_type == ABSOLUTE) {
			auto constant = s->constant->get_value();
			if (constant < 0) {
				error_report(&s->constant->location,"Expression's value must be a positive number");
			}
			else if ((constant & ~MAKE_MASK(SHIFT_CONST_SIZE, 0)) != 0) {
				warning_report(&s->constant->location,
					string_printf("Expression's value = %d (0x%x) not encodable in %d bit, truncate to %d (0x%x)",
							constant, constant, SHIFT_CONST_SIZE,
							constant & MAKE_MASK(SHIFT_CONST_SIZE, 0),
							constant & MAKE_MASK(SHIFT_CONST_SIZE, 0)));
			}
			Sections::write16(s->section_index, s->section_offset,
				static_cast<uint16_t>(((
					s->operation == LSL ? LSL_OPCODE
				  		: s->operation == LSR ? LSR_OPCODE
							: s->operation == ASR ? ASR_OPCODE
								: s->operation == ROR ? ROR_OPCODE : 0))
					+ ((constant & MAKE_MASK(SHIFT_CONST_SIZE, 0)) << SHIFT_CONST_POSITION)
					+ (s->rn->n << RN_POSITION) + (s->rd->n << RD_POSITION)));
		}
		else if (position_type == Value_type::LABEL)
			error_report(&s->constant->location,"Can't be a label, must be a constant");
	}
}

void Code_generator::visit(Inst_rrx *s)
{
	s->check_alignment();
	if (s->rn->n > 7)
		error_report(&s->rn->location, "Invalid register");
	Sections::write16(s->section_index, s->section_offset,
			  static_cast<uint16_t>(RRX_OPCODE + (s->rn->n << RN_POSITION) + (s->rd->n << RD_POSITION)));
}

void Code_generator::visit(Inst_arith
 *s)
{
	s->check_alignment();
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
		if (s->expression->evaluate()) {
			auto expression_type = s->expression->get_type();
			if (expression_type == ABSOLUTE) {
				auto constant = s->expression->get_value();
				if (constant < 0) {
					error_report(&s->expression->location,"Expression's value must be a positive number");
				}
				else if ((constant & ~MAKE_MASK(ARITH_CONST_SIZE, 0)) != 0) {
					warning_report(&s->expression->location,
						string_printf( "Expression's value = %d (0x%x) not encodable in %d bit, truncate to %d (0x%x)",
								constant, constant, ARITH_CONST_SIZE,
								constant & MAKE_MASK(ARITH_CONST_SIZE, 0),
								constant & MAKE_MASK(ARITH_CONST_SIZE, 0)));
				}
				code |= (constant & MAKE_MASK(ARITH_CONST_SIZE, 0)) << ARITH_CONST_POSITION;
			}
			else {
				error_report(&s->expression->location,"Can't be a label, must be a constant");
			}
		}
	}
	Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(code));
}

void Code_generator::visit(Inst_logic *s)
{
	s->check_alignment();
	if (s->rn->n > 7)
		error_report(&s->rn->location, "Invalid register");

	Sections::write16(s->section_index, s->section_offset,
		      static_cast<uint16_t> ((
				s->operation == AND ? AND_OPCODE
		      			: s->operation == OR ? OR_OPCODE
						: s->operation == EOR ? XOR_OPCODE : 0)
				+ (s->rm->n << RM_POSITION)
				+ (s->rn->n << RN_POSITION)
				+ (s->rd->n << RD_POSITION)));
}

void Code_generator::visit(Inst_not *s) {
	s->check_alignment();
	Sections::write16(s->section_index, s->section_offset,
		static_cast<uint16_t>(NOT_OPCODE + (s->rn->n << RS_POSITION) + (s->rd->n << RD_POSITION)));
}

void Code_generator::visit(Inst_mov *s)
{
	s->check_alignment();
	auto code = s->rd->n << RD_POSITION;
	if (s->constant == nullptr) {	//	mov	rd, rs
		code |= MOV_REG_OPCODE | (s->rs->n << MOV_RS_POSITION);
	} else {                        //      mov | movt      rd, constant
		code |= s->high == MOV_LOW ? MOV_CONST_OPCODE : MOVT_CONST_OPCODE;
		if (s->constant->evaluate()) {
			auto constant_value = s->constant->get_value();
			auto constant_type = s->constant->get_type();
			if (constant_type == ABSOLUTE ) {
				if ((constant_value & ~MAKE_MASK(MOV_CONST_SIZE, 0)) != 0) {
					warning_report(&s->constant->location,
						string_printf("Expression's value = %d (0x%x) exceeds domain(0-255), truncate to %d (0x%x)",
								constant_value, constant_value,
								constant_value & MAKE_MASK(MOV_CONST_SIZE, 0),
								constant_value & MAKE_MASK(MOV_CONST_SIZE, 0)));
				}
				code |= (constant_value & MAKE_MASK(MOV_CONST_SIZE, 0)) << MOV_CONST_POSITION;
			}
			else if (constant_type == Value_type::LABEL) {
				auto symbol = s->constant->get_symbol();
				auto value = s->constant->get_value();
				auto *reloc = new Relocation {&s->location, &s->constant->location,
							s->section_index, s->section_offset,
							MOV_CONST_POSITION, MOV_CONST_SIZE,
							Relocation::Relocation_type::ABSOLUTE, symbol, value};
				Relocations::add(reloc);
			}
		}
	}
	Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(code));
}

void Code_generator::visit(Inst_movs *s)
{
	s->check_alignment();
	Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(MOVS_OPCODE));
}

void Code_generator::visit(Inst_compare *s) 
{
	s->check_alignment();
	if (s->rn->n > 7)
		error_report(&s->rn->location, "Invalid register");
	auto code = s->rn->n << RN_POSITION;
	code |= CMP_OPCODE | (s->rm->n << RM_POSITION);
	Sections::write16(s->section_index, s->section_offset, static_cast<uint16_t>(code));
}

void Code_generator::visit(Inst_msr *s)
{
	s->check_alignment();
	if (s->rd->n != CPSR && s->rd->n != SPSR)
		error_report(&s->rd->location, "Invalid register");
	Sections::write16(s->section_index, s->section_offset,
		static_cast<uint16_t>(MSR_OPCODE 
			| ((s->rd->n == SPSR) << SPSR_INDICATION_POSITION)
			| (s->rs->n << MOV_RS_POSITION)));
}

void Code_generator::visit(Inst_mrs *s)
{
	s->check_alignment();
	if (s->rs->n != CPSR && s->rs->n != SPSR)
		error_report(&s->rs->location, "Invalid register");
	Sections::write16(s->section_index, s->section_offset,
		static_cast<uint16_t>(MRS_OPCODE
			| ((s->rs->n == SPSR) << SPSR_INDICATION_POSITION)
			| (s->rd->n << RD_POSITION)));
}

void Code_generator::visit(Inst_push_pop *s)
{
	s->check_alignment();
	Sections::write16(s->section_index, s->section_offset,
		static_cast<uint16_t>((s->push == PUSH ? PUSH_OPCODE : POP_OPCODE)
			| (s->r->n << RD_POSITION)));
}

//------------------------------------------------------------------------------------------
//	Directive

void Code_generator::visit(Dir_equ *e)
{
	if (e->symbol->type == UNDEFINED && e->symbol->value_expression != nullptr) {
		e->symbol->value_expression->evaluate(); 	//	Valor ainda não avaliado
		e->symbol->type = e->symbol->value_expression->get_type();
	}
}

void Code_generator::visit(Dir_space *s)
{
	if (s->size->evaluate()) {
		auto size_value = s->size->get_value();
		auto size_type = s->size->get_type();
		if (size_type == ABSOLUTE) {
			if (static_cast<int>(size_value) < 0) {
				error_report(&s->size->location,
					string_printf("Invalid: %d (0x%x), size must be a positive value.\n",
						size_value, size_value));
					size_value = 0;
			}
			else if ((size_value & ~MAKE_MASK(16, 0)) != 0) {
				warning_report(&s->size->location,
					string_printf("Size for space directive %d (0x%x) exceds memory size, truncate to %d (0x%x)",
						size_value, size_value,
						size_value & MAKE_MASK(SPACE_SIZE, 0),
						size_value & MAKE_MASK(SPACE_SIZE, 0)));
				size_value &= MAKE_MASK(SPACE_SIZE, 0);
			}
			if (s->initial->evaluate()) {
				auto initial_value = s->initial->get_value();
				auto initial_type = s->initial->get_type();
				if (initial_type == ABSOLUTE) {
					if ((abs(static_cast<int>(initial_value)) & ~MAKE_MASK(8, 0)) != 0) {
						warning_report(&s->initial->location,
							string_printf("Initial value = %d (0x%x) not encodable in %d bit, truncate to %d (0x%x)",
								initial_value, initial_value, SPACE_INITIAL_VALUE_SIZE,
								initial_value & MAKE_MASK(SPACE_INITIAL_VALUE_SIZE, 0),
								initial_value & MAKE_MASK(SPACE_INITIAL_VALUE_SIZE, 0)));

						initial_value &= MAKE_MASK(SPACE_INITIAL_VALUE_SIZE, 0);
					}
					Sections::fill(s->section_index, s->section_offset, initial_value, size_value);
				}
				else if (initial_type == Value_type::LABEL)
					error_report(&s->initial->location,"Can't be a label, must be a constant");
			}
		}
		else if (size_type == Value_type::LABEL)
			error_report(&s->initial->location,"Can't be a label, must be a constant");
	}
}

void Code_generator::visit(Dir_align *s)
{
	if (s->size->evaluate()) {
		auto size_value = s->size->get_value();
		auto size_type = s->size->get_type();
		if (size_type == ABSOLUTE) {
			if (size_value < 0 && size_value > 1)
				warning_report(&s->size->location, "Invalid value for alignment. Must be 1 or 0");
			Sections::fill(s->section_index, s->section_offset, 0, s->size_in_memory);
		}
		else if (size_type == Value_type::LABEL)
			error_report(&s->size->location,"Can't be a label, must be a constant");
	}
}

void Code_generator::visit(Dir_byte *s)
{
	if ((s->section_offset & (s->grain_size - 1)) != 0)
		warning_report(&s->location,
			string_printf("Misaligned address - multibyte value (%d bytes) not in an address multiple of %d",
					s->grain_size, s->grain_size));
	auto i = 0U;
	auto mask = MAKE_MASK(s->grain_size * 8, 0);
	if (s->value_list->empty()) {
		warning_report(&s->location,
			string_printf("Directives .byte or .word must have an argument"));
	}
	for (auto e: *s->value_list) {
		auto value = 0;
		if (e->evaluate()) {
			value = e->get_value();
			if (value != 0 && (Sections::get_section(s->section_index)->flags & Section::BSS) != 0)
				error_report(&e->location, "Attempt to store non-zero value in section \'.bss\'");
			auto exp_type = e->get_type();
			if (exp_type == ABSOLUTE) {
				if ((abs(static_cast<int>(value)) & ~mask) != 0)
					warning_report(&e->location,
						string_printf("Expression's value = %d (0x%x) not encodable in %d bit, truncate to %d (0x%x)",
						value, value, s->grain_size * 8, value & mask, value & mask));
			}
			else if (exp_type == Value_type::LABEL) {
				auto symbol = e->get_symbol();
				auto *reloc = new Relocation{&s->location, &e->location,
						s->section_index, s->section_offset + i, 0, s->grain_size * 8,
						Relocation::Relocation_type::ABSOLUTE, symbol, value};
				Relocations::add(reloc);
			}
		}
		switch (s->grain_size) {
		case 1:
			Sections::write8(s->section_index, s->section_offset + i, static_cast<uint8_t>(value));
			break;
		case 2:
			Sections::write16(s->section_index, s->section_offset + i, static_cast<uint16_t>(value));
			break;
		case 4:
			Sections::write32(s->section_index, s->section_offset + i, static_cast<uint32_t>(value));
		}
		i += s->grain_size;
	}
}

