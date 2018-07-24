#ifndef	RELOCATION_H
#define RELOCATION_H

#include "statement.h"

namespace ast {

struct Relocation {
	enum Type { ABSOLUTE, RELATIVE };
	Statement *statement;
	Location *exp_location;
	unsigned position, width;
	Type type;
	string symbol;
	unsigned addend;
};

class Relocations {
public:
	static std::list<Relocation*> list;
	static void add(Relocation *reloc) { list.push_back(reloc); }
	static void deallocate();
	static void print(ostream &);
	static void relocate();
};

}

#endif
