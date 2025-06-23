#include "os.hpp"

#include <vendor/SDL3/include/SDL3/SDL.h>
#include <vendor/imgui/backends/imgui_impl_sdl3.h>
#include <vendor/imgui/backends/imgui_impl_sdlrenderer3.h>
#include <vendor/imgui/imgui.h>

#include <filesystem>
#include <locale>
#include <string>

#include "SDL3/SDL_video.h"

namespace Saura
{
OS::OS ()
{
	window_ctx		  = std::make_unique<Window_Context> ();
	window_ctx->title = "(Saura Studios) [Dev] Notes";
	window_ctx->w	  = 1280 / 2;
	window_ctx->h	  = 720 / 2;
}

void
OS::init ()
{
	std::locale::global (std::locale ("en_US.UTF-8"));

	if (!SDL_Init (SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
	{
		throw std::runtime_error ("Failed SDL_Init!");
	}

	SDL_WindowFlags window_flags =
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
	window_ctx->handle = SDL_CreateWindow (
		window_ctx->title.c_str (), window_ctx->w, window_ctx->h, window_flags);
	if (window_ctx->handle == nullptr)
	{
		throw std::runtime_error ("Failed SDL_CreateWindow!");
	}

	window_ctx->renderer = SDL_CreateRenderer (window_ctx->handle, nullptr);
	if (window_ctx->renderer == nullptr)
	{
		throw std::runtime_error ("Failed SDL_CreateRenderer!");
	}
	SDL_SetRenderVSync (window_ctx->renderer, SDL_RENDERER_VSYNC_ADAPTIVE);
	SDL_SetWindowPosition (window_ctx->handle, SDL_WINDOWPOS_CENTERED,
						   SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow (window_ctx->handle);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION ();
	ImGui::CreateContext ();
	ImGuiIO& io = ImGui::GetIO ();
	(void) io;
	io.ConfigFlags |=
		ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |=
		ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark ();

	std::string defualt_font_path =
		"res/fonts/JetBrainsMonoNerdFontMono-Regular.ttf";
	if (fs::exists (defualt_font_path))
	{
		ImGui::GetIO ().Fonts->AddFontFromFileTTF (
			defualt_font_path.c_str (), 20.0f, nullptr,
			ImGui::GetIO ().Fonts->GetGlyphRangesCyrillic ());
	}

	// Setup Platform/Renderer backends
	ImGui_ImplSDL3_InitForSDLRenderer (window_ctx->handle, window_ctx->renderer);
	ImGui_ImplSDLRenderer3_Init (window_ctx->renderer);
}

void
OS::deinit ()
{
	ImGui_ImplSDLRenderer3_Shutdown ();
	ImGui_ImplSDL3_Shutdown ();
	ImGui::DestroyContext ();

	SDL_DestroyRenderer (window_ctx->renderer);
	SDL_DestroyWindow (window_ctx->handle);
	SDL_Quit ();
}

void
OS::update ()
{
	SDL_GetWindowSize (window_ctx->handle, &window_ctx->w, &window_ctx->h);
}

bool
OS::update_events ()
{
	SDL_Event event;
	while (SDL_PollEvent (&event))
	{
		ImGui_ImplSDL3_ProcessEvent (&event);

		switch (event.type)
		{
		case SDL_EVENT_QUIT:
		{
			return false;
		}
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
		{
			if (event.window.windowID == SDL_GetWindowID (window_ctx->handle))
			{
				return false;
			}
			break;
		}
		}
	}
	return true;
}

void
OS::draw_begin ()
{
	ImGui_ImplSDLRenderer3_NewFrame ();
	ImGui_ImplSDL3_NewFrame ();
	ImGui::NewFrame ();
}

void
OS::draw_end ()
{
	ImVec4	 clear_color = {0.0f, 0.0f, 0.0f, 0.0f};
	ImGuiIO& io			 = ImGui::GetIO ();
	(void) io;
	ImGui::Render ();
	SDL_SetRenderScale (window_ctx->renderer, io.DisplayFramebufferScale.x,
						io.DisplayFramebufferScale.y);
	SDL_SetRenderDrawColorFloat (window_ctx->renderer, clear_color.x,
								 clear_color.y, clear_color.z, clear_color.w);
	SDL_RenderClear (window_ctx->renderer);
	ImGui_ImplSDLRenderer3_RenderDrawData (ImGui::GetDrawData (),
										   window_ctx->renderer);
	SDL_RenderPresent (window_ctx->renderer);
}

void
OS::sleep (double ms)
{
	SDL_Delay ((Uint32) ms * 1000);
}

int
OS::get_window_width ()
{
	return window_ctx->w;
}

int
OS::get_window_height ()
{
	return window_ctx->h;
}

double
OS::get_performance_frequency ()
{
	return (double) SDL_GetPerformanceFrequency ();
}

double
OS::get_performance_counter ()
{
	return (double) SDL_GetPerformanceCounter ();
}

std::string
OS::get_safe_getenv (const std::string key)
{
	return SDL_getenv (key.c_str ());
}

fs::path
OS::get_home_config_path ()
{
	fs::path res = {};

#if defined(_WIN32)
	auto config = get_safe_getenv ("APPDATA");

	if (config.empty ())
	{
		throw std::runtime_error ("APPDATA environment variable not found");
	}

	res = fs::path (config);
#elif defined(__linux__)
	auto config = get_safe_getenv ("HOME");

	if (config.empty ())
	{
		throw std::runtime_error ("HOME environment variable not found");
	}

	res = fs::path (config) / ".config";
#endif

	return res;
}

std::string
OS::get_user_name ()
{
	return get_safe_getenv ("USERNAME");
}
} // namespace Saura
