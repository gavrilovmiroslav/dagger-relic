#pragma once

#include "Algebra.h"
#include "GameComponents.h"
#include "LevelManager.h"
#include "Player.h"
#include "Prelude.h"
#include "Random.h"
#include "SoundManager.h"

using namespace core;

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

			for (auto&& [entity2, movement2, pos2] : access_storage().each())
			{
				F32 dx, dy;
				F32 collision_width = movement.collision_box_width + movement2.collision_box_width;
				F32 collision_height = movement.collision_box_height + movement2.collision_box_height;

				if (entity <= entity2)
				{
					continue;
				}

				dx = pos2.xy.x - pos.xy.x;
				dy = pos2.xy.y - pos.xy.y;

				if (fabsf(dx) < collision_width / 2.0f && fabsf(dy) < collision_height / 2.0f)
				{
					if (fabsf(dx) > fabsf(dy))
					{
						if (dx > 0.0f)
						{
							pos.xy.x -= collision_width / 2.0f - dx;
						}
						else
						{
							pos.xy.x += collision_width / 2.0f + dx;
						}

						/*
						movement.velocity.x = 0.0f;
						movement2.velocity.x = 0.0f;
						*/
					}
					else
					{
						if (dy > 0.0f)
						{
							pos.xy.y -= collision_height / 2.0f - dy;
						}
						else
						{
							pos.xy.y += collision_height / 2.0f + dy;
						}

						/*
						movement.velocity.y = 0.0f;
						movement2.velocity.y = 0.0f;
						*/
					}
				}

			}

			pos.xy              += movement.velocity*Time::delta_time();
			movement.velocity.x += (fx/m)*Time::delta_time();
			movement.velocity.y += (fy/m)*Time::delta_time();
			movement.velocity.x = fclampf(movement.velocity.x, -movement.velocity_max.x, movement.velocity_max.x);
			movement.velocity.y = fclampf(movement.velocity.y, -movement.velocity_max.y, movement.velocity_max.y);
		}
	}
};

struct ClickControlSystem
	: public ecs::System
{
	void on_tick() override
	{
		const auto& key = KeyState::get();

		if (key.is_down(KeyCode::KEY_ESCAPE))
		{
			Engine::get_instance().quit();
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

		for (auto&& [entity, key_binding, movement, sprite] : access_storage().each())
		{
			if (movement.force.x == 0.0f && movement.force.y == 0.0f)
			{
				sprite.change_to("pyramidplunder/archaeologist_standing");
			}

			if (key.is_down(key_binding.up))
			{
				sprite.change_to("pyramidplunder/archaeologist_running");
				movement.force.y = -movement.move_force;
			}
			else if (key.is_down(key_binding.down))
			{
				sprite.change_to("pyramidplunder/archaeologist_running");
				movement.force.y =  movement.move_force;
			}
			else
			{
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
			else
			{
				movement.force.x -= fsignf(movement.force.x)*movement.move_force;
			}

			// spdlog::info("force: {} {}", movement.force.x, movement.force.y);
		}
	}
};

struct PyramidPlunder : public Game
{
	LevelManager level_manager;

	PyramidPlunder()
	{
		auto& engine = Engine::get_instance();
		engine.use<BlindfoldChangingSystem>();
		engine.use<ClickControlSystem>();
		engine.use<MovementSystem>();
		engine.use<MovementControlSystem>();
		engine.use<SoundControlSystem>();
	}

	void on_start() override
	{
		level_manager = LevelManager();
		level_manager.load_level("Levels/level1.txt");

		for(U32 i = 0; i < TILE_ROWS; i++)
		{
        		for(U32 j = 0; j < TILE_COLS; j++)
			{
				Char c = level_manager.level_map[j][i];
				if(c == 'x')
				{
					spawn()
					.with<Wall>(false)
					.with<Sprite>(ecs::no_entity, 6)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/wall"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96 + 24, j*96 + 25})
					.done();
				}
				if(c == 'o' || c == 'a' || c == 'b' || c == 'x' || c == 'd')
				{
					spawn()
					.with<Sprite>(ecs::no_entity, 2)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/sand"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96 + 24, j*96+25})
					.done();
				}
				if(c == 'b')
				{
					spawn()
					.with<Box>()
					.with<Scale>(geometry::Vec2{1.0f, 1.0f})
					.with<Sprite>(ecs::no_entity, 5)
					.with<SpriteAnimation>(Spritesheet::get_by_name("box/box_1"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96 + 24, j*96 + 25})
					.with<Movement>(2000.0f, 50.0f, 32, 32)
					.done();
				}
				if(c == 'p')
				{
					spawn()
					.with<Sprite>(ecs::no_entity, -1)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pushplate/pushplate_3"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96, j*96 })
					.done();
				}
				if(c == 'a')
				{
					spawn()
					.with<Player>(SpecialBlindfold::HumanEyes)
					.with<Sprite>(ecs::no_entity, 6)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/archaeologist_standing"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96, j*96 + 25})
					.with<Movement>(3000.0f, 50.0f, 16, 16)
					.with<KeyBinding>(KeyCode::KEY_LEFT, KeyCode::KEY_DOWN, KeyCode::KEY_UP, KeyCode::KEY_RIGHT, KeyCode::KEY_SPACE, KeyCode::KEY_LSHIFT)
					.done();
				}
				if (c == 'd')
				{
					spawn()
					.with<Wall>(true)
					.with<Sprite>(ecs::no_entity, 8)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/door"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i * 96 + 24, j * 96 + 25 })
					.done();
				}
        		}
		}

		spawn()
		.with<SoundManager>()
		.done();

		auto pause_button = spawn()
			.with<Button>(ButtonType::PauseMusic)
			.with<Sprite>(ecs::no_entity, 0)
			.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/audio_active"))
			.with<Position>(geometry::Vec2{ 880.0f, 910.0f })
			.with<Visibility>(true)
			.done();

		auto play_next = spawn()
			.with<Button>(ButtonType::PlayNext)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/play_next"))
			.with<Position>(geometry::Vec2{ 925.0f, 910.0f })
			.with<Visibility>(true)
			.done();

		auto play_previous = spawn()
			.with<Button>(ButtonType::PlayPrevious)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/play_previous"))
			.with<Position>(geometry::Vec2{ 835.0f, 910.0f })
			.with<Visibility>(true)
			.done();
	}
};

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
