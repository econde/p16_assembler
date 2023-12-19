cmake_minimum_required(VERSION 3.13)
project(pas)

set(CMAKE_CXX_STANDARD 11)

find_package(BISON)
BISON_TARGET(p16_parser src/p16.ypp ${CMAKE_CURRENT_BINARY_DIR}/p16_parser.cpp)

find_package(FLEX)
FLEX_TARGET(p16_lexer src/p16.l ${CMAKE_CURRENT_BINARY_DIR}/p16_lexer.cpp)

ADD_FLEX_BISON_DEPENDENCY(p16_lexer p16_parser)

include_directories(src ${CMAKE_CURRENT_BINARY_DIR})

add_executable(p16as
	${BISON_p16_parser_OUTPUTS}
	${FLEX_p16_lexer_OUTPUTS}
	src/cpp_printf.cpp
	src/cpp_printf.h
	src/pas.cpp
	src/directive.h
	src/directive.cpp
	src/expression_base.h
	src/expression.h
	src/expression.cpp
	src/instruction.cpp
	src/instruction.h
	src/listing.cpp
	src/code_generator.h
	src/code_generator.cpp
	src/location.h
	src/memory_space.h
	src/memory_space.cpp
	src/sections.h
	src/sections.cpp
	src/statement.h
	src/symbols.h
	src/symbols.cpp
	src/relocations.h
	src/relocations.cpp
	src/visitor.h
	src/utils.h
	src/properties.h
	src/error.h
	src/error.cpp
	src/value_type.h
	src/value_type.cpp)
