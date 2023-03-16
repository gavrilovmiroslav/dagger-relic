#include "SpriteEditorTool.h"

#include "SpriteRendering.h"
#include "Textures.h"

#include "imgui.h"

#include <yaml-cpp/yaml.h>

SpriteEditorTool::SpriteEditorTool()
    : texture_filepath{ "" }
    , spritesheet_filepath{ "" }
{}

String SpriteEditorTool::get_menu_tool_name()
{
	return "Sprites";
}

void SpriteEditorTool::process_signal(core::EditorAssetSelectedIntent& selection)
{
    fs::Dir path{ selection.path };
    auto extension = path.path().extension().string();

    if (extension == String(".png"))
    {
        texture_filepath = selection.path;
        spritesheet_filepath = selection.path + ".spritesheet.yaml";
    }
    else if (extension == String(".yaml"))
    {
        if (selection.path.find(".spritesheet.yaml") != selection.path.npos)
        {
            spritesheet_filepath = selection.path;
            texture_filepath = selection.path.substr(0, selection.path.length() - 17);
        }
        else
        {
            spritesheet_filepath = "";
        }
    }
    else
    {
        spritesheet_filepath = "";
    }

    if (spritesheet_filepath != "")
    {
        load_spritesheet();
    }
}

namespace ImGui
{
    void Rect(ImVec2& start_pos, ImVec2& end_pos)
    {
        ImDrawList* draw_list = ImGui::GetForegroundDrawList();
        draw_list->AddRect(start_pos, end_pos, ImGui::GetColorU32(IM_COL32(0, 130, 216, 255)));
    }

    void Dot(ImVec2& dot_pos)
    {
        ImDrawList* draw_list = ImGui::GetForegroundDrawList();
        draw_list->AddCircleFilled(dot_pos, 5, ImColor(1.0f, 1.0f, 1.0f));
    }

    bool SelectionRect(ImVec2& start_pos, ImVec2& end_pos, ImGuiMouseButton mouse_button = ImGuiMouseButton_Left)
    {
        if (ImGui::IsMouseClicked(mouse_button))
            start_pos = ImGui::GetMousePos();
        if (ImGui::IsMouseDown(mouse_button))
        {
            end_pos = ImGui::GetMousePos();
            ImDrawList* draw_list = ImGui::GetForegroundDrawList();
            draw_list->AddRect(start_pos, end_pos, ImGui::GetColorU32(IM_COL32(0, 130, 216, 255)));
            draw_list->AddRectFilled(start_pos, end_pos, ImGui::GetColorU32(IM_COL32(0, 130, 216, 50)));
        }
        return ImGui::IsMouseReleased(mouse_button);
    }
}

void SpriteEditorTool::save_spritesheet()
{
    YAML::Emitter emit;
    emit << YAML::BeginMap;

    emit << YAML::Key << "texture" << YAML::Value << texture_filepath;
    emit << YAML::Key << "dimensions" << YAML::Value;
    emit << YAML::Flow << YAML::BeginSeq << cell_count.x << cell_count.y << YAML::EndSeq;
    emit << YAML::Key << "sprites" << YAML::Value;

    emit << YAML::BeginSeq;

    for (auto& sprite : current_sprites)
    {
        emit << YAML::BeginMap;

        emit << YAML::Key << "clip" << YAML::Value;
        emit << YAML::Flow << YAML::BeginSeq;
        emit << sprite.clip.x << sprite.clip.y << sprite.clip.w << sprite.clip.h;
        emit << YAML::EndSeq;

        emit << YAML::Key << "pivot" << YAML::Value;
        emit << YAML::Flow << YAML::BeginSeq;
        emit << sprite.pivot.x << sprite.pivot.y;
        emit << YAML::EndSeq;

        emit << YAML::EndMap;
    }

    emit << YAML::EndSeq;
    emit << YAML::EndMap;

    assert(emit.good());
    std::ofstream save("data/" + texture_filepath + ".spritesheet.yaml");
    if (save.good())
    {
        save << emit.c_str();
    }
    save.close();
}

void SpriteEditorTool::load_spritesheet()
{
    current_sprites.clear();

    fs::Dir path{ "data/" + spritesheet_filepath };
    if (!path.exists())
    {
        return;
    }

    auto loaded = YAML::LoadFile("data/" + spritesheet_filepath);
    assert(texture_filepath == loaded["texture"].as<String>());

    auto texture = AccessSystem<core::AssetModule>::access_system()->get_asset<Texture>(texture_filepath);
    assert (texture != ecs::no_entity);

    if (auto dimensions = loaded["dimensions"])
    {
        cell_count.x = dimensions[0].as<F32>();
        cell_count.y = dimensions[1].as<F32>();
    }

    for (const auto& sprite_data : loaded["sprites"])
    {
        Sprite sprite;
        sprite.texture = texture;

        auto& clip = sprite_data["clip"];
        I32 x{ clip[0].as<I32>() };
        I32 y{ clip[1].as<I32>() };
        I32 w{ clip[2].as<I32>() };
        I32 h{ clip[3].as<I32>() };
        sprite.clip = geometry::Rect{ x, y, w, h };

        auto& pivot = sprite_data["pivot"];
        sprite.pivot.x = pivot[0].as<F32>();
        sprite.pivot.y = pivot[1].as<F32>();

        this->pivot.x = sprite.pivot.x;
        this->pivot.y = sprite.pivot.y;

        current_sprites.push_back(sprite);
    }

    recently_loaded = true;
}

void SpriteEditorTool::show_tool_window()
{
    static ImVec2 start;
    static ImVec2 end;
    static I32 u[2]{ 1, 1 };
    static I32 v[2]{ 1, 1 };
    static F32 pv[2]{ 0.5f, 0.5f };

    if (recently_loaded)
    {
        pv[0] = pivot.x;
        pv[1] = pivot.y;

        u[0] = (I32)cell_count.x;
        u[1] = (I32)cell_count.y;

        recently_loaded = false;
    }

    if (texture_filepath != "")
    {
        if (ImGui::Button("Save Spritesheet"))
        {
            save_spritesheet();
        }

        ImGui::Separator();

        ImGui::BeginTabBar("SpriteEditorToolZoomTabs");
        if (ImGui::BeginTabItem("1x")) { scale = 1.0f; ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("2x")) { scale = 2.0f; ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("4x")) { scale = 4.0f; ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("8x")) { scale = 8.0f; ImGui::EndTabItem(); }
        ImGui::EndTabBar();

        auto texture = AccessSystem<core::AssetModule>::access_system()->get_asset<Texture>(texture_filepath);
        if (texture == ecs::no_entity) return;

        const auto& textures = AccessStorage<Texture>::access_storage();

        auto preview = textures.get<Texture>(texture);
        SDL_Point texture_size;
        SDL_QueryTexture(preview.inner, nullptr, nullptr, &texture_size.x, &texture_size.y);
        ImGui::Text("Image size: %d, %d", texture_size.x, texture_size.y);

        ImGui::BeginTabBar("SpriteEditorToolSliceTabs");
        if (ImGui::BeginTabItem("Slice By Cell Count"))
        {
            ImGui::BeginChild("SpriteEditorToolSliceTabs_ByCellCount", ImVec2(400, 100));
            if (ImGui::InputInt2("Cell Count", u))
            {
                cell_count.x = (F32)u[0];
                cell_count.y = (F32)u[1];

                if (cell_count.x == 0) cell_count.x = 1;
                if (cell_count.y == 0) cell_count.y = 1;
            }

            if (ImGui::InputFloat2("Pivot", pv))
            {
                pivot.x = (F32)pv[0];
                pivot.y = (F32)pv[1];
            }

            if (ImGui::Button("Slice"))
            {
                auto cell_size_x = texture_size.x / cell_count.x;
                auto cell_size_y = texture_size.y / cell_count.y;
                
                current_sprites.clear();

                for (I32 i = 0; i < cell_count.x; i++)
                {
                    for (I32 j = 0; j < cell_count.y; j++)
                    {
                        geometry::Rect clip{ i * (I32)cell_size_x, j * (I32)cell_size_y, (I32)cell_size_x, (I32)cell_size_y };
                        geometry::Vec2 pvt{ pivot.x, pivot.y };
                        current_sprites.push_back(Sprite{ texture, clip, pvt });
                    }
                }
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Slice By Cell Size"))
        {
            ImGui::BeginChild("SpriteEditorToolSliceTabs_ByCellSize", ImVec2(400, 100));

            if (ImGui::InputInt2("Cell Size", v))
            {
                cell_count.x = (F32)v[0];
                cell_count.y = (F32)v[1];

                if (cell_count.x == 0) cell_count.x = 1;
                if (cell_count.y == 0) cell_count.y = 1;
            }

            if (ImGui::InputFloat2("Pivot", pv))
            {
                pivot.x = (F32)pv[0];
                pivot.y = (F32)pv[1];
            }

            if (ImGui::Button("Slice"))
            {
                auto cell_size_x = texture_size.x / cell_count.x;
                auto cell_size_y = texture_size.y / cell_count.y;

                current_sprites.clear();

                std::swap(cell_size_x, cell_count.x);
                std::swap(cell_size_y, cell_count.y);

                for (I32 i = 0; i < cell_count.x; i++)
                {
                    for (I32 j = 0; j < cell_count.y; j++)
                    {
                        geometry::Rect clip{ i * (I32)cell_size_x, j * (I32)cell_size_y, (I32)cell_size_x, (I32)cell_size_y };
                        geometry::Vec2 pvt{ pivot.x, pivot.y };                        
                        current_sprites.push_back(Sprite{ texture, clip, pvt });
                    }
                }
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();

        auto window_pos = ImGui::GetCursorScreenPos();
        ImGui::Image(preview.inner, ImVec2{ (F32)texture_size.x * scale, (F32)texture_size.y * scale });

        ImGui::SetCursorScreenPos(window_pos);

        // TODO (reported by @mstojanovic): correct (F32) hell
        for (auto& sprite : current_sprites)
        {
            auto& rect = sprite.clip;
            auto& pvt = sprite.pivot;

            ImVec2 startPos(
                (F32)(window_pos.x + rect.x * scale),
                (F32)(window_pos.y + rect.y * scale));
            ImVec2 endPos(
                (F32)(window_pos.x + (rect.x + rect.w) * scale),
                (F32)(window_pos.y + (rect.y + rect.h) * scale));
            ImGui::Rect(startPos, endPos);

            ImVec2 dotPos(
                (F32)(window_pos.x + ((F32)rect.x + pvt.x * (F32)rect.w) * scale),
                (F32)(window_pos.y + ((F32)rect.y + pvt.y * (F32)rect.h) * scale));
            ImGui::Dot(dotPos);
        }
    }
}
