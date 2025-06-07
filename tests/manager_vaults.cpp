#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <iostream>

#include <utils.hpp>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

TEST_CASE("get appdata and create app folder") {
  fs::path appdata_path = get_appdata_path();
  fs::path app_path = appdata_path / "SauraStudios" / "Notes";
  std::cout << app_path << std::endl;
  if (fs::exists(app_path) && fs::is_directory(app_path)) {
    std::cout << "Folder exists." << std::endl;
  } else {
    std::cout << "Folder does not exist." << std::endl;
    fs::create_directories(app_path);
  }
}
