#pragma once

#include "Prelude.h"
#include "Random.h"

#define INITIAL_VALUE 3

using namespace core;

struct Movement 
{
	geometry::Vec2 velocity;
	geometry::Vec2 velocitymax;
	geometry::Vec2 force;
	F32            moveforce;

	Movement(F32 moveforce, F32 velocitymax) : velocity(0.0f, 0.0f), moveforce(moveforce), velocitymax(velocitymax), force(0.0f, 0.0f) { }
};

struct KeyBindings 
{
	KeyCode left, down, up, right;
};

enum SpecialBlindfold
{
	HumanEyes = 0,
	FoxEyes = 1,
	ScorpionEyes = 2
};

struct Player
{
	Bool is_pushing;
	Bool all_platforms_checked;
	U32 number_of_platforms_checked;
	SpecialBlindfold current_blindfold;
	Map<SpecialBlindfold, U32> available_blindfolds;
};

static float
fsignf(float x)
{
	if (x > 0.0f) return  1.0f;
	if (x < 0.0f) return -1.0f;
	else          return  0.0f;
}

static float
fclampf(float x, float min, float max)
{
	if (x < min) return min;
	if (x > max) return max;
	else         return x;
}

struct MovementSystem
	: public ecs::System
	, public MutAccessGroupStorage<Movement, Position>
{
	void on_tick() override
	{
		for (auto&& [entity, movement, pos] : access_storage().each())
		{
			F32 u = 16.0f;                              /* Friction coefficient. */
			F32 m = 5.0f;                               /* Object mass (kg). */
			F32 g = 9.807f;                             /* Gravity acceleration. */
			F32 n = m*g;                                /* Normal force. */
			F32 fnx = -fsignf(movement.velocity.x)*u*n; /* Friction force. */
			F32 fny = -fsignf(movement.velocity.y)*u*n; 
			F32 fx  = fnx+movement.force.x;             /* Total force. */
			F32 fy  = fny+movement.force.y;

			pos.xy              += movement.velocity*Time::delta_time();
			movement.velocity.x += (fx/m)*Time::delta_time();
			movement.velocity.y += (fy/m)*Time::delta_time();
			movement.velocity.x = fclampf(movement.velocity.x, -movement.velocitymax.x, movement.velocitymax.x);
			movement.velocity.y = fclampf(movement.velocity.y, -movement.velocitymax.y, movement.velocitymax.y);
		}
	}
};

struct MovementControlSystem
	: public ecs::System
	, public MutAccessGroupStorage<KeyBindings, Movement>
{
	void on_tick() override
	{
		const auto& keys = KeyState::get();

		for (auto&& [entity, keybinding, movement] : access_storage().each())
		{
			if (keys.is_down(keybinding.up))         movement.force.y = -movement.moveforce;
			else if (keys.is_down(keybinding.down))  movement.force.y =  movement.moveforce;
			else                                     movement.force.y -= fsignf(movement.force.y)*movement.moveforce;

			if (keys.is_down(keybinding.left))       movement.force.x = -movement.moveforce;
			else if (keys.is_down(keybinding.right)) movement.force.x =  movement.moveforce;
			else                                     movement.force.x -= fsignf(movement.force.x)*movement.moveforce;

			spdlog::info("force: {} {}", movement.force.x, movement.force.y);
		}
	}
};

struct Pong : public Game
{
	Pong()
	{
		auto& engine = Engine::get_instance();
		engine.use<MovementSystem>();
		engine.use<MovementControlSystem>();
	}

	void on_start() override
	{
		auto player = spawn()
			.with<Player>(false, false, 0u, HumanEyes, Map<SpecialBlindfold, U32>({{HumanEyes, INITIAL_VALUE}, {FoxEyes, INITIAL_VALUE}, 
																				   {ScorpionEyes, INITIAL_VALUE}}))
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("pong/pad"))
			.with<Position>(geometry::Vec2{ 40, 550 })
			.with<Visibility>(true)
			.with<Movement>(2000.0f, 50.0f)
			.with<KeyBindings>(KeyCode::KEY_LEFT, KeyCode::KEY_DOWN, KeyCode::KEY_UP, KeyCode::KEY_RIGHT)
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

	Pong game;
	engine.run();

	return 0;
}
