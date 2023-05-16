#pragma once

#include "Prelude.h"
#include "Random.h"
#include <iostream>
#include <algorithm>
#include <random>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 16
#define SPEED_MOD 200.0f
#define ENEMY_SPEED_MOD 150.0f
#define RANGE_X 801
#define RANGE_Y 601

#include "Common.h"
#include "FSM.h"
#include "Physics.h"
#include "Enemies.h"
#include "Players.h"
#include "DamageSystem.h"
#include "TimeRender.h"

using namespace core;

struct Brawl : public Game
{
	Brawl()
	{
		auto &engine = Engine::get_instance();
		engine.use<PlayerControlsSystem>();
		engine.use<EnemyMovementSystem>();
		engine.use<PhysicsSystem>();
		engine.use<DamageSystem>();

		// TODO: make a system to resolve damage and call it here
		engine.use<TimeRenderControlSystem>();
	}

	
	int num;
	void on_start() override
	{
		spawn()
			.with<Player>()
			.with<Health>(100)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("Moose/Moose1_Idle"))
			.with<Position>(geometry::Vec2{400, 300})
			.with<Visibility>(true)
			.with<Flip>(None)
			.with<AnimationSpeedController>(15.0f)
			.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S, KeyCode::KEY_A, KeyCode::KEY_D, KeyCode::KEY_J)
			.done();


		num = rand() % RANGE_X;
		spawn()
			.with<Sprite>(ecs::no_entity)
			.with<Health>(100)
			.with<SpriteAnimation>(Spritesheet::get_by_name("Skeleton/Skeleton_Idle"))
			.with<Position>(geometry::Vec2{ num, 0 })
			.with<Visibility>(true)
			.with<AnimationSpeedController>(1.0f)
			.with<Enemy>(geometry::Vec2{ 1, 0 })
			.done();

		num = rand() % RANGE_X;
		spawn()
			.with<Sprite>(ecs::no_entity)
			.with<Health>(100)
			.with<SpriteAnimation>(Spritesheet::get_by_name("Skeleton/Skeleton_Idle"))
			.with<Position>(geometry::Vec2{ num, 600 })
			.with<Visibility>(true)
			.with<AnimationSpeedController>(1.0f)
			.with<Enemy>(geometry::Vec2{ 1, 0 })
			.done();

		num = rand() % RANGE_Y;
		spawn()
			.with<Sprite>(ecs::no_entity)
			.with<Health>(100)
			.with<SpriteAnimation>(Spritesheet::get_by_name("Skeleton/Skeleton_Idle"))
			.with<Position>(geometry::Vec2{ 0, num })
			.with<Visibility>(true)
			.with<AnimationSpeedController>(1.0f)
			.with<Enemy>(geometry::Vec2{ 1, 0 })
			.done();

		num = rand() % RANGE_Y;
		spawn()
			.with<Sprite>(ecs::no_entity)
			.with<Health>(100)
			.with<SpriteAnimation>(Spritesheet::get_by_name("Skeleton/Skeleton_Idle"))
			.with<Position>(geometry::Vec2{ 800, num })
			.with<Visibility>(true)
			.with<AnimationSpeedController>(1.0f)
			.with<Enemy>(geometry::Vec2{ 1, 0 })
			.done();

		TimeRender::init();
		auto time = spawn()
			.with<TimeRender>("00:00:00", 30)
			.with<Sprite>(ecs::no_entity)
			.with<Visibility>(true)
			.with<Position>(geometry::Vec2{ 600, 30 })
			.with<AnimationSpeedController>(1.0f)
			.done();

		auto health = spawn()
            .with<Healthbar>(100.0f)
			.with<Sprite>(ecs::no_entity,-7)
			.with<SpriteAnimation>(Spritesheet::get_by_name("healthbar/health"))
			.with<Scale>(geometry::Vec2{ 0.5, 0.5})
			.with<Position>(geometry::Vec2{ 500, 100 })
			.with<Visibility>(true)
			.with<AnimationSpeedController>(1.0f)
			
			.done();

		spawn()
			.with<Sprite>(ecs::no_entity, -10)
			.with<SpriteAnimation>(Spritesheet::get_by_name("background/background"))
			.with<Position>(geometry::Vec2{ SCREEN_WIDTH / 2 , SCREEN_HEIGHT / 2})
			.with<AnimationSpeedController>(1.0f)
			.with<Visibility>(true)
			.done();
	}

	

	void on_end() override
	{
		TimeRender::deinit();
	}
};

int main(int argc, char *argv[])
{
	auto &engine = Engine::get_instance();
	std::string filePath = "dagger.ini";
	engine.configure(filePath.data(), argc, argv);

	engine.setup<Default2D>();

	Brawl game;
	engine.run();

	return 0;
}
