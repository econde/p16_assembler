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

#include "visitor.h"
#include "instruction.h"
#include "directive.h"

using namespace ast;

class Code_generator: public Visitor {

	//	Statement

public:
	void visit(Statement_empty *) {}

	//----------------------------------------------------------------------
	//	Instruction

	void visit(Inst_load_relative *s);
	void visit(Inst_load_store_indirect *s);
	void visit(Inst_branch
 *s);
	void visit(Inst_shift *s);
	void visit(Inst_rrx *s);
	void visit(Inst_arith
 *s);
	void visit(Inst_logic *s);
	void visit(Inst_not *s);
	void visit(Inst_mov *s);
	void visit(Inst_movs *s);
	void visit(Inst_compare *s);
	void visit(Inst_msr *s);
	void visit(Inst_mrs *s);
	void visit(Inst_push_pop *s);

	//----------------------------------------------------------------------
	//	Directive

	void visit(Dir_section *) {}
	void visit(Dir_ascii *) {}
	void visit(Dir_equ *);
	void visit(Dir_space *);
	void visit(Dir_align *);
	void visit(Dir_byte *);

	//----------------------------------------------------------------------
	//	Expression

	void visit(Value *) {}
	void visit(Identifier *) {}
	void visit(Priority *) {}
	void visit(Unary_expression *) {}
	void visit(Binary_expression *) {}
	void visit(Conditional_expression *) {}
};

