#pragma once

#include "Prelude.h"
#include "Random.h"
#include <iostream>
#include <algorithm>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 16
#define SPEED_MOD 200.0f
#define ENEMY_SPEED_MOD 150.0f

#include "Common.h"
#include "FSM.h"
#include "Physics.h"
#include "Enemies.h"
#include "Players.h"
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
		// TODO: make a system to resolve damage and call it here
		engine.use<TimeRenderControlSystem>();
	}

	void on_start() override
	{
		spawn()
			.with<Player>(100.0f)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("Moose/Moose1_Idle"))
			.with<Position>(geometry::Vec2{400, 300})
			.with<Visibility>(true)
			.with<Flip>(None)
			.with<AnimationSpeedController>(15.0f)
			.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S, KeyCode::KEY_A, KeyCode::KEY_D, KeyCode::KEY_J)
			.done();

		for (auto i = 0; i < 4 ; ++i)
		{
			spawn()
				.with<Sprite>(ecs::no_entity)
				.with<SpriteAnimation>(Spritesheet::get_by_name("Skeleton/Skeleton_Idle"))
				.with<Position>(geometry::Vec2{ 760 - 30*i, 60*i })
				.with<Visibility>(true)
				.with<AnimationSpeedController>(1.0f)
				.with<Enemy>(geometry::Vec2{ 1, 0 }, 100.0f)
				.done();
		}

		TimeRender::init();
		auto time = spawn()
			.with<TimeRender>("00:00:00", 32)
			.with<Sprite>(ecs::no_entity)
			.with<Visibility>(true)
			.with<Position>(geometry::Vec2{ 400, 300 })
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
