#ifndef APP_HPP_
#define APP_HPP_

#include <vendor/json/include/nlohmann/json.hpp>

using json = nlohmann::json;

namespace Saura::App {
json read_config();
bool save_config(const json& new_config);
}

#endif
