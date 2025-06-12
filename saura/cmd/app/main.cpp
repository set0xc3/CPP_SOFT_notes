#include "saura/app/app.hpp"

int main() {
  auto app_ctx = new (Saura::App);
  app_ctx->init();
  app_ctx->update();
  return 0;
}
