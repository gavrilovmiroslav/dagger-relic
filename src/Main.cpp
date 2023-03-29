#pragma once

#include "Prelude.h"
#include "Random.h"
#include <iostream>

using namespace core;

struct Player {};
struct Background {};

struct KeyBindings 
{
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
	KeyCode attack;
};


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 16
#define SPEED_MOD 200.0f

struct PlayerControlsSystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, KeyBindings, Position, SpriteAnimation>
{
	bool side = true;

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
				side = false;
				sprite.change_to("fallingHero/runningLeft");
				pos.xy.x -= SPEED_MOD * Time::delta_time();
			}
			else if (keys.is_down(bindings.right))
			{
				side = true;
				sprite.change_to("fallingHero/runningRight");
				pos.xy.x += SPEED_MOD * Time::delta_time();
			}
			else if (keys.is_down(bindings.attack))
			{
				if(side == true)
					sprite.change_to("fallingHero/attack");
				if(side == false)
					sprite.change_to("fallingHero/attackLeft");
			}
			else{
				if(side == true)
					sprite.change_to("fallingHero/heroStanding");
				if(side == false)
					sprite.change_to("fallingHero/leftHeroStanding");
				
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
			.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S, KeyCode::KEY_A, KeyCode::KEY_D, KeyCode::KEY_SPACE)
			.done();
	}
};

int main(int argc, char* argv[])
{
	auto& engine = Engine::get_instance();
	std::string filePath = "dagger.ini";
	engine.configure(filePath.data(), argc, argv);

	engine.setup<Default2D>();


	FallingHero game;
	engine.run();

	return 0;
}

