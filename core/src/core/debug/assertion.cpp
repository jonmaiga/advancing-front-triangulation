#include "assertion.h"
#include "core/debug/log.h"

namespace playchilla::assertion {

void __assertion(bool condition, const char* message, const char* condition_str, const char* file, int line) {
	if (!condition) {
		logger() << file << " (" << line << "): " << condition_str << " " << message << "\n";
		//assert(false);
	}
}

void __assertion_2(bool condition, const char* message1, const char* message2, const char* condition_str, const char* file, int line) {
	if (!condition) {
		logger() << file << " (" << line << "): " << condition_str << " " << message1 << " " << message2 << "\n";
		//assert(false);
	}
}

}
