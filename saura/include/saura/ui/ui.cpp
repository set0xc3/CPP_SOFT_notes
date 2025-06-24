#include "ui.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "saura/config/config.hpp"

namespace fs = std::filesystem;

// ---------------------------------------------------------------------------
// Camera state (could be globals or members of some struct/class)
static ImVec2 camPos  = ImVec2 (0.0f, 0.0f); // world-space center
static float  camZoom = 1.0f;				 // scale factor

// ---------------------------------------------------------------------------
// Utility: transform world->screen
static ImVec2
WorldToScreen (const ImVec2& world, const ImVec2& screenCenter)
{
	return ImVec2 (
		(world.x - camPos.x) * camZoom + screenCenter.x,
		(world.y - camPos.y) * camZoom + screenCenter.y);
}

// Utility: compute world bounds of the viewport
static ImVec4
GetWorldBounds (const ImVec2& screenSize, const ImVec2& screenCenter)
{
	ImVec2 topLeft = ImVec2 (
		(0.0f - screenCenter.x) / camZoom + camPos.x,
		(0.0f - screenCenter.y) / camZoom + camPos.y);
	ImVec2 bottomRight = ImVec2 (
		(screenSize.x - screenCenter.x) / camZoom + camPos.x,
		(screenSize.y - screenCenter.y) / camZoom + camPos.y);
	return ImVec4 (topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
}

// ---------------------------------------------------------------------------
// Demo
void
Show2DCameraDemo ()
{
	ImGuiIO& io			  = ImGui::GetIO ();
	ImVec2	 ioDisplay	  = io.DisplaySize;
	ImVec2	 screenCenter = ImVec2 (ioDisplay.x * 0.5f, ioDisplay.y * 0.5f);

	ImGui::SetNextWindowPos (ImVec2 (0, 0));
	ImGui::SetNextWindowSize (ioDisplay);
	ImGui::Begin ("##CamCapture", nullptr,
				  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
					  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
					  ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground |
					  ImGuiWindowFlags_NoBringToFrontOnFocus);

	ImDrawList* draw = ImGui::GetWindowDrawList ();

	// Zoom (mouse wheel)
	float wheel = io.MouseWheel;
	if (wheel != 0.0f)
	{
		ImVec2 mouseScreen = io.MousePos;
		ImVec2 before	   = ImVec2 (
			 (mouseScreen.x - screenCenter.x) / camZoom + camPos.x,
			 (mouseScreen.y - screenCenter.y) / camZoom + camPos.y);

		camZoom *= (wheel > 0) ? 1.1f : 0.9f;
		camZoom = std::clamp (camZoom, 0.02f, 20.0f);

		camPos = ImVec2 (
			before.x - (mouseScreen.x - screenCenter.x) / camZoom,
			before.y - (mouseScreen.y - screenCenter.y) / camZoom);
	}

	// Panning (right mouse button)
	if (ImGui::IsMouseDragging (ImGuiMouseButton_Right))
	{
		ImVec2 delta = io.MouseDelta;
		camPos.x -= delta.x / camZoom;
		camPos.y -= delta.y / camZoom;
	}

	// Draw adaptive grid
	float baseSpacing = 50.0f;
	float zoomLevel	  = camZoom;

	// Snap spacing to power-of-two multiples
	float spacing = baseSpacing;
	while (spacing * zoomLevel < 20.0f)
		spacing *= 2.0f;
	while (spacing * zoomLevel > 100.0f)
		spacing *= 0.5f;

	ImVec4 bounds = GetWorldBounds (ioDisplay, screenCenter);

	int xMin = static_cast<int> (std::floor (bounds.x / spacing));
	int xMax = static_cast<int> (std::ceil (bounds.z / spacing));
	int yMin = static_cast<int> (std::floor (bounds.y / spacing));
	int yMax = static_cast<int> (std::ceil (bounds.w / spacing));

	ImU32 gridColor = IM_COL32 (200, 200, 200, 50);
	ImU32 axisColor = IM_COL32 (255, 255, 0, 100);

	for (int x = xMin; x <= xMax; ++x)
	{
		ImVec2 p0  = WorldToScreen (ImVec2 (x * spacing, bounds.y), screenCenter);
		ImVec2 p1  = WorldToScreen (ImVec2 (x * spacing, bounds.w), screenCenter);
		ImU32  col = (x == 0) ? axisColor : gridColor;
		draw->AddLine (p0, p1, col);
	}

	for (int y = yMin; y <= yMax; ++y)
	{
		ImVec2 p0  = WorldToScreen (ImVec2 (bounds.x, y * spacing), screenCenter);
		ImVec2 p1  = WorldToScreen (ImVec2 (bounds.z, y * spacing), screenCenter);
		ImU32  col = (y == 0) ? axisColor : gridColor;
		draw->AddLine (p0, p1, col);
	}

	// Draw animated circle
	float  t			= ImGui::GetTime ();
	ImVec2 circleCenter = WorldToScreen (ImVec2 (sinf (t) * 200, cosf (t) * 150), screenCenter);
	float  circleRadius = 30.0f * camZoom;
	draw->AddCircleFilled (circleCenter, circleRadius, IM_COL32 (255, 100, 100, 200));

	// UI Overlay
	ImGui::SetCursorScreenPos (ImVec2 (10, 10));
	ImGui::Text ("Use Right-drag to pan, MouseWheel to zoom (%.2fx)", camZoom);

	ImGui::End ();
}

namespace Saura
{
UI::Master_Popup::Master_Popup (UI* _ui_ctx)
{
	ui_ctx = _ui_ctx;
	title  = "[popop:master]";
}

void
UI::Master_Popup::draw ()
{
	ImVec2 size{500.0f, 300.0f};
	ImVec2 padding{0.0f, 12.0f};
	auto   reg_min	 = ImGui::GetWindowContentRegionMin ();
	auto   reg_avail = ImGui::GetContentRegionAvail ();

	ImGui::SetNextWindowPos (
		{(reg_avail.x - size.x) * 0.5f, reg_min.y + padding.y});
	ImGui::SetNextWindowSize ({size.x, 0.0f});

	if (ImGui::BeginPopup ("[popop:master]"))
	{
		for (auto& node : ui_ctx->workspaces)
		{
			if (!node->is_dir)
			{
				continue;
			}

			auto& title = node->text;
			if (ImGui::TreeNodeEx (
					title.c_str (),
					ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanFullWidth))
			{
				if (ImGui::IsItemClicked ())
				{
					std::cout << "Clicked" << std::endl;

					if (!fs::exists (title))
					{
						if (fs::create_directories (title))
						{
							std::cout << "Yes" << std::endl;

							std::vector<fs::directory_entry> dirs;
							std::vector<fs::directory_entry> files;

							for (const auto& entry : fs::directory_iterator (title))
							{
								if (entry.is_directory ())
								{
									dirs.push_back (entry);
								}
								else
								{
									files.push_back (entry);
								}
							}

							// Сортируем по имени
							auto sort_by_name = [] (const fs::directory_entry& a,
													const fs::directory_entry& b)
							{
								return a.path ().filename ().string () <
									   b.path ().filename ().string ();
							};

							std::sort (dirs.begin (), dirs.end (), sort_by_name);
							std::sort (files.begin (), files.end (), sort_by_name);

							for (const auto& dir : dirs)
							{
								std::cout << "[DIR]  " << dir.path ().filename ().string ()
										  << '\n';
							}
							for (const auto& file : files)
							{
								std::cout << "[FILE] " << file.path ().filename ().string ()
										  << '\n';
							}
						}
					}
					else
					{
						std::vector<fs::directory_entry> dirs;
						std::vector<fs::directory_entry> files;

						for (const auto& entry : fs::directory_iterator (title))
						{
							if (entry.is_directory ())
							{
								dirs.push_back (entry);
							}
							else
							{
								files.push_back (entry);
							}
						}

						// Сортируем по имени
						auto sort_by_name = [] (const fs::directory_entry& a,
												const fs::directory_entry& b)
						{
							return a.path ().filename ().string () <
								   b.path ().filename ().string ();
						};

						std::sort (dirs.begin (), dirs.end (), sort_by_name);
						std::sort (files.begin (), files.end (), sort_by_name);

						for (const auto& dir : dirs)
						{
							auto new_node	 = std::make_shared<Node> ();
							new_node->is_dir = true;
							new_node->text	 = dir.path ().filename ().string ();
							ui_ctx->files.push_back (new_node);

							std::cout << "[DIR]  " << new_node->text << '\n';
						}
						for (const auto& file : files)
						{
							auto new_node  = std::make_shared<Node> ();
							new_node->text = file.path ().filename ().string ();
							ui_ctx->files.push_back (new_node);

							std::cout << "[FILE] " << new_node->text << '\n';
						}
					}
				}
				ImGui::TreePop ();
			}
		}

		if (ImGui::IsKeyPressed (ImGuiKey_Escape))
		{
			ImGui::CloseCurrentPopup ();
		}
		ImGui::EndPopup ();
	}
}

void
UI::Master_Popup::open ()
{
	ImGui::OpenPopup (title.c_str ());
}

bool
UI::Master_Popup::is_open ()
{
	return ImGui::IsPopupOpen (title.c_str ());
}

UI::UI ()
{
	master_popup = std::make_shared<Master_Popup> (this);

	curr_area = Empty;

// TEST
#if 0
  {
    fs::path file_path = "imgui.ini";
    std::ifstream file(file_path);
    if (!file.is_open()) {
      throw std::runtime_error("Failed open file!");
    }

    auto test_node = std::make_shared<Node>();
    test_node->data = std::string((std::istreambuf_iterator<char>(file)),
                                  std::istreambuf_iterator<char>());
    test_node->text = file_path.filename().string();
    nodes.push_back(test_node);
    curr_node = test_node;

    file.close();
  }
#endif
}

void
UI::update ()
{
	process_keybinds ();

	ImGui::SetNextWindowPos ({});
	ImGui::SetNextWindowSize ({root_size.x, root_size.y});
	if (ImGui::Begin ("[window:main]", nullptr,
					  ImGuiWindowFlags_NoDecoration |
						  ImGuiWindowFlags_NoBackground |
						  ImGuiWindowFlags_NoSavedSettings))
	{
	}
	auto reg_avail = ImGui::GetContentRegionAvail ();

	if (curr_node.lock () == nullptr)
	{
		// ImGui::BeginChild ("##[buffer:empty]", {}, ImGuiChildFlags_Border);
		// {
		// 	const auto main_view_text = "- empty -";
		// 	auto	   main_view_size = ImGui::CalcTextSize (main_view_text);
		// 	ImGui::SetCursorPos ({(reg_avail.x - main_view_size.x) * 0.5f,
		// 						  (reg_avail.y - main_view_size.y) * 0.5f});
		// 	ImGui::Text ("%s", main_view_text);
		// }
		// ImGui::EndChild ();
	}
	else
	{
		ImGui::BeginChild ("##[buffer:view]", {}, ImGuiChildFlags_Border);
		{
			auto main_view_text = curr_node.lock ()->data.c_str ();
			ImGui::Text ("%s", main_view_text);
		}
		ImGui::EndChild ();
	}

	for (auto it = commands.begin (); it != commands.end ();)
	{
		switch (it->kind)
		{
		case Command_Kind_ShowAllCommands:
		{
			if (!master_popup->is_open ())
			{
				master_popup->open ();
			}
			else
			{
				it = commands.erase (it);
			}
			break;
		}
		case Command_Kind_ShowAllWorkspaces:
		{
			if (!master_popup->is_open ())
			{
				master_popup->open ();
			}
			else
			{
				workspaces.clear ();
				it = commands.erase (it);
			}

			auto config = Saura::Config::read_config ();
			if (!config.empty ())
			{
				for (auto [path, _] : config["vaults"].items ())
				{
					auto node	 = std::make_shared<Node> ();
					node->is_dir = true;
					node->text	 = path;
					workspaces.push_back (node);
				}
			}

			break;
		}
		case Command_Kind_ShowAllViews:
		{
			if (!master_popup->is_open ())
			{
				master_popup->open ();
			}
			else
			{
				files.clear ();
				it = commands.erase (it);
			}
			break;
		}
		default:
		{
			it += 1;
			break;
		}
		}
	}

	master_popup->draw ();

	Show2DCameraDemo ();

	ImGui::End ();
}

// TODO: Refactoring.
void
UI::process_keybinds ()
{
	if (ImGui::IsKeyPressed (ImGuiKey_F1))
	{
		std::cout << "[F1] Show All Commands" << std::endl;
		Command cmd = {
			.kind = Command_Kind_ShowAllCommands,
		};
		commands.push_back (cmd);
		return;
	}

	// Workspace
	if (ImGui::IsKeyDown (ImGuiKey_LeftCtrl) &&
		ImGui::IsKeyDown (ImGuiKey_LeftShift) && ImGui::IsKeyPressed (ImGuiKey_O))
	{
		Command cmd = {
			.kind = Command_Kind_ShowAllWorkspaces,
		};
		commands.push_back (cmd);
		std::cout << "[Ctrl+Shift+O] Show All Workspaces" << std::endl;
		return;
	}
	if (ImGui::IsKeyDown (ImGuiKey_LeftCtrl) &&
		ImGui::IsKeyDown (ImGuiKey_LeftShift) && ImGui::IsKeyPressed (ImGuiKey_F))
	{
		std::cout << "[Ctrl+Shift+F] Search In All Workspaces" << std::endl;
		return;
	}

	// File
	if (ImGui::IsKeyDown (ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed (ImGuiKey_O))
	{
		std::cout << "[Ctrl+O] Search File By Name" << std::endl;
		return;
	}
	if (ImGui::IsKeyDown (ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed (ImGuiKey_F))
	{
		std::cout << "[Ctrl+F] Search In File" << std::endl;
		return;
	}
}

void
UI::set_root_size (const ImVec2 new_size)
{
	root_size = new_size;
}

} // namespace Saura
