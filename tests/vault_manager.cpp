#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <utils.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

TEST_CASE("get appdata and create app folder") {
  fs::path appdata_path = Saura::get_appdata_path();
  fs::path app_path = appdata_path / "SauraStudios" / "Notes";
  fs::path app_config = app_path / "config.json";
  fs::path test_config = "C:\\vault\\docs\\test";

  test_config = Saura::normalize_path(test_config);
  std::cout << test_config << std::endl;

  std::cout << app_path << std::endl;

  if (fs::exists(app_path) && fs::is_directory(app_path)) {
    std::cout << "Folder exists." << std::endl;

    if (!fs::is_directory(app_config)) {
      std::ofstream file(app_config);

      // Write to file
      if (file.is_open()) {
        json config;
        config["vaults"][test_config.string()] = {
            {"ts", "1749298616267"},
            {"open", false},
        };
        file << config.dump() << std::endl;

        file.close();
        std::cout << "File created successfully!" << std::endl;
      } else {
        std::cerr << "Error creating file." << std::endl;
      }
    }
  } else {
    std::cout << "Folder does not exist." << std::endl;
    fs::create_directories(app_path);
  }
}
