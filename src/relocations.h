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

#ifndef	RELOCATION_H
#define RELOCATION_H

#include "statement.h"

namespace ast {

struct Relocation {
	Location *location;		// local que vai receber o valor
	Location *exp_location;		// local do simbolo vai fornecer o valor
	unsigned section_index;		// número da secção
	unsigned section_offset;	// offset na secção
	unsigned position;		// offset em bits na palavra
	unsigned width;			// dimensão em bit
	enum Relocation_type { ABSOLUTE, RELATIVE, RELATIVE_UNSIGNED };
	Relocation_type type;
	string symbol;			// simbolo que vai fornecer o valor
	unsigned addend;		// valor a adicionar
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
