#pragma once

#include "Assets.h"
#include "Diagnostics.h"
#include "Engine.h"
#include "EventBus.h"
#include "FPSCalculator.h"
#include "Geometry.h"
#include "Loaders.h"
#include "Rendering.h"
#include "SpriteRendering.h"
#include "Windowing.h"
#include "Tooling.h"
#include "Textures.h"
#include "Animations.h"

#include "DaggerEditorTool.h"
#include "SpriteEditorTool.h"
#include "DataBrowserTool.h"
#include "DiagnosticsTool.h"
#include "ImagePreviewTool.h"
#include "TextEditorTool.h"

using namespace core;

struct Default2D
{
	Default2D()
	{
		auto& engine = Engine::get_instance();
		engine.use<WindowingModule>();
		engine.use<RenderingModule>();
		engine.use<EventBusModule>();
		engine.use<DiagnosticsModule>();
		engine.use<SMAFPSCalculatorModule>();

		auto& assets = engine.use<AssetModule>();
		assets->add_loader<TextureLoader>();
		assets->add_loader<SpritesheetLoader>();

		engine.use<ToolModule>();

		engine.use<SpriteRenderingModule>();
		engine.use<AnimationModule>();
	}
};

struct Editor
{
	Editor()
	{
		auto& engine = Engine::get_instance();

		engine.use<DaggerEditorTool>();
		engine.use<DiagnosticsTool>();
		engine.use<DataBrowserTool>();
		engine.use<SpriteEditorTool>();
		engine.use<ImagePreviewTool>();
		engine.use<TextEditorTool>();
	}
};
