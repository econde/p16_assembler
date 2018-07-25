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

class Load_store_direct;
class Load_store_indirect;
class Branch;
class Arith;
class Compare;
class Logic;
class Not;
class Shift;
class Rrx;
class Move;
class Moves;
class Msr;
class Mrs;
class Push_pop;

class DSection;
class Byte;
class Ascii;
class Space;
class Equ;
class Align;

}

class Visitor {
public:
	virtual ~Visitor() { }
	virtual void visit(ast::Statement_empty *) = 0;

	virtual void visit(ast::Load_store_direct *) = 0;
	virtual void visit(ast::Load_store_indirect *) = 0;
	virtual void visit(ast::Branch *) = 0;
	virtual void visit(ast::Arith *) = 0;
	virtual void visit(ast::Compare *) = 0;
	virtual void visit(ast::Logic *) = 0;
	virtual void visit(ast::Not *) = 0;
	virtual void visit(ast::Shift *) = 0;
	virtual void visit(ast::Rrx *) = 0;
	virtual void visit(ast::Move *) = 0;
	virtual void visit(ast::Moves *) = 0;
	virtual void visit(ast::Msr *) = 0;
	virtual void visit(ast::Mrs *) = 0;
	virtual void visit(ast::Push_pop *) = 0;

	virtual void visit(ast::DSection *) = 0;
	virtual void visit(ast::Byte *) = 0;
	virtual void visit(ast::Ascii *) = 0;
	virtual void visit(ast::Space *) = 0;
	virtual void visit(ast::Equ *) = 0;
	virtual void visit(ast::Align *) = 0;

	virtual void visit(ast::Value *) = 0;
	virtual void visit(ast::Identifier *) = 0;
	virtual void visit(ast::Priority *) = 0;
	virtual void visit(ast::Unary_expression *) = 0;
	virtual void visit(ast::Binary_expression *) = 0;
	virtual void visit(ast::Conditional_expression *) = 0;

};

#endif

