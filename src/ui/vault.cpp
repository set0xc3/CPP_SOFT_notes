#include "vault.hpp"

namespace Saura {
Vault::Vault() {
  root = std::make_shared<Node>();
  root->is_dir = true;
  root->open = true;
}

void Vault::build_directory_tree(const fs::path& rootPath,
                                 DirectoryNode& rootNode) {
  try {
    for (const auto& entry : fs::directory_iterator(rootPath)) {
      const auto& path = entry.path();
      std::string pathStr = path.filename().string();

      if (fs::is_directory(entry)) {
        DirectoryNode childNode;
        build_directory_tree(path, childNode);
        rootNode.Children[pathStr] = childNode;
      } else if (fs::is_regular_file(entry)) {
        rootNode.Files.push_back(pathStr);
      }
    }
  } catch (...) {
    // Handle errors (if needed)
  }
}

void Vault::draw_node(const DirectoryNode& node) {
  // Папки (уже отсортированы благодаря std::map)
  for (const auto& [name, childNode] : node.Children) {
    if (ImGui::TreeNode(name.c_str())) {
      draw_node(childNode);
      ImGui::TreePop();
    }
  }

  // Файлы
  for (const auto& file : node.Files) {
    ImGui::BulletText("%s", file.c_str());
  }
}

void Vault::draw() {
  DirectoryNode rootNode;
  build_directory_tree(root->path, rootNode);
  draw_node(rootNode);
}

}  // namespace Saura
