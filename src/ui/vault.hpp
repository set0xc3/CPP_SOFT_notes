#ifndef VAULT_HPP_
#define VAULT_HPP_

#include <vendor/imgui/imgui.h>

#include <filesystem>
#include <vector>
#include <vendor/json/include/nlohmann/json.hpp>

namespace Saura {
namespace fs = std::filesystem;
using json = nlohmann::json;

struct Node {
  fs::path path;
  bool is_dir;
  bool open;
  std::map<fs::path, std::shared_ptr<Node>> children;
};

struct DirectoryNode {
  std::map<std::string, DirectoryNode> Children;
  std::vector<std::string> Files;
};

class Vault {
 public:
  std::shared_ptr<Node> root;

  Vault();

 private:
  void draw_node(const DirectoryNode& node);
  void build_directory_tree(const fs::path& rootPath, DirectoryNode& rootNode);

 public:
  void draw();
};
}  // namespace Saura

#endif
