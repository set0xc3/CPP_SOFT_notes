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

}  // namespace Saura
