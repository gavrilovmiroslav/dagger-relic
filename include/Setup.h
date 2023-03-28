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
#include "Textures.h"
#include "Animations.h"

using namespace core;

struct Default2D
{
	Default2D()
	{
		auto& engine = Engine::get_instance();
		engine.use<WindowingModule>();
		engine.use<RenderingModule>();
		engine.use<EventBusModule>();
		engine.use<SMAFPSCalculatorModule>();

		engine.use<AssetModule>();
		engine.get<AssetModule>()->add_loader<TextureLoader>();
		engine.get<AssetModule>()->add_loader<SpritesheetLoader>();

		engine.use<SpriteRenderingModule>();
		engine.use<AnimationModule>();
	}
};
