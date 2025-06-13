#include "app.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "saura/core/os/os.hpp"

namespace Saura {
App::App() { is_running = true; }

void App::init() {
  ui_ctx = std::make_unique<UI>();
  os_ctx = std::make_unique<OS>();
  os_ctx->init();
}

void App::deinit() { os_ctx->deinit(); }

void App::update() {
  const double fps_max = 60.0;
  const double period_max = 1.0 / fps_max;
  const double perf_frequency = os_ctx->get_performance_frequency();

  double time = 0.0;
  double begin_counter = 0.0;
  double end_counter = 0.0;

  while (is_running) {
    begin_counter = os_ctx->get_performance_counter();

    double counter_elapsed = begin_counter - end_counter;
    double dt = counter_elapsed / perf_frequency;
    double fps = perf_frequency / counter_elapsed;

    if (dt >= period_max) {
      if (dt >= 1.0) {
        dt = period_max;
      }

      os_ctx->update();
      os_ctx->draw_begin();
      ui_ctx->set_root_size({(float)os_ctx->get_window_width(),
                             (float)os_ctx->get_window_height()});
      ui_ctx->update();
      os_ctx->draw_end();
    }

    if (!os_ctx->update_events()) {
      is_running = false;
    }

    os_ctx->sleep(period_max);
  }
}

json App::read_config() {
  json ret;
  fs::path config_path = os_ctx->get_home_config_path();
  fs::path app_path = config_path / "SauraStudios" / "Notes";
  fs::path app_config = app_path / "config.json";

  if (fs::exists(app_path) && fs::is_directory(app_path)) {
    std::ifstream f(app_config);
    if (fs::exists(app_config)) {
      ret = json::parse(f);
    } else {
      ret["vaults"] = {};
    }
  } else {
    std::cout << "Folder does not exist." << std::endl;
    fs::create_directories(app_path);
  }
  return ret;
}

bool App::save_config(const json &new_config) {
  fs::path config_path = os_ctx->get_home_config_path();
  fs::path app_path = config_path / "SauraStudios" / "Notes";
  fs::path app_config = app_path / "config.json";

  std::ofstream file(app_config);
  if (file.is_open()) {
    file << new_config.dump() << std::endl;
    file.close();
    std::cout << "File created successfully!" << std::endl;
  } else {
    std::cerr << "Error creating file." << std::endl;
    return false;
  }

  return true;
}
}  // namespace Saura
