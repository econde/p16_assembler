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

#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <unordered_map>
#include "expression_base.h"
#include "sections.h"
#include "cpp_printf.h"

namespace ast {

	struct Symbol {
		Location location;
		std::string name;	//	Nome do símbolo
		Value_type type;	//
		unsigned section;	//	Secção a que pertence
		Expression *value_expression;	//	Expressão do valor associado ao símbolo

		Symbol(Location location, std::string name) : location{location}, name{name} { }
		Symbol(Location location, std::string name, Value_type type, unsigned section, Expression *value) :
				location{location}, name{name}, type{type}, section{section}, value_expression{value} { }

		void set_properties(Value_type type, unsigned section, Expression *value) {
			this->section = section;
			this->type = type;
			this->value_expression = value;
		}

		int get_value();

		Value_type get_type();
	};

	class Symbols {
		static std::unordered_map<std::string, Symbol *> table;
	public:
		static void deallocate();

		static void add(Symbol *symbol)  { table[symbol->name] = symbol; }

		static unsigned get_section(std::string name) {
			auto pair_symbol = table.find(name);
			if (pair_symbol != table.end())
				return pair_symbol->second->section;
			return Sections::table.size();  //  Secção inexistente
		}

		static int get_value(std::string name) {
			auto pair_symbol = table.find(name);
			if (pair_symbol != table.end())
				return pair_symbol->second->get_value();
			return 0;
		}

		static Value_type get_type(std::string name) {
			auto pair_symbol = table.find(name);
			if (pair_symbol != table.end()) {
				auto symbol = pair_symbol->second;
				return symbol->get_type();
			}
			return Value_type::UNDEFINED;
		}

		static Symbol *search(std::string name) {
			auto pair_symbol = table.find(name);
			if (pair_symbol != table.end())
				return pair_symbol->second;
			return nullptr;
		}

		static void print(std::ostream &os);

		static void listing(std::ostream &lst_file);

		static void evaluate();
	};
}

#endif
