#include <vendor/SDL3/include/SDL3/SDL.h>
#include <vendor/imgui/backends/imgui_impl_sdl3.h>
#include <vendor/imgui/backends/imgui_impl_sdlrenderer3.h>
#include <vendor/imgui/imgui.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <vendor/json/include/nlohmann/json.hpp>
#include "src/ui.hpp"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

namespace fs = std::filesystem;
using json = nlohmann::json;

class Tree {
 public:
  struct Node {
    fs::path path;
    bool is_dir;
    bool open;
    std::vector<std::shared_ptr<Node>> children;
  };

  std::shared_ptr<Node> root;
  fs::path selected;

  Tree() {
    root = std::make_shared<Node>();
    root->path = fs::current_path();
    root->is_dir = true;
    root->open = false;

    std::ifstream f("example.json");
    json data = json::parse(f);
    std::cout << data << std::endl;

    refresh();
  }

  void refresh() {
    if (root->is_dir) {
      refresh_node(root);
    }
  }

 private:
  void refresh_node(std::shared_ptr<Node> node) {
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

  void draw_node(std::shared_ptr<Node> node) {
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

 public:
  void draw() { draw_node(root); }
};

int main() {
  // Setup SDL
  // [If using SDL_MAIN_USE_CALLBACKS: all code below until the main loop starts
  // would likely be your SDL_AppInit() function]
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    printf("Error: SDL_Init(): %s\n", SDL_GetError());
    return -1;
  }

  // Create window with SDL_Renderer graphics context
  SDL_WindowFlags window_flags =
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
  SDL_Window *window =
      SDL_CreateWindow("Dear ImGui SDL3+SDL_Renderer example", WINDOW_WIDTH,
                       WINDOW_HEIGHT, window_flags);
  if (window == nullptr) {
    printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
    return -1;
  }
  SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
  SDL_SetRenderVSync(renderer, 1);
  if (renderer == nullptr) {
    SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
    return -1;
  }
  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(window);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);

  // Our state
  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Main loop
  bool done = false;

  std::unique_ptr<Tree> tree = std::make_unique<Tree>();
  std::unique_ptr<ManagerVaults> manager_vaults = std::make_unique<ManagerVaults>();

  while (!done) {
    int window_width, window_height = 0;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type == SDL_EVENT_QUIT) done = true;
      if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
          event.window.windowID == SDL_GetWindowID(window))
        done = true;
    }

    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
      SDL_Delay(10);
      continue;
    }

    SDL_GetWindowSize(window, &window_width, &window_height);

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({(float)window_width, (float)window_height});
    if (ImGui::Begin("Note Editor", nullptr,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
      manager_vaults->draw();
    }

    ImGui::End();

    // Rendering
    ImGui::Render();
    SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x,
                       io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y,
                                clear_color.z, clear_color.w);
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
  }

  // Cleanup
  // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your
  // SDL_AppQuit() function]
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
