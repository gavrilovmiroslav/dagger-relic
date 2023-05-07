#include "Platforms.h"

void PlatformSystem::on_tick()
{
	for (auto &&[player_entity, player, player_pos] : QueryPlayers::access_storage().each())
	{
		for (auto &&[platform_entity, platform, platform_pos] : QueryPlatforms::access_storage().each())
		{
			if (player_pos.xy.x - 60 <= platform_pos.xy.x + platform.width / 2  &&
			    player_pos.xy.x - 40 >= platform_pos.xy.x - platform.width / 2  &&
			    player_pos.xy.y - 18 >= platform_pos.xy.y - 1 &&
			    player_pos.xy.y - 18 <= platform_pos.xy.y + 1
			    )
			{
				player.isGrounded = true;
				player.visina=player_pos.xy.y;
			}
		}

	}
}
