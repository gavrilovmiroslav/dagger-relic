#include "Pickup.h"
#include <iostream>

void PickupSystem::on_tick()
{
    for (auto&& [player_entity, player, player_pos] : QueryPlayers::access_storage().each())
    {
        std::vector<entt::entity> to_remove;
        for (auto&& [pickup_entity, pickup, pickup_pos] : QueryPickups::access_storage().each())
        {
            if (abs(player_pos.xy.x - pickup_pos.xy.x) <= pickup.radius &&
             	abs(player_pos.xy.y - pickup_pos.xy.y) <= pickup.radius)
            {
                replace_component<Item>(player_entity, pickup.name, pickup.uses); // Removing this line, in addition to the despawn line will stop the crashing issue.                
                to_remove.push_back(pickup_entity);                
            }
        }

        for (auto e : to_remove)
        {
            despawn(e);
        }
    }
}