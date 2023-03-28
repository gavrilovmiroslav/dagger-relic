#pragma once

#include "Prelude.h"
#include "Random.h"

using namespace core;

struct Player {};
struct Background {};

struct KeyBindings 
{
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
};


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 16
#define SPEED_MOD 200.0f

struct PlayerControlsSystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, KeyBindings, Position, SpriteAnimation>
{
	void on_tick() override
	{
		const auto& keys = KeyState::get();

		if (keys.is_pressed(KEY_ESCAPE))
		{
			Engine::get_instance().quit();
		}

		for (auto&& [entity, bindings, pos, sprite] : access_storage().each())
		{
			if (keys.is_down(bindings.up))
			{
				pos.xy.y -= SPEED_MOD * Time::delta_time();
			}
			if (keys.is_down(bindings.down))
			{
				pos.xy.y += SPEED_MOD * Time::delta_time();
			}
			if (keys.is_down(bindings.left))
			{
				pos.xy.x -= SPEED_MOD * Time::delta_time();
			}
			if (keys.is_down(bindings.right))
			{
				sprite.change_to("fallingHero/runningRight");
				pos.xy.x += SPEED_MOD * Time::delta_time();
			}
			else{
				sprite.change_to("fallingHero/heroStanding");
			}
			if (keys.is_down(KeyCode::KEY_SPACE))
			{
				sprite.change_to("fallingHero/attack");
			}
			else{
				sprite.change_to("fallingHero/heroStanding");
			}
			
		
		}
	}
};

struct FallingHero : public Game
{
	FallingHero()
	{
		auto& engine = Engine::get_instance();
		engine.use<PlayerControlsSystem>();
	}

	void on_start() override
	{
		auto hero = spawn()
			.with<Player>()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("fallingHero/heroStanding"))
			.with<Position>(geometry::Vec2{ 300, 100 })
			.with<Visibility>(true)
			.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S, KeyCode::KEY_A, KeyCode::KEY_D)
			.done();
	}
};

#undef main
int main(int argc, char* argv[])
{
	auto& engine = Engine::get_instance();
	engine.configure("dagger.ini", argc, argv);

	engine.setup<Default2D>();


	FallingHero game;
	engine.run();

	return 0;
}

