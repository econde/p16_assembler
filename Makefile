ifndef WINDOWS

all: das

GPP = g++

GCC = gcc

EXECUTABLE = das

CFLAGS	= -c -g -Wa,-a=build/$*.lst -Wall

src/iarm.lex.cpp:	src/iarm.l src/iarm.tab.cpp
	flex -o src/iarm.lex.cpp src/iarm.l

src/iarm.tab.cpp src/iarm.tab.hpp: src/iarm.ypp
	bison --defines=src/iarm.tab.hpp --output=src/iarm.tab.cpp -v -t src/iarm.ypp

#-------------------------------------------------------------------------------

else

all: das.exe

GPP = /usr/bin/x86_64-w64-mingw32-g++

GCC = /usr/bin/x86_64-w64-mingw32-gcc

EXECUTABLE = das.exe

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

OBJECTS = das.o iarm.tab.o iarm.lex.o instruction.o expression.o sections.o symbols.o \
	relocations.o cpp_printf.o listing.o code_generator.o error.o value_type.o directive.o

$(EXECUTABLE): $(addprefix build/, $(OBJECTS))
	$(GPP) $(LDFLAGS) -o $@ $(addprefix build/, $(OBJECTS))
