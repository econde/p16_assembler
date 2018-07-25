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
#include <iostream>
#include <memory>

#define	DEFAULT_SIZE	128

std::string string_printf(const char* fmt, ...) {
	auto buffer = std::get_temporary_buffer<char>(DEFAULT_SIZE);
	if (buffer.second > 0) {
		va_list vl;
		va_start(vl, fmt);
		auto actual_size = vsnprintf(buffer.first, buffer.second, fmt, vl);
		if (buffer.second <= actual_size) {
			std::return_temporary_buffer (buffer.first);
			buffer = std::get_temporary_buffer<char>(actual_size + 1);
            if (buffer.second > 0) {
                va_start(vl, fmt);
                vsnprintf(buffer.first, buffer.second, fmt, vl);
                std::string result(buffer.first);
                std::return_temporary_buffer(buffer.first);
                return result;
            }
		}
        else {
            std::string result(buffer.first);
            std::return_temporary_buffer(buffer.first);
            return result;
        }
	}
    return std::string();
}

std::ostream& ostream_printf(std::ostream& stream, const char* fmt, ...) {
    auto buffer = std::get_temporary_buffer<char>(DEFAULT_SIZE);
    if (buffer.second > 0) {
        va_list vl;
        va_start(vl, fmt);
        auto actual_size = vsnprintf(buffer.first, buffer.second, fmt, vl);
        if (buffer.second <= actual_size) {
            std::return_temporary_buffer(buffer.first);
            buffer = std::get_temporary_buffer<char>(actual_size + 1);
            if (buffer.second > 0) {
                va_start(vl, fmt);
                vsnprintf(buffer.first, buffer.second, fmt, vl);
                stream << buffer.first;
                std::return_temporary_buffer(buffer.first);
            }
        }
        else {
            stream << buffer.first;
            std::return_temporary_buffer(buffer.first);
        }
    }
    return stream;
}
