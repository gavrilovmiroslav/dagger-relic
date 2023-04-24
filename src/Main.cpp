#pragma once

#include "Prelude.h"
#include "Components.h"
#include "PlayerControlsSystem.h"

struct PurgatoryPals : public Game
{
	PurgatoryPals()
	{
		auto& engine = Engine::get_instance();
		engine.use<PlayerControlsSystem>();
	}

	void on_start() override
	{
		auto rogue = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("Rogue/idle"))
			.with<Position>(geometry::Vec2{600, 300})
			.with<Gravity>(geometry::Vec2{0, 0.03})
			.with<Visibility>(true)
			.with<Player>(geometry::Vec2{ 0, 0 }, false)
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
			.with<Player>(geometry::Vec2{ 0, 0 }, true)
			.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_A, KeyCode::KEY_D)
			.with<Animation>("Knight/jump", "Knight/walk", "Knight/idle")
			.with<Flip>(None)
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

