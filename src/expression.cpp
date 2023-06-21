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

#include <assert.h>
#include "p16.h"
#include "p16_parser.hpp"
#include "utils.h"

namespace ast {

	using namespace std;

	const char *operation_to_string(int operation) {
		switch (operation) {
			case ASTERISK:
				return "*";
			case SLASH:
				return "/";
			case PERCENT:
				return "%";
			case PLUS:
				return "+";
			case MINUS:
				return "-";
			case AMPERSAND:
				return "&";
			case CIRCUMFLEX:
				return "^";
			case PIPE:
				return "|";
			case AMPERSAND_AMPERSAND:
				return "&&";
			case PIPE_PIPE:
				return "||";
			case SHIFT_LEFT:
				return "<<";
			case SHIFT_RIGHT:
				return ">>";
			case LESSER:
				return "<";
			case GREATER:
				return ">";
			case LESSER_EQUAL:
				return "<=";
			case GREATER_EQUAL:
				return ">=";
			case EQUAL_EQUAL:
				return "==";
			case NOT_EQUAL:
				return "!=";
			case EQUAL:
				return "=";
			case TILDE:
				return "~";
			case EXCLAMATION:
				return "!";
			default:
				return "???";
		}
	}

	bool Value::evaluate() {
		if ((abs(value ) & ~MAKE_MASK(16, 0)) != 0) {
			warning_report(&location,	string_printf("value = %d (0x%x) not encodable in a %d bit word",
					value, value, 16));
			return false;
		}
		return true;
	}

	bool Identifier::evaluate() {
		auto symbol_type = symbol->get_type();
		if (symbol_type == UNDEFINED) {
			error_report(&location, "Undefined symbol");
			type = INVALID;
			return false;
		}
		type = symbol_type;
		if (symbol_type == INVALID)
			return false;
		value = symbol->get_value();
		return true;
	}

	//------------------------------------------------------------------------------
	//	Unary_expression

	bool Unary_expression::evaluate() {
		if (!expression->evaluate()) {
			type = expression->get_type();
			return false;
		}
		type = expression->get_type();
		if (type == LABEL) {
			error_report(&expression->location,
				string_printf("A label can't be operated by \"%s\" unary operator",
					operation_to_string(operation)));
			type = INVALID;
			return false;
		}
		assert(type == ABSOLUTE);
		switch (operation) {
			case PLUS:
				value = expression->get_value();
				break;
			case MINUS:
				value = -expression->get_value();
				break;
			case TILDE:
				value = ~expression->get_value();
				break;
			case EXCLAMATION:
				value = !expression->get_value();
		}
		if ((abs(value) & ~MAKE_MASK(16, 0)) != 0) {
			warning_report(&location,	string_printf("value = %d (0x%x) not encodable in a %d bit word",
					value, value, 16));
			return false;
		}
		return true;
	}

	string Unary_expression::to_string() {
		switch (this->operation) {
			case PLUS:
				return "+" + expression->to_string();
			case MINUS:
				return "-" + expression->to_string();
			case TILDE:
				return "~" + expression->to_string();
			case EXCLAMATION:
				return "!" + expression->to_string();
			default:
				return string();
		}
	}

//------------------------------------------------------------------------------
//	Binary_expression

	string Binary_expression::get_symbol() {
		switch (operation) {
			case PLUS:
			case MINUS: {
				Value_type left_type = expression_left->get_type();
				Value_type right_type = expression_right->get_type();
				return left_type == LABEL && right_type == ABSOLUTE
						? expression_left->get_symbol()
						: left_type == ABSOLUTE && right_type == LABEL
							? expression_right->get_symbol()
							: string();
			}
			case SHIFT_LEFT:
			case SHIFT_RIGHT:
			case ASTERISK:
			case SLASH:
			case PERCENT:
			case AMPERSAND_AMPERSAND:
			case PIPE_PIPE:
			case PIPE:
			case AMPERSAND:
			case LESSER:
			case GREATER:
			case LESSER_EQUAL:
			case GREATER_EQUAL:
			case EQUAL_EQUAL:
			case NOT_EQUAL:
			case CIRCUMFLEX:
			default:
				return string();
		}
	}

	bool Binary_expression::evaluate() {
		if (!expression_left->evaluate()) {
			type = expression_left->get_type();
			return false;
		}
		auto left_value = expression_left->get_value();
		auto left_type = expression_left->get_type();
		assert(left_type == ABSOLUTE || left_type == LABEL);

		if (!expression_right->evaluate()) {
			type = expression_right->get_type();
			return false;
		}
		auto right_value = expression_right->get_value();
		auto right_type = expression_right->get_type();
		assert(right_type == ABSOLUTE || right_type == LABEL);

		if (operation == ASTERISK || operation == SLASH || operation == PERCENT
			|| operation == SHIFT_LEFT || operation == SHIFT_RIGHT
			|| operation == AMPERSAND || operation == CIRCUMFLEX || operation == PIPE
			|| operation == AMPERSAND_AMPERSAND || operation == PIPE_PIPE) {
			if (left_type == LABEL) {
				error_report(&expression_left->location,
					string_printf("A label can't be operated by \"%s\" operation",
						operation_to_string(operation)));
				type = INVALID;
				return false;
			}
			if (right_type == LABEL) {
				error_report(&expression_right->location,
					string_printf("A label can't be operated by \"%s\" operation",
						operation_to_string(operation)));
				type = INVALID;
				return false;
			}

			assert(left_type == ABSOLUTE && right_type == ABSOLUTE);

			switch (operation) {
				case ASTERISK:
					value = left_value * right_value;
					type = ABSOLUTE;
					break;
				case SLASH:
					if (right_value == 0) {
						error_report(&expression_right->location, "Division by zero");
						type = INVALID;
						return false;
					}
					value = left_value / right_value;
					type = ABSOLUTE;
					break;
				case PERCENT:
					if (right_value == 0) {
						error_report(&expression_right->location, "Division by zero");
						type = INVALID;
						return false;
					}
					value = left_value % right_value;
					type = ABSOLUTE;
					break;
				case SHIFT_LEFT:
					value = left_value << right_value;
					type = ABSOLUTE;
					break;
				case SHIFT_RIGHT:
					value = left_value >> right_value;
					type = ABSOLUTE;
					break;
				case AMPERSAND:
					value = left_value & right_value;
					type = ABSOLUTE;
					break;
				case CIRCUMFLEX:
					value = left_value ^ right_value;
					type = ABSOLUTE;
					break;
				case PIPE:
					value = left_value | right_value;
					type = ABSOLUTE;
					break;
				case AMPERSAND_AMPERSAND:
					value = left_value != 0 && 0 != right_value ? 1 : 0;
					type = ABSOLUTE;
					break;
				case PIPE_PIPE:
					value = left_value != 0 || 0 != right_value ? 1 : 0;
					type = ABSOLUTE;
					break;
			}
			if ((abs(value) & ~MAKE_MASK(16, 0)) != 0) {
				warning_report(&location,	string_printf(" result value = %d (0x%x) not encodable in a %d bit word",
					value, value, 16));
				return false;
			}
			return true;
		}
		if (operation == PLUS) {
			if (left_type == LABEL && right_type == LABEL) {
				error_report(&location, "Tow labels can't be added");
				type = INVALID;
				return false;
			}
			else {
				if ((left_type == LABEL || right_type == LABEL))
					type = LABEL;
				else
					type = ABSOLUTE;
				value = left_value + right_value;
				if ((abs(value) & ~MAKE_MASK(16, 0)) != 0) {
					error_report(&location,	string_printf(" result value = %d (0x%x) not encodable in a %d bit word",
						value, value, 16));
					return false;
				}
				return true;
			}
		}
		if (operation == MINUS) {
			if ((left_type == LABEL && right_type == LABEL)
				&& (Symbols::get_section(expression_left->get_symbol())
					!= Symbols::get_section(expression_right->get_symbol()))) {
				error_report(&location, "Two labels to be operated, must belong to the same section");
				type = INVALID;
				return false;
			}
			if ((left_type == LABEL && right_type == ABSOLUTE)
				|| (left_type == ABSOLUTE && right_type == LABEL))
				type = LABEL;
			else
				type = ABSOLUTE;
			value = left_value - right_value;
			if ((abs(value) & ~MAKE_MASK(16, 0)) != 0) {
				warning_report(&location,	string_printf(" result value = %d (0x%x) not encodable in a %d bit word",
					value, value, 16));
				return false;
			}
			return true;
		}
		if (operation == EQUAL_EQUAL || operation == NOT_EQUAL
			|| operation == GREATER_EQUAL || operation == GREATER
			|| operation == LESSER_EQUAL || operation == LESSER) {
			if ((left_type == LABEL && right_type == LABEL)
				&& (Symbols::get_section(expression_left->get_symbol())
				!= Symbols::get_section(expression_right->get_symbol()))) {
				error_report(&location, "Two labels to be operated, must belong to the same section");
				type = INVALID;
				return false;
			}
			if ((left_type == LABEL && right_type == ABSOLUTE)
				|| (left_type == ABSOLUTE && right_type == LABEL)) {
				error_report(&location,
					string_printf("A label and an absolute can't be operated by \"%s\" operation",
						operation_to_string(operation)));
				type = INVALID;
				return false;
			}
			switch(operation) {
				case LESSER:
					value = left_value - right_value < 0;
					type = ABSOLUTE;
					return true;
				case GREATER:
					value = left_value - right_value > 0;
					type = ABSOLUTE;
					return true;
				case LESSER_EQUAL:
					value = left_value - right_value <= 0;
					type = ABSOLUTE;
					return true;
				case GREATER_EQUAL:
					value = left_value - right_value >= 0;
					type = ABSOLUTE;
					return true;
				case EQUAL_EQUAL:
					value = left_value - right_value == 0;
					type = ABSOLUTE;
					return true;
				case NOT_EQUAL:
					value = left_value - right_value != 0;
					type = ABSOLUTE;
					return true;
			}
		}
		assert(false);
		return false;
	}

	string Binary_expression::to_string() {
		const char *sign;
		switch (operation) {
			case ASTERISK:
				sign = " * ";
				break;
			case SLASH:
				sign = " / ";
				break;
			case PERCENT:
				sign = " % ";
				break;
			case PLUS:
				sign = " + ";
				break;
			case MINUS:
				sign = " - ";
				break;
			case AMPERSAND_AMPERSAND:
				sign = " && ";
				break;
			case PIPE_PIPE:
				sign = " || ";
				break;
			case SHIFT_LEFT:
				sign = " << ";
				break;
			case SHIFT_RIGHT:
				sign = " >> ";
				break;
			case LESSER:
				sign = " < ";
				break;
			case GREATER:
				sign = " > ";
				break;
			case LESSER_EQUAL:
				sign = " <= ";
				break;
			case GREATER_EQUAL:
				sign = " >= ";
				break;
			case EQUAL_EQUAL:
				sign = " == ";
				break;
			case NOT_EQUAL:
				sign = " != ";
				break;
			default:
				sign = " ??? ";
		}
		return expression_left->to_string() + sign + expression_right->to_string();
	}

	//------------------------------------------------------------------------------
	//	Conditional expression

	bool Conditional_expression::evaluate() {
		if (!logical_expression->evaluate()) {
			type = INVALID;
			return false;
		}
		auto logical_type = logical_expression->get_type();
		if (logical_type != ABSOLUTE) {
			error_report(&logical_expression->location, "Most be an absolute expression");
			type = INVALID;
			return false;
		}
		if (logical_expression->get_value() != 0) {
			if (!true_expression->evaluate()) {
				type = INVALID;
				return false;
			}
			type = true_expression->get_type();
			value = true_expression->get_value();
			return true;
		}
		else {
			if (!false_expression->evaluate()) {
				type = INVALID;
				return false;
			}
			type = false_expression->get_type();
			value = false_expression->get_value();
			return true;
		}
	}


}   /* ast */
