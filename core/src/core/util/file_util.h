#pragma once

#include <optional>
#include <string>

namespace playchilla::file {
std::optional<std::string> read_as_string(const std::string& filename, size_t max_size = SIZE_MAX);
std::string read_as_string_must_exist(const std::string& filename, size_t max_size = SIZE_MAX);
std::string read_part_of_file_must_exist(const std::string& filename, size_t size);

bool write(const std::string& filename, const std::string& data);
}
