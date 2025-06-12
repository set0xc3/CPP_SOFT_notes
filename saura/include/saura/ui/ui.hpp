#ifndef UI_HPP_
#define UI_HPP_

#include <vendor/imgui/imgui.h>

#include <list>
#include <string>

namespace Saura {
class UI {
  class Node {
    std::list<Node> node;
    std::string title;
  };

private:
  ImVec2 root_size;

public:
  void update();
  void set_root_size(const ImVec2 new_size);
};
} // namespace Saura

#endif
