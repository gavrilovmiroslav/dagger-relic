#pragma once
#include "Prelude.h" 
#include "Common.h"

struct PlayerIsDeadSignal
{

};


class DamageSystem : public ecs::System,
                     public MutAccessComponentById<Health>,
                     public AccessComponentById<Damage>,
                     public MutAccessStorage<Damage>,
                     public MutAccessStorage<Player>,
                     public MutAccessComponentById<Score>,
                     public SignalEmitter<GameEndSignal>
{
        void on_tick() override
        {
            for (auto && damage: MutAccessStorage<Damage>::access_storage())
            {
                auto& d = AccessComponentById<Damage>::get(damage);
                deal_damage(d.from, d.to, d.damage);
                despawn(damage);
            }

            
        }

    private:
        inline void deal_damage(ecs::Entity from, ecs::Entity to, I32 damage) const
        {
            
            auto size = MutAccessStorage<Player>::access_storage().size();
            Logger::info("Number of players: {}", size);
            if(size == 0)
            {
                return;
            }
            
            auto& health_to = MutAccessComponentById<Health>::get(to);
            Logger::info("Health before damage: {} -> {}", (int)to, health_to.health);
            health_to.health -=  damage; 
            Logger::info("Health after damage: {} -> {}", (int)to, health_to.health);

            if(health_to.health <= 0)
            { 
                auto player = MutAccessStorage<Player>::access_storage().front();
                auto& score = MutAccessComponentById<Score>::get(player).score;
                if(player != to)
                {
                    score++;
                }
                else
                {
                    emit(GameEndSignal{});
                }
                despawn(to);
                Logger::info("Current score: {}", score);
            }
           
        }

};