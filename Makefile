ifndef WINDOWS

all: pas

GPP = g++

GCC = gcc

EXECUTABLE = pas

CFLAGS	= -c -g -Wa,-a=build/$*.lst -Wall

src/p16.lex.cpp:	src/p16.l src/p16.tab.cpp
	flex -o src/p16.lex.cpp src/p16.l

src/p16.tab.cpp src/p16.tab.hpp: src/p16.ypp
	bison --defines=src/p16.tab.hpp --output=src/p16.tab.cpp -v -t src/p16.ypp

#-------------------------------------------------------------------------------

else

all: pas.exe

GPP = /usr/bin/x86_64-w64-mingw32-g++

GCC = /usr/bin/x86_64-w64-mingw32-gcc

EXECUTABLE = pas.exe

CFLAGS	= -c -O3 -Wa,-a=build/$*.lst -Wall

LDFLAGS = -Wl,-static -static-libgcc -static-libstdc++

endif
#-------------------------------------------------------------------------------


CXXFLAGS = $(CFLAGS) -std=c++11 -U__STRICT_ANSI__

build/%.o: src/%.c
	$(GPP) -o build/$*.o $(CXXFLAGS) $<

build/%.o: src/%.cpp
	$(GPP) -o build/$*.o $(CXXFLAGS) $<
	
clean:
	rm -rf build/*

#-------------------------------------------------------------------------------

OBJECTS = pas.o p16.tab.o p16.lex.o instruction.o expression.o sections.o symbols.o memory_space.o \
	relocations.o cpp_printf.o listing.o code_generator.o error.o value_type.o directive.o

$(EXECUTABLE): $(addprefix build/, $(OBJECTS))
	$(GPP) $(LDFLAGS) -o $@ $(addprefix build/, $(OBJECTS))
