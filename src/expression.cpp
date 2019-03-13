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

#include "p16.h"
#include "p16.tab.hpp"

namespace ast {

    using namespace std;

    //------------------------------------------------------------------------------
    //	Unary_expression

    unsigned Unary_expression::get_value() {
        switch (operation) {
            case PLUS:
                return expression->get_value();
            case MINUS:
                return static_cast<unsigned>(-expression->get_value());
            case TILDE:
                return ~expression->get_value();
            default:
                return 0;
        }
    }

    Value_type Unary_expression::get_type() {
        Value_type exp_type = expression->get_type();
        if (operation == PLUS || exp_type == Value_type::ABSOLUTE)
            return Value_type::ABSOLUTE;
        else
            return Value_type::INVALID;
    }

    string Unary_expression::to_string() {
        switch (this->operation) {
            case PLUS:
                return "+" + expression->to_string();
            case MINUS:
                return "-" + expression->to_string();
            case TILDE:
                return "~" + expression->to_string();
            default:
                return string();
        }
    }

//------------------------------------------------------------------------------
//	Binary_expression

    string Binary_expression::get_symbol() {
        switch (operation) {
            case ASTERISK:
            case SLASH:
            case PERCENT:
            case AMPERSAND_AMPERSAND:
            case PIPE_PIPE:
                return string();

            case PIPE:        //	?	Não se pode fazer na relocação
            case AMPERSAND:    //	?
            case PLUS:
            case MINUS: {
                Value_type left_type = expression_left->get_type();
                Value_type right_type = expression_right->get_type();
                return !left_type == ABSOLUTE && right_type == ABSOLUTE
                       ? expression_left->get_symbol() :
                       left_type == ABSOLUTE && !right_type == ABSOLUTE
                       ? expression_right->get_symbol() : string();
            }
            case SHIFT_LEFT:
            case SHIFT_RIGHT: {
                Value_type left_type = expression_left->get_type();
                Value_type right_type = expression_right->get_type();
                return !left_type == ABSOLUTE && right_type == ABSOLUTE
                       ? expression_left->get_symbol() : string();
            }
            case LESSER:
            case GREATER:
            case LESSER_EQUAL:
            case GREATER_EQUAL:
            case EQUAL_EQUAL:
            case NOT_EQUAL:
            case CIRCUMFLEX:
                return string();
            default:
                return string();
        }
    }

    unsigned Binary_expression::get_value() {
        switch (operation) {
            case ASTERISK:
                return expression_left->get_value() * expression_right->get_value();
            case SLASH:
                return expression_left->get_value() / expression_right->get_value();
            case PERCENT:
                return expression_left->get_value() % expression_right->get_value();
            case PLUS:
                return expression_left->get_value() + expression_right->get_value();
            case MINUS:
                return expression_left->get_value() - expression_right->get_value();
            case SHIFT_LEFT:
                return expression_left->get_value() << expression_right->get_value();
            case SHIFT_RIGHT:
                return expression_left->get_value() >> expression_right->get_value();
            case LESSER:
                return expression_left->get_value() - expression_right->get_value() < 0 ? 1 : 0;
            case GREATER:
                return expression_left->get_value() - expression_right->get_value() > 0 ? 1 : 0;
            case LESSER_EQUAL:
                return expression_left->get_value() - expression_right->get_value() <= 0 ? 1 : 0;
            case GREATER_EQUAL:
                return expression_left->get_value() - expression_right->get_value() >= 0 ? 1 : 0;
            case EQUAL_EQUAL:
                return expression_left->get_value() - expression_right->get_value() == 0 ? 1 : 0;
            case NOT_EQUAL:
                return expression_left->get_value() - expression_right->get_value() != 0 ? 1 : 0;
            case AMPERSAND:
                return expression_left->get_value() & expression_right->get_value();
            case CIRCUMFLEX:
                return expression_left->get_value() ^ expression_right->get_value();
            case PIPE:
                return expression_left->get_value() | expression_right->get_value();
            case AMPERSAND_AMPERSAND:
                return expression_left->get_value() != 0 && 0 != expression_right->get_value() ? 1 : 0;
            case PIPE_PIPE:
                return expression_left->get_value() != 0 || 0 != expression_right->get_value() ? 1 : 0;
            default:
                return 0;
        }
    }

    Value_type Binary_expression::get_type() {
        auto left_type = expression_left->get_type();
        auto right_type = expression_right->get_type();

        if (left_type == Value_type::ABSOLUTE && right_type == ABSOLUTE)
            return Value_type::ABSOLUTE;
        else if ((left_type == Value_type::ABSOLUTE && right_type == LABEL)
                 || (left_type == LABEL && right_type == Value_type::ABSOLUTE)) {
            if (operation == PLUS || operation == MINUS)
                return Value_type::LABEL;
            else if (operation == EQUAL_EQUAL || operation == NOT_EQUAL
                     || operation == GREATER_EQUAL || operation == GREATER
                     || operation == LESSER_EQUAL || operation == LESSER)
                return ABSOLUTE;
            else
                return Value_type::INVALID;
        } else if ((left_type == Value_type::ABSOLUTE && right_type == UNDEFINED)
                   || (left_type == UNDEFINED && right_type == Value_type::ABSOLUTE)) {
            return Value_type::UNDEFINED;
        } else if (left_type == LABEL && right_type == LABEL) {
            /*  labels só podem ser operadas se pertencerem à mesma secção */
            if (Symbols::get_section(expression_left->get_symbol())
                == Symbols::get_section(expression_right->get_symbol())
                && (operation == MINUS
                    || operation == EQUAL_EQUAL || operation == NOT_EQUAL
                    || operation == GREATER_EQUAL || operation == GREATER
                    || operation == LESSER_EQUAL || operation == LESSER))
                return ABSOLUTE;
            else
                return Value_type::INVALID;
        } else if (left_type == UNDEFINED && right_type == UNDEFINED)
            return Value_type::UNDEFINED;
        else
            return Value_type::INVALID;
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

    Value_type Conditional_expression::get_type() {
        if (logical_expression->get_type() == ABSOLUTE)
            return logical_expression->get_value() != 0 ? expression->get_type() : conditional_expression->get_type();
        else
            return Value_type::INVALID;
    }

}   /* ast */
