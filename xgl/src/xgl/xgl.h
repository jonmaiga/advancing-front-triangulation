#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <glad/glad.h>

namespace playchilla {
std::string get_error_string(GLenum error_code);
std::optional<std::string> get_error_strings();

namespace internal {
inline void gl_pop_errors(const char* expression, const char* file, int line) {
	if (const auto maybeErrors = get_error_strings()) {
		std::cout << file << " (" << line << ") " << expression << ": " << *maybeErrors << "\n";
	}
}

template <typename T>
void gl_error_return_value(T returnValue, T error, const char* expression, const char* file, int line) {
	if (returnValue == error) {
		std::cout << file << " (" << line << ") " << expression << ": Return value indicates an error: " << returnValue << "\n";
	}
}
}}

#ifdef DEVELOPMENT
#define gl_check(expression) (expression, playchilla::internal::gl_pop_errors(#expression, __FILE__, __LINE__))
#define gl_check_ret(error, expression) [&](){auto r = expression; playchilla::internal::gl_error_return_value(r, error, #expression, __FILE__, __LINE__); playchilla::internal::gl_pop_errors(#expression, __FILE__, __LINE__); return r;}();
#define gl_check_zero(expression) gl_check_ret(static_cast<GLuint>(0), expression)
#define gl_check_neg(expression) gl_check_ret(static_cast<GLint>(-1), expression)
#else
#define gl_check(expression) (expression)
#define gl_check_zero(expression) (expression)
#define gl_check_neg(expression) (expression)
#endif
