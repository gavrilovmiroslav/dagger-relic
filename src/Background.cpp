#include "Background.h"
#include <iostream>

int counter = 0;
void BackgroundSystem::on_tick()
{
	const auto &keys = KeyState::get();
	auto &ourGlobal = MutAccessUnique<OurGlobalVar>::access_unique();
	for (auto &&[background_entity, background, bindings, background_pos, sprite] : access_storage().each())
	{
		sprite.depth = -100;
		if (keys.is_down(bindings.up))
		{
			background.verticalVelocity = 200;
		}
		if (!ourGlobal.isGrounded)
		{
			if (counter < NUMBER_OF_BACKGROUND - 2)
			{
				background.verticalVelocity += GRAVITY * Time::delta_time();
				background_pos.xy.y -= background.verticalVelocity * Time::delta_time();
			}
			else
			{
				background.verticalVelocity = 0;
				ourGlobal.canPlayerMove = true;
			}
			if (background_pos.xy.y < -SCREEN_HEIGHT * 2)
			{
				counter++;
				despawn(background_entity);
			}
		}
	}
}
