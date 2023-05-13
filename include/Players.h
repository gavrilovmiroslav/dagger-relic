#pragma once
#include "Prelude.h"
#include "PlayerFSM.h"

struct PlayerControlsSystem
	: public ecs::System,
	  public MutAccessGroupStorage<Player, KeyBindings, Position, SpriteAnimation, Flip>,
	  public MutAccessComponentById<Enemy>,
	  public AccessComponentById<Player>,
	  public SignalProcessor<AnimationIsFinishedSignal>,
	  public SignalProcessor<PlayerCollisionSignal>

{
	
	bool animation_is_finished = false;
	bool attacking = false; 
	ecs::Entity ent = ecs::no_entity;
	PlayerFSM pfsm();

	virtual void process_signal(AnimationIsFinishedSignal& signal)
	{
			//ent is later used to make sure that the emited signal is related to the player
			ent = signal.ent;
			animation_is_finished = signal.finished;
	}
	virtual void process_signal(PlayerCollisionSignal& signal)
	{
		auto &enemy_ent = signal.enemy;
		auto &enemy = MutAccessComponentById<Enemy>::get(enemy_ent);
		if(attacking)
		{
			std::cout << "enemy " << (int)enemy_ent << " before " << enemy.health <<std::endl;
			enemy.health -= 30;
			std::cout << "enemy " << (int)enemy_ent << "health: " << enemy.health << std::endl;
			if(enemy.health <= 0.0f)
			{
						despawn(enemy_ent);
			}
		}
		attacking = false;
	}

	void on_tick() override
	{
		const auto &keys = KeyState::get();

		if (keys.is_pressed(KEY_ESCAPE))
		{
			Engine::get_instance().quit();
		}

		for (auto &&[entity, player ,bindings, pos, sprite, flip] : MutAccessGroupStorage<Player, KeyBindings, Position, SpriteAnimation, Flip>::access_storage().each())
		{
			
			if (animation_is_finished && entity == ent)
			{
				if (keys.is_down(bindings.up))
				{
					attacking = false; 
					sprite.change_to("Moose/Moose1_run");
					pos.xy.y -= SPEED_MOD * Time::delta_time();

				}
				else if (keys.is_down(bindings.down))
				{
					attacking = false; 
					sprite.change_to("Moose/Moose1_run");
					pos.xy.y += SPEED_MOD * Time::delta_time();

				}
				else if (keys.is_down(bindings.left))
				{
					attacking = false; 
					flip = Flip::Horizontal;
					sprite.change_to("Moose/Moose1_run");
					pos.xy.x -= SPEED_MOD * Time::delta_time();

				}
				else if (keys.is_down(bindings.right))
				{
					attacking = false; 
					flip = Flip::None;
					sprite.change_to("Moose/Moose1_run");
					pos.xy.x += SPEED_MOD * Time::delta_time();
				}
				else if(keys.is_down(bindings.attack))
				{
					
					//logika za napad treba da ide ovde
					attacking = true; 
					animation_is_finished = false;
					sprite.change_to("Moose/Moose1_attack_all_effects");
					
				}
				else
				{
					sprite.change_to("Moose/Moose1_Idle");
					attacking = false; 
				}
			}
		}
	}
};