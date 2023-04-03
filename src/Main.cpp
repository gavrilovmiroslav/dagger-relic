#pragma once

#include "Prelude.h"
#include "Random.h"
#include <iostream>

using namespace core;

struct PlayerCollisionSignal {
	ecs::Entity enemy;
	ecs::Entity player;
};

struct Player
{
};

struct Enemy
{
	geometry::Vec2 speed;
};

// struct Ball
// {
// 	geometry::Vec2 speed;
// };

struct KeyBindings
{
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
};

struct PhysicsSystem
	: public ecs::System,
	  public AccessGroupStorage<Player, Position>
	, public AccessGroupStorage<Enemy, Position>
	, public SignalEmitter<PlayerCollisionSignal>
{
	using QueryPlayers = AccessGroupStorage<Player, Position>;
		using QueryEnemies = AccessGroupStorage<Enemy, Position>;

		Bool intersects(const geometry::Vec2& player_center, const geometry::Vec2& enemy_center, float enemy_radius)
		{
			static constexpr const int player_width = 11;
			static constexpr const int player_height = 32;

			const geometry::Vec2 circle_distance{
				abs(enemy_center.x - player_center.x),
				abs(enemy_center.y - player_center.y)
			};

			if (circle_distance.x > (player_width + enemy_radius)) { return false; }
			if (circle_distance.y > (player_height + enemy_radius)) { return false; }

			if (circle_distance.x <= player_width) { return true; }
			if (circle_distance.y <= player_height) { return true; }

			const auto dx = circle_distance.x - player_width;
			const auto dy = circle_distance.y - player_height;
			const auto corner_distance = dx * dx + dy * dy;

			return corner_distance <= enemy_radius * enemy_radius;
		}


		void on_tick() override
		{
			for (auto&& [enemy_entity, enemy, pos] : QueryEnemies::access_storage().each())
			{
				for (auto&& [player_entity, player] : QueryPlayers::access_storage().each())
				{
					if (intersects(player.xy, pos.xy, 16))
					{
						emit(PlayerCollisionSignal{ enemy_entity, player_entity});
					}
				}
			}
		}
};

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 16
#define SPEED_MOD 200.0f
#define ENEMY_SPEED_MOD 150.0f

struct EnemyMovementSystem
	: public ecs::System
	, public MutAccessGroupStorage<Enemy, Position>
 	, public AccessGroupStorage<Player, Position>
	, public MutAccessComponentById<Enemy>
	, public MutAccessComponentById<Position>
	, public SignalProcessor<PlayerCollisionSignal>
{
	
	using QueryPlayers = AccessGroupStorage<Player, Position>;
	using QueryEnemies = MutAccessGroupStorage<Enemy, Position>;
	
	virtual void process_signal(PlayerCollisionSignal& signal)
	{
		auto& enemy = MutAccessComponentById<Enemy>::get(signal.enemy);
		auto& enemy_pos = MutAccessComponentById<Position>::get(signal.enemy);
		
		enemy.speed *= -1;
		enemy_pos.xy += enemy.speed * ENEMY_SPEED_MOD;

	}

	void on_tick() override
	{
		
		
		for (auto &&[player_entity, player_pos] : QueryPlayers::access_storage().each())
		{
			for (auto&& [entity, enemy, pos] : QueryEnemies::access_storage().each())
			{
				if (pos.xy.y >= SCREEN_HEIGHT - BALL_RADIUS || pos.xy.y <= BALL_RADIUS)
				{
					enemy.speed.y *= -1;
				}

				enemy.speed = (player_pos.xy - pos.xy);
				enemy.speed /= sqrt(pow(enemy.speed.x, 2) + pow(enemy.speed.y, 2));
				
				pos.xy += enemy.speed* Time::delta_time() * ENEMY_SPEED_MOD;

			}
		}
	}
};

struct PlayerControlsSystem
	: public ecs::System,
	  public MutAccessGroupStorage<Player, KeyBindings, Position>
{
	void on_tick() override
	{
		const auto &keys = KeyState::get();

		if (keys.is_pressed(KEY_ESCAPE))
		{
			Engine::get_instance().quit();
		}

		for (auto &&[entity, bindings, pos] : access_storage().each())
		{
			if (keys.is_down(bindings.up))
			{
				pos.xy.y -= SPEED_MOD * Time::delta_time();

			}
			else if (keys.is_down(bindings.down))
			{
				pos.xy.y += SPEED_MOD * Time::delta_time();
			}
			else if (keys.is_down(bindings.left))
			{
				pos.xy.x -= SPEED_MOD * Time::delta_time();
			}
			else if (keys.is_down(bindings.right))
			{
				pos.xy.x += SPEED_MOD * Time::delta_time();
			}
		}
	}
};

struct Pong : public Game
{
	Pong()
	{
		auto &engine = Engine::get_instance();
		engine.use<PlayerControlsSystem>();
		engine.use<EnemyMovementSystem>();
		engine.use<PhysicsSystem>();
	}

	void on_start() override
	{
		auto pad_left = spawn()
							.with<Player>()
							.with<Sprite>(ecs::no_entity)
							.with<SpriteAnimation>(Spritesheet::get_by_name("pong/pad"))
							.with<Position>(geometry::Vec2{400, 300})
							.with<Visibility>(true)
							.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S, KeyCode::KEY_A, KeyCode::KEY_D)
							.done();

		// auto pad_right = spawn()
		// 	.with<Player>()
		// 	.with<Sprite>(ecs::no_entity)
		// 	.with<SpriteAnimation>(Spritesheet::get_by_name("pong/pad"))
		// 	.with<Position>(geometry::Vec2{ 750, 300 })
		// 	.with<Visibility>(true)
		// 	.with<KeyBindings>(KeyCode::KEY_UP, KeyCode::KEY_DOWN)
		// 	.done();

				auto enemy = spawn()
					.with<Sprite>(ecs::no_entity)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pong/ball"))
					.with<Position>(geometry::Vec2{ 760, 0 })
					.with<Visibility>(true)
					.with<Enemy>(geometry::Vec2{ 1, 0 })
					.done();
	}
};

int main(int argc, char *argv[])
{
	auto &engine = Engine::get_instance();
	std::string filePath = "dagger.ini";
	engine.configure(filePath.data(), argc, argv);

	engine.setup<Default2D>();

	Pong game;
	engine.run();

	return 0;
}
