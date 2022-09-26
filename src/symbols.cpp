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

#include <algorithm>
#include "symbols.h"

namespace ast {

	using namespace std;

	std::unordered_map<std::string, Symbol *> Symbols::table;

	void Symbols::deallocate() {
		for (auto s: table) {
			delete s.second;
		}
	}
	void Symbols::print(std::ostream &os) {
		for (auto pair_symbol: table) {
			Symbol *symbol = pair_symbol.second;
			uint16_t value = symbol->get_value() + Sections::get_address(symbol->section);
			ostream_printf(os, "%-30s %-10s %5d %04x %d\n",
							symbol->name.c_str(), value_type_name[symbol->type],
							value, value, symbol->section);
		}
	}

	static bool case_insensitive_compare(Symbol *a, Symbol *b) {
		auto size = a->name.size();
		for (auto i = 0U; i < size; ++i) {
			auto char_a = tolower(a->name[i]);
			auto char_b = tolower(b->name[i]);
			if (char_a != char_b)
				return char_a < char_b;
		}
		return a->name.size() < b->name.size();
	}

	void Symbols::listing(std::ostream &lst_file) {
		ostream_printf(lst_file, "\nSymbols\n");
		ostream_printf(lst_file, "%-24s%-10s%-12s%s\n", "Name", "Type", "Value", "Section");
		vector<Symbol*>table_sort;
		for (auto pair_symbol: table)
			table_sort.push_back(pair_symbol.second);
		sort(table_sort.begin(), table_sort.end(), case_insensitive_compare);
		for (auto symbol: table_sort) {
			uint16_t value = symbol->get_value() + Sections::get_address(symbol->section);
			ostream_printf(lst_file, "%-23s %-10s%04X %-6d %s\n",
							symbol->name.c_str(), value_type_name[symbol->type],
							value, value,
							(symbol->type == Value_type::UNDEFINED)
								? "*UNDEF*" : Sections::table.at(symbol->section)->name.c_str());
		}
		ostream_printf(lst_file, "\n");
	}

	/* O valor de uma label é o offset em relação à secção a que pertence.
	 * O valor absoluto pode ser calculado, no respetivo contexto,
	 * adicionando o endereço de início da secção.
	 */
	unsigned Symbol::get_value() {
		if (type == ABSOLUTE || type == LABEL)
			return value_expression->get_value();
		return 0;
	}

	Value_type Symbol::get_type() {
		if (type == UNDEFINED && value_expression != nullptr) {
			value_expression->evaluate(); 	//	Valor ainda não avaliado
			type = value_expression->get_type();
		}
		return type;
	}
}
