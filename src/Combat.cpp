#include "Combat.h"

void CombatSystem::on_tick()
{
    for(auto&& [attack_entity, attack, attack_pos, sprite, flip] : QueryAttacks :: access_storage().each()){
        if(flip == None){
            attack_pos.xy.x += attack.speed * Time::delta_time();
            if(attack_pos.xy.x > attack.range){
                despawn(attack_entity);
            }
        }
        else{
            attack_pos.xy.x -= attack.speed * Time:: delta_time();
            if(attack_pos.xy.x < attack.range){
                despawn(attack_entity);
            }
        }

        for(auto&& [player_entity, player, player_pos] : QueryPlayers :: access_storage().each()){
            if (	abs(player_pos.xy.x - attack_pos.xy.x) <= attack.radius
                && 	abs(player_pos.xy.y - attack_pos.xy.y) <= attack.radius){
                    despawn(player_entity); // Just completely despawns the guy, for now.
                    despawn(attack_entity);
                }
        }
    }
}