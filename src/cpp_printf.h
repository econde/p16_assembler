#ifndef CPP_PRINTF_H
#define CPP_PRINTF_H

#include <string>
#include <iostream>

std::string string_printf(const char* fmt, ...);
std::ostream& ostream_printf(std::ostream& stream, const char* fmt, ...);

#endif
