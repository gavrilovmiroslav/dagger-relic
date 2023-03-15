#pragma once

#include "Access.h"
#include "Tooling.h"

namespace core
{
	class AssetModule;
}

class DataBrowserTool
	: public core::Tool
	, public AccessSystem<core::AssetModule>
	, public SignalEmitter<core::EditorAssetSelectedIntent>
{
	String current_path;
	String selected_item;

	String get_menu_tool_name() override;
	void show_tool_window() override;

	void draw_tree(fs::Dir path);

public:

	DataBrowserTool();
};