#pragma once

#include "Prelude.h"
#include "Random.h"
#include <iostream>

using namespace core;

struct Player {};
struct Background {};
struct Platform{};

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
#define NUMBER_OF_BACKGROUND 10
#define NUMBER_OF_PLATFORM 13

bool playerMove = false;

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

		for (auto&& [entity, bindings, pos, sprite] :access_storage().each())
		{
			// if (keys.is_down(bindings.up))
			// {
			// 	background_pos.xy.y += SPEED_MOD * Time::delta_time();
			// }

			if (keys.is_down(bindings.down) && playerMove)
			{
				pos.xy.y += SPEED_MOD * Time::delta_time();
			}
			if (keys.is_down(bindings.left))
			{
				side = false;
				sprite.change_to("fallingHero/runningLeft");
				pos.xy.x -= SPEED_MOD * Time::delta_time();
				if(pos.xy.x < 11)
					pos.xy.x = 11;
			}
			else if (keys.is_down(bindings.right))
			{
				side = true;
				sprite.change_to("fallingHero/runningRight");
				pos.xy.x += SPEED_MOD * Time::delta_time();
				if(pos.xy.x > SCREEN_WIDTH - 157)
					pos.xy.x = SCREEN_WIDTH - 157;
			}
			else if (keys.is_down(bindings.attack))
			{
				if(side)
					sprite.change_to("fallingHero/attack");
				if(!side)
					sprite.change_to("fallingHero/attackLeft");
			}
			else
			{
				if(side)
					sprite.change_to("fallingHero/heroStanding");
				if(!side)
					sprite.change_to("fallingHero/leftHeroStanding");
			}
				
		}
	}
};

struct BackgroundSystem
	: public ecs::System
	, public MutAccessGroupStorage<Background,KeyBindings, Position, Sprite>
{
	int counter = 0;

	void on_tick() override
	{
		const auto& keys = KeyState::get();
		
		for (auto&& [background_entity, bindings, background_pos, sprite] :access_storage().each())
		{
			sprite.depth = -100;

			if (keys.is_down(bindings.up))
			{
				background_pos.xy.y += SPEED_MOD * Time::delta_time();
			}	
			if (keys.is_down(bindings.down))
			{
				if(counter < 8)
				{
					background_pos.xy.y -= SPEED_MOD * Time::delta_time();
				}
				else
					playerMove = true;
				if(background_pos.xy.y <  -SCREEN_HEIGHT*2)
				{
					counter++;	
					despawn(background_entity);
				}
			}
		}
	}
};

struct PlatformSystem
	: public ecs::System
	, public MutAccessGroupStorage<Platform, KeyBindings, Position, Sprite>
{

	void on_tick() override
	{
				
		const auto& keys = KeyState::get();
		
		for (auto&& [platform_entity, bindings, platform_pos, sprite] :access_storage().each())
		{	
			sprite.depth = -50;
			if (keys.is_down(bindings.down))
			{
				if(platform_pos.xy.y >  SCREEN_HEIGHT -64)
				{
					platform_pos.xy.y -= SPEED_MOD * Time::delta_time();
				}	
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
		engine.use<BackgroundSystem>();
		engine.use<PlatformSystem>();

	}

	void on_start() override
	{
		for(int i = 0; i<NUMBER_OF_PLATFORM; i++)
			spawn()
				.with<Platform>()
				.with<Sprite>(ecs::no_entity)
				.with<SpriteAnimation>(Spritesheet::get_by_name("platform/platformPack"))
				.with<Position>(geometry::Vec2{ 64*i, SCREEN_HEIGHT*NUMBER_OF_BACKGROUND-64})
				.with<Visibility>(true)
				.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S).done();
		for(int i = 0; i<NUMBER_OF_BACKGROUND; i++)
			spawn()
				.with<Background>()
				.with<Sprite>(ecs::no_entity)
				.with<SpriteAnimation>(Spritesheet::get_by_name("background/bg47"))
				.with<Scale>(geometry::Vec2{4,3})
				.with<Position>(geometry::Vec2{ 0, SCREEN_HEIGHT*i })
				.with<Visibility>(true)
				.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S).done();
		auto hero = spawn()
			.with<Player>()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("fallingHero/heroStanding"))
			.with<Position>(geometry::Vec2{ 300, 100 })
			.with<Scale>(geometry::Vec2{3,3})
			.with<Visibility>(true)
			.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S, KeyCode::KEY_A, KeyCode::KEY_D, KeyCode::KEY_SPACE)
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


	FallingHero game;
	engine.run();

	return 0;
}

