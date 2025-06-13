#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <vendor/json/include/nlohmann/json.hpp>

namespace Saura::Config {

using json = nlohmann::json;

json read_config();
bool save_config(const json &new_config);

};  // namespace Saura::Config

#endif
