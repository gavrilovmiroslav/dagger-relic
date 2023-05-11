#pragma once

#include "Prelude.h"
#include "Components.h"
#include "PlayerControlsSystem.h"
#include "Platforms.h"
#include "BoxMovingSystem.h"

struct PurgatoryPals : public Game
{
	PurgatoryPals()
	{
		auto& engine = Engine::get_instance();
		engine.use<PlayerControlsSystem>();
		engine.use<PlatformSystem>();
		engine.use<BoxMovingSystem>();
	}

	void on_start() override
	{
		auto rogue = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("Rogue/idle"))
			.with<Position>(geometry::Vec2{500, 300})
			.with<Gravity>(geometry::Vec2{0, 0.02})
			.with<Visibility>(true)
			.with<Player>(geometry::Vec2{ 0, 0 }, false, 300.0f)
			.with<KeyBindings>(KeyCode::KEY_UP, KeyCode::KEY_LEFT, KeyCode::KEY_RIGHT)
			.with<Animation>("Rogue/jump", "Rogue/walk", "Rogue/idle")
			.with<Flip>(None)
			.done();

		auto knight = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("Knight/idle"))
			.with<Position>(geometry::Vec2{200, 300})
			.with<Gravity>(geometry::Vec2{0, 0.03})
			.with<Visibility>(true)
			.with<Player>(geometry::Vec2{ 0, 0 }, false, 300.0f)
			.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_A, KeyCode::KEY_D)
			.with<Animation>("Knight/jump", "Knight/walk", "Knight/idle")
			.with<Flip>(None)
			.done();

		auto platform1 = spawn()
			.with<Platform>(128.0f, 4.0f)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("platform/platform"))
			.with<Position>(geometry::Vec2{ 550, 400 })
			.with<Visibility>(true)
			.done();

		auto platform5 = spawn()
			.with<Platform>(128.0f, 4.0f)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("platform/platform"))
			.with<Position>(geometry::Vec2{ 250, 400 })
			.with<Visibility>(true)
			.done();	

		auto platform3 = spawn()
			.with<Platform>(128.0f, 4.0f)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("platform/platform"))
			.with<Position>(geometry::Vec2{ 50, 100 })
			.with<Visibility>(true)
			.done();

		auto platform4 = spawn()
			.with<Platform>(128.0f, 4.0f)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("platform/platform"))
			.with<Position>(geometry::Vec2{ 750, 100 })
			.with<Visibility>(true)
			.done();	
			
		auto platform2 = spawn()
			.with<Platform>(1380.0f, 4.0f)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("platform/platform"))
			.with<Position>(geometry::Vec2{128, 600})
			.with<Visibility>(true)
			.done();

		auto box = spawn()

			.with<Position>(geometry::Vec2{250, 368})
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("box/box"))
			.with<Visibility>(true)
			.with<Box>(geometry::Vec2{0.0f, 0.0f}, false)
			.with<Gravity>(geometry::Vec2{0.0f, 0.03f})
			.done();
	}
};

int main(int argc, char* argv[])
{
	auto& engine = Engine::get_instance();
	engine.configure("dagger.ini", argc, argv);

	engine.setup<Default2D>();

	PurgatoryPals game;
	engine.run();

	return 0;
}
