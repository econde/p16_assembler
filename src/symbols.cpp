#include "symbols.h"

namespace ast {

    using namespace std;

    std::unordered_map<std::string, Symbol *> Symbols::table;

    void Symbols::deallocate() {
        for (auto s: table) {
            delete s.second;
        }
    }

    int Symbols::add(std::string name, unsigned section, Expression *value) {
        auto pair_symbol = table.find(name);

        Symbol *symbol;
        if (pair_symbol != table.end()) {   //  O símbolo já existe
            if (pair_symbol->second->get_type() == Value_type::UNDEFINED)
                symbol = pair_symbol->second;
            else
                return 0;
        } else
            symbol = new Symbol(name);
        symbol->value_expression = value;
        symbol->type = value->get_type();
        symbol->value = value->get_value();
        table[name] = symbol;
        return 1;
    }

    void Symbols::add(std::string name, Value_type type, unsigned section, Expression *value) {
        Symbol *symbol =  new Symbol{name, type, section, value};
        symbol->value = (value != nullptr) ? value->get_value() : 0;
        table[name] = symbol;
    }

    void Symbols::set_properties(std::string name, Value_type type, unsigned section, Expression *value) {
        Symbol *symbol = table.at(name);
        symbol->section = section;
        symbol->type = type;
        symbol->value_expression = value;
        symbol->value = (value != nullptr) ? value->get_value() : 0;
    }


    void Symbols::print(std::ostream &os) {
        for (auto pair_symbol: table) {
            Symbol *symbol = pair_symbol.second;
            ostream_printf(os, "%-30s %-10s %5d %04x %d\n",
                           symbol->name.c_str(), value_type_name[symbol->type],
                           symbol->get_value(), symbol->get_value(), symbol->section);
        }
    }

    void Symbols::listing(std::ostream &lst_file) {
        ostream_printf(lst_file, "\nSímbolos\n");
        ostream_printf(lst_file, "%-24s%-10s%-11s%s\n", "Nome", "Tipo", "Valor", "Secção");
        for (auto pair_symbol: table) {
            Symbol *symbol = pair_symbol.second;
            ostream_printf(lst_file, "%-24s%-10s%04x %-6d%s\n",
                           symbol->name.c_str(), value_type_name[symbol->type],
                           symbol->get_value(), symbol->get_value(),
                           (symbol->type == Value_type::UNDEFINED) ? "*UNDEF*" : Sections::table.at(
                                   symbol->section)->name.c_str());
        }
        ostream_printf(lst_file, "\n");
    }

    unsigned Symbol::get_value() {
        if (type == UNDEFINED && value_expression != nullptr) {
            type = value_expression->get_type();
            value = value_expression->get_value();
        }
        if (type == LABEL)
            return value + Sections::get_address(section);
        return value;
    }

    Value_type Symbol::get_type() {
        if (type == UNDEFINED && value_expression != nullptr) {
            type = value_expression->get_type();
            value = value_expression->get_value();
        }
        return type;
    }
}
