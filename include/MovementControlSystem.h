#pragma once

#include "Algebra.h"
#include "PlayerFSM.h"
#include "Random.h"
#include "Prelude.h"

struct PlayerMovement
{
	PlayerFSM p_fsm;
	F32 vertical_velocity;
};

struct Movement
{
	geometry::Vec2 velocity;
	geometry::Vec2 velocity_max;
	geometry::Vec2 force;
	F32            move_force;
	F32            collision_box_width;
	F32            collision_box_height;

	Movement(F32 move_force, F32 velocity_max, F32 collision_w, F32 collision_h)
		: velocity(0.0f, 0.0f)
		, move_force(move_force)
		, velocity_max(velocity_max)
		, force(0.0f, 0.0f)
		, collision_box_width(collision_w)
		, collision_box_height(collision_h)
	{

	}
};

struct KeyBinding
{
	KeyCode left, down, up, right;
	KeyCode blindfold_change;
};

struct MovementControlSystem
	: public ecs::System
	, public MutAccessGroupStorage<PlayerMovement, KeyBinding, Movement, SpriteAnimation>
{
	void on_tick()
	{
		const auto& key = KeyState::get();

		for (auto&& [entity, player, key_binding, movement, sprite] : access_storage().each())
		{
			if (key.is_down(key_binding.up))
			{
				//Staviti walk
				player.p_fsm.trigger(PlayerTransition::STAND);
				movement.force.y = -movement.move_force;
			}
			else if (key.is_down(key_binding.down))
			{
				//Staviti walk
				player.p_fsm.trigger(PlayerTransition::STAND);
				movement.force.y =  movement.move_force;
			}
			else
			{
				movement.force.y -= fsignf(movement.force.y)*movement.move_force;
			}

			if (key.is_down(key_binding.left))
			{
				player.p_fsm.trigger(PlayerTransition::RUN);
				replace_component<Flip>(entity, Horizontal);
				movement.force.x = -movement.move_force;
			}
			else if (key.is_down(key_binding.right))
			{
				player.p_fsm.trigger(PlayerTransition::RUN);
				replace_component<Flip>(entity, None);
				movement.force.x =  movement.move_force;
			}
			else if ((player.p_fsm.currentState == PlayerState::RUNNING) && !key.is_down(key_binding.left) && !key.is_down(key_binding.right))
        		{
            			player.p_fsm.trigger(PlayerTransition::STAND);
        		}
			else
			{
				movement.force.x -= fsignf(movement.force.x)*movement.move_force;
			}


			spdlog::info("force: {} {}", movement.force.x, movement.force.y);
		}
	}
};
