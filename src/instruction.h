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
		return string_printf("%4d %04X %02X%02X    \t", location.line,
							 Sections::get_address(section_index) + section_offset,
							 Sections::read8(section_index, section_offset),
							 Sections::read8(section_index, section_offset + 1)
		);
	}
};

struct Load_relative: public Instruction {
	unsigned ldst;
	Register *rd;
	Expression *constant;
	Load_relative(unsigned ldst, Register *rd, Expression *d, Location left) :
		Instruction {left}, ldst {ldst}, rd {rd}, constant {d} { }
	~Load_relative() { delete constant; }
	void accept(Visitor *v) { v->visit(this); }
};

struct Load_store_indirect: public Instruction {
	unsigned ldst;
	Register *rd, *rn, *rm;
	Expression *constant;
	Load_store_indirect(unsigned ldst, Register *rd, Register *rn, Register *rm, Location left) :
		Instruction {left}, ldst {ldst}, rd {rd}, rn {rn}, rm {rm}, constant {nullptr} { }

	Load_store_indirect(unsigned ldst, Register *rd, Register *rn, Expression *c, Location left) :
		Instruction {left}, ldst {ldst}, rd {rd}, rn {rn}, constant {c} { }

	~Load_store_indirect() { delete constant; }

	void accept(Visitor *v) { v->visit(this); }
};

struct Branch: public Instruction {
	Expression *expression;
	unsigned condition;
	Branch(unsigned condition, Expression *e, Location left) :
		Instruction {left}, expression {e}, condition {condition} { }
	~Branch() { delete expression; }
	void accept(Visitor *v) { v->visit(this); }
};
 
struct Arith: public Instruction {
	Register *rd, *rn, *rm;
	Expression *expression;
	unsigned operation;

	Arith(unsigned o, Register *rd, Register *rn, Register *rm, Location left) :
		Instruction {left}, rd {rd}, rn {rn}, rm {rm}, expression {nullptr}, operation {o} { }

	Arith(unsigned o, Register *rd, Register *rn, Expression *e, Location left) :
		Instruction {left}, rd {rd}, rn {rn}, expression {e}, operation {o} { }

	~Arith() { delete expression; }

	void accept(Visitor *v) { v->visit(this); }
};

struct Compare: public Instruction {
	Register *rn, *rm;
	Expression *constant;

	Compare(Register *rn, Register *rm, Location left) :
		Instruction {left}, rn {rn}, rm {rm}, constant {nullptr} { }

	Compare(Register *rn, Expression *c, Location left) :
		Instruction {left}, rn {rn}, constant {c} { }

	~Compare() { delete constant; }

	void accept(Visitor *v) { v->visit(this); }
};

struct Logic: public Instruction {
	Register *rd, *rn, *rm;
	unsigned operation;

	Logic(unsigned o, Register *rd, Register *rn, Register *rm, Location left) :
		Instruction {left}, rd {rd}, rn {rn}, rm {rm}, operation {o} { }

	void accept(Visitor *v) { v->visit(this); }
};

struct Not: public Instruction {
	Register *rd, *rn;

	Not(Register *rd, Register *rn, Location left) :
		Instruction {left}, rd {rd}, rn {rn} { }

	void accept(Visitor *v) { v->visit(this); }
};


struct Shift: public Instruction {
	Register *rd, *rn;
	unsigned operation;
	Expression *constant;

	Shift(unsigned op, Register *rd, Register *rn, Expression *p, Location left) :
		Instruction {left}, rd {rd}, rn {rn}, operation {op}, constant {p} { }

	~Shift() { delete constant; }

	void accept(Visitor *v) { v->visit(this); }
};


struct Rrx: public Instruction {
	Register *rd, *rn;

	Rrx(Register *rd, Register *rn, Location left) :
		Instruction {left}, rd {rd}, rn {rn} {	}

	void accept(Visitor *v) { v->visit(this); }
};

struct Move: public Instruction {
	Register *rd, *rs;
	Expression *constant;
	unsigned high;

	Move(Register *rd, Register *rs, Location left) :
		Instruction {left}, rd {rd}, rs {rs}, constant {nullptr} { }
 
 	Move(unsigned high, Register *rd, Expression *c, Location left) :
		Instruction {left}, rd {rd}, constant {c}, high(high) { }

	~Move() { delete constant; }

	void accept(Visitor *v) { v->visit(this); }
};

struct Moves: public Instruction {
	Register *rd, *rn;

	Moves(Register *rd, Register *rn, Location left) :
		Instruction {left}, rd {rd}, rn {rn} { }
 
	void accept(Visitor *v) { v->visit(this); }
};

struct Msr: public Instruction {
	Register *rd, *rs;

	Msr(Register *rd, Register *rs, Location left) :
		Instruction {left}, rd {rd}, rs {rs} { }

	void accept(Visitor *v) { v->visit(this); }
};

struct Mrs: public Instruction {
	Register *rd, *rs;

	Mrs(Register *rd, Register *rs, Location left) :
		Instruction {left}, rd {rd}, rs {rs} { }

	void accept(Visitor *v) { v->visit(this); }
};

struct Push_pop: public Instruction {
	unsigned push;
	Register *r;

	Push_pop(unsigned push, Register *r, Location left) :
		Instruction {left}, push {push}, r {r} { }

	void accept(Visitor *v) { v->visit(this); }
};

}

#endif

