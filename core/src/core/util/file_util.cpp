#include "file_util.h"

#include <algorithm>
#include <fstream>

#include "core/debug/assertion.h"

namespace playchilla::file {

std::optional<std::string> read_as_string(const std::string& filename, size_t max_size) {
	std::ifstream t(filename, std::ios::binary);
	if (!t.good()) {
		return {};
	}

	t.seekg(0, std::ios::end);
	const size_t g = t.tellg();
	const auto size = std::min(g, max_size);
	t.seekg(0, std::ios::beg);

	std::string str(size, '\0');
	t.read(str.data(), size);
	assertion_2(str.size() == size, "Unexpected size of read string ", filename.c_str());
	return str;
}

std::string read_as_string_must_exist(const std::string& filename, size_t max_size) {
	const auto result = read_as_string(filename, max_size);
	assertion_2(result.has_value(), "File not found: ", filename.c_str());
	return *result;
}

std::string read_part_of_file_must_exist(const std::string& filename, size_t size) {
	const std::string& data = read_as_string_must_exist(filename, size);
	const auto last_nl = data.rfind('\n');
	assertion_2(last_nl != std::string::npos, "Could not find a last new line: ", filename.c_str());
	return data.substr(0, last_nl);
}

bool write(const std::string& filename, const std::string& data) {
	std::ofstream file(filename, std::ios::out | std::ios::trunc);
	file << data;
	file.close();
	return !file.fail();
}

}
