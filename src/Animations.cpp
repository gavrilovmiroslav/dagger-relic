#include "Animations.h"

#include "Geometry.h"
#include "SpriteRendering.h"

void SpriteAnimation::change_to(String animation_name)
{
	const auto current = Spritesheet::get_by_name(animation_name);
	if (this->spritesheet == current) return;

	spritesheet = current;
	current_frame = 0;
}

void AnimationModule::process_signal(core::PostRenderSignal&)
{
	const auto& sprites = AccessStorage<Sprite>::access_storage();
	const auto& sheets = AccessStorage<Spritesheet>::access_storage();
	const auto& storage = MutAccessGroupStorage<Sprite, SpriteAnimation>::access_storage();

	static containers::Map<ecs::Entity, F32> frame_switch_delay{};

	for (auto& entity : storage)
	{
		auto& sprite = storage.get<Sprite>(entity);
		auto& anim = storage.get<SpriteAnimation>(entity);
		I32 d = sprite.depth;
		I32 layer = sprite.layer;

		const auto sheet = sheets.get<Spritesheet>(anim.spritesheet);
		auto current_sprite = sheet.sprites[anim.current_frame];
		sprite = sprites.get<Sprite>(current_sprite);
		
		sprite.depth = d;
		sprite.layer = layer; 

		frame_switch_delay[entity] += Time::delta_time() * 10.0f;
		if (frame_switch_delay[entity] >= anim.frame_length)
		{
			frame_switch_delay[entity] *= 0;
			anim.current_frame++;
			if (anim.current_frame >= sheet.sprites.size())
			{
				anim.current_frame = 0;
			}
		}
	}
}
