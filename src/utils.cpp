#include "utils.hpp"

#include <algorithm>

namespace Saura {
fs::path normalize_path(const fs::path &path) {
  std::string clean_str = path.string();
  clean_str.erase(std::remove(clean_str.begin(), clean_str.end(), '"'),
                  clean_str.end());
  clean_str.erase(std::remove(clean_str.begin(), clean_str.end(), '\''),
                  clean_str.end());

  fs::path cleaned_path(clean_str);

  return fs::absolute(cleaned_path).lexically_normal();
}

bool is_directory_modified(const fs::path &dir_path,
                           fs::file_time_type &last_known_time) {
  auto current_time = fs::last_write_time(dir_path);
  if (current_time != last_known_time) {
    last_known_time = current_time;
    return true;
  }
  return false;
}

} // namespace Saura
