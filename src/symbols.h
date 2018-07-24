#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <unordered_map>
#include "expression_base.h"
#include "sections.h"
#include "cpp_printf.h"

namespace ast {

    struct Symbol {
        //  Tipo do valor associado ao simbolo:
        //      - ABSOLUTE - valores absolutos calculáveis imediatamente
        //      - LABEL - label cujo valor é relativo à secção a que pertence
        //      - UNDEFINED - simbolo apenas referido;
        //          pode ser uma expressão da forma (label + ADDEND) em que ADDEND é uma constante
        //      - INVALID - expressão que envolve operações invalidas sobre labels;
        //          (label * CONST) (label + label) ...

        std::string name;
        Value_type type;
        unsigned section;
        Expression *value_expression;
        unsigned value;

        Symbol(std::string name) : name{name} { }
        Symbol(std::string name, Value_type type, unsigned section, Expression *value) :
                name{name}, type{type}, section{section}, value_expression{value} { }

        unsigned get_value();

        Value_type get_type();
    };

    class Symbols {
        static std::unordered_map<std::string, Symbol *> table;
    public:
        static void deallocate();

        static bool do_exist(std::string name) {
            return table.find(name) != table.end();
        }

        static void add(std::string name, Value_type type, unsigned section, Expression *value);

        static int add(std::string name, unsigned section, Expression *value);

        static void set_properties(std::string name, Value_type type, unsigned section, Expression *value);

        static unsigned get_section(std::string name) {
            auto pair_symbol = table.find(name);
            if (pair_symbol != table.end())
                return pair_symbol->second->section;
            return Sections::table.size();  //  Secção inexistente
        }

        static unsigned get_value(std::string name) {
            auto pair_symbol = table.find(name);
            if (pair_symbol != table.end())
                return pair_symbol->second->get_value();
            return 0;
        }

        static Value_type get_type(std::string name) {
            auto pair_symbol = table.find(name);
            if (pair_symbol != table.end())
                return pair_symbol->second->get_type();
            return Value_type::UNDEFINED;
        }

        static void print(std::ostream &os);

        static void listing(std::ostream &lst_file);
    };
}

#endif
