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
protected:
	int value;
public:
	Value(Location location): Expression {location}, value {0} { }
	Value(int v, Location location) : Expression {location}, value {v} { }
	~Value() { }

	unsigned get_value() { return value; }
	string get_symbol() { return string(); }
	Value_type get_type() { return Value_type::ABSOLUTE; }

	string to_string() { return string_printf("%d", value); }
	void accept(Visitor *v) { v->visit(this); }
};

	//-----------------------------------------------------------------------------------------------------

class Identifier: public Value {
	string name;
public:
	Identifier(string name, Location location) :
		Value {location}, name {name} {
		if (name.compare(0, 5, "line#") == 0)	//	Simbolo .	 (ponto)
			Symbols::add(name, Value_type::LABEL, Sections::current_section->number,
                         new Value(Sections::current_section->content_size, location));
		else if ( ! Symbols::do_exist(name))
			Symbols::add(name, Value_type::UNDEFINED, 0, 0);
	}

	unsigned get_value() { return Symbols::get_value(name); }
    Value_type get_type() { return Symbols::get_type(name); }
	string get_symbol() { return name; }
	
	void set_value(int v) { value = v; }

    string to_string() { return name; }
	void accept(Visitor *v) { v->visit(this); }
};

	//-----------------------------------------------------------------------------------------------------

class Priority: public Expression {
	Expression *expression;
public:
	Priority(Expression *e, Location location): Expression {location}, expression {e} { }

	virtual ~Priority() { delete expression; }

	unsigned get_value() { return expression->get_value(); }
	string get_symbol() { return expression->get_symbol(); }
    Value_type get_type() { return expression->get_type(); }

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
	unsigned get_value();
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
	unsigned get_value();
	Value_type get_type();

	string to_string();
	void accept(Visitor *v) { v->visit(this); }
};

class Conditional_expression: public Expression {
	Expression *logical_expression, *expression, *conditional_expression;
public:
	Conditional_expression(Expression *le, Expression *e, Expression *ce, Location location) :
			Expression {location}, logical_expression {le}, expression{e}, conditional_expression{ce} { }

	virtual ~Conditional_expression() {
		 delete logical_expression;
		 delete expression;
		 delete conditional_expression;
	}

	unsigned get_value() {
		return logical_expression->get_value() != 0
               ? expression->get_value() : conditional_expression->get_value();
	}

	string get_symbol() {	//	É preciso avaliar se se pode faze get_value antes da localização.
		return logical_expression->get_value() != 0
               ? expression->get_symbol() : conditional_expression->get_symbol();
	}

 	Value_type get_type();

	string to_string() {
		return logical_expression->to_string() + " ? " +
				expression->to_string() + " : " + conditional_expression->to_string();
	}

	void accept(Visitor *v) { v->visit(this); }
};

}

#endif
