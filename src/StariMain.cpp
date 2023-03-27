#pragma once

#include "Prelude.h"
#include "Random.h"

using namespace core;

struct PadCollisionSignal {
	ecs::Entity ball;
	ecs::Entity pad;
};

struct Player {};

struct Ball 
{
	geometry::Vec2 speed;
};

struct KeyBindings 
{
	KeyCode up;
	KeyCode down;
};

struct PhysicsSystem
	: public ecs::System
	, public AccessGroupStorage<Player, Position>
	, public AccessGroupStorage<Ball, Position>
	, public SignalEmitter<PadCollisionSignal>
{
	using QueryPlayers = AccessGroupStorage<Player, Position>;
	using QueryBalls = AccessGroupStorage<Ball, Position>;

	Bool intersects(const geometry::Vec2& pad_center, const geometry::Vec2& ball_center, float ball_radius)
	{
		static constexpr const int pad_width = 11;
		static constexpr const int pad_height = 32;

		const geometry::Vec2 circle_distance{
			abs(ball_center.x - pad_center.x),
			abs(ball_center.y - pad_center.y)
		};

		if (circle_distance.x > (pad_width + ball_radius)) { return false; }
		if (circle_distance.y > (pad_height + ball_radius)) { return false; }

		if (circle_distance.x <= pad_width) { return true; }
		if (circle_distance.y <= pad_height) { return true; }

		const auto dx = circle_distance.x - pad_width;
		const auto dy = circle_distance.y - pad_height;
		const auto corner_distance = dx * dx + dy * dy;

		return corner_distance <= ball_radius * ball_radius;
	}

	void on_tick() override
	{
		for (auto&& [ball_entity, ball, pos] : QueryBalls::access_storage().each())
		{
			for (auto&& [player_entity, pad] : QueryPlayers::access_storage().each())
			{
				if (intersects(pad.xy, pos.xy, 16))
				{
					emit(PadCollisionSignal{ ball_entity, player_entity });
				}
			}
		}
	}
};

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 16
#define SPEED_MOD 200.0f

struct BallMovementSystem
	: public ecs::System
	, public MutAccessGroupStorage<Ball, Position>
	, public MutAccessComponentById<Ball>
	, public SignalProcessor<PadCollisionSignal>
{
	virtual void process_signal(PadCollisionSignal& signal)
	{
		auto& ball = MutAccessComponentById<Ball>::get(signal.ball);
		ball.speed.x *= -1;
		ball.speed.y = get_random(-0.8f, 0.8f);
	}

	void on_tick() override
	{
		for (auto&& [entity, ball, pos] : access_storage().each())
		{
			if (pos.xy.y >= SCREEN_HEIGHT - BALL_RADIUS || pos.xy.y <= BALL_RADIUS)
			{
				ball.speed.y *= -1;
			}

			pos.xy += ball.speed * SPEED_MOD * Time::delta_time();
		}
	}
};

struct PlayerControlsSystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, KeyBindings, Position>
{
	void on_tick() override
	{
		const auto& keys = KeyState::get();

		if (keys.is_pressed(KEY_ESCAPE))
		{
			Engine::get_instance().quit();
		}

		for (auto&& [entity, bindings, pos] : access_storage().each())
		{
			if (keys.is_down(bindings.up))
			{
				pos.xy.y -= SPEED_MOD * Time::delta_time();
			}
			else if (keys.is_down(bindings.down))
			{
				pos.xy.y += SPEED_MOD * Time::delta_time();
			}
		}
	}
};

struct Pong : public Game
{
	Pong()
	{
		auto& engine = Engine::get_instance();
		engine.use<PlayerControlsSystem>();
		engine.use<BallMovementSystem>();
		engine.use<PhysicsSystem>();
	}

	void on_start() override
	{
		auto pad_left = spawn()
			.with<Player>()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("pong/pad"))
			.with<Position>(geometry::Vec2{ 50, 300 })
			.with<Visibility>(true)
			.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S)
			.done();

		auto pad_right = spawn()
			.with<Player>()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("pong/pad"))
			.with<Position>(geometry::Vec2{ 750, 300 })
			.with<Visibility>(true)
			.with<KeyBindings>(KeyCode::KEY_UP, KeyCode::KEY_DOWN)
			.done();

		auto ball = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("pong/ball"))
			.with<Position>(geometry::Vec2{ 300, 100 })
			.with<Visibility>(true)
			.with<Ball>(geometry::Vec2{ 1, 0 })
			.done();
	}
};

int main(int argc, char* argv[])
{
	auto& engine = Engine::get_instance();
	engine.configure("dagger.ini", argc, argv);

	engine.setup<Default2D>();

	Pong game;
	engine.run();

	return 0;
}

