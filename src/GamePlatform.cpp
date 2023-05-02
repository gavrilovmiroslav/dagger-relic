#include "GamePlatform.h"

void PlatformSystem::on_tick()
{

    for (auto&& [player_entity, player, player_pos] : QueryPlayers::access_storage().each())
    {
        bool on_platform = false;
        for (auto&& [platform_entity, platform, platform_pos] : QueryPlatforms::access_storage().each())
        {
            if (player_pos.xy.x <= platform_pos.xy.x + platform.width / 2 + 11 &&
                player_pos.xy.x >= platform_pos.xy.x - platform.width / 2 - 11 &&
                player_pos.xy.y + platform.height >= platform_pos.xy.y - 1 &&
                player_pos.xy.y + platform.height <= platform_pos.xy.y + 1 &&
                player.vertical_velocity >= 0.0f) // last check = check if player is falling
            {
                on_platform = true;
                player.vertical_velocity = 0.0f;
                break;
            }
        }
        player.is_grounded = on_platform;
    }
}


