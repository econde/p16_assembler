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

	//--------------------------------------------------------------------------
	//	Instruction

	void visit(Load_relative *s);
	void visit(Load_store_indirect *s);
	void visit(Branch *s);
	void visit(Shift *s);
	void visit(Rrx *s);
	void visit(Arith *s);
	void visit(Logic *s);
	void visit(Not *s);
	void visit(Move *s);
	void visit(Moves *s);
	void visit(Compare *s);
	void visit(Msr *s);
	void visit(Mrs *s);
	void visit(Push_pop *s);

	//--------------------------------------------------------------------------
	//	Directive

	void visit(DSection *) {}
	void visit(Ascii *) {}
	void visit(Equ *) {}
	void visit(Space *s);
	void visit(Align *s);
	void visit(Byte *s);

	//--------------------------------------------------------------------------
	//	Expression

	void visit(Value *) {}
	void visit(Identifier *) {}
	void visit(Priority *) {}
	void visit(Unary_expression *) {}
	void visit(Binary_expression *) {}
	void visit(Conditional_expression *) {}
};

