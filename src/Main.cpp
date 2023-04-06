#pragma once

#include "Prelude.h"
#include "Random.h"
#include "Algebra.h"

using namespace core;

struct Movement 
{
	geometry::Vec2 velocity;
	geometry::Vec2 velocity_max;
	geometry::Vec2 force;
	F32            move_force;

	Movement(F32 move_force, F32 velocity_max) : velocity(0.0f, 0.0f), move_force(move_force), velocity_max(velocity_max), force(0.0f, 0.0f) { }
};

struct KeyBinding 
{
	KeyCode left, down, up, right;
};

struct MovementSystem
	: public ecs::System
	, public MutAccessGroupStorage<Movement, Position>
{
	void on_tick() override
	{
		for (auto&& [entity, movement, pos] : access_storage().each())
		{
			F32 u = 16.0f;                              /* Friction coefficient. */
			F32 m = 5.0f;                               /* Object mass (kg). */
			F32 g = 9.807f;                             /* Gravity acceleration. */
			F32 n = m*g;                                /* Normal force. */
			F32 fnx = -fsignf(movement.velocity.x)*u*n; /* Friction force. */
			F32 fny = -fsignf(movement.velocity.y)*u*n; 
			F32 fx  = fnx+movement.force.x;             /* Total force. */
			F32 fy  = fny+movement.force.y;

			pos.xy              += movement.velocity*Time::delta_time();
			movement.velocity.x += (fx/m)*Time::delta_time();
			movement.velocity.y += (fy/m)*Time::delta_time();
			movement.velocity.x = fclampf(movement.velocity.x, -movement.velocity_max.x, movement.velocity_max.x);
			movement.velocity.y = fclampf(movement.velocity.y, -movement.velocity_max.y, movement.velocity_max.y);
		}
	}
};

struct MovementControlSystem
	: public ecs::System
	, public MutAccessGroupStorage<KeyBinding, Movement, SpriteAnimation>
{
	void on_tick() override
	{
		const auto& key = KeyState::get();

		if (key.is_down(KeyCode::KEY_ESCAPE))
		{
			Engine::get_instance().quit();
		}

		for (auto&& [entity, key_binding, movement, sprite] : access_storage().each())
		{
			if (key.is_down(key_binding.up)){       
				movement.force.y = -movement.move_force;
			}
			else if (key.is_down(key_binding.down)){
				movement.force.y =  movement.move_force;
			}
			else{                                     
				movement.force.y -= fsignf(movement.force.y)*movement.move_force;
			}

			if (key.is_down(key_binding.left))
			{
				replace_component<Flip>(entity, Horizontal);
				sprite.change_to("pyramidplunder/archaeologist_running");       
				movement.force.x = -movement.move_force;
			}
			else if (key.is_down(key_binding.right))
			{
				replace_component<Flip>(entity, None);
				sprite.change_to("pyramidplunder/archaeologist_running");
				movement.force.x =  movement.move_force;
			}
			else{                                    
				movement.force.x -= fsignf(movement.force.x)*movement.move_force;
			}

			spdlog::info("force: {} {}", movement.force.x, movement.force.y);
		}
	}
};

struct PyramidPlunder : public Game
{
	PyramidPlunder()
	{
		auto& engine = Engine::get_instance();
		engine.use<MovementSystem>();
		engine.use<MovementControlSystem>();
	}

	void on_start() override
	{
		auto ball = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/archaeologist_standing"))
			.with<Position>(geometry::Vec2{ 300, 100 })
			.with<Visibility>(true)
			.with<Movement>(2000.0f, 50.0f)
			.with<KeyBinding>(KeyCode::KEY_LEFT, KeyCode::KEY_DOWN, KeyCode::KEY_UP, KeyCode::KEY_RIGHT)
			.done();
	}
};

#undef main
int main(int argc, char* argv[])
{
	auto& engine = Engine::get_instance();
	std::string filePath = "dagger.ini";
	engine.configure(filePath.data(), argc, argv);

	engine.setup<Default2D>();

	PyramidPlunder game;
	engine.run();

	return 0;
}
