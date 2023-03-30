#include "Pickup.h"

void PickupSystem::on_tick()
{
    for (auto&& [player_entity, player, player_pos] : QueryPlayers :: access_storage().each())
    {
        for (auto&& [pickup_entity, pickup, pickup_pos] : QueryPlickups :: access_storage().each())
        {
            if (	abs(player_pos.xy.x - pickup_pos.xy.x) <= pickup.radius
                && 	abs(player_pos.xy.y - pickup_pos.xy.y) <= pickup.radius
                &&  !pickup.is_picked){
                    pickup.is_picked = true;

                    replace_component<Pickup>(player_entity, pickup.name,pickup.radius, pickup.is_picked);
                    despawn(pickup_entity);
            }
        }
    }
}