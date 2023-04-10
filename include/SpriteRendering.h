#pragma once

#include "Access.h"
#include "Definitions.h"
#include "Geometry.h"

struct Texture;

enum Flip
{
	None = 0,
	Horizontal = 1,
	Vertical = 2
};

struct Sprite
{
	ecs::Entity texture;
	I32 depth;
	geometry::Rect clip;
	geometry::Vec2 pivot{ 0.5f, 0.5f };
};

struct Image
{
	String sprite;
	geometry::Vec2 scale{ 1, 1 };
};

struct Spritesheet
{
	containers::DynamicArray<ecs::Entity> sprites;

	static ecs::Entity get_by_name(String name);
};

namespace core {
	struct WindowingState;
	struct RenderSignal;

	template<typename>
	struct AssetUnloadingSignal;
}

class SpriteRenderingModule
	: public ecs::System
	, public AccessDirect
	, public AccessStorage<Texture>
	, public AccessStorage<Flip>
	, public AccessStorage<Position>
	, public AccessStorage<Scale>
	, public AccessStorage<Visibility>
	, public MutAccessStorage<Sprite>
	, public AccessGroupStorage<Sprite>
	, public AccessUnique<core::WindowingState>
	, public SignalProcessor<core::RenderSignal>
	, public SignalProcessor<core::AssetUnloadingSignal<Texture>>
{
	void process_signal(core::RenderSignal& signal) override;
	void process_signal(core::AssetUnloadingSignal<Texture>& signal) override;
};