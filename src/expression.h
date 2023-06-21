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

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>
#include <iostream>
#include "expression_base.h"
#include "location.h"
#include "visitor.h"
#include "symbols.h"
#include "cpp_printf.h"

namespace ast {

using namespace std;

class Value: public Expression {

public:

	Value(int value, Location location)
		: Expression {location, value, Value_type::ABSOLUTE} { }
	~Value() { }

	bool evaluate();

	string get_symbol() { return string(); }	//	string vazia

	string to_string() { return string_printf("%d", value); }
	void accept(Visitor *v) { v->visit(this); }
};

	//-----------------------------------------------------------------------------------------------------

class Identifier: public Expression {
	Symbol *symbol;
public:
	Identifier(string name, Location location) :
		Expression {location} {
		if (name.compare(0, 5, "line#") == 0)	{ //	Simbolo .	 (ponto)
			symbol = new Symbol(location, name, Value_type::LABEL, Sections::current_section->number,
						 new Value(Sections::current_section->content_size, location));
			Symbols::add(symbol);
		}
		else {
			symbol = Symbols::search(name);
			if (symbol == nullptr)
			symbol = new Symbol(location, name, Value_type::UNDEFINED, 0, nullptr);
			Symbols::add(symbol);
		}
	}

	bool evaluate();
	Value_type get_type() { return symbol->get_type(); }
	string get_symbol() { return symbol->name; }

//	void set_value(int v) { value = v; }

	string to_string() { return symbol->name; }
	void accept(Visitor *v) { v->visit(this); }
};

	//-----------------------------------------------------------------------------------------------------

class Priority: public Expression {
	Expression *expression;
public:
	Priority(Expression *e, Location location): Expression {location}, expression {e} { }

	virtual ~Priority() { delete expression; }

	bool evaluate() {
		if (expression->evaluate()) {
			type = expression->get_type();
			value = expression->get_value();
			return true;
		}
		else {
			type = expression->get_type();
			return false;
		}
	}
	string get_symbol() { return expression->get_symbol(); }

	string to_string() { return "(" + expression->to_string() + ")"; }
	void accept(Visitor *v) { v->visit(this); }
};

	//-----------------------------------------------------------------------------------------------------

class Unary_expression: public Expression {
	Expression *expression;
	int operation;
public:
	Unary_expression(int op, Expression *e, Location location):
			Expression {location}, expression {e}, operation {op} { }

	~Unary_expression() { delete expression; }

	string get_symbol() { return expression->get_symbol(); }
	bool evaluate();
	Value_type get_type();

	string to_string();
	void accept(Visitor *v) { v->visit(this); }
};

	//-----------------------------------------------------------------------------------------------------

class Binary_expression: public Expression {
	Expression *expression_left, *expression_right;
	int operation;
public:
	Binary_expression(Expression *el, int op, Expression *er, Location location) :
			Expression {location}, expression_left {el}, expression_right(er), operation(op) { }

	virtual ~Binary_expression() {
		 delete expression_left;
		 delete expression_right;
	}

	string get_symbol();
	bool evaluate();
	Value_type get_type();

	string to_string();
	void accept(Visitor *v) { v->visit(this); }
};

class Conditional_expression: public Expression {
	Expression *logical_expression, *true_expression, *false_expression;
public:
	Conditional_expression(Expression *le, Expression *e, Expression *ce, Location location) :
			Expression {location}, logical_expression {le}, true_expression{e}, false_expression{ce} { }

	virtual ~Conditional_expression() {
		 delete logical_expression;
		 delete true_expression;
		 delete false_expression;
	}

	string get_symbol() {	//	get_type() só pode ser invocado depois de evaluate().
		return get_type() == LABEL && logical_expression->get_value()
					? true_expression->get_symbol()
					: false_expression->get_symbol();
	}
	bool evaluate();

	string to_string() {
		return logical_expression->to_string()
			+ " ? " + true_expression->to_string()
			+ " : " + false_expression->to_string();
	}

	void accept(Visitor *v) { v->visit(this); }
};

}

#endif
