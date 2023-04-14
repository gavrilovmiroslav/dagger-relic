#include "Gravity.h"

void GravitySystem::on_tick()
{
    for (auto&& [player_entity, player, pos] : access_storage().each())
    {
        if (!player.is_grounded)
        {
            player.vertical_velocity += GRAVITY_MOD * Time::delta_time();
            //pos.xy.y += player.vertical_velocity * Time::delta_time();
        }
    }
}