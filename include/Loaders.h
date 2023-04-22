#pragma once

#include "Assets.h"
#include "Textures.h"
#include "Windowing.h"

// TODO (reported by @mstojanovic): one loader per file and just link them together

class TextureLoader
	: public core::AssetLoader<Texture>
	, public AccessStorage<Texture>
	, public MutAccessUnique<core::WindowingState>
	, public SignalProcessor<core::WindowShutdownSignal>
	, public SignalEmitter<core::AssetUnloadingSignal<Texture>>
{
	memory::RawPtr<Texture> load_texture(String);
	String get_default_asset_name() const override;
	void load_assets() override;
	ecs::Entity load_asset(String texture_name, String texture_path) override;

	void process_signal(core::WindowShutdownSignal&) override;

public:
	TextureLoader();

	Bool contains(String) const override;
};

struct Spritesheet;

class SpritesheetLoader
	: public core::AssetLoader<Spritesheet>
	, public MutAccessStorage<Spritesheet>
	, public AccessSystem<core::AssetModule>
	, public SignalEmitter<core::AssetUnloadingSignal<Spritesheet>>
{
	String get_default_asset_name() const override;
	void load_assets() override;
	ecs::Entity load_asset(String, String) override;

public:
	SpritesheetLoader();

	Bool contains(String) const override;
};
