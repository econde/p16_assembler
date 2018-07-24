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
