#ifndef OS_HPP_
#define OS_HPP_

#include <filesystem>

#include "vendor/SDL3/include/SDL3/SDL_render.h"
#include "vendor/SDL3/include/SDL3/SDL_video.h"

namespace fs = std::filesystem;

namespace Saura {
struct Window_Context {
  int w, h;
  SDL_Window *handle;
  SDL_Renderer *renderer;
};

class OS {
private:
  std::unique_ptr<Window_Context> window_ctx;

public:
  OS();

  void init();
  void deinit();
  void update();
  bool update_events();
  void draw_begin();
  void draw_end();
  void sleep(double ms);

  int get_window_width();
  int get_window_height();

  double get_performance_frequency();
  double get_performance_counter();
};

fs::path home_config_path();
} // namespace Saura

#endif
