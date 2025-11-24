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

#ifndef EXPRESSION_BASE_H
#define EXPRESSION_BASE_H

#include <stdint.h>

#include "value_type.h"
#include "location.h"
#include "visitor.h"

namespace ast {

	struct Expression {
		Location location;
		int value;		// Valor associado à expressão
		Value_type type;	// Tipo associado à expressão
		// Estes campos apenas são atualizados no método evaluate
	
		Expression(Location location, int value, Value_type type)
			: location{location} , value{value}, type{type} {}

		Expression(Location location) : location{location} {}

		virtual ~Expression() {};

		virtual std::string to_string() = 0;

		uint16_t get_value() { return value; }
		Value_type get_type() { return type; }

		virtual bool evaluate() = 0;
		virtual std::string get_symbol() = 0;
		virtual void accept(Visitor *v) = 0;
	};
}

#endif
