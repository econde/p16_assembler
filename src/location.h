#ifndef LOCATION_H
#define LOCATION_H

#include <string>

namespace ast {

struct Location {
		const char *unit;
		int line, first_column, last_column;
};

}

#endif
