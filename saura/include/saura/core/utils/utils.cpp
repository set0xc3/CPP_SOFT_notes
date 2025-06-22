#include "utils.hpp"

#include <algorithm>

#include "saura/core/os/os.hpp"

namespace Saura::Utils {
fs::path normalize_path(const fs::path& input) {
  std::string path = input.string();

  path.erase(std::remove(path.begin(), path.end(), '"'), path.end());
  path.erase(std::remove(path.begin(), path.end(), '\''), path.end());

  std::string env_home = "";
#if __WIN32__
  env_home = "USERPROFILE";
#elif __GNUC__
  env_home = "HOME";
#endif

  // Замена $HOME на переменную окружения
  size_t home_pos = path.find("$" + env_home);
  if (home_pos != std::string::npos) {
    const char* home_dir = Saura::OS::get_safe_getenv(env_home).c_str();
    if (home_dir) {
      path.replace(home_pos, 5, home_dir);
    }
  }

  // Замена ~ на домашний каталог
  if (!path.empty() && path[0] == '~') {
    const char* home_dir = Saura::OS::get_safe_getenv(env_home).c_str();
    if (home_dir) {
      path.erase(0, 1);
      path.insert(0, home_dir);
    }
  }

  // Нормализация пути
  std::filesystem::path p(path);
  return p.lexically_normal().string();
}

bool is_directory_modified(const fs::path& dir_path,
                           fs::file_time_type& last_known_time) {
  auto current_time = fs::last_write_time(dir_path);
  if (current_time != last_known_time) {
    last_known_time = current_time;
    return true;
  }
  return false;
}

}  // namespace Saura::Utils
