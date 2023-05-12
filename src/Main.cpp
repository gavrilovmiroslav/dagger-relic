#pragma once

#include <stdlib.h>
#include <time.h>

#include "Prelude.h"

#include "GameSpecificComponents.h"
#include "GameSpecificSystems.h"

using namespace core;

struct SWMG 
	: public Game
	, public SignalEmitter<OnStartSignal> 
	, public MutAccessComponentById<SpriteAnimation>
{
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
		engine.use<HealthBarAnimationSystem>();
	}

	void on_start() override
	{

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

		auto player_one_entity = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/Wizard1/WizardIdle"))
			.with<Position>(geometry::Vec2{ 50, 600 - 32})
			.with<Visibility>(true)
			.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S, KeyCode::KEY_A, KeyCode::KEY_D, KeyCode::KEY_F, KeyCode::KEY_G)
			.with<Flip>(None)
			.with<Item>("None", 3)
			.with<Status>(100)
			.done();
		auto player_one_component = Player { PlayerFSM(player_one_entity, MutAccessComponentById<SpriteAnimation>::get(player_one_entity), PlayerColor::PURPLE), 0.0f };
		add_component<Player>(player_one_entity, player_one_component);

		auto player_two_entity = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/Wizard2/WizardIdle"))
			.with<Position>(geometry::Vec2{ 750, 600 - 32})
			.with<Visibility>(true)
			.with<KeyBindings>(KeyCode::KEY_UP, KeyCode::KEY_DOWN, KeyCode::KEY_LEFT, KeyCode::KEY_RIGHT, KeyCode::KEY_K, KeyCode::KEY_L)
			.with<Flip>(Horizontal)
			.with<Item>("None", 3)
			.with<Status>(100)
			.done();
		auto player_two_component = Player { PlayerFSM(player_two_entity, MutAccessComponentById<SpriteAnimation>::get(player_two_entity), PlayerColor::GREEN), 0.0f };
		add_component<Player>(player_two_entity, player_two_component);

		auto health_bar_one = spawn()
			.with<Status>(100)
			.with<PlayerId>(player_one_entity)
			.with<Sprite>(ecs::no_entity, 1)
			.with<HealthBarSpriteAnimation>(Spritesheet::get_by_name("test/HealthBar"), 1.0f, 0)
            .with<Scale>(geometry::Vec2{3.0f,3.0f})
			.with<Position>(geometry::Vec2{50, 50})
			.with<Visibility>(true)
			.done();

		auto health_bar_two = spawn()
			.with<Status>(100)
			.with<PlayerId>(player_two_entity)
			.with<Sprite>(ecs::no_entity, 1)
			.with<HealthBarSpriteAnimation>(Spritesheet::get_by_name("test/HealthBar"), 1.0f, 0)
            .with<Scale>(geometry::Vec2{3.0f,3.0f})
			.with<Position>(geometry::Vec2{700, 50})
			.with<Visibility>(true)
			.done();

		SignalEmitter<OnStartSignal>::emit(OnStartSignal{100});

		auto background = spawn()
			.with<Platform>()
			.with<Sprite>(ecs::no_entity, -100)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/Background"))
			.with<Visibility>(true)
			.with<Position>(geometry::Vec2{ SCREEN_WIDTH/2, SCREEN_HEIGHT/2})
			.done();
	}
};

int main(int argc, char* argv[])
{
	srand(time(NULL));
	auto& engine = Engine::get_instance();
	std::string filePath = "dagger.ini";
	engine.configure(filePath.data(), argc, argv);

	engine.setup<Default2D>();

	SWMG game;
	engine.run();

	return 0;
}
