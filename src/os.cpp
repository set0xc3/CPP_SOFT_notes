#include "os.hpp"

#include <filesystem>

namespace Saura::OS {
fs::path home_config_path() {
  fs::path res;

  // Windows
#if defined(_WIN32)
  auto config = std::getenv("APPDATA");
  if (config == nullptr) {
    throw std::runtime_error("APPDATA environment variable not found");
  }
  res = fs::path(config);
// Linux
#elif defined(__linux__)
  auto config = std::getenv("HOME");
  if (config == nullptr) {
    throw std::runtime_error("HOME environment variable not found");
  }
  res = fs::path(config) / ".config";
#endif

  return res;
}
} // namespace Saura::OS
