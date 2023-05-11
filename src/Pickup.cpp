#include "Pickup.h"
#include "Access.h"
#include <iostream>

void PickupSystem::on_tick()
{
    for (auto&& [player_entity, player, player_pos] : QueryPlayers::access_storage().each())
    {
        for (auto&& [pickup_entity, pickup, pickup_pos] : QueryPickups::access_storage().each())
        {
            if (abs(player_pos.xy.x - pickup_pos.xy.x) <= pickup.radius &&
             	abs(player_pos.xy.y - pickup_pos.xy.y) <= pickup.radius)
            {

                Item* current_item = ItemGetter::get(player_entity);
                current_item->name = pickup.name;
                current_item->uses = pickup.uses;
                
                remove_component<Visibility>(pickup_entity);
                remove_component<Pickup>(pickup_entity); 
            }
        }
    }
}