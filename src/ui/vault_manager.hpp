#ifndef VAULT_MANAGER_HPP_
#define VAULT_MANAGER_HPP_

#include <vendor/imgui/imgui.h>

#include <vector>

#include "vault.hpp"

namespace Saura {
class VaultManager {
 public:
  void draw();
  void create();
  void open();

 private:
  std::vector<std::shared_ptr<Vault>> vaults;
};
}  // namespace Saura

#endif
