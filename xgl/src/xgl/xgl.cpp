#include "xgl.h"

namespace playchilla {
std::string get_error_string(GLenum error_code) {
	if (error_code == GL_NO_ERROR) {
		return "GL_NO_ERROR(0x0): No error has been recorded.";
	}

	if (error_code == GL_INVALID_ENUM) {
		return "GL_INVALID_ENUM(0x500): An unacceptable value is specified for an enumerated argument.";
	}

	if (error_code == GL_INVALID_VALUE) {
		return "GL_INVALID_VALUE(0x501): A numeric argument is out of range.";
	}

	if (error_code == GL_INVALID_OPERATION) {
		return "GL_INVALID_OPERATION(0x502): The specified operation is not allowed in the current state. ";
	}

	if (error_code == GL_STACK_OVERFLOW) {
		return "GL_STACK_OVERFLOW(0x503): An attempt has been made to perform an operation that would cause an internal stack to overflow.";
	}

	if (error_code == GL_STACK_UNDERFLOW) {
		return "GL_STACK_UNDERFLOW(0x504): An attempt has been made to perform an operation that would cause an internal stack to underflow.";
	}

	if (error_code == GL_OUT_OF_MEMORY) {
		return "GL_OUT_OF_MEMORY(0x505): There is not enough memory left to execute the command.";
	}

	if (error_code == GL_INVALID_FRAMEBUFFER_OPERATION) {
		return "GL_INVALID_FRAMEBUFFER_OPERATION(0x506): The framebuffer object is not complete.";
	}

	if (error_code == GL_CONTEXT_LOST) {
		return "GL_CONTEXT_LOST(0x507): Given if the OpenGL context has been lost, due to a graphics card reset.";
	}

	if (error_code == 0x8031) {
		//GL_TABLE_TOO_LARGE) {
		return "GL_TABLE_TOO_LARGE(0x8031): Part of the ARB_imaging extension.";
	}

	return "Unknown error code: " + std::to_string(error_code);
}

std::optional<std::string> get_error_strings() {
	std::string errors;
	GLenum err;
	int error_count = 0;
	while ((err = glGetError()) != GL_NO_ERROR) {
		errors += get_error_string(err);
		++error_count;
		if (error_count >= 10) {
			errors += "\nWARNING: only showing first 10 errors.";
			break;
		}
	}
	return errors.empty() ? std::optional<std::string>{} : std::optional(errors);
}
}
