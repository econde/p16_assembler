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
	enum Type { ABSOLUTE, RELATIVE, RELATIVE_UNSIGNED };
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
