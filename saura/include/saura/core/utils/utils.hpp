#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <filesystem>
#include <string>

namespace Saura::Utils {

namespace fs = std::filesystem;

fs::path normalize_path(const fs::path& path);

std::string to_forward_slashes(std::string path);

bool is_directory_modified(const fs::path& dir_path,
                           fs::file_time_type& last_known_time);

// Функция для естественной сортировки (учитывает числа в строках)
bool natural_compare(const std::string& a, const std::string& b);

}  // namespace Saura

#endif
