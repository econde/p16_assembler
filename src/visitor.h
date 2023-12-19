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

#ifndef VISITOR_H
#define VISITOR_H

namespace ast {

class Value;
class Identifier;
class Priority;
class Unary_expression;
class Binary_expression;
class Conditional_expression;

class Statement_empty;

class Inst_load_relative;
class Inst_load_store_indirect;
class Inst_branch;
class Inst_arith;
class Inst_compare;
class Inst_logic;
class Inst_not;
class Inst_shift;
class Inst_rrx;
class Inst_mov;
class Inst_movs;
class Inst_msr;
class Inst_mrs;
class Inst_push_pop;

class Dir_section;
class Dir_byte;
class Dir_ascii;
class Dir_space;
class Dir_equ;
class Dir_align;

}

class Visitor {
public:
	virtual ~Visitor() { }
	virtual void visit(ast::Statement_empty *) = 0;

	virtual void visit(ast::Inst_load_relative *) = 0;
	virtual void visit(ast::Inst_load_store_indirect *) = 0;
	virtual void visit(ast::Inst_branch
 *) = 0;
	virtual void visit(ast::Inst_arith
 *) = 0;
	virtual void visit(ast::Inst_compare *) = 0;
	virtual void visit(ast::Inst_logic *) = 0;
	virtual void visit(ast::Inst_not *) = 0;
	virtual void visit(ast::Inst_shift *) = 0;
	virtual void visit(ast::Inst_rrx *) = 0;
	virtual void visit(ast::Inst_mov *) = 0;
	virtual void visit(ast::Inst_movs *) = 0;
	virtual void visit(ast::Inst_msr *) = 0;
	virtual void visit(ast::Inst_mrs *) = 0;
	virtual void visit(ast::Inst_push_pop *) = 0;

	virtual void visit(ast::Dir_section *) = 0;
	virtual void visit(ast::Dir_byte *) = 0;
	virtual void visit(ast::Dir_ascii *) = 0;
	virtual void visit(ast::Dir_space *) = 0;
	virtual void visit(ast::Dir_equ *) = 0;
	virtual void visit(ast::Dir_align *) = 0;

	virtual void visit(ast::Value *) = 0;
	virtual void visit(ast::Identifier *) = 0;
	virtual void visit(ast::Priority *) = 0;
	virtual void visit(ast::Unary_expression *) = 0;
	virtual void visit(ast::Binary_expression *) = 0;
	virtual void visit(ast::Conditional_expression *) = 0;
};

#endif

