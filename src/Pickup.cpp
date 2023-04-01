#include "Pickup.h"

void PickupSystem::on_tick()
{
    for (auto&& [player_entity, player, player_pos] : QueryPlayers::access_storage().each())
    {
        for (auto&& [pickup_entity, pickup, pickup_pos] : QueryPickups::access_storage().each())
        {
            if (abs(player_pos.xy.x - pickup_pos.xy.x) <= pickup.radius &&
             	abs(player_pos.xy.y - pickup_pos.xy.y) <= pickup.radius)
            {
                replace_component<Item>(player_entity, pickup.name);
                despawn(pickup_entity);
            }
        }
    }
}