#include "ui.hpp"

#include "imgui.h"

namespace Saura {
UI::UI() { curr_area = Empty; }

void UI::update() {
  ImGui::SetNextWindowPos({});
  ImGui::SetNextWindowSize({root_size.x, root_size.y});
  if (ImGui::Begin("[window:main]", nullptr,
                   ImGuiWindowFlags_NoDecoration |
                       ImGuiWindowFlags_NoBackground |
                       ImGuiWindowFlags_NoSavedSettings)) {
  }
  switch (curr_area) {
    case Empty: {
      auto reg_avail = ImGui::GetContentRegionAvail();
      ImGui::BeginChild("##[area:empty]", {0.0f, 0.0f}, ImGuiChildFlags_Border);
      {
        const auto main_view_text = "- empty -";
        auto main_view_size = ImGui::CalcTextSize(main_view_text);
        ImGui::SetCursorPos({(reg_avail.x - main_view_size.x) * 0.5f,
                             (reg_avail.y - main_view_size.y) * 0.5f});
        ImGui::Text(main_view_text);
      }
      ImGui::EndChild();
      break;
    }
    case Master_Popup: {
      break;
    }
    case View: {
      break;
    }
  }
  ImGui::End();
}

void UI::set_root_size(const ImVec2 new_size) { root_size = new_size; }

}  // namespace Saura
