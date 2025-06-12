#ifndef OS_HPP_
#define OS_HPP_

#include <filesystem>

namespace fs = std::filesystem;

namespace Saura::OS {
fs::path home_config_path();
} // namespace Saura::OS

#endif
