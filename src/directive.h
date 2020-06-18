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

	Directive(Location l) : Statement {l} { }
	virtual void accept(Visitor *v) = 0;

	virtual string listing();
	virtual string more_listing();
};

//------------------------------------------------------------------------------
//	.section

struct DSection: public Directive {
	string name;
	DSection(string name, Location left) :
		Directive {left}, name {name} {
		Sections::set_section(name);
		section_index = Sections::current_section->number;
		section_offset = Sections::current_section->content_size;
		size_in_memory = 0;
	}

	string listing() {
		return string_printf("%4d%11c\t", location.line, ' ');
	}

	void accept(Visitor *v) { v->visit(this); }
};

//------------------------------------------------------------------------------
//	.byte .hword .word

struct Byte: public Directive {
	list<Expression*> *value_list;
	unsigned grain_size;

	Byte(unsigned s, list<Expression*> *vl, Location left) :
		Directive {left}, value_list {vl}, grain_size {s} {
		section_index = Sections::current_section->number;
		section_offset = Sections::current_section->content_size;
		size_in_memory = grain_size * value_list->size();
		Sections::increase(section_index, size_in_memory);
	}
	~Byte() {
		for (auto s: *value_list)
			delete s;
		delete value_list;
	}

	string listing();

	string more_listing();

	void accept(Visitor *v) { v->visit(this); }
};

//------------------------------------------------------------------------------
//	.ascii

struct Ascii: public Directive {
	list<string> *string_list;
	Ascii(unsigned asciz, list<string> *sl, Location left) :
		Directive {left}, string_list {sl} {
		section_index = Sections::current_section->number;
		section_offset = Sections::current_section->content_size;
		for (auto s: *string_list) {
			Sections::append_block(Sections::current_section->number, (const uint8_t *)s.data(), s.size());
		}
		if (asciz) {
			Sections::append8(Sections::current_section->number, 0);
		}
		size_in_memory = Sections::current_section->content_size - section_offset;
	}
	~Ascii() { delete string_list; }

	string listing();

	string more_listing();

	void accept(Visitor *v) { v->visit(this); }
};

//------------------------------------------------------------------------------
//	.space

struct Space: public Directive {
	Expression *size, *initial;
	Space (Expression *s, Expression *i, Location left) :
		Directive {left}, size {s}, initial {i} {
		section_index = Sections::current_section->number;
		section_offset = Sections::current_section->content_size;
		size_in_memory = size->get_value();
		Sections::increase(section_index, size_in_memory);
	}
	~Space() { delete size; delete initial; }

	string more_listing();

	void accept(Visitor *v) { v->visit(this); }
};

//------------------------------------------------------------------------------
//	.equ

struct Equ: public Directive {
	string name;
	Expression *value;

	Equ(string name, Expression *value, Location left) :
		Directive {left}, name {name}, value {value} {
		section_index = Sections::current_section->number;		/* Um simbolo equ não pertence a uma secção ... */
		section_offset = Sections::current_section->content_size;
		size_in_memory = 0;
		if (Symbols::add(name, Sections::current_section->number, value) == 0) {
			error_report(&location, "Symbol \"" + name + "\" is already defined");
		}
	}
	~Equ() { delete value; }

	string listing() {
		return string_printf("%4d%10c\t", location.line, ' ');
	}

	void accept(Visitor *v) { v->visit(this); }
};

//------------------------------------------------------------------------------
//	.align

struct Align: public Directive {
	Expression *size;

	Align (Expression *size, Location left) :
		Directive {left}, size {size} {
		section_index = Sections::current_section->number;
		section_offset = Sections::current_section->content_size;
		size_in_memory = Sections::align(section_offset, size->get_value()) - section_offset;
		Sections::increase(section_index, size_in_memory);
	}
	~Align() { delete size; }

	string listing();

	void accept(Visitor *v) { v->visit(this); }
};

}

#endif
