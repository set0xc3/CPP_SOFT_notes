/*
 *  State:
 *      - Focus
 *      - Hot
 *      - Hover
 *      - Add/Delete/Rename
 *      - Search
 *  Area:
 *      - Empty
 *      - Master Popup
 *      - View
 *  Master Popup (Commands):
 *      - [F1]               : Show All Commands
 *      - Workspace:
 *          - [Ctrl+Shift+O] : Show All Workspaces
 *          - [Ctrl+Shift+F] : Search In All Workspaces
 *      - File:
 *          - [Ctrl+O]       : Search File By Name
 *          - [Ctrl+F]       : Search In File
 */

#ifndef UI_HPP_
#define UI_HPP_

#include <vendor/imgui/imgui.h>

#include <functional>
#include <list>
#include <string>
#include <vector>

namespace Saura {
using Action = std::function<void()>;
using Key_Code = ImGuiKey;

enum Area { Empty, Master_Popup, View };

struct Node {
  std::list<Node> node;
  bool is_open;
  std::string name;
  std::string data;
};

class UI {
 private:
  ImVec2 root_size;
  Area curr_area;
  std::vector<Node> nodes;
  Node *curr_node;

 public:
  UI();

  void update();
  void update_keybinds();

  void set_root_size(const ImVec2 new_size);
};
}  // namespace Saura

#endif
