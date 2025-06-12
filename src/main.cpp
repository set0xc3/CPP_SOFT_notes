#include <locale>

#include <vendor/SDL3/include/SDL3/SDL.h>
#include <vendor/imgui/backends/imgui_impl_sdl3.h>
#include <vendor/imgui/backends/imgui_impl_sdlrenderer3.h>
#include <vendor/imgui/imgui.h>

#include "SDL3/SDL_timer.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

int main() {
  std::locale::global(std::locale("en_US.UTF-8"));

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
  SDL_SetRenderVSync(renderer, SDL_RENDERER_VSYNC_ADAPTIVE);
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
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  ImGui::GetIO().Fonts->AddFontFromFileTTF(
      "res/fonts/JetBrainsMonoNerdFontMono-Regular.ttf", 20.0f, NULL,
      ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);

  // Our state
  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Main loop
  bool done = false;

  const double fps_max = 60.0;
  const double period_max = 1.0 / fps_max;
  const double perf_frequency = (double)SDL_GetPerformanceFrequency();

  double time = 0.0;
  double begin_counter = 0.0;
  double end_counter = 0.0;

  while (!done) {
    begin_counter = (double)SDL_GetPerformanceCounter();
    double counter_elapsed = (double)(begin_counter - end_counter);
    double dt = (double)(counter_elapsed / perf_frequency);
    double fps = (double)(perf_frequency / counter_elapsed);

    int window_width, window_height = 0;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type == SDL_EVENT_QUIT)
        done = true;
      if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
          event.window.windowID == SDL_GetWindowID(window))
        done = true;
    }
    if (dt >= period_max) {
      // Ограничиваем dt, чтобы избежать скачков
      if (dt >= 1.0) {
        dt = period_max;
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
      if (ImGui::Begin(
              "Note Editor", nullptr,
              ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
        {
          ImVec2 available = ImGui::GetContentRegionAvail();

          {
            if (ImGui::IsKeyDown(ImGuiKey_F1)) {
              ImGui::OpenPopup("Show All Commands");
            }

            if (ImGui::BeginPopup("Show All Commands",
                                  ImGuiWindowFlags_AlwaysAutoResize)) {
              if (ImGui::IsKeyDown(ImGuiKey_Escape)) {
                ImGui::CloseCurrentPopup();
              }
              ImGui::Text("[Ctrl] + [O]               Navigate To File");
              ImGui::Text("[Ctrl] + [Shift] + [O]     Navigate To Workspace");
              ImGui::Text("[Ctrl] + [F]               Search In Open File");
              ImGui::EndPopup();
            }

            // Аналогично для второго popup’а:
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) &&
                ImGui::IsKeyDown(ImGuiKey_LeftShift) &&
                ImGui::IsKeyDown(ImGuiKey_O)) {
              ImGui::OpenPopup("Navigate To Workspace");
            }

            // Установка позиции popup по позиции мыши
            ImVec2 popup_pos = ImGui::GetMousePos();
            ImGui::SetNextWindowPos(popup_pos, ImGuiCond_Appearing);

            if (ImGui::BeginPopup("Navigate To Workspace",
                                  ImGuiWindowFlags_AlwaysAutoResize)) {
              if (ImGui::IsKeyDown(ImGuiKey_Escape)) {
                ImGui::CloseCurrentPopup();
              }
              ImGui::BeginGroup();
              static char input_path[1024];
              ImGui::InputText("##Input", input_path, sizeof(input_path));
              ImGui::Spacing();
              if (ImGui::TreeNodeEx("Vault", ImGuiTreeNodeFlags_Bullet)) {
                ImGui::TreePop();
              }
              if (ImGui::TreeNodeEx("PARA Template",
                                    ImGuiTreeNodeFlags_Bullet)) {
                ImGui::TreePop();
              }

              ImGui::EndGroup();
              ImGui::EndPopup();
            }
          }

          ImGui::BeginChild("##Empty", {0.0f, 0.0f}, ImGuiChildFlags_Border);

          static ImVec2 group_size;
          ImGui::SetCursorPos(
              {ImGui::GetCursorPos().x + (available.x - group_size.x) * 0.5f,
               ImGui::GetCursorPos().y + (available.y - group_size.y) * 0.5f});
          ImGui::BeginGroup();
          ImGui::Text("- empty -");
          ImGui::EndGroup();
          group_size = ImGui::GetItemRectSize();

          ImGui::EndChild();
        }

        ImGui::End();
      }

      // Rendering
      ImGui::Render();
      SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x,
                         io.DisplayFramebufferScale.y);
      SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y,
                                  clear_color.z, clear_color.w);
      SDL_RenderClear(renderer);
      ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
      SDL_RenderPresent(renderer);

      double sleep_time_ms = period_max;
      SDL_Delay((Uint32)sleep_time_ms * 1000);
    }
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
