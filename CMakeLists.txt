cmake_minimum_required(VERSION 3.9)
project(pas)

set(CMAKE_CXX_STANDARD 11)

add_executable(pas
        src/cpp_printf.cpp
        src/cpp_printf.h
        src/pas.cpp
        src/directive.h
        src/directive.cpp
        src/expression_base.h
        src/expression.h
        src/expression.cpp
        src/p16.h
        src/p16.l
        src/p16.ypp
        src/p16.tab.cpp
        src/p16.lex.cpp
        src/instruction.cpp
        src/instruction.h
        src/listing_generator.h
        src/listing.cpp
        src/code_generator.h
        src/code_generator.cpp
        src/location.h
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

