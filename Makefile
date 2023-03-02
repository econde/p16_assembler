ifdef WINDOWS

EXECUTABLE = p16as.exe

CXX = /usr/bin/x86_64-w64-mingw32-g++

LDFLAGS = -Wl,-static -static-libgcc -static-libstdc++

else

EXECUTABLE = p16as

CXX = g++

endif

SOURCES = \
	build/p16_parser.cpp \
	build/p16_lexer.cpp  \
	src/code_generator.cpp \
 	src/cpp_printf.cpp \
	src/directive.cpp \
	src/error.cpp \
	src/expression.cpp \
	src/instruction.cpp \
	src/listing.cpp \
	src/pas.cpp \
	src/relocations.cpp \
	src/memory_space.cpp \
	src/sections.cpp \
	src/symbols.cpp \
	src/value_type.cpp

OBJECTS = $(SOURCES:%.cpp=build/%.o)

DEPENDENCIES = $(OBJECTS:%.o=%.d)

all: build_dir build/$(EXECUTABLE)

CXXFLAGS = -MMD -c -g -Wall -std=c++11 -Ibuild -Isrc

build_dir:
	mkdir -p build/src build/build

build/p16_lexer.cpp: src/p16.l build/p16_parser.hpp
	flex -o $@ src/p16.l

build/p16_parser.cpp build/p16_parser.hpp: src/p16.ypp
	bison --defines=build/p16_parser.hpp --output=build/p16_parser.cpp -v -t src/p16.ypp

-include $(DEPENDENCIES)

build/build/%.o: build/%.cpp
	$(CXX) $(CXXFLAGS) -Wno-free-nonheap-object $< -o $@

build/src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

build/$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -rf build/src build/build build/*.cpp build/p16_parser.hpp build/p16_parser.output build/$(EXECUTABLE)
