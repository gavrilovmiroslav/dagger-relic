#include "HealthBar.h"
#include "SpriteRendering.h"
#include "Geometry.h"

void HealthBarSpriteAnimation::change_to(String animation_name)
{
	const auto current = Spritesheet::get_by_name(animation_name);
	if (this->spritesheet == current) return;

	spritesheet = current;
	current_frame = 0;
}

void HealthBarAnimation::process_signal(HealthUpdateSignal& signal)
{
	const auto& sprites = AccessStorage<Sprite>::access_storage();
	const auto& sheets = AccessStorage<Spritesheet>::access_storage();
	const auto& storage = MutAccessGroupStorage<Sprite, HealthBarSpriteAnimation>::access_storage();

	if(is_ok(signal.player)){
		for (auto& entity : storage)
		{
			auto& sprite = storage.get<Sprite>(entity);
			auto& anim = storage.get<HealthBarSpriteAnimation>(entity);

			const auto sheet = sheets.get<Spritesheet>(anim.spritesheet);
			auto current_sprite = sheet.sprites[anim.current_frame];
			sprite = sprites.get<Sprite>(current_sprite);

			I32 health_update = signal.update;

			if (health_update == 0)
			{
				anim.current_frame += 4;
			}else if(health_update <= 25){
				anim.current_frame += 3;
			}else if(health_update <= 50){
				anim.current_frame += 2;
			}else if(health_update <= 75){
				anim.current_frame += 1;
			}
		}
	}
}


