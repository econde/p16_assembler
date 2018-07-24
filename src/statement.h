#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>
#include "location.h"
#include "visitor.h"
#include "symbols.h"
#include "error.h"
#include "expression.h"

namespace ast {

using namespace std;

struct Statement {

	Statement(Location l) :
		location {l},
		section_index {0},
		section_offset {0},
		size_in_memory {0} { }

	virtual ~Statement( ) { }
	
	string label;
	
	//	Localização no ficheiro fonte
	Location location;

	//	Localização em memória
	unsigned section_index;
    unsigned section_offset;
    unsigned size_in_memory;
	
	void add_label(string *l) {
		label = l->substr(0, l->find(':'));
		if ( ! Symbols::do_exist(label))
			Symbols::add(label, Value_type::LABEL, section_index, new Value(section_offset, location));
		else if (Symbols::get_type(label) == Value_type::UNDEFINED)
			Symbols::set_properties(label, Value_type::LABEL, section_index, new Value(section_offset, location));
		else
			error_report(&location, "Symbol \'" + label + "\' is already defined");
	}
	
	virtual void accept(Visitor *v) = 0;
	
	virtual string to_string() {
		return label.empty() ? string("\t") : label + ':';
	}
	virtual string listing() = 0;
	virtual string more_listing() {	return string(); }
};

class Statement_empty: public Statement {
public:
	Statement_empty(string *l, Location left):
		Statement {left} {
		section_index = Sections::current_section;
		section_offset = Sections::current_offset;
		size_in_memory = 0;
		add_label(l);
	 }
	 Statement_empty(Location left):
		Statement {left} { }
		
	 string to_string() {
		 return label.empty() ? label : (label + ':');
	 }

	 string listing() {
		 return string_printf("%4d%14c\t", location.line, ' ');
	 }
	 
	 void accept(Visitor *v) {
		 v->visit(this);
	 }
};

}

#endif
