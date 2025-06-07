#include "vault.hpp"

namespace Saura {
Vault::Vault() {
  root = std::make_shared<Node>();
  root->is_dir = true;
  root->open = true;
  refresh();
}

void Vault::refresh() {
  if (root->is_dir) {
    refresh_node(root);
  }
}

void Vault::refresh_node(std::shared_ptr<Node> node) {
  if (!node || !node->is_dir) return;

  // node->children.clear();

  // try {
  //   for (const auto& entry : fs::directory_iterator(node->path)) {
  //     auto child = std::make_shared<Node>();
  //     child->path = entry.path();
  //     child->is_dir = fs::is_directory(entry);
  //     child->open = false;
  //     node->children.push_back(child);
  //   }
  // } catch (...) {
  // }
}

void Vault::draw_node(std::shared_ptr<Node> node) {
  if (!node) return;

  ImGuiTreeNodeFlags flags = (node->is_dir ? ImGuiTreeNodeFlags_OpenOnArrow
                                           : ImGuiTreeNodeFlags_Leaf) |
                             ImGuiTreeNodeFlags_SpanFullWidth;

  // ImGui::TreeNodeEx("Vault", flags);

  // if (node->is_dir && !node->open) {
  //   refresh_node(node);
  //   node->open = true;
  // }

  // for (auto &child : node->children) {
  //   draw_node(child);
  // }

  // ImGui::TreePop();
}

void Vault::draw() { draw_node(root); }

}  // namespace Saura
