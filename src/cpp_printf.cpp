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

#include <string>
#include <cstdarg>
#include <cassert>
#include <iostream>
#include <memory>
#include <new>

#include "cpp_printf.h"

#define	DEFAULT_SIZE	128

std::string string_printf(const char* fmt, ...) {
	char *buffer = new char[DEFAULT_SIZE];
	assert(buffer != NULL);
	va_list vl;
	va_start(vl, fmt);
	auto actual_size = vsnprintf(buffer, DEFAULT_SIZE, fmt, vl);
	if (DEFAULT_SIZE <= actual_size) {
		delete[](buffer);
		buffer = new char[actual_size + 1];
		assert(buffer != NULL);
		va_start(vl, fmt);
		vsnprintf(buffer, actual_size + 1, fmt, vl);
	}
	std::string result(buffer);
	delete[](buffer);
	return result;
}

std::ostream& ostream_printf(std::ostream& stream, const char* fmt, ...) {
	char *buffer = new char[DEFAULT_SIZE];
	assert(buffer != NULL);
	va_list vl;
	va_start(vl, fmt);
	auto actual_size = vsnprintf(buffer, DEFAULT_SIZE, fmt, vl);
	if (DEFAULT_SIZE <= actual_size) {
		delete[](buffer);
		buffer = new char[actual_size + 1];
		va_start(vl, fmt);
		vsnprintf(buffer, actual_size + 1, fmt, vl);
	}
	stream << buffer;
	delete[] buffer;
	return stream;
}

