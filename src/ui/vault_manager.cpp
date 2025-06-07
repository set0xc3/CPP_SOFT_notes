#include "vault_manager.hpp"

namespace Saura {
void VaultManager::draw() {
  ImGui::Button("Create");
  ImGui::SameLine();
  ImGui::Button("Open");

  ImGui::BeginChild("Tree", ImVec2(300, 0), true);
  ImGui::Text("C:/vault/docs/test");
  ImGui::EndChild();

  ImGui::SameLine();
}

void VaultManager::create() {}

void VaultManager::open() {}

}  // namespace Saura
