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
 *      - Misc:
 *          - [F1]           : Show All Commands
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
#include <memory>
#include <string>
#include <vector>

namespace Saura {
using Action = std::function<void()>;
using Key_Code = ImGuiKey;

enum Area { Empty, Master_Popup, View };

struct Node {
  std::list<std::shared_ptr<Node>> node;
  bool is_open;
  bool is_dir;
  std::string text;
  std::string data;
};

enum Command_Kind {
  Command_Kind_None,
  Command_Kind_ShowAllCommands,
  Command_Kind_ShowAllWorkspaces,
  Command_Kind_ShowAllViews,
};

struct Command {
  Command_Kind kind;
};

class UI {
  class Master_Popup {
   private:
    UI *ui_ctx;

    std::string title;

   public:
    Master_Popup(UI *ui_ctx);

    void draw();
    void open();

    bool is_open();
  };

 private:
  ImVec2 root_size;
  Area curr_area;

  std::shared_ptr<Master_Popup> master_popup;

  std::vector<std::shared_ptr<Node>> workspaces;
  std::vector<std::shared_ptr<Node>> files;
  std::weak_ptr<Node> curr_node;

  std::vector<Command> commands;

 public:
  UI();

  void update();
  void process_keybinds();

  void set_root_size(const ImVec2 new_size);
};
}  // namespace Saura

#endif
