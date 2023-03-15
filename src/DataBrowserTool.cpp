#include "DataBrowserTool.h"

#include "imgui.h"
#include <filesystem>

#include "Assets.h"
#include "Loaders.h"

DataBrowserTool::DataBrowserTool()
	: current_path{ "data/" }
	, selected_item{ "" }
{}

String DataBrowserTool::get_menu_tool_name()
{
	return "Data Browser";
}

inline void show_data_browser_tooltip(String text) 
{
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) 
	{ 
		ImGui::SetTooltip(text.c_str());
	}
}

void DataBrowserTool::draw_tree(fs::Dir path)
{
	// TODO (reported by @mstojanovic): Sus path handling, fix this plez
	auto& name = fs::get_simple_name(path);
	auto fullname = path.path().parent_path().string() + "/" + name;	
	fullname.erase(0, 5);
	std::replace(fullname.begin(), fullname.end(), '\\', '/');
	if (fullname == "") fullname = "<root>";

	if (path.is_directory())
	{
		if (name == "") name = "<root>";
		ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
		if (ImGui::TreeNode(name.c_str())) {
			show_data_browser_tooltip(fullname);

			for (const auto& entry : fs::DirIter(path.path()))
			{
				draw_tree(entry);
			}
			ImGui::TreePop();
		}
		else
		{
			show_data_browser_tooltip(fullname);
		}
	}
	else 
	{
		if (path.path().extension() == ".png")
		{
			if (AccessSystem<core::AssetModule>::access_system()->contains<Texture>(fullname))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
			}
		}

		if (ImGui::MenuItem(name.c_str())) 
		{
			auto item_name = path.path().string().erase(0, 5);
			std::replace(item_name.begin(), item_name.end(), '\\', '/');

			selected_item = item_name;
			SignalEmitter<core::EditorAssetSelectedIntent>::emit(core::EditorAssetSelectedIntent{ item_name.c_str() });
		}
		ImGui::PopStyleColor();

		show_data_browser_tooltip(fullname);
	}
}

void DataBrowserTool::show_tool_window()
{
	ImGui::SetNextWindowSize(ImVec2(500, 800), ImGuiCond_FirstUseEver);
	auto size = ImGui::GetWindowSize();
	size.x -= 20;
	size.y -= 40;
	ImGui::BeginChild("DataBrowser", size, false);
	size.y = 20;

	draw_tree(fs::Dir { current_path });
	ImGui::EndChild();
}
