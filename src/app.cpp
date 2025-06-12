#include "app.hpp"
#include "os.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace Saura::App {
json read_config() {
  json ret;
  fs::path config_path = Saura::OS::home_config_path();
  fs::path app_path = config_path / "SauraStudios" / "Notes";
  fs::path app_config = app_path / "config.json";

  if (fs::exists(app_path) && fs::is_directory(app_path)) {
    std::ifstream f(app_config);
    if (fs::exists(app_config)) {
      ret = json::parse(f);
    } else {
      ret["vaults"] = {};
    }
  } else {
    std::cout << "Folder does not exist." << std::endl;
    fs::create_directories(app_path);
  }
  return ret;
}

bool save_config(const json &new_config) {
  fs::path config_path = Saura::OS::home_config_path();
  fs::path app_path = config_path / "SauraStudios" / "Notes";
  fs::path app_config = app_path / "config.json";

  std::ofstream file(app_config);
  if (file.is_open()) {
    file << new_config.dump() << std::endl;
    file.close();
    std::cout << "File created successfully!" << std::endl;
  } else {
    std::cerr << "Error creating file." << std::endl;
    return false;
  }

  return true;
}
} // namespace Saura::App
