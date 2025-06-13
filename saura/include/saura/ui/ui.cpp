#include "ui.hpp"

#include <fstream>
#include <iostream>

#include "imgui.h"

namespace Saura {
UI::UI() {
  curr_area = Empty;
  nodes.reserve(1024);
  curr_node = nullptr;

  std::ifstream file(
      "C:\\vault\\dev\\CPP_ROOT\\CPP_SOFT_notes\\build\\Debug\\imgui.ini");
  if (!file.is_open()) {
    throw std::runtime_error("Failed open file!");
  }

  Node test_node{};
  test_node.data = std::string((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
  nodes.push_back(test_node);
  curr_node = &nodes[0];

  file.close();
}

void UI::update() {
  update_keybinds();

  ImGui::SetNextWindowPos({});
  ImGui::SetNextWindowSize({root_size.x, root_size.y});
  if (ImGui::Begin("[window:main]", nullptr,
                   ImGuiWindowFlags_NoDecoration |
                       ImGuiWindowFlags_NoBackground |
                       ImGuiWindowFlags_NoSavedSettings)) {
  }
  auto reg_avail = ImGui::GetContentRegionAvail();

  switch (curr_area) {
    case Empty: {
      break;
    }
    case Master_Popup: {
      break;
    }
    case View: {
      break;
    }
  }

  if (curr_node == nullptr) {
    ImGui::BeginChild("##[buffer:empty]", {}, ImGuiChildFlags_Border);
    {
      const auto main_view_text = "- empty -";
      auto main_view_size = ImGui::CalcTextSize(main_view_text);
      ImGui::SetCursorPos({(reg_avail.x - main_view_size.x) * 0.5f,
                           (reg_avail.y - main_view_size.y) * 0.5f});
      ImGui::Text("%s", main_view_text);
    }
    ImGui::EndChild();
  } else {
    ImGui::BeginChild("##[buffer:view]", {}, ImGuiChildFlags_Border);
    {
      auto main_view_text = curr_node->data.c_str();
      ImGui::Text("%s", main_view_text);
    }
    ImGui::EndChild();
  }

  ImGui::End();
}

// TODO: Refactoring.
void UI::update_keybinds() {
  if (ImGui::IsKeyReleased(ImGuiKey_F1)) {
    std::cout << "[F1] Show All Commands" << std::endl;
    return;
  }

  // Workspace
  if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) &&
      ImGui::IsKeyDown(ImGuiKey_LeftShift) &&
      ImGui::IsKeyReleased(ImGuiKey_O)) {
    std::cout << "[Ctrl+Shift+O] Show All Workspaces" << std::endl;
    return;
  }
  if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) &&
      ImGui::IsKeyDown(ImGuiKey_LeftShift) &&
      ImGui::IsKeyReleased(ImGuiKey_F)) {
    std::cout << "[Ctrl+Shift+F] Search In All Workspaces" << std::endl;
    return;
  }

  // File
  if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyReleased(ImGuiKey_O)) {
    std::cout << "[Ctrl+O] Search File By Name" << std::endl;
    return;
  }
  if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyReleased(ImGuiKey_F)) {
    std::cout << "[Ctrl+F] Search In File" << std::endl;
    return;
  }
}

void UI::set_root_size(const ImVec2 new_size) { root_size = new_size; }

}  // namespace Saura
