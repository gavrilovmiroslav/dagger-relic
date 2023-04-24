#include "Platform.h"
#include <iostream>

void PlatformSystem::on_tick()
{
	const auto &keys = KeyState::get();
	auto &ourGlobal = MutAccessUnique<OurGlobalVar>::access_unique();
	for (auto &&[player_entity, player, player_pos] : QueryPlayer::access_storage().each())
	{
		for (auto &&[platform_entity, platform, bindings, platform_pos, sprite] : QueryPlatform::access_storage().each())
		{
			sprite.depth = -50;
			if (keys.is_down(bindings.up))
			{
				platform.verticalVelocity = 200;
			}
			if (!ourGlobal.isGrounded)
			{
				if (platform_pos.xy.y > SCREEN_HEIGHT - PLATFORM_OFFSET)
				{
					platform.verticalVelocity += GRAVITY * Time::delta_time();
					platform_pos.xy.y -= platform.verticalVelocity * Time::delta_time();
				}
			}
			if (player_pos.xy.y + PLAYER_OFFSET_DOWN >= platform_pos.xy.y)
			{
				ourGlobal.isGrounded = true;
				player.verticalVelocity = 0;
				break;
			}
		}
	}
}
