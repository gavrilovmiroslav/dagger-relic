#pragma once
#include "Prelude.h" 
#include "Common.h"


struct DeathSignal
{
    ecs::Entity ent;
};

class DamageSystem : public ecs::System,
                     public MutAccessComponentById<Health>,
                     public AccessComponentById<Damage>,
                     public MutAccessStorage<Damage>,
                     public SignalEmitter<DeathSignal>
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
           auto& health_to = MutAccessComponentById<Health>::get(to);
           Logger::info("Health before damage: {} -> {}", (int)to, health_to.health);
           health_to.health -=  damage; 
           Logger::info("Health after damage: {} -> {}", (int)to, health_to.health);


           // TODO: maybe make a DeathSystem
           if(health_to.health <= 0)
           { 
                despawn(to);
           }
           
        }

};