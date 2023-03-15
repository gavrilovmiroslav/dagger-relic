#include "SpriteRendering.h"

#include "Assets.h"
#include "Rendering.h"
#include "Textures.h"
#include "Windowing.h"

using namespace core;

ecs::Entity Spritesheet::get_by_name(String name)
{
	return AccessSystem<AssetModule>{}.access_system()->get_asset<Spritesheet>(name);
}

void SpriteRenderingModule::process_signal(RenderSignal& signal)
{
	const auto& position_storage = AccessStorage<Position>::access_storage();
	const auto& scale_storage = AccessStorage<Scale>::access_storage();
	const auto& flip_storage = AccessStorage<Flip>::access_storage();

	const auto& state = AccessUnique<WindowingState>::access_unique();
	const auto& textures = AccessStorage<Texture>::access_storage();
	
	for (const auto&& [ entity, sprite, show ] : AccessGroupStorage<Sprite, Visibility>::access_storage().each())
	{
		if (sprite.texture == (ecs::Entity)0 || sprite.texture == ecs::no_entity) continue;
		
		const auto& pos = position_storage.contains(entity) ? position_storage.get<Position>(entity) : Position{ geometry::Vec2{ 0, 0 } };
		const auto& scale = scale_storage.contains(entity) ? scale_storage.get<Scale>(entity) : Scale{ geometry::Vec2{ 1, 1 } };
		const auto& flip = flip_storage.contains(entity) ? flip_storage.get<Flip>(entity) : Flip::None;

		if (show.state)
		{
			const auto& texture = textures.get<Texture>(sprite.texture);
			geometry::Rect dest{ (int)(pos.xy.x - sprite.pivot.x * sprite.clip.w), (int)(pos.xy.y - sprite.pivot.y * sprite.clip.h), (int)(sprite.clip.w * scale.xy.x), (int)(sprite.clip.h * scale.xy.y)};
			SDL_RenderCopyEx(state.renderer, texture.inner, &sprite.clip, &dest, 0.0f, nullptr, (SDL_RendererFlip)flip);
		}
	}
}

void SpriteRenderingModule::process_signal(AssetUnloadingSignal<Texture>& signal)
{
	const auto& current_sprites = MutAccessStorage<Sprite>::access_storage();

	for (const auto entity : current_sprites)
	{
		auto& sprite = current_sprites.get<Sprite>(entity);
		if (sprite.texture == signal.id)
			sprite.texture = ecs::no_entity;
	}
}