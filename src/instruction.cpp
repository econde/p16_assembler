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

#include "cpp_printf.h"

#include "p16.h"
#include "p16.tab.hpp"

namespace ast {

std::string register_name(int r) {
	switch (r) {
		case 0: case 1: case 2: case 3:	case 4: case 5: case 6: case 7:
		case 8: case 9: case 10: case 11: case 12:
			return string_printf("r%d", r);
		case 13:
			return std::string("sp");
		case 14:
			return std::string("lr");
		case 15:
			return std::string("pc");
	}
	return std::string("???");
}

 std::string register_special_name(int r) {
	switch (r) {
		case 14:
			return std::string("lr");
		case 15:
			return std::string("pc");
	}
	return std::string("???");
}


std::string arith_name(int operation) {
	switch (operation) {
		case ADD:
			return std::string("add");
		case ADC:
			return std::string("adc");
		case SUB:
			return std::string("sub");
		case SBC:
			return std::string("sbb");
	}
	return std::string("???");
}
	 
std::string logic_name(int operation) {
	switch (operation) {
		case AND:
			return std::string("and");
		case ORR:
			return std::string("orr");
		case EOR:
			return std::string("eor");
	}
	return std::string("???");
}

std::string shift_name(int operation) {
	switch (operation) {
		case LSL:
			return std::string("lsl");
		case LSR:
			return std::string("lsr");
		case ASR:
			return std::string("asr");
		case ROR:
			return std::string("ror");
	}
	return std::string("???");
}

std::string branch_name(int condition) {
	switch (condition) {
		case NE:
			return std::string("bne");
		case EQ:
			return std::string("beq");
		case CC:
			return std::string("bcc");
		case CS:
			return std::string("bcs");
		case LT:
			return std::string("blt");
		case GE:
			return std::string("bge");
		case B:
			return std::string("b");
		case BL:
			return std::string("bl");
	}
	return std::string("???");
}

}
