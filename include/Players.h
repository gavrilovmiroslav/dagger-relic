#pragma once
#include "Prelude.h"

struct PlayerControlsSystem
	: public ecs::System,
	  public MutAccessGroupStorage<Player, KeyBindings, Position, SpriteAnimation, Flip>,
	  public AccessComponentById<Player>,
	  public SignalProcessor<AnimationIsFinishedSignal>
{
	
	bool animation_is_finished = false;
	ecs::Entity ent = ecs::no_entity;

	virtual void process_signal(AnimationIsFinishedSignal& signal)
	{
			//ent is later used to make sure that the emited signal is related to the player
			ent = signal.ent;
			animation_is_finished = signal.finished;
	}

	void on_tick() override
	{
		const auto &keys = KeyState::get();

		if (keys.is_pressed(KEY_ESCAPE))
		{
			Engine::get_instance().quit();
		}

		for (auto &&[entity, bindings, pos, sprite, flip] : access_storage().each())
		{
			
			if (animation_is_finished && entity == ent)
			{
				if (keys.is_down(bindings.up))
				{
					sprite.change_to("Moose/Moose1_run");
					pos.xy.y -= SPEED_MOD * Time::delta_time();

				}
				else if (keys.is_down(bindings.down))
				{
					sprite.change_to("Moose/Moose1_run");
					pos.xy.y += SPEED_MOD * Time::delta_time();

				}
				else if (keys.is_down(bindings.left))
				{
					flip = Flip::Horizontal;
					sprite.change_to("Moose/Moose1_run");
					pos.xy.x -= SPEED_MOD * Time::delta_time();

				}
				else if (keys.is_down(bindings.right))
				{
					flip = Flip::None;
					sprite.change_to("Moose/Moose1_run");
					pos.xy.x += SPEED_MOD * Time::delta_time();
				}
				else if(keys.is_down(bindings.attack))
				{
					animation_is_finished = false;
					sprite.change_to("Moose/Moose1_attack_all_effects");
					
				}
				else
				{
					sprite.change_to("Moose/Moose1_Idle");
				}
			}
		}
	}
};