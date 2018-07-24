#ifndef EXPRESSION_BASE_H
#define EXPRESSION_BASE_H

#include "value_type.h"
#include "location.h"
#include "visitor.h"

namespace ast {

	struct Expression {

		Location location;

		Expression(Location location) : location{location} {}

		Expression(Expression *e, Location location) : location{location} {}

		virtual ~Expression() {};

		virtual std::string to_string() = 0;

		virtual unsigned get_value() = 0;
		virtual Value_type get_type() = 0;
		virtual std::string get_symbol() = 0;

		virtual void accept(Visitor *v) = 0;
	};
}

#endif
