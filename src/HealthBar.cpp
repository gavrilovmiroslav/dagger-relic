#include "HealthBar.h"
#include "SpriteRendering.h"
#include "Geometry.h"
#include <iostream>

void HealthBarAnimationSystem::process_signal(OnStartSignal& signal)
{
	const auto& sprites = AccessStorage<Sprite>::access_storage();
	const auto& sheets = AccessStorage<Spritesheet>::access_storage();
	const auto& storage = MutAccessGroupStorage<Sprite, HealthBarSpriteAnimation, CharacterId>::access_storage();

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
	const auto& storage = MutAccessGroupStorage<Sprite, HealthBarSpriteAnimation, CharacterId>::access_storage();

	for (auto& entity : storage)
	{
		auto& characterId = storage.get<CharacterId>(entity);

		if(characterId.characterId != signal.player)
			continue;

		auto& sprite = storage.get<Sprite>(entity);
		auto& anim = storage.get<HealthBarSpriteAnimation>(entity);

		I32 health_update = signal.update;
		if (health_update <= 0)
		{
			anim.current_frame = 7;
		}
		else if(health_update <= 10)
		{
			anim.current_frame = 6;
		}
		else if(health_update <= 20)
		{
			anim.current_frame = 5;
		}
		else if(health_update <= 30)
		{
			anim.current_frame = 4;
		}
		else if(health_update <= 40)
		{
			anim.current_frame = 3;
		}
		else if(health_update <= 50)
		{
			anim.current_frame = 2;
		}
		else if(health_update <= 60)
		{
			anim.current_frame = 1;
		}
		const auto sheet = sheets.get<Spritesheet>(anim.spritesheet);
		auto current_sprite = sheet.sprites[anim.current_frame];
		sprite = sprites.get<Sprite>(current_sprite);
	}

}
