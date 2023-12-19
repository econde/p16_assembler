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

#ifndef	DIRECTIVE_H
#define DIRECTIVE_H

#include "visitor.h"
#include "expression.h"
#include "statement.h"
#include "cpp_printf.h"

namespace ast {

using namespace std;

struct Directive: public Statement {

	Directive(Location l) : Statement {l} {}
	virtual void accept(Visitor *v) = 0;

	virtual string listing();
	virtual string more_listing();
};

//------------------------------------------------------------------------------
//	.section

struct Dir_section: public Directive {
	string name;
	Dir_section(string name, Location left);

	string listing() {
		return string_printf("%4d%11c\t", location.line, ' ');
	}

	void accept(Visitor *v) { v->visit(this); }
};

//------------------------------------------------------------------------------
//	.byte .word

struct Dir_byte: public Directive {
	list<Expression*> *value_list;
	unsigned grain_size;
	Dir_byte(unsigned s, list<Expression*> *vl, Location left);
	~Dir_byte();
	string listing();
	string more_listing();
	void accept(Visitor *v) { v->visit(this); }
};

//------------------------------------------------------------------------------
//	.ascii

struct Dir_ascii: public Directive {
	list<string> *string_list;
	Dir_ascii(unsigned asciz, list<string> *sl, Location left);
	~Dir_ascii() { delete string_list; }

	string listing();

	string more_listing();

	void accept(Visitor *v) { v->visit(this); }
};

//------------------------------------------------------------------------------
//	.space

struct Dir_space: public Directive {
	Expression *size, *initial;
	Dir_space (Expression *s, Expression *i, Location left);
	~Dir_space() { delete size; delete initial; }
	string more_listing();
	void accept(Visitor *v) { v->visit(this); }
};

//------------------------------------------------------------------------------
//	.equ

struct Dir_equ: public Directive {
	Symbol *symbol;
	Dir_equ(Symbol *symbol, Expression *value, Location left);
	string listing() {
		return string_printf("%4d%10c\t", location.line, ' ');
	}
	void accept(Visitor *v) { v->visit(this); }
};

//------------------------------------------------------------------------------
//	.align

struct Dir_align: public Directive {
	Expression *size;

	Dir_align (Expression *size, Location left);
	~Dir_align() { delete size; }

	string listing();

	void accept(Visitor *v) { v->visit(this); }
};

}

#endif
