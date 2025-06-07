#ifndef VAULT_HPP_
#define VAULT_HPP_

#include <vendor/imgui/imgui.h>

#include <filesystem>
#include <vector>
#include <vendor/json/include/nlohmann/json.hpp>

namespace Saura {
namespace fs = std::filesystem;
using json = nlohmann::json;

class Vault {
 public:
  struct Node {
    fs::path path;
    bool is_dir;
    bool open;
    std::vector<std::shared_ptr<Node>> children;
  };

  std::shared_ptr<Node> root;

  Vault();

 private:
  void refresh();
  void refresh_node(std::shared_ptr<Node> node);
  void draw_node(std::shared_ptr<Node> node);

 public:
  void draw();
};
}  // namespace Saura

#endif
