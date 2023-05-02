#include "HealthBar.h"
#include "SpriteRendering.h"
#include "Geometry.h"
#include <iostream>

void HealthBarAnimationSystem::process_signal(OnStartSignal& signal)
{
	const auto& sprites = AccessStorage<Sprite>::access_storage();
	const auto& sheets = AccessStorage<Spritesheet>::access_storage();
	const auto& storage = MutAccessGroupStorage<Sprite, HealthBarSpriteAnimation, PlayerId>::access_storage();

	for (auto& entity : storage)
	{
		auto& sprite = storage.get<Sprite>(entity);
		auto& anim = storage.get<HealthBarSpriteAnimation>(entity);

		anim.current_frame = 0;

		const auto sheet = sheets.get<Spritesheet>(anim.spritesheet);
		auto current_sprite = sheet.sprites[anim.current_frame];
		sprite = sprites.get<Sprite>(current_sprite);
	}
}


void HealthBarAnimationSystem::process_signal(HealthUpdateSignal& signal)
{
	const auto& sprites = AccessStorage<Sprite>::access_storage();
	const auto& sheets = AccessStorage<Spritesheet>::access_storage();
	const auto& storage = MutAccessGroupStorage<Sprite, HealthBarSpriteAnimation, PlayerId>::access_storage();

	for (auto& entity : storage)
	{
		auto& playerId = storage.get<PlayerId>(entity);

		if(playerId.playerId != signal.player)
			continue;

		auto& sprite = storage.get<Sprite>(entity);
		auto& anim = storage.get<HealthBarSpriteAnimation>(entity);

		I32 health_update = signal.update;
		if (health_update <= 0)
		{
			anim.current_frame = 4;
		}else if(health_update <= 25){
			anim.current_frame = 3;
		}else if(health_update <= 50){
			anim.current_frame = 2;
		}else if(health_update <= 75){
			anim.current_frame = 1;
		}

		const auto sheet = sheets.get<Spritesheet>(anim.spritesheet);
		auto current_sprite = sheet.sprites[anim.current_frame];
		sprite = sprites.get<Sprite>(current_sprite);




	}

}


