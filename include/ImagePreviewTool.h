#pragma once

#include "Access.h"
#include "Definitions.h"
#include "Lifecycle.h"

#include "Tooling.h"

struct Texture;

namespace core 
{
	class RenderingModule;
	class AssetModule;
	struct WindowingState;
}

struct ImagePreviewTool
	: public core::Tool 
	, public AccessUnique<core::WindowingState>
	, public AccessSystem<core::RenderingModule>
	, public AccessSystem<core::AssetModule>	
	, public AccessStorage<Texture>
	, public SignalProcessor<core::EditorAssetSelectedIntent>
{
	ImagePreviewTool();

	void process_signal(core::EditorAssetSelectedIntent&) override;

	String get_menu_tool_name() override;
	String get_tool_window_appendix() override;
	void show_tool_window() override;

private:
	String filepath;
};
