#pragma once
#include "Prelude.h"

struct PlayerControlsSystem
	: public ecs::System,
	  public MutAccessGroupStorage<Player, KeyBindings, Position, SpriteAnimation, Flip>
{
	void on_tick() override
	{
		const auto &keys = KeyState::get();

		if (keys.is_pressed(KEY_ESCAPE))
		{
			Engine::get_instance().quit();
		}

		for (auto &&[entity, bindings, pos, sprite, flip] : access_storage().each())
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
				sprite.change_to("Moose/Moose1_attack_all_effects");


			}
			else
			{
				sprite.change_to("Moose/Moose1_Idle");
			}
		}
	}
};