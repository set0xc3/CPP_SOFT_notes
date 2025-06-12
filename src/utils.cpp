#include "utils.hpp"

#include <algorithm>

namespace Saura {
std::filesystem::path home_config_path() {
  std::filesystem::path res;

  // Windows
#if defined(_WIN32)
  auto config = std::getenv("APPDATA");
  if (config == nullptr) {
    throw std::runtime_error("APPDATA environment variable not found");
  }
  res = std::filesystem::path(config);
// Linux
#elif defined(__linux__)
  auto config = std::getenv("HOME");
  if (config == nullptr) {
    throw std::runtime_error("HOME environment variable not found");
  }
  res = std::filesystem::path(config) / ".config";
#endif

  return res;
}

fs::path normalize_path(const fs::path &path) {
  std::string clean_str = path.string();
  clean_str.erase(std::remove(clean_str.begin(), clean_str.end(), '"'),
                  clean_str.end());
  clean_str.erase(std::remove(clean_str.begin(), clean_str.end(), '\''),
                  clean_str.end());

  fs::path cleaned_path(clean_str);

  return fs::absolute(cleaned_path).lexically_normal();
}

std::string to_forward_slashes(std::string path) {
  std::replace(path.begin(), path.end(), '\\', '/');
  return path;
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

bool natural_compare(const std::string &a, const std::string &b) {
  size_t ia = 0, ib = 0;
  while (ia < a.size() && ib < b.size()) {
    if (std::isdigit(static_cast<unsigned char>(a[ia])) &&
        std::isdigit(static_cast<unsigned char>(b[ib]))) {
      // Извлекаем числа из обеих строк
      long numA = 0;
      while (ia < a.size() && std::isdigit(static_cast<unsigned char>(a[ia]))) {
        numA = numA * 10 + (a[ia] - '0');
        ia++;
      }

      long numB = 0;
      while (ib < b.size() && std::isdigit(static_cast<unsigned char>(b[ib]))) {
        numB = numB * 10 + (b[ib] - '0');
        ib++;
      }

      if (numA != numB) {
        return numA < numB;
      }
    } else {
      // Сравниваем символы без учета регистра
      char ca = std::tolower(static_cast<unsigned char>(a[ia]));
      char cb = std::tolower(static_cast<unsigned char>(b[ib]));

      if (ca != cb) {
        return ca < cb;
      }

      ia++;
      ib++;
    }
  }
  return a.size() < b.size();
}
} // namespace Saura
