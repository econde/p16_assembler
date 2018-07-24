#ifndef VALUE_TYPE_H
#define VALUE_TYPE_H

namespace ast {

enum Value_type {
    ABSOLUTE, LABEL, UNDEFINED, INVALID
};

extern const char *value_type_name[];

}

#endif
