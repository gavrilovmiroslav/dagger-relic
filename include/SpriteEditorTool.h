#pragma once

#include "Access.h"
#include "Assets.h"
#include "Tooling.h"

struct Texture;
struct Sprite;

class SpriteEditorTool
	: public core::Tool
	, public AccessSystem<core::AssetModule>
	, public AccessStorage<Texture>
	, public AccessStorage<Sprite>
	, public SignalProcessor<core::EditorAssetSelectedIntent>
{
	String get_menu_tool_name() override;
	void show_tool_window() override;

	void save_spritesheet();
	void load_spritesheet();

	void process_signal(core::EditorAssetSelectedIntent& selection);

	String texture_filepath;
	String spritesheet_filepath;

	ImVec2 cell_count{ 1, 1 };
	geometry::Vec2 pivot{ 0.5f, 0.5f };
	F32 scale{ 2 };

	containers::DynamicArray<Sprite> current_sprites;
	Bool recently_loaded{ false };

public:

	SpriteEditorTool();

};