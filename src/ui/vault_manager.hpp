#ifndef VAULT_MANAGER_HPP_
#define VAULT_MANAGER_HPP_

#include <map>
#include <memory>

#include "vault.hpp"

namespace saura {

using json = nlohmann::json;

class VaultManager {
 public:
  void init();
  void draw();
  void create();
  void open();

  json read_config();
  bool save_config(const json& new_config);

 private:
  void draw_vault_manager();
  void draw_vault();

 private:
  std::map<fs::path, std::shared_ptr<Vault>> vaults;
  std::weak_ptr<Vault> hot_vault;
};
}  // namespace saura

#endif
