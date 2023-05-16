#pragma once
#include "Prelude.h" //16
#include "PlayerFSM.h"

struct PlayerControlsSystem
    : public ecs::System,
      public MutAccessGroupStorage<Player, KeyBindings, SpriteAnimation>,
      public MutAccessComponentById<SpriteAnimation>,	  
      public MutAccessComponentById<Flip>,
      // TODO: remove Enemy from here, the player controls have nothing to do with it
      // at best, spawn a Damage entity; at worst, send a signal to it instead
      public MutAccessComponentById<Enemy>,
      public MutAccessComponentById<Position>,
      public MutAccessComponentById<KeyBindings>,
      public AccessComponentById<Player>,	  
      public SignalProcessor<PlayerCollisionSignal>,
      public MutAccessGroupStorage<Healthbar,SpriteAnimation>
{
    ecs::Entity player_entity_cache;
    PlayerFSM fsm;
    Bool doing_damage = false;


    String base_string="healthbar/healthbar_";
    F32 player_health;

    PlayerControlsSystem()
    {
        fsm.events_for_action("TryMove").connect<&PlayerControlsSystem::TryMove>(this);
        fsm.events_for_action("TryStop").connect<&PlayerControlsSystem::TryStop>(this);
        fsm.events_for_action("TryAttack").connect<&PlayerControlsSystem::TryAttack>(this);	
        fsm.events_for_action("TryHit").connect<&PlayerControlsSystem::TryHit>(this);
        fsm.current_state = "Idle";
    }
    
    void TryMove() 
    {
        auto& sprite = MutAccessComponentById<SpriteAnimation>::get(player_entity_cache);
        sprite.change_to("Moose/Moose1_run");

        auto& flip = MutAccessComponentById<Flip>::get(player_entity_cache);
        auto& pos = MutAccessComponentById<Position>::get(player_entity_cache);
        auto& bindings  = MutAccessComponentById<KeyBindings>::get(player_entity_cache);		

        geometry::Vec2 xy { 0.0f, 0.0f };
        F32 speed = 1.0f;

        const auto &keys = KeyState::get();

        if (keys.is_down(bindings.up))
        {
            xy.y -= speed;
        } 
        
        if (keys.is_down(bindings.down))
        {
            xy.y += speed;
        }
        
        if (keys.is_down(bindings.left))
        {
            flip = Flip::Horizontal;
            xy.x -= speed;
        }
        
        if (keys.is_down(bindings.right))
        {
            flip = Flip::None;
            xy.x += speed;
        }
        
        if (glm::length(xy) > 0.0f)
        {
            pos.xy += glm::normalize(xy) * SPEED_MOD * Time::delta_time();
        }
    }

    void TryStop() 
    {
        auto& sprite = MutAccessComponentById<SpriteAnimation>::get(player_entity_cache);
        sprite.change_to("Moose/Moose1_Idle");
    }

    void TryAttack() 
    {
        auto& sprite = MutAccessComponentById<SpriteAnimation>::get(player_entity_cache);
        sprite.change_to("Moose/Moose1_attack_all_effects");        
    }

    void TryHit()
    {
        auto& sprite = MutAccessComponentById<SpriteAnimation>::get(player_entity_cache);
        doing_damage = (sprite.current_frame >= 10 && sprite.current_frame <= 13);
    }

    virtual void process_signal(PlayerCollisionSignal& signal)
    {
        auto &enemy_ent = signal.enemy;
        auto &enemy = MutAccessComponentById<Enemy>::get(enemy_ent);

        if (doing_damage)
        {
            spawn().
                with<Damage>(player_entity_cache, enemy_ent, 30)
                .done();
        }
    }

    void on_tick() override
    {
        const auto &keys = KeyState::get();

        if (keys.is_pressed(KEY_ESCAPE))
        {
            Engine::get_instance().quit();
        }

        for (auto &&[ entity, bindings, anim ] : MutAccessGroupStorage<Player, KeyBindings, SpriteAnimation>::access_storage().each())
        {
            player_entity_cache = entity;
            player_health = player.health;

            if (anim.is_finished())
            {
                fsm.trigger("animDone");
            }
            else 
            {
                fsm.trigger("checkForHit");
            }
            
            if (keys.is_down(bindings.attack))
            {
                fsm.trigger("attack");
            }
            else if (keys.is_down(bindings.up) || keys.is_down(bindings.down) || keys.is_down(bindings.left) || keys.is_down(bindings.right))
            {
                fsm.trigger("walk");
            } 
            else
            {
                fsm.trigger("stop");
            }

            
            // player.health -= 0.25;
            // std::cout << player.health << std::endl;

        }
        
        // health
        for (auto &&[healthbar_entity, healthbar,sprite] : MutAccessGroupStorage<Healthbar, SpriteAnimation>::access_storage().each())
        {
                for(int i = 0; i < 10; i++){
                    if((100 - 10*(i+1) <= player_health) && (player_health < 100 - 10*i)){
                        String health_sprite = base_string + std::to_string(9-i);
                        sprite.change_to(health_sprite);
                    }
                } 
        }
    }
};
