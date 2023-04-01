#include "Combat.h"
#include <iostream>

void SpellMovementSystem::on_tick()
{
    for (auto&& [spell_entity, spell, spell_pos, sprite, flip, move, clipping] : QuerySpells::access_storage().each())
    {
        if(move == Line)
        {
            if(flip == None)
            {
                spell_pos.xy.x += spell.speed * Time::delta_time();
                if(spell_pos.xy.x > spell.range)
                {
                    despawn(spell_entity);
                }
            }
            else
            {
                spell_pos.xy.x -= spell.speed * Time::delta_time();
                if(spell_pos.xy.x < spell.range)
                {
                    despawn(spell_entity);
                }
            }
        }

        for (auto&& [player_entity, player, player_pos] : QueryPlayers::access_storage().each())
        {
            if (abs(player_pos.xy.x - spell_pos.xy.x) <= spell.radius &&
             	abs(player_pos.xy.y - spell_pos.xy.y) <= spell.radius)
            {
                PlayerCollisionEmitter::emit(SpellPlayerCollisionSignal{player_entity, spell_entity});
            }
        }

        if(clipping == Collides) // Only if the spell is supposed to collide with platforms
        {
            for (auto&& [platform_entity, platform, platform_pos] : QueryPlatforms::access_storage().each())
            {
                if (spell_pos.xy.x <= platform_pos.xy.x + platform.width / 2 + spell.radius &&
                    spell_pos.xy.x >= platform_pos.xy.x - platform.width / 2 - spell.radius && 
                    spell_pos.xy.y + spell.radius >= platform_pos.xy.y - 1 && 
                    spell_pos.xy.y + spell.radius <= platform_pos.xy.y + 1) 
                {
                    PlatformCollisionEmitter::emit(SpellPlatformCollisionSignal{platform_entity, spell_entity});
                }
            }
        }
    }
}

void SpellCollisionSystem::process_signal(SpellPlayerCollisionSignal& signal){
    despawn(signal.player);
    auto &linger = MutAccessComponentById<Duration>::get(signal.spell);
    if (linger != Lingers){
        despawn(signal.spell);
    }
}