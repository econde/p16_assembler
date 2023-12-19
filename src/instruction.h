

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

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "visitor.h"
#include "expression.h"
#include "statement.h"


namespace ast {

std::string register_name(int r);
std::string register_special_name(int r);
std::string arith_name(int operation);
std::string logic_name(int operation);
std::string shift_name(int operation);
std::string branch_name(int operation);

struct Register {
	unsigned n;
	Location location;
	Register(unsigned n, Location location): n {n}, location {location} {}
};

struct Instruction: public Statement {
	explicit Instruction(Location location) : Statement {location} {
		section_index = Sections::current_section->number;
		section_offset = Sections::current_section->content_size;
		size_in_memory = 2;
		Sections::increase(section_index, size_in_memory);
	}

	string listing() {
		return string_printf("%4d %04X %02X %02X\t", location.line,
			Sections::get_address(section_index) + section_offset,
			Sections::read8(section_index, section_offset),
			Sections::read8(section_index, section_offset + 1)
		);
	}
	void check_alignment() {
		if ((section_offset & 1) != 0)
			warning_report(&location,
				string_printf("Misaligned address - "
					"instruction binary code must be located in an even address"));
	}
};

struct Inst_load_relative: public Instruction {
	unsigned ldst;
	Register *rd;
	Expression *target;
	Inst_load_relative(unsigned ldst, Register *rd, Expression *d, Location left) :
		Instruction {left}, ldst {ldst}, rd {rd}, target {d} { }
	~Inst_load_relative() { delete target; }
	void accept(Visitor *v) { v->visit(this); }
};

struct Inst_load_store_indirect: public Instruction {
	unsigned ldst;
	Register *rd, *rn, *rm;
	Expression *constant;
	Inst_load_store_indirect(unsigned ldst, Register *rd, Register *rn, Register *rm, Location left) :
		Instruction {left}, ldst {ldst}, rd {rd}, rn {rn}, rm {rm}, constant {nullptr} { }

	Inst_load_store_indirect(unsigned ldst, Register *rd, Register *rn, Expression *c, Location left) :
		Instruction {left}, ldst {ldst}, rd {rd}, rn {rn}, constant {c} { }

	~Inst_load_store_indirect() { delete constant; }

	void accept(Visitor *v) { v->visit(this); }
};

struct Inst_branch
: public Instruction {
	Expression *expression;
	unsigned condition;
	Inst_branch
(unsigned condition, Expression *e, Location left) :
		Instruction {left}, expression {e}, condition {condition} { }
	~Inst_branch
() { delete expression; }
	void accept(Visitor *v) { v->visit(this); }
};

struct Inst_arith
: public Instruction {
	Register *rd, *rn, *rm;
	Expression *expression;
	unsigned operation;

	Inst_arith
(unsigned o, Register *rd, Register *rn, Register *rm, Location left) :
		Instruction {left}, rd {rd}, rn {rn}, rm {rm}, expression {nullptr}, operation {o} { }

	Inst_arith
(unsigned o, Register *rd, Register *rn, Expression *e, Location left) :
		Instruction {left}, rd {rd}, rn {rn}, expression {e}, operation {o} { }

	~Inst_arith
() { delete expression; }

	void accept(Visitor *v) { v->visit(this); }
};

struct Inst_compare: public Instruction {
	Register *rn, *rm;

	Inst_compare(Register *rn, Register *rm, Location left) :
		Instruction {left}, rn {rn}, rm {rm} { }

	void accept(Visitor *v) { v->visit(this); }
};

struct Inst_logic: public Instruction {
	Register *rd, *rn, *rm;
	unsigned operation;

	Inst_logic(unsigned o, Register *rd, Register *rn, Register *rm, Location left) :
		Instruction {left}, rd {rd}, rn {rn}, rm {rm}, operation {o} { }

	void accept(Visitor *v) { v->visit(this); }
};

struct Inst_not: public Instruction {
	Register *rd, *rn;

	Inst_not(Register *rd, Register *rn, Location left) :
		Instruction {left}, rd {rd}, rn {rn} { }

	void accept(Visitor *v) { v->visit(this); }
};


struct Inst_shift: public Instruction {
	Register *rd, *rn;
	unsigned operation;
	Expression *constant;

	Inst_shift(unsigned op, Register *rd, Register *rn, Expression *p, Location left) :
		Instruction {left}, rd {rd}, rn {rn}, operation {op}, constant {p} { }

	~Inst_shift() { delete constant; }

	void accept(Visitor *v) { v->visit(this); }
};


struct Inst_rrx: public Instruction {
	Register *rd, *rn;

	Inst_rrx(Register *rd, Register *rn, Location left) :
		Instruction {left}, rd {rd}, rn {rn} {	}

	void accept(Visitor *v) { v->visit(this); }
};

struct Inst_mov: public Instruction {
	Register *rd, *rs;
	Expression *constant;
	unsigned high;

	Inst_mov(Register *rd, Register *rs, Location left) :
		Instruction {left}, rd {rd}, rs {rs}, constant {nullptr} { }

 	Inst_mov(unsigned high, Register *rd, Expression *c, Location left) :
		Instruction {left}, rd {rd}, constant {c}, high(high) { }

	~Inst_mov() { delete constant; }

	void accept(Visitor *v) { v->visit(this); }
};

struct Inst_movs: public Instruction {
	Register *rd, *rn;

	Inst_movs(Register *rd, Register *rn, Location left) :
		Instruction {left}, rd {rd}, rn {rn} { }

	void accept(Visitor *v) { v->visit(this); }
};

struct Inst_msr: public Instruction {
	Register *rd, *rs;

	Inst_msr(Register *rd, Register *rs, Location left) :
		Instruction {left}, rd {rd}, rs {rs} { }

	void accept(Visitor *v) { v->visit(this); }
};

struct Inst_mrs: public Instruction {
	Register *rd, *rs;

	Inst_mrs(Register *rd, Register *rs, Location left) :
		Instruction {left}, rd {rd}, rs {rs} { }

	void accept(Visitor *v) { v->visit(this); }
};

struct Inst_push_pop: public Instruction {
	unsigned push;
	Register *r;

	Inst_push_pop(unsigned push, Register *r, Location left) :
		Instruction {left}, push {push}, r {r} { }

	void accept(Visitor *v) { v->visit(this); }
};

}

#endif
