#pragma once

#include "Algebra.h"
#include "GameComponents.h"
#include "LevelManager.h"
#include "Player.h"
#include "Prelude.h"
#include "PushPlate.h"
#include "Random.h"
#include "SoundManager.h"
#include "TextRender.h"
#include "Scene.h"

using namespace core;

struct PyramidPlunder;
static PyramidPlunder *game;

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
						if (movement.velocity_max.x > 0 || movement.velocity_max.y > 0)
						{
							if (dx > 0.0f)
							{
								pos.xy.x -= collision_width / 2.0f - dx;
							}
							else
							{
								pos.xy.x += collision_width / 2.0f + dx;
							}
						}
						else
						{
							if (dx > 0.0f)
							{
								pos2.xy.x += collision_width / 2.0f - dx;
							}
							else
							{
								pos2.xy.x -= collision_width / 2.0f + dx;
							}
						}

					}
					else
					{
						if (movement.velocity_max.x > 0 || movement.velocity_max.y > 0)
						{
							if (dy > 0.0f)
							{
								pos.xy.y -= collision_height / 2.0f - dy;
							}
							else
							{
								pos.xy.y += collision_height / 2.0f + dy;
							}
						}
						else
						{
							if (dy > 0.0f)
							{
								pos2.xy.y += collision_height / 2.0f - dy;
							}
							else
							{
								pos2.xy.y -= collision_height / 2.0f + dy;
							}
						}
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

struct PushPlateBoxSystem
	: public ecs::System
	, public MutAccessGroupStorage<Position, PushPlate, SpriteAnimation>
	, public MutAccessGroupStorage<Position, Movement, Box>
	, public SignalEmitter<PushPlateActivatedSignal>
{
	void on_tick() override;
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

struct DebugRectangleSystem
	: public ecs::System
	, public MutAccessGroupStorage<Position, PostProcessRectangle>
{
	void on_tick() override
	{
		for (auto&& [entity, position, rectangle] : access_storage().each())
		{
			rectangle.x = position.xy.x;
			rectangle.y = position.xy.y;
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
		}
	}
};


struct PyramidPlunder : public Game
{
	LevelManager level_manager;
	U8           level;

	PyramidPlunder()
	{
		auto& engine = Engine::get_instance();
		engine.use<BlindfoldChangingSystem>();
		engine.use<ClickControlSystem>();
		engine.use<MovementSystem>();
		engine.use<MovementControlSystem>();
		engine.use<PushPlateBoxSystem>();
		engine.use<SoundControlSystem>();
		engine.use<TextRenderControlSystem>();
	}

	void remove_components(DynamicArray<ecs::Entity>& entities)
	{
		for (auto entity : entities)
		{
			remove_component<Position>(entity);
			remove_component<Visibility>(entity);
			remove_component<Sprite>(entity);
			remove_component<SpriteAnimation>(entity);
			remove_component<Movement>(entity);
			remove_component<Box>(entity);
			remove_component<PostProcessRectangle>(entity);
			remove_component<Player>(entity);
			remove_component<Wall>(entity);
			remove_component<Button>(entity);
			remove_component<Box>(entity);
		};
		entities.clear();
	}

	void start_level(const U8 id)
	{
		level_manager = LevelManager();
		remove_components(scene.entity);
		scene.Reset();

		switch(id)
		{
			case 0: level_manager.load_level("Levels/level1.txt"); break;
			case 1: level_manager.load_level("Levels/level2.txt"); break;
		}

		for(U32 i = 0; i < TILE_ROWS; i++)
		{
        		for(U32 j = 0; j < TILE_COLS; j++)
			{
				ecs::Entity en = ecs::no_entity;
				Char c  = level_manager.level_map[j][i];
				if(c == 'x')
				{
					en = spawn()
					.with<Wall>(false)
					.with<Sprite>(ecs::no_entity, 6)
					.with<Movement>(0.0f, 0.0f, 96, 96)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/wall"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96, j*96 })
					.done();
					scene.entity.push_back(en);
				}
				if(c == 'o' || c == 'a' || c == 'b' || c == 'x' || c == 'd' || c == 'p' || c == 'r')
				{
					en = spawn()
					.with<Sprite>(ecs::no_entity, 2)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/sand"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96, j*96 })
					.done();
					scene.entity.push_back(en);
				}
				if(c == 'b')
				{
					en = spawn()
					.with<Box>(scene.boxcount)
					.with<Sprite>(ecs::no_entity, 5)
					.with<SpriteAnimation>(Spritesheet::get_by_name("box/box_4"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96 , j*96 })
					.with<Movement>(2000.0f, 50.0f, 28, 56)
					.done();

					scene.boxcount++;
					scene.entity.push_back(en);
				}
				if(c == 'p')
				{
					en = spawn()
					.with<PushPlate>(scene.pushplatecount)
					.with<Sprite>(ecs::no_entity, 3)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pushplate/pushplate_4"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96 + 24, j*96 + 24 })
					.done();

					scene.pushplatecount++;
					scene.entity.push_back(en);
				}
				if(c == 'a')
				{
					en = spawn()
					.with<Player>(SpecialBlindfold::HumanEyes)
					.with<Sprite>(ecs::no_entity, 10)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/archaeologist_standing"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96, j*96 })
					.with<Movement>(3000.0f, 80.0f, 14, 12)
					.with<KeyBinding>(KeyCode::KEY_LEFT, KeyCode::KEY_DOWN, KeyCode::KEY_UP, KeyCode::KEY_RIGHT, KeyCode::KEY_SPACE, KeyCode::KEY_LSHIFT)
					.done();
					scene.entity.push_back(en);
				}
				if (c == 'd')
				{
					en = spawn()
					.with<Wall>(true)
					.with<Sprite>(ecs::no_entity, 8)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/door"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i * 96 + 24, j * 96 + 25 })
					.done();
					scene.entity.push_back(en);
				}
				if(c == 'r')
				{
					en = spawn()
					.with<Boulder>()
					.with<Sprite>(ecs::no_entity, 5)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/boulder"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96 , j*96 })
					.with<Movement>(2000.0f, 50.0f, 28, 56)
					.done();

					scene.entity.push_back(en);
				}
        		}
		}

		auto pause_button = spawn()
			.with<Button>(ButtonType::PauseMusic)
			.with<Sprite>(ecs::no_entity, 8)
			.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/audio_active"))
			.with<Position>(geometry::Vec2{ 880.0f, 910.0f })
			.with<Visibility>(true)
			.done();
		scene.entity.push_back(pause_button);

		auto play_next = spawn()
			.with<Button>(ButtonType::PlayNext)
			.with<Sprite>(ecs::no_entity, 8)
			.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/play_next"))
			.with<Position>(geometry::Vec2{ 925.0f, 910.0f })
			.with<Visibility>(true)
			.done();
		scene.entity.push_back(play_next);

		auto play_previous = spawn()
			.with<Button>(ButtonType::PlayPrevious)
			.with<Sprite>(ecs::no_entity, 8)
			.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/play_previous"))
			.with<Position>(geometry::Vec2{ 835.0f, 910.0f })
			.with<Visibility>(true)
			.done();
		scene.entity.push_back(play_previous);
	}

	void on_start() override
	{
		TextRender::init();

		level = 0;
		start_level(level);

		/*
		spawn()
		.with<PushPlateCounter>()
		.with<Sprite>(ecs::no_entity, (int) (~0u))
		.with<SpriteAnimation>(Spritesheet::get_by_name("tool/ppress"))
		.with<Visibility>(true)
		.with<Position>(geometry::Vec2{0, 0})
		.done();
		*/
	}

	void on_end() override
	{
		TextRender::deinit();
	}
};


void PushPlateBoxSystem::on_tick()
{
	/* For each PushPlate, check if it's colliding with a Box. */
	for (auto&& [entity, pushplate_pos, pushplate, sprite] : MutAccessGroupStorage<Position, PushPlate, SpriteAnimation>::access_storage().each())
	{
		Bool collide = false;

		for (auto&& [entity, box_pos, box_movement, box] : MutAccessGroupStorage<Position, Movement, Box>::access_storage().each())
		{
			F32 dx, dy;
			F32 collision_width  = box_movement.collision_box_width  + 48;
			F32 collision_height = box_movement.collision_box_height + 48;

			/* Pivot has to be adjusted (to center of push plate). */
			dx = (box_pos.xy.x+24) - pushplate_pos.xy.x;
			dy = (box_pos.xy.y+24) - pushplate_pos.xy.y;

			if (fabsf(dx) < collision_width / 2.0f && fabsf(dy) < collision_height / 2.0f)
			{
				collide = true;
				break;
			}
		}

		if (collide)
		{
			if (!pushplate.active)
			{
				emit(PushPlateActivatedSignal{});
				pushplate.active = true;
				scene.pushplate_activenow++;
				sprite.change_to("pushplate/pushplate_5");
				spdlog::info("[o] Pushplate ACTIVE count is {} (new activated)", scene.pushplate_activenow);
				if (scene.pushplate_activenow >= scene.pushplatecount)
				{
					spdlog::info("All pushplates active!");
					game->level++;
					game->start_level(game->level);
				}
			}
		}
		else
		{
			if (pushplate.active)
			{
				spdlog::info("[o] Pushplate ACTIVE count is {} (one deactivated)", scene.pushplate_activenow);
				pushplate.active = false;
				scene.pushplate_activenow--;
				sprite.change_to("pushplate/pushplate_4");
			}
		}
	}
};

#ifdef _WIN32
#include <Windows.h>
void PlatformMain(void)
{
	FILE *fp;

	AllocConsole();
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
	freopen_s(&fp, "CONIN$", "r", stdin);
}
#else
void PlatformMain(void)
{
}
#endif

int main(int argc, char* argv[])
{
	PlatformMain();

	auto& engine = Engine::get_instance();
	std::string filePath = "dagger.ini";
	engine.configure(filePath.data(), argc, argv);

	engine.setup<Default2D>();
	engine.setup<PostProcess2D>();

	game = new PyramidPlunder();
	engine.run();

	return 0;
}
