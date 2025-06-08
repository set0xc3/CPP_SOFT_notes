#include "utils.hpp"

namespace Saura {
fs::path get_appdata_path() {
  fs::path res;
  const char* appdata = nullptr;

// Для MSVC и Clang-cl (режим MSVC)
#if defined(_MSC_VER) || (defined(__clang__) && defined(_MSC_VER))
  char* buffer = nullptr;
  size_t len = 0;
  errno_t err = _dupenv_s(&buffer, &len, "APPDATA");
  if (err || buffer == nullptr) {
    if (buffer) free(buffer);
    throw std::runtime_error("APPDATA environment variable not found");
  }
  appdata = buffer;
#else
  // Для MinGW и других компиляторов
  appdata = std::getenv("APPDATA");
  if (!appdata) {
    throw std::runtime_error("APPDATA environment variable not found");
  }
#endif

  res = fs::path(appdata);

// Освобождаем буфер для MSVC/Clang-cl
#if defined(_MSC_VER) || (defined(__clang__) && defined(_MSC_VER))
  free(buffer);
#endif

  return res;
}

fs::path normalize_path(const fs::path& path) {
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

bool is_directory_modified(const fs::path& dir_path,
                           fs::file_time_type& last_known_time) {
  auto current_time = fs::last_write_time(dir_path);
  if (current_time != last_known_time) {
    last_known_time = current_time;
    return true;
  }
  return false;
}

bool natural_compare(const std::string& a, const std::string& b) {
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
}  // namespace Saura
