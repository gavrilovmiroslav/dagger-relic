#pragma once

#include "Prelude.h"

#include "GameSpecificComponents.h"
#include "GameSpecificSystems.h"

using namespace core;

struct SWMG : public Game {
	SWMG()
	{
		auto& engine = Engine::get_instance();
		engine.use<PlayerControlsSystem>();
		engine.use<GravitySystem>();
		engine.use<PlatformSystem>();
		engine.use<PickupSystem>();
		engine.use<SpellMovementSystem>();
		engine.use<SpellCollisionSystem>();
		engine.use<ProjectileSpawnSystem>();
	}

	void on_start() override
	{
		for (int i = 0; i < 10; i++)
		{
			spawn()
				.with<Pickup>("curseball" , 32.0f, 5)
				.with<Sprite>(ecs::no_entity)
				.with<SpriteAnimation>(Spritesheet::get_by_name("test/pickup"))
				.with<Position>(geometry::Vec2{ rand() % 600, rand() % 570 })
				.with<Visibility>(true);
		}

		auto platform1 = spawn()
			.with<Platform>(130.0f, 21.0f)
			.with<Sprite>(ecs::no_entity, 20)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/pixel-platform"))
            .with<Scale>(geometry::Vec2{0.25f,0.1f})
			.with<Position>(geometry::Vec2{ 400, 550 })
			.with<Visibility>(true);

		auto platform2 = spawn()
			.with<Platform>(130.0f, 21.0f)
			.with<Sprite>(ecs::no_entity, 20)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/pixel-platform"))
            .with<Scale>(geometry::Vec2{0.25f,0.1f})
			.with<Position>(geometry::Vec2{ 200, 450 })
			.with<Visibility>(true);

		auto platform3 = spawn()
			.with<Platform>(130.0f, 21.0f)
			.with<Sprite>(ecs::no_entity, 20)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/pixel-platform"))
            .with<Scale>(geometry::Vec2{0.25f,0.1f})
			.with<Position>(geometry::Vec2{ 300, 500 })
			.with<Visibility>(true);

		auto platform4 = spawn()
			.with<Platform>(130.0f, 21.0f)
			.with<Sprite>(ecs::no_entity, 20)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/pixel-platform"))
            .with<Scale>(geometry::Vec2{0.25f,0.1f})
			.with<Position>(geometry::Vec2{ 450, 450 })
			.with<Visibility>(true);

		auto platform5 = spawn()
			.with<Platform>(130.0f, 21.0f)
			.with<Sprite>(ecs::no_entity, 20)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/pixel-platform"))
            .with<Scale>(geometry::Vec2{0.25f,0.1f})
			.with<Position>(geometry::Vec2{ 600, 400 })
			.with<Visibility>(true);

		auto ground = spawn()
			.with<Platform>((F32)SCREEN_WIDTH, 16.0f)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/platform"))
			.with<Position>(geometry::Vec2{ SCREEN_WIDTH / 2, SCREEN_HEIGHT })
			.with<Visibility>(true);

		auto player_one = spawn()
			.with<Player>(false, 0.0f)
			.with<Sprite>(ecs::no_entity, 1)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/WizardIdle"))
			.with<Position>(geometry::Vec2{ 50, 600 - 32})
			.with<Visibility>(true)
			.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S, KeyCode::KEY_A, KeyCode::KEY_D, KeyCode::KEY_F, KeyCode::KEY_G)
			.with<Flip>(None)
			.with<Item>("None", 3)
			.with<Status>(100)
			.done();

		auto player_two = spawn()
			.with<Player>(false, 0.0f)
			.with<Sprite>(ecs::no_entity, 1)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/WizardIdle"))
			.with<Position>(geometry::Vec2{ 750, 600 - 32})
			.with<Visibility>(true)
			.with<KeyBindings>(KeyCode::KEY_UP, KeyCode::KEY_DOWN, KeyCode::KEY_LEFT, KeyCode::KEY_RIGHT, KeyCode::KEY_K, KeyCode::KEY_L)
			.with<Flip>(Horizontal)
			.with<Item>("None", 3)
			.with<Status>(100)
			.done();

		auto health_bar_one = spawn()
			.with<Status>(100)
			.with<Sprite>(ecs::no_entity, 1)
			.with<HealthBarSpriteAnimation>(Spritesheet::get_by_name("test/HealthBar"))
            .with<Scale>(geometry::Vec2{3.0f,3.0f})
			.with<Position>(geometry::Vec2{50, 50})
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

	SWMG game;
	engine.run();

	return 0;
}
