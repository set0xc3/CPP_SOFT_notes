#include "ui.hpp"

#include <iostream>
#include <memory>
#include <string>

#include "saura/config/config.hpp"

namespace Saura {
UI::Master_Popup::Master_Popup(const UI* _ui_ctx) {
  ui_ctx = _ui_ctx;
  title = "[popop:master]";
}

void UI::Master_Popup::draw() {
  ImVec2 size{500.0f, 300.0f};
  ImVec2 padding{0.0f, 12.0f};
  auto reg_min = ImGui::GetWindowContentRegionMin();
  auto reg_avail = ImGui::GetContentRegionAvail();

  ImGui::SetNextWindowPos(
      {(reg_avail.x - size.x) * 0.5f, reg_min.y + padding.y});
  ImGui::SetNextWindowSize({size.x, 0.0f});

  if (ImGui::BeginPopup("[popop:master]")) {
    for (auto& node : ui_ctx->nodes) {
      if (!node->is_dir) {
        continue;
      }

      auto title = node->text;
      if (ImGui::TreeNodeEx(
              title.c_str(),
              ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanFullWidth)) {
        ImGui::TreePop();
      }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

void UI::Master_Popup::open() { ImGui::OpenPopup(title.c_str()); }

bool UI::Master_Popup::is_open() { return ImGui::IsPopupOpen(title.c_str()); }

UI::UI() {
  master_popup = std::make_shared<Master_Popup>(this);

  curr_area = Empty;

// TEST
#if 0
  {
    fs::path file_path = "imgui.ini";
    std::ifstream file(file_path);
    if (!file.is_open()) {
      throw std::runtime_error("Failed open file!");
    }

    auto test_node = std::make_shared<Node>();
    test_node->data = std::string((std::istreambuf_iterator<char>(file)),
                                  std::istreambuf_iterator<char>());
    test_node->text = file_path.filename().string();
    nodes.push_back(test_node);
    curr_node = test_node;

    file.close();
  }
#endif
}

void UI::update() {
  process_keybinds();

  ImGui::SetNextWindowPos({});
  ImGui::SetNextWindowSize({root_size.x, root_size.y});
  if (ImGui::Begin("[window:main]", nullptr,
                   ImGuiWindowFlags_NoDecoration |
                       ImGuiWindowFlags_NoBackground |
                       ImGuiWindowFlags_NoSavedSettings)) {
  }
  auto reg_avail = ImGui::GetContentRegionAvail();

  if (curr_node.lock() == nullptr) {
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
      auto main_view_text = curr_node.lock()->data.c_str();
      ImGui::Text("%s", main_view_text);
    }
    ImGui::EndChild();
  }

  for (auto it = commands.begin(); it != commands.end();) {
    switch (it->kind) {
      case Command_Kind_ShowAllCommands: {
        if (!master_popup->is_open()) {
          master_popup->open();
        } else {
          nodes.clear();
          it = commands.erase(it);
        }
        break;
      }
      case Command_Kind_ShowAllWorkspaces: {
        if (!master_popup->is_open()) {
          master_popup->open();
        } else {
          nodes.clear();
          it = commands.erase(it);
        }

        auto config = Saura::Config::read_config();
        if (!config.empty()) {
          for (auto [path, _] : config["vaults"].items()) {
            auto node = std::make_shared<Node>();
            node->is_dir = true;
            node->text = path;
            nodes.push_back(node);
          }
        }

        break;
      }
      default: {
        it += 1;
        break;
      }
    }
  }

  master_popup->draw();

  ImGui::End();
}

// TODO: Refactoring.
void UI::process_keybinds() {
  if (ImGui::IsKeyPressed(ImGuiKey_F1)) {
    std::cout << "[F1] Show All Commands" << std::endl;
    Command cmd = {
        .kind = Command_Kind_ShowAllCommands,
    };
    commands.push_back(cmd);
    return;
  }

  // Workspace
  if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) &&
      ImGui::IsKeyDown(ImGuiKey_LeftShift) && ImGui::IsKeyPressed(ImGuiKey_O)) {
    std::cout << "[Ctrl+Shift+O] Show All Workspaces" << std::endl;
    Command cmd = {
        .kind = Command_Kind_ShowAllWorkspaces,
    };
    commands.push_back(cmd);
    return;
  }
  if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) &&
      ImGui::IsKeyDown(ImGuiKey_LeftShift) && ImGui::IsKeyPressed(ImGuiKey_F)) {
    std::cout << "[Ctrl+Shift+F] Search In All Workspaces" << std::endl;
    return;
  }

  // File
  if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_O)) {
    std::cout << "[Ctrl+O] Search File By Name" << std::endl;
    return;
  }
  if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_F)) {
    std::cout << "[Ctrl+F] Search In File" << std::endl;
    return;
  }
}

void UI::set_root_size(const ImVec2 new_size) { root_size = new_size; }

}  // namespace Saura
