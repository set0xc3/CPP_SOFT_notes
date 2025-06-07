#ifndef UI_HPP_
#define UI_HPP_

#include <vendor/imgui/imgui.h>

#include <filesystem>
#include <vector>
#include <vendor/json/include/nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

class ManagerVaults {
 public:
  void draw() {
    ImGui::Button("Create");
    ImGui::SameLine();
    ImGui::Button("Open");

    ImGui::BeginChild("Tree", ImVec2(300, 0), true);
    ImGui::Text("C:/vault/docs/test");
    ImGui::EndChild();

    ImGui::SameLine();
  }

  void create() {}

  void open() {}

 private:
  std::vector<fs::path> paths;
};

#endif
