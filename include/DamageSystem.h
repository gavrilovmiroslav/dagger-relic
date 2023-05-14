#pragma once
#include "Prelude.h" 
#include "Common.h"

class DamageSystem : public ecs::System,
                     public MutAccessComponentById<Health>,
                     public AccessComponentById<Damage>,
                     public MutAccessStorage<Damage>
{
        void on_tick() override
        {
            for (auto && damage: MutAccessStorage<Damage>::access_storage())
            {
                auto d = AccessComponentById<Damage>::get(damage);
                deal_damage(d.from, d.to, d.damage);
            }

        }

    private:
        inline void deal_damage(ecs::Entity from, ecs::Entity to, I32 damage) const
        {
           auto health_to = MutAccessComponentById<Health>::get(to);
           health_to.health -=  damage;  
           
        }

};