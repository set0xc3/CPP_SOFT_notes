#include "vault_manager.hpp"

#include <vendor/imgui/imgui.h>
#include <vendor/imgui/imgui_internal.h>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

#include "ui/vault.hpp"

#include <utils.hpp>

namespace Saura {
void VaultManager::init() {
  json config = read_config();
  for (auto& vault : config["vaults"].items()) {
    auto& path = vault.key();

    std::shared_ptr<Vault> new_vault = std::make_shared<Vault>();
    new_vault->root = std::make_shared<Node>();
    new_vault->root->path = path;
    vaults[new_vault->root->path.string()] = new_vault;
  }
}

void VaultManager::draw() {
  if (hot_vault.lock().get() == nullptr) {
    draw_vault_manager();
  } else {
    draw_vault();
  }
}

void VaultManager::create() {}

void VaultManager::open() {}

json VaultManager::read_config() {
  json ret;
  fs::path appdata_path = get_appdata_path();
  fs::path app_path = appdata_path / "SauraStudios" / "Notes";
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

bool VaultManager::save_config(const json& new_config) {
  fs::path appdata_path = get_appdata_path();
  fs::path app_path = appdata_path / "SauraStudios" / "Notes";
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

void VaultManager::draw_vault_manager() {
  static char vault_path_buff[1024] = "";

  if (ImGui::BeginPopupModal(
          "##vault_manager_create", nullptr,
          ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar |
              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {
    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  if (ImGui::Button("Add")) {
    if (!std::string(vault_path_buff).empty()) {
      std::shared_ptr<Vault> new_vault = std::make_shared<Vault>();
      new_vault->root->path = vault_path_buff;

      json new_config = read_config();
      // std::cout << vault->path << std::endl;
      if (!new_config["vaults"].contains(normalize_path(new_vault->root->path))) {
        std::cout << new_config << std::endl;
        new_config["vaults"][new_vault->root->path.string()] = {
            {"ts", ""},
            {"open", false},
        };
        if (save_config(new_config)) {
          vaults[new_vault->root->path.string()] = new_vault;
        }
      }
    }
  }
  ImGui::SameLine();
  ImGui::SetNextItemWidth(300);
  ImGui::InputText("##open_vault", vault_path_buff, sizeof(vault_path_buff));

  ImGui::BeginChild("Tree", ImVec2(300, 0), true);
  for (const auto& [key, vault] : vaults) {
    bool is_selected = false;
    if (ImGui::Selectable(vault.get()->root->path.string().c_str(), is_selected)) {
      is_selected = !is_selected;
      hot_vault = vault;
      fs::create_directory(hot_vault.lock().get()->root->path);
      if (fs::exists(hot_vault.lock().get()->root->path)) {
          // hot_vault.lock().get()->root.get()->children;
      }
    }
  }
  ImGui::EndChild();
}

void VaultManager::draw_vault() {
  if (ImGui::Button("<")) {
    hot_vault.reset();
  }
  ImGui::SameLine();
  if (hot_vault.lock().get() != nullptr) {
    ImGui::Text("%s", hot_vault.lock().get()->root->path.string().c_str());
    ImGui::BeginChild("Tree", ImVec2(300, 0), true);
    hot_vault.lock()->draw();
    ImGui::EndChild();
  }
}
}  // namespace Saura
