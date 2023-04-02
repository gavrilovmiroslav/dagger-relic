#include "Pickup.h"
#include <iostream>

void PickupSystem::on_tick()
{
    for (auto&& [pickup_entity, pickup, pickup_pos] : QueryPickups::access_storage().each())
    {
        for (auto&& [player_entity, player, player_pos] : QueryPlayers::access_storage().each())
        {
            if (abs(player_pos.xy.x - pickup_pos.xy.x) <= pickup.radius &&
             	abs(player_pos.xy.y - pickup_pos.xy.y) <= pickup.radius)
            {
                replace_component<Item>(player_entity, pickup.name); // Removing this line, in addition to the despawn line will stop the crashing issue.
                remove_component<Pickup>(pickup_entity);
                remove_component<Visibility>(pickup_entity);
                despawn(pickup_entity); // Removing this line, in addition to the replace_component line will stop the crashing issue.
            }
        }
    }
}