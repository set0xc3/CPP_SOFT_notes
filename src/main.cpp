#include <vendor/imgui/imgui.h>
#include <vendor/imgui/backends/imgui_impl_sdl3.h>
#include <vendor/imgui/backends/imgui_impl_sdlrenderer3.h>
#include <vendor/SDL3/include/SDL3/SDL.h>

#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <optional>
#include <set>

namespace fs = std::filesystem;

class FileTree {
public:
    struct Node {
        fs::path path;
        bool is_directory;
        bool open = false;
        std::vector<Node> children;
    };

    Node root;
    std::set<fs::path> selected_paths;
    std::optional<fs::path> rename_target;
    char rename_buffer[1024] = "";
    bool creating_new = false;
    bool is_creating_directory = false;
    char new_name_buffer[1024] = "";
    fs::path create_parent_path;

    FileTree(const fs::path& root_path) : root{root_path, fs::is_directory(root_path)} {
        refresh();
    }

    void refresh() {
        refresh_node(root);
    }

    void refresh_node(Node& node) {
        if (!node.is_directory) return;
        
        std::vector<Node> new_children;
        for (const auto& entry : fs::directory_iterator(node.path)) {
            new_children.push_back({entry.path(), entry.is_directory()});
        }

        std::sort(new_children.begin(), new_children.end(), [](const Node& a, const Node& b) {
            if (a.is_directory != b.is_directory)
                return a.is_directory > b.is_directory;
            return a.path.filename().string() < b.path.filename().string();
        });

        // Preserve open state
        for (auto& new_child : new_children) {
            auto it = std::find_if(node.children.begin(), node.children.end(),
                [&](const Node& old) { return old.path == new_child.path; });
            if (it != node.children.end()) {
                new_child.open = it->open;
            }
        }

        node.children = std::move(new_children);
    }

    void draw() {
        if (creating_new) {
            ImGui::OpenPopup("Create New");
            creating_new = false;
        }

        if (ImGui::BeginPopupModal("Create New", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Enter name:");
            bool create = false;
            
            if (ImGui::InputText("##new_name", new_name_buffer, sizeof(new_name_buffer), 
                ImGuiInputTextFlags_EnterReturnsTrue)) {
                create = true;
            }
            
            if (ImGui::Button("Create")) {
                create = true;
            }
            
            ImGui::SameLine();
            if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                ImGui::CloseCurrentPopup();
                new_name_buffer[0] = '\0';
            }
            
            if (create && new_name_buffer[0] != '\0') {
                auto new_path = create_parent_path / new_name_buffer;
                try {
                    if (is_creating_directory) {
                        fs::create_directory(new_path);
                    } else {
                        std::ofstream(new_path).put(' ');
                    }
                    refresh();
                } catch (...) {
                    // Handle error
                }
                new_name_buffer[0] = '\0';
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::EndPopup();
        }

        draw_node(root);
    }

private:
    void draw_node(Node& node) {
        ImGuiTreeNodeFlags flags = (node.is_directory ? ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Leaf) | 
                                  ImGuiTreeNodeFlags_SpanFullWidth;
        
        bool is_selected = selected_paths.count(node.path);
        if (is_selected) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        bool is_open = ImGui::TreeNodeEx(node.path.filename().string().c_str(), flags);
        
        // Handle selection
        if (ImGui::IsItemClicked()) {
            if (ImGui::GetIO().KeyCtrl) {
                if (is_selected) {
                    selected_paths.erase(node.path);
                } else {
                    selected_paths.insert(node.path);
                }
            } else {
                selected_paths.clear();
                selected_paths.insert(node.path);
            }
        }

        // Drag and drop target
        if (node.is_directory && ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_TREE_NODES")) {
                const fs::path* dropped_paths = static_cast<const fs::path*>(payload->Data);
                size_t count = payload->DataSize / sizeof(fs::path);
                
                for (size_t i = 0; i < count; i++) {
                    const fs::path& src_path = dropped_paths[i];
                    const fs::path dst_path = node.path / src_path.filename();
                    
                    // Prevent moving into self or equivalent paths
                    if (!fs::equivalent(src_path.parent_path(), node.path) && 
                        !fs::equivalent(src_path, node.path)) {
                        try {
                            // Check if destination exists
                            if (!fs::exists(dst_path)) {
                                fs::rename(src_path, dst_path);
                            }
                        } catch (...) {
                            // Handle error
                        }
                    }
                }
                refresh();
            }
            ImGui::EndDragDropTarget();
        }

        // Context menu
        if (ImGui::BeginPopupContextItem()) {
            if (node.is_directory) {
                if (ImGui::MenuItem("New File")) {
                    create_parent_path = node.path;
                    is_creating_directory = false;
                    creating_new = true;
                    new_name_buffer[0] = '\0';
                }
                if (ImGui::MenuItem("New Folder")) {
                    create_parent_path = node.path;
                    is_creating_directory = true;
                    creating_new = true;
                    new_name_buffer[0] = '\0';
                }
            }
            
            if (ImGui::MenuItem("Rename")) {
                rename_target = node.path;
                auto filename = node.path.filename().string();
                strncpy(rename_buffer, filename.c_str(), sizeof(rename_buffer) - 1);
                rename_buffer[sizeof(rename_buffer) - 1] = '\0';
            }
            
            if (ImGui::MenuItem("Delete")) {
                try {
                    if (node.is_directory) {
                        fs::remove_all(node.path);
                    } else {
                        fs::remove(node.path);
                    }
                    refresh();
                } catch (...) {
                    // Handle error
                }
                selected_paths.erase(node.path);
            }
            
            ImGui::EndPopup();
        }

        // Rename input
        if (rename_target == node.path) {
            ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::InputText("##rename", rename_buffer, sizeof(rename_buffer), 
                                 ImGuiInputTextFlags_EnterReturnsTrue)) {
                auto new_path = node.path.parent_path() / rename_buffer;
                try {
                    if (!fs::exists(new_path)) {
                        fs::rename(node.path, new_path);
                        rename_target.reset();
                        refresh();
                    }
                } catch (...) {
                    // Handle error
                }
            }
            if (ImGui::IsItemDeactivated() || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                rename_target.reset();
            }
            ImGui::PopItemWidth();
        }

        // Drag and drop source
        if (ImGui::BeginDragDropSource()) {
            std::vector<fs::path> drag_paths;
            if (selected_paths.empty()) {
                drag_paths.push_back(node.path);
            } else {
                drag_paths.assign(selected_paths.begin(), selected_paths.end());
            }
            
            ImGui::SetDragDropPayload("FILE_TREE_NODES", drag_paths.data(), 
                                     drag_paths.size() * sizeof(fs::path), ImGuiCond_Once);
            
            if (drag_paths.size() > 1) {
                ImGui::Text("Moving %zu items", drag_paths.size());
            } else {
                ImGui::Text("Move %s", node.path.filename().string().c_str());
            }
            
            ImGui::EndDragDropSource();
        }

        // Process children
        if (is_open) {
            if (node.is_directory) {
                if (node.open) {
                    for (auto& child : node.children) {
                        draw_node(child);
                    }
                } else {
                    refresh_node(node);
                    node.open = true;
                }
            }
            ImGui::TreePop();
        }
    }
};

class NoteEditor {
public:
    void open_file(const fs::path& path) {
        if (current_file == path) return;
        save();
        current_file = path;
        need_refresh = true;
    }

    void close_file() {
        save();
        current_file.reset();
        text_content.clear();
    }

    void draw() {
        if (!current_file) return;

        if (need_refresh) {
            text_content.clear();
            std::ifstream file(*current_file);
            if (file) {
                text_content.assign(
                    std::istreambuf_iterator<char>(file),
                    std::istreambuf_iterator<char>()
                );
            }
            need_refresh = false;
            changed = false;
        }

        // Создаем временный буфер достаточного размера
        std::vector<char> buffer;
        buffer.resize(text_content.size() + 1024, '\0');
        std::copy(text_content.begin(), text_content.end(), buffer.begin());

        if (ImGui::InputTextMultiline("##editor", buffer.data(), buffer.size(), 
                                     ImVec2(-1, -1))) {
            text_content = buffer.data();
            changed = true;
        }

        if (changed) {
            ImGui::Text("Modified");
            ImGui::SameLine();
            if (ImGui::Button("Save")) {
                save();
            }
        }
    }

private:
    void save() {
        if (current_file && changed) {
            std::ofstream file(*current_file);
            file << text_content;
            changed = false;
        }
    }

    std::optional<fs::path> current_file;
    std::string text_content;
    bool changed = false;
    bool need_refresh = false;
};

int main()
{
    // Setup SDL
    // [If using SDL_MAIN_USE_CALLBACKS: all code below until the main loop starts would likely be your SDL_AppInit() function]
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return -1;
    }

    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    SDL_Window *window = SDL_CreateWindow("Dear ImGui SDL3+SDL_Renderer example", 1280, 720, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderVSync(renderer, 1);
    if (renderer == nullptr)
    {
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
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    // IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;

    FileTree file_tree(".");
    NoteEditor editor;

    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        // [If using SDL_MAIN_USE_CALLBACKS: call ImGui_ImplSDL3_ProcessEvent() from your SDL_AppEvent() function]
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // Main application window
        if (ImGui::Begin("Note Editor", nullptr, ImGuiWindowFlags_NoCollapse)) {
            // File tree
            ImGui::BeginChild("Tree", ImVec2(300, 0), true);
            file_tree.draw();
            ImGui::EndChild();

            ImGui::SameLine();

            // Editor
            ImGui::BeginChild("Editor", ImVec2(0, 0), true);
            if (!file_tree.selected_paths.empty()) {
                const fs::path& path = *file_tree.selected_paths.begin();
                if (!fs::is_directory(path)) {
                    if (ImGui::Button("Close File")) {
                        editor.close_file();
                        file_tree.selected_paths.clear();
                    }
                    ImGui::SameLine();
                    editor.open_file(path);
                    editor.draw();
                }
            }
            ImGui::EndChild();
        }

        ImGui::End();

        // // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        // if (show_demo_window)
        //     ImGui::ShowDemoWindow(&show_demo_window);

        // // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        // {
        //     static float f = 0.0f;
        //     static int counter = 0;

        //     ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

        //     ImGui::Text("This is some useful text.");          // Display some text (you can use a format strings too)
        //     ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
        //     ImGui::Checkbox("Another Window", &show_another_window);

        //     ImGui::SliderFloat("float", &f, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
        //     ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats representing a color

        //     if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
        //         counter++;
        //     ImGui::SameLine();
        //     ImGui::Text("counter = %d", counter);

        //     ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        //     ImGui::End();
        // }

        // // 3. Show another simple window.
        // if (show_another_window)
        // {
        //     ImGui::Begin("Another Window", &show_another_window); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        //     ImGui::Text("Hello from another window!");
        //     if (ImGui::Button("Close Me"))
        //         show_another_window = false;
        //     ImGui::End();
        // }

        // Rendering
        ImGui::Render();
        SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}