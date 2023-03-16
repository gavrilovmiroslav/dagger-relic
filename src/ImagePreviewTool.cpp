#include "ImagePreviewTool.h"

#include "Windowing.h"
#include "Rendering.h"
#include "Textures.h"
#include "Assets.h"

#include <SDL.h>
#include <imgui.h>
#include <string>

using namespace core;

ImagePreviewTool::ImagePreviewTool()
    : filepath{ "" }
{}

void ImagePreviewTool::process_signal(core::EditorAssetSelectedIntent& selection)
{
    if (selection.path.find(".png") != selection.path.npos)
    {
        filepath = selection.path;
    }
}

String ImagePreviewTool::get_menu_tool_name()
{
	return "Image Preview";
}

String ImagePreviewTool::get_tool_window_appendix()
{
    return filepath;
}

void ImagePreviewTool::show_tool_window()
{
    if (filepath != "")
    {
        auto texture = AccessSystem<AssetModule>::access_system()->get_asset<Texture>(filepath);
        if (texture == ecs::no_entity) return;

        auto& state = AccessUnique<WindowingState>::access_unique();
        const auto& textures = AccessStorage<Texture>::access_storage();

        const auto& preview = textures.get<Texture>(texture);
        SDL_Point preview_size;
        SDL_QueryTexture(preview.inner, nullptr, nullptr, &preview_size.x, &preview_size.y);

        auto size = ImGui::GetWindowSize();
        size.x -= 20;
        size.y -= 20;
        size.y = preview_size.y * size.x / preview_size.x;
        ImGui::Image(preview.inner, size);
    }
}