#include "ui.hpp"

namespace Saura {
void UI::update() {
  ImGui::SetNextWindowPos({0, 0});
  ImGui::SetNextWindowSize({root_size.x, root_size.y});
  if (ImGui::Begin("Note Editor", nullptr,
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
  {
    ImVec2 available = ImGui::GetContentRegionAvail();

    {
      if (ImGui::IsKeyDown(ImGuiKey_F1)) {
        ImGui::OpenPopup("Show All Commands");
      }

      if (ImGui::BeginPopup("Show All Commands",
                            ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::IsKeyDown(ImGuiKey_Escape)) {
          ImGui::CloseCurrentPopup();
        }
        ImGui::Text("[Ctrl] + [O]               Navigate To File");
        ImGui::Text("[Ctrl] + [Shift] + [O]     Navigate To Workspace");
        ImGui::Text("[Ctrl] + [F]               Search In Open File");
        ImGui::EndPopup();
      }

      // Аналогично для второго popup’а:
      if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) &&
          ImGui::IsKeyDown(ImGuiKey_LeftShift) &&
          ImGui::IsKeyDown(ImGuiKey_O)) {
        ImGui::OpenPopup("Navigate To Workspace");
      }

      // Установка позиции popup по позиции мыши
      ImVec2 popup_pos = ImGui::GetMousePos();
      ImGui::SetNextWindowPos(popup_pos, ImGuiCond_Appearing);

      if (ImGui::BeginPopup("Navigate To Workspace",
                            ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::IsKeyDown(ImGuiKey_Escape)) {
          ImGui::CloseCurrentPopup();
        }
        ImGui::BeginGroup();
        static char input_path[1024];
        ImGui::InputText("##Input", input_path, sizeof(input_path));
        ImGui::Spacing();
        if (ImGui::TreeNodeEx("Vault", ImGuiTreeNodeFlags_Bullet)) {
          ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("PARA Template", ImGuiTreeNodeFlags_Bullet)) {
          ImGui::TreePop();
        }

        ImGui::EndGroup();
        ImGui::EndPopup();
      }
    }

    ImGui::BeginChild("##Empty", {0.0f, 0.0f}, ImGuiChildFlags_Border);

    static ImVec2 group_size;
    ImGui::SetCursorPos(
        {ImGui::GetCursorPos().x + (available.x - group_size.x) * 0.5f,
         ImGui::GetCursorPos().y + (available.y - group_size.y) * 0.5f});
    ImGui::BeginGroup();
    ImGui::Text("- empty -");
    ImGui::EndGroup();
    group_size = ImGui::GetItemRectSize();

    ImGui::EndChild();

    ImGui::End();
  }
}

void UI::set_root_size(const ImVec2 new_size) { root_size = new_size; }

} // namespace Saura
