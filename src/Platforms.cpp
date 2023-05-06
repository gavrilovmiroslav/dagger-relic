#include "Platforms.h"

void PlatformSystem::on_tick()
{
	for (auto &&[player_entity, player, player_pos] : QueryPlayers::access_storage().each())
	{
		for (auto &&[platform_entity, platform, platform_pos] : QueryPlatforms::access_storage().each())
		{
			if (player_pos.xy.x <= platform_pos.xy.x + platform.width / 2 + 11 &&
			    player_pos.xy.x >= platform_pos.xy.x - platform.width / 2 - 11 &&
			    player_pos.xy.y + 16 >= platform_pos.xy.y - 1 &&
			    player_pos.xy.y + 16 <= platform_pos.xy.y + 1 &&
			    player.speed.x >= 0.0f && player.speed.y >= 0.0f) // last check = check if player is falling
			{
				player.isGrounded = true;
				player.speed = (geometry::Vec2)0.0f;
				break;
			}
		}
	}
}
